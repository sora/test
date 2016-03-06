#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/smp.h>
#include <linux/err.h>

#define NAPIHOOK_VERSION  "0.0.0"


struct rxthread {
	unsigned int cpu;
	struct task_struct *tsk;
	struct completion start_done;
	struct list_head list;
};

struct nh_dev {
	struct net_device *dev;
	char dev_name[IFNAMSIZ];

	struct rxthread rxth;
	
	// tmp
	unsigned int cpu;
};

/* Module parameters, defaults. */
//static int debug = 0;
static char *ifname = "eth0";

/* Global variables */
static struct nh_dev *nh;


static inline void rxthread_body(void)
{
	pr_info("rxthread_body\n");
}

static int rxthread_worker(void *arg)
{
	unsigned int cpu = smp_processor_id();
	pr_info("starting knapihookd/%d:  pid=%d\n", cpu, task_pid_nr(current));

	set_current_state(TASK_INTERRUPTIBLE);

	while (!kthread_should_stop()) {
		schedule_timeout_interruptible(1000);

		__set_current_state(TASK_RUNNING);

		rxthread_body();
		if (need_resched())
			schedule();
		else
			cpu_relax();
		set_current_state(TASK_INTERRUPTIBLE);
	}

	pr_info("kthread_exit: cpu=%d\n", cpu);

	return 0;
}

static int rxthread_start(unsigned int cpu)
{
	struct task_struct *p = kthread_create_on_node(rxthread_worker,
	                        NULL,
	                        cpu_to_node(cpu),
	                        "knapihookd_%d", cpu);
	if (IS_ERR(p)) {
		pr_info("kthread_create_on_node() failed for cpu %d\n", cpu);
		return PTR_ERR(p);
	}
	kthread_bind(p, cpu);
	wake_up_process(p);

	nh->rxth.tsk = p;

	return 0;
}

static void rxthread_stop(void)
{
	if (nh->rxth.tsk)
		kthread_stop(nh->rxth.tsk);

	pr_info("kthread stop\n");
}

static int __init nh_init(void)
{
	int rc = 0, err;

	pr_info("napihook (v%s) is loaded\n", NAPIHOOK_VERSION);

	nh = kmalloc(sizeof(struct nh_dev), GFP_KERNEL);
	if (nh == 0) {
		pr_info("fail to kmalloc: *nh_dev\n");
		rc = -1;
		goto out;
	}

	nh->dev = dev_get_by_name(&init_net, ifname);
	if (!nh->dev) {
		pr_err("Could not find %s\n", ifname);
		rc = -1;
		goto out;
	}

	nh->cpu = smp_processor_id();
	pr_info("cpuid: %d\n", nh->cpu);

	err = rxthread_start(nh->cpu);
	if (err)
		pr_info("cannot create thread for cpu %d (%d)\n", nh->cpu, err);

out:
	return rc;
}
module_init(nh_init);


static void __exit nh_release(void)
{
	pr_info("napihook (v%s) is unloaded\n", NAPIHOOK_VERSION);

	rxthread_stop();

	kfree(nh);
	nh = NULL;

	return;
}
module_exit(nh_release);


MODULE_AUTHOR("Yohei Kuga <sora@haeena.net>");
MODULE_DESCRIPTION("NAPI hook");
MODULE_LICENSE("GPL");
MODULE_VERSION(NAPIHOOK_VERSION);

module_param(ifname, charp, S_IRUGO);
MODULE_PARM_DESC(ifname, "Target network device name");

