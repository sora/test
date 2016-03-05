#include <linux/kernel.h>
#include <linux/module.h>

#define NAPIHOOK_VERSION  "0.0.0"

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt


static int __init nh_init(void)
{
	int rc = 0;

	pr_info("napihook (v%s) is loaded", NAPIHOOK_VERSION);

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

