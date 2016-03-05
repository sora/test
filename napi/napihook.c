#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <trace/events/napi.h>

struct nh_dev {
	struct net_device *dev;
	char dev_name[IFNAMSIZ];
	const char *name;
};

#define NAPIHOOK_VERSION  "0.0.0"

/* Module parameters, defaults. */
//static int debug = 0;
static char *ifname = "eth0";


static int __init nh_init(void)
{
	struct nh_dev nh;
	int rc = 0;

	pr_info("napihook (v%s) is loaded", NAPIHOOK_VERSION);

	nh.dev = dev_get_by_name(&init_net, ifname);
	if (!nh.dev) {
		pr_warn("Could not find %s\n", ifname);
		rc = -1;
	}

	return rc;
}
module_init(nh_init);


static void __exit nh_release(void)
{
	pr_info("napihook (v%s) is unloaded", NAPIHOOK_VERSION);
	return;
}
module_exit(nh_release);


MODULE_AUTHOR("Yohei Kuga <sora@haeena.net>");
MODULE_DESCRIPTION("NAPI hook");
MODULE_LICENSE("GPL");
MODULE_VERSION(NAPIHOOK_VERSION);

module_param(ifname, charp, S_IRUGO);
MODULE_PARM_DESC(ifname, "Target network device name");

