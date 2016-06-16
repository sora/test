#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/pci.h>

#define DRV_NAME       "tlptap"
#define IFNAMSIZ       16
#define WRMOD_VERSION  "0.0.0"

struct mmio {
	uint8_t *virt;
	uint64_t start;
	uint64_t end;
	uint64_t flags;
	uint64_t len;
};

struct nfsume {
	struct mmio bar0;
	struct mmio bar2;
};

struct tt_dev {
	struct nfsume dev;
};

/* Global variables */
static struct tt_dev *tt;


static int tlptap_open(struct inode *inode, struct file *filp)
{
	pr_info("%s\n", __func__);
	return 0;
}

static int tlptap_release(struct inode *inode, struct file *filp)
{
	pr_info("%s\n", __func__);
	return 0;
}

static ssize_t tlptap_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *ppos)
{
	pr_info("%s\n", __func__);
	return 0;
}

static ssize_t tlptap_read(struct file *filp, char __user *buf,
		size_t count, loff_t *ppos)
{
	pr_info("%s\n", __func__);
	return 0;
}


static int tlptap_pci_init(struct pci_dev *pdev,
		const struct pci_device_id *ent)
{
	int rc;
	struct mmio *bar0 = &tt->dev.bar0;
	struct mmio *bar2 = &tt->dev.bar2;

	pr_info("%s\n", __func__);

	rc = pci_enable_device(pdev);
	if (rc)
		goto error;

	rc = pci_request_regions(pdev, DRV_NAME);
	if (rc)
		goto error;

	/* BAR0 (pcie pio) */
	bar0->start = pci_resource_start(pdev, 0);
	bar0->end   = pci_resource_end(pdev, 0);
	bar0->flags = pci_resource_flags(pdev, 0);
	bar0->len   = pci_resource_len(pdev, 0);
	bar0->virt  = ioremap(bar0->start, bar0->len);
	if(!bar0->virt) {
		pr_info("cannot ioremap MMIO0 base\n");
		goto error;
	}
	pr_info("bar0_start: %X\n", (unsigned int)bar0->start);
	pr_info("bar0_end  : %X\n", (unsigned int)bar0->end);
	pr_info("bar0_flags: %X\n", (unsigned int)bar0->flags);
	pr_info("bar0_len  : %X\n", (unsigned int)bar0->len);

	/* BAR1 (pcie pio) */
	bar2->start = pci_resource_start(pdev, 2);
	bar2->end   = pci_resource_end(pdev, 2);
	bar2->flags = pci_resource_flags(pdev, 2);
	bar2->len   = pci_resource_len(pdev, 2);
	bar2->virt  = ioremap(bar2->start, bar2->len);
	if (!bar2->virt) {
		pr_info("cannot ioremap MMIO1 base\n");
		goto error;
	}
	pr_info("bar2_virt : %p\n", bar2->virt);
	pr_info("bar2_start: %X\n", (unsigned int)bar2->start);
	pr_info("bar2_end  : %X\n", (unsigned int)bar2->end);
	pr_info("bar2_flags: %X\n", (unsigned int)bar2->flags);
	pr_info("bar2_len  : %X\n", (unsigned int)bar2->len);

	return 0;

error:
	pr_info("tlptap_pci_init error\n");
	pci_release_regions(pdev);
	pci_disable_device(pdev);
	return -1;
}

static void tlptap_pci_remove(struct pci_dev *pdev)
{
	struct mmio *bar0 = &tt->dev.bar0;
	struct mmio *bar2 = &tt->dev.bar2;

	pr_info("%s\n", __func__);

	if (bar0->virt) {
		iounmap(bar0->virt);
		bar0->virt = 0;
	}
	if (bar2->virt) {
		iounmap(bar2->virt);
		bar2->virt = 0;
	}
	pci_release_regions(pdev);
	pci_disable_device(pdev);
}

static struct file_operations tlptap_fops = {
	.owner        = THIS_MODULE,
	.read         = tlptap_read,
	.write        = tlptap_write,
//	.poll         = tlptap_poll,
//	.compat_ioctl = tlptap_ioctl,
	.open         = tlptap_open,
	.release      = tlptap_release,
};

static struct miscdevice tlptap_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DRV_NAME,
	.fops = &tlptap_fops,
};

DEFINE_PCI_DEVICE_TABLE(tlptap_pci_tbl) = {
	{0x3776, 0x8032, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{0,}
};
MODULE_DEVICE_TABLE(pci, tlptap_pci_tbl);

struct pci_driver tlptap_pci_driver = {
	.name = DRV_NAME,
	.id_table = tlptap_pci_tbl,
	.probe = tlptap_pci_init,
	.remove = tlptap_pci_remove,
	//	.suspend = tlptap_suspend,
	//	.resume = tlptap_resume,
};

static int __init tt_init(void)
{
	int rc = 0;

	pr_info("tlptap (v%s) is loaded\n", WRMOD_VERSION);

	tt = kmalloc(sizeof(struct tt_dev), GFP_KERNEL);
	if (tt == 0) {
		pr_err("fail to kmalloc: *tt_dev\n");
		rc = -1;
		goto error;
	}

	rc = misc_register(&tlptap_dev);
	if (rc) {
		pr_err("fail to misc_register (MISC_DYNAMIC_MINOR)\n");
		rc = -1;
		goto error;
	}

	return pci_register_driver(&tlptap_pci_driver);

error:
	kfree(tt);
	tt = NULL;
	return rc;
}
module_init(tt_init);


static void __exit tt_release(void)
{
	pr_info("tlptap (v%s) is unloaded\n", WRMOD_VERSION);

	misc_deregister(&tlptap_dev);
	pci_unregister_driver(&tlptap_pci_driver);

	kfree(tt);
	tt = NULL;

	return;
}
module_exit(tt_release);


MODULE_AUTHOR("Yohei Kuga <sora@haeena.net>");
MODULE_DESCRIPTION("tlptap");
MODULE_LICENSE("GPL");
MODULE_VERSION(WRMOD_VERSION);

