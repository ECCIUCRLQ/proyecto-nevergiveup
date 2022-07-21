// kernel
#include <linux/kernel.h>
// pci driver
#include <linux/module.h>
#include <linux/pci.h>
// character device
#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

/***************************************************************************************************************/
/* Constants */
/***************************************************************************************************************/
#define PCI_CAPTURE_DRIVER "pci_capture_driver"             // name of PCI driver
#define PCI_CAPTURE_DRIVER_CHR_DEV "pci_capture_chr_dev"    // name of character device driver
#define MAJOR_NUMBER 0                                      // major number for character device
#define MAX_CHR_DEV 1                                       // amount of character devices
#define MAX_BUFFER_DATA_LEN 30                              // max length of buffer

// PCI device offsets
#define PCI_DEVICE_BUFFER_OFFSET 0x18                       // offset of buffer registers in PCI device
#define PCI_DEVICE_COMMAND_OFFSET 0x14                      // offset of command register
#define PCI_DEVICE_WIDTH_OFFSET 0x2a 
#define PCI_DEVICE_HEIGHT_OFFSET 0x2e 
#define PCI_DEVICE_ACCESS_BUFFER_OFFSET 0x36
#define PCI_DEVICE_VALUE_BUFFER_OFFSET 0x3a 

/* IOCTL */
#define WR_VALUE _IOW('a', 'a', int32_t *)
#define WRITE_ACCESS_BUFFER _IOW('h', 'h', int32_t *)
#define RD_VALUE _IOR('b', 'b', int32_t *)
#define READ_VALUE_FROM_PCI_DEVICE _IOR('c', 'c', int32_t *)
#define LOAD_PICTURE _IOR('d', 'd', int32_t *)
#define READ_WIDTH_FROM_PCI_DEVICE _IOR('e', 'e', int32_t *)
#define READ_HEIGHT_FROM_PCI_DEVICE _IOR('f', 'f', int32_t *)
#define READ_BUFFER _IOR('g', 'g', int32_t *)
#define READ_VALUE_BUFFER_FROM_PCI_DEVICE _IOR('i', 'i', int32_t *)

/***************************************************************************************************************/
/* Driver functions */
/***************************************************************************************************************/

/* Generic functions to install/uninstall driver */
static int __init init_pci_capture_driver(void);
static void __exit finalize_pci_capture_driver(void);

/***************************************************************************************************************/
/* PCI driver */
/***************************************************************************************************************/

/* Functions for register/unregister PCI-Express driver */
static int register_pci_capture_device_driver(void);
static void unregister_pci_capture_device_driver(void);

/* Functions for install/uninstall PCI driver */
static int probe_pci_capture_driver(struct pci_dev *pdev, const struct pci_device_id *ent);
static void remove_pci_capture_driver(struct pci_dev *pdev);

/* This driver supports device with Vendor ID = 0x104C, and Product ID = 0xAC10 */
static struct pci_device_id pci_capture_driver_table[] = {
    {PCI_DEVICE(0x104C, 0xAC10)},
    {0,0}
};

/* PCI-Express Driver registration structure */
static struct pci_driver pci_capture_driver_registration = {
    .name = PCI_CAPTURE_DRIVER,
    .id_table = pci_capture_driver_table,
    .probe = probe_pci_capture_driver,
    .remove = remove_pci_capture_driver
};

/***************************************************************************************************************/
/* Character Device driver */
/***************************************************************************************************************/

/* Functions for install/uninstall character device driver */
static int register_pci_capture_chr_dev(void);
static void unregister_pci_capture_chr_dev(void);

/* Required functions for character device interaction */
static int open_pci_capture_chr_dev(struct inode *pinode, struct file *pfile);
static int close_pci_capture_chr_dev(struct inode *pinode, struct file *pfile);
static ssize_t read_pci_capture_chr_dev(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
static ssize_t write_pci_capture_chr_dev(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
static long ioctl_pci_capture_chr_dev(struct file *file, unsigned int cmd, unsigned long arg);
static int uevent_pci_capture_chr_dev(struct device *dev, struct kobj_uevent_env *env);

/* Character device registration structure */
static struct file_operations pci_capture_chr_dev_registration = {
    .owner = THIS_MODULE,
    .open = open_pci_capture_chr_dev,
    .release = close_pci_capture_chr_dev,
    .read = read_pci_capture_chr_dev,
    .write = write_pci_capture_chr_dev,
    .unlocked_ioctl = ioctl_pci_capture_chr_dev,
};

/***************************************************************************************************************/
/* Internal/Private driver functions */
/***************************************************************************************************************/
// here you can add whatever function you need
// for example
u8 read_device_at_offset(uint32_t offset);
void write_command_to_pci_device(uint32_t command);
void write_access_buffer_to_pci_device(uint32_t command);
int32_t read_width_to_pci_device(void);
int32_t read_height_to_pci_device(void);
int32_t read_value_buffer_to_pci_device(void);

/***************************************************************************************************************/
/* Global variables */
/***************************************************************************************************************/

struct pci_dev *pci_dev;

/* This is a "private" data structure */
/* You can store there any data that should be passed between driver's functions */
struct pci_driver_internal_data {
    u8 __iomem *hwmem;
};

struct character_device_internal_data {
    struct cdev cdev;
};

static int dev_major = MAJOR_NUMBER;
static struct class *character_device_class = NULL;
static struct character_device_internal_data chr_dev_data[MAX_CHR_DEV];
int32_t ioctl_buffer;
/***************************************************************************************************************/
/* Functions definitions */
/***************************************************************************************************************/

/* Register drivers */
static int __init init_pci_capture_driver(void) {
    int error = 0;

    /* Try register PCI-Express driver */
    error = register_pci_capture_device_driver();
    if (error != 0) {
        printk("Cannot register PCI-Express driver for PCI_CAPTURE device");
        goto cannot_enable;
    }

    /* Try register character device driver */
    error = register_pci_capture_chr_dev();
    if (error < 0) {
        printk("Cannot register character device for PCI_CAPTURE device");
        goto chr_dev_failed;
    }

    /* If everything is OK */
    return 0;

    /* If something fails */
    chr_dev_failed:
    unregister_pci_capture_device_driver();

    cannot_enable:
    return error;
}

/* Unregister drivers */
static void __exit finalize_pci_capture_driver(void) {
    unregister_pci_capture_chr_dev();
    unregister_pci_capture_device_driver();
}

static int register_pci_capture_device_driver(void) {
    /* Register new PCI-Express driver on the system */
    return pci_register_driver(&pci_capture_driver_registration);
}

static int register_pci_capture_chr_dev(void) {
    /* Register new character device on the system */
    int error, i;
    dev_t dev;
    const char *chr_dev_name = PCI_CAPTURE_DRIVER_CHR_DEV;

    error = alloc_chrdev_region(&dev, 0, MAX_CHR_DEV, chr_dev_name);
    
    dev_major = MAJOR(dev);
    
    character_device_class = class_create(THIS_MODULE, chr_dev_name);
    character_device_class->dev_uevent = uevent_pci_capture_chr_dev;

    for (i = 0; i < MAX_CHR_DEV; i++) {
        cdev_init(&chr_dev_data[i].cdev, &pci_capture_chr_dev_registration);
        chr_dev_data[i].cdev.owner = THIS_MODULE;

        cdev_add(&chr_dev_data[i].cdev, MKDEV(dev_major, i), 1);

        device_create(character_device_class, NULL, MKDEV(dev_major, i), NULL, "pci_capture_chr_dev-%d", i);
    }

    return 0;
}

static void unregister_pci_capture_device_driver(void) {
    /* Unregister PCI-Express driver */
    pci_unregister_driver(&pci_capture_driver_registration);
}

static void unregister_pci_capture_chr_dev(void) {
    /* Unregister character device */
    int i;

    for (i = 0; i < MAX_CHR_DEV; i++) {
        device_destroy(character_device_class, MKDEV(dev_major, i));
    }

    class_unregister(character_device_class);
    class_destroy(character_device_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
}

/* test function */
/* Write some data to the device */
void write_sample_data(struct pci_dev *pdev)
{
    int data_to_write = 0xDEADBEEF; /* Just a random trash */
    struct pci_driver_internal_data *pci_capture_data = (struct pci_driver_internal_data *) pci_get_drvdata(pdev);
    
    u32 data_read = (u32) ioread32(pci_capture_data->hwmem + 0x4);

    if (!pci_capture_data) {
        printk(" >> test: writing some values. pci_driver_internal_data error");
        return;
    }

    /* Write 32-bit data to the device memory */
    printk(" >>> write_sample_data to BAR0 @ 0x0");
    iowrite32(data_to_write, pci_capture_data->hwmem);

    printk(" >>> read BAR0 @ 0x18");
    ioread8(pci_capture_data->hwmem + 0x4);

    printk(" >>> read BAR0 @ 0x%x", 0x18 + data_read - 1);
    ioread8(pci_capture_data->hwmem + 0x18 + data_read - 1);
}

/***************************************************************************************************************/
/* Function for enabling PCI-Express driver */
/***************************************************************************************************************/
static int probe_pci_capture_driver(struct pci_dev *pdev, const struct pci_device_id *ent) {
    int error;
    u16 vendor, device;
    unsigned long mmio_start, mmio_len;
    struct pci_driver_internal_data *pci_capture_data;

    /* Let's read data from the PCI device configuration registers */
    pci_read_config_word(pdev, PCI_VENDOR_ID, &vendor);
    pci_read_config_word(pdev, PCI_DEVICE_ID, &device);

    printk(KERN_INFO "Device vid: 0x%X pid: 0x%X\n", vendor, device);

    /* Enable PCI-Express device */
    error = pci_enable_device(pdev);
    if (error != 0) {
        printk("Failed while enabling PCI-Express device. Error: %d\n", error);
        goto disable_chr_dev;
    }

    /* Request memory region for the BAR */
    error = pci_request_regions(pdev, PCI_CAPTURE_DRIVER);
    if (error != 0) {
        printk("Failed while requesting BAR regions PCI-Express device. Error: %d\n", error);
        goto disable_pci_device;
    }

    if( !(pci_resource_flags(pdev,0) & IORESOURCE_MEM) ) {
        printk(KERN_ERR "Incorrect BAR configuration. Error: %d\n", error);
        goto disable_pci_device;
    }

    /* Get start and stop memory offsets for BAR0 */
    mmio_start = pci_resource_start(pdev, 0);
    mmio_len = pci_resource_len(pdev, 0);

    printk("PCI_CAPTURE device BAR0: start at 0x%lx with lenght %lu\n", mmio_start, mmio_len);

    /* Allocate memory for the driver internal data */
    pci_capture_data = kzalloc(sizeof(struct pci_driver_internal_data), GFP_KERNEL);
    if (!pci_capture_data) {
        error = -ENOMEM;
        printk("Cannot alloc memory for internal data. Error: %d\n", error);
        goto release_regions;
    }

    /* Remap BAR0 to the local pointer */
    pci_capture_data->hwmem = ioremap(mmio_start, mmio_len);
    if (!pci_capture_data->hwmem)
    {
        error = -EIO;
        printk("Failed while remapping BAR0. Error: %d\n", error);
        goto release_regions;
    }

    /* Set driver private data */
    /* Now we can access mapped "hwmem" from any driver's function */
    pci_set_drvdata(pdev, pci_capture_data);

    write_sample_data(pdev);
    pci_dev = pdev;

    return 0;

    release_regions:
    pci_release_regions(pdev);

    disable_pci_device:
    pci_disable_device(pdev);

    disable_chr_dev:
    unregister_pci_capture_chr_dev();
    return error;
}

/***************************************************************************************************************/
/* Function for disabling PCI-Express driver */
/***************************************************************************************************************/
static void remove_pci_capture_driver(struct pci_dev *pdev) {
    struct pci_driver_internal_data *pci_capture_data;
    pci_capture_data = pci_get_drvdata(pdev);

    if (pci_capture_data) {
        if (pci_capture_data->hwmem) {
            iounmap(pci_capture_data->hwmem);
        }

        kfree(pci_capture_data);
    }

    pci_release_regions(pdev);
    pci_disable_device(pdev);
}

/***************************************************************************************************************/
/* Character device file operations */
/***************************************************************************************************************/
static int open_pci_capture_chr_dev(struct inode *pinode, struct file *pfile) {
    printk("Character device file opened\n");
    return 0;
}

static int close_pci_capture_chr_dev(struct inode *pinode, struct file *pfile) {
    printk("Character device file closed\n");
    return 0;
}

static ssize_t read_pci_capture_chr_dev(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
    char *data = "Hello from the kernel world!\n";
    size_t datalen = strlen(data);
    
    printk("Reading character device: %d\n", MINOR(pfile->f_path.dentry->d_inode->i_rdev));

    if (length > datalen) {
        length = datalen;
    }

    if (copy_to_user(buffer, data, length)) {
        return -EFAULT;
    }

    return length;
}

static ssize_t write_pci_capture_chr_dev(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {
    size_t n_copied;
    size_t max_data_len = MAX_BUFFER_DATA_LEN;
    char data_buf[MAX_BUFFER_DATA_LEN];

    printk("Writing character device: %d\n", MINOR(pfile->f_path.dentry->d_inode->i_rdev));

    if (length < max_data_len) {
        max_data_len = length;
    }

    n_copied = copy_from_user(data_buf, buffer, max_data_len);

    if (n_copied == 0) {
        printk("Copied %zd bytes from the user\n", max_data_len);
    }
    else {
        printk("Couldn't copy %zd bytes from the user\n", n_copied);
    }

    data_buf[max_data_len] = 0;

    printk("Data from the user: %s\n", data_buf);

    return length;
}

static long ioctl_pci_capture_chr_dev(struct file *file, unsigned int cmd, unsigned long arg) {
    int error;
    //int32_t array = kmalloc(5000, GFP_USER);

    switch (cmd) {
        case WR_VALUE:
            error = copy_from_user(&ioctl_buffer, (int32_t *) arg, sizeof(ioctl_buffer));
            if (error != 0) {
                printk("IOCTL write data failed. Error: %d\n", error);
            }
            else {
                printk("IOCTL data received: %d\n", ioctl_buffer);
            }
            break;

        case RD_VALUE:
            error = copy_to_user((int32_t *) arg, &ioctl_buffer, sizeof(ioctl_buffer));
            if (error != 0) {
                printk("IOCTL failed while sending data to user. Error: %d\n", error);
            }
            break;
        
        case READ_VALUE_FROM_PCI_DEVICE:
            ioctl_buffer = (int32_t) read_device_at_offset(ioctl_buffer);
            error = copy_to_user((int32_t *) arg, &ioctl_buffer, sizeof(ioctl_buffer));
            if (error != 0) {
                printk("IOCTL failed while sending data to user. Error: %d\n", error);
            }
            break;

        case LOAD_PICTURE:
            write_command_to_pci_device(1);
            error = 0;
            break;

        case READ_WIDTH_FROM_PCI_DEVICE:
            ioctl_buffer = (int32_t) read_width_to_pci_device();
            error = copy_to_user((int32_t *) arg, &ioctl_buffer, sizeof(ioctl_buffer));
            if (error != 0) {
                printk("IOCTL failed while sending data to user. Error: %d\n", error);
            }
            break;
        case READ_HEIGHT_FROM_PCI_DEVICE:
            ioctl_buffer = (int32_t) read_height_to_pci_device();
            error = copy_to_user((int32_t *) arg, &ioctl_buffer, sizeof(ioctl_buffer));
            if (error != 0) {
                printk("IOCTL failed while sending data to user. Error: %d\n", error);
            }
            break;
        case READ_BUFFER:
            ioctl_buffer = (int32_t) read_device_at_offset(ioctl_buffer);
            error = copy_to_user((int32_t *) arg, &ioctl_buffer, sizeof(ioctl_buffer));
            if (error != 0) {
                printk("IOCTL failed while sending data to user. Error: %d\n", error);
            }
            break;
        case WRITE_ACCESS_BUFFER:
            write_access_buffer_to_pci_device(1);
            error = 0;
            break;
        case READ_VALUE_BUFFER_FROM_PCI_DEVICE:
            ioctl_buffer = (int32_t) read_value_buffer_to_pci_device();
            error = copy_to_user((int32_t *) arg, &ioctl_buffer, sizeof(ioctl_buffer));
            if (error != 0) {
                printk("IOCTL failed while sending data to user. Error: %d\n", error);
            }
            break;
        default:
            printk("IOCTL command not recognized");
            error = ENOTTY;
    }
    
    return error;
}

static int uevent_pci_capture_chr_dev(struct device *dev, struct kobj_uevent_env *env) {
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

/***************************************************************************************************************/
/* Internal/Private functions definition */
/***************************************************************************************************************/
u8 read_device_at_offset(uint32_t offset) {
    u8 ret;
    struct pci_driver_internal_data *pci_capture_data;

    pci_capture_data = (struct pci_driver_internal_data *) pci_get_drvdata(pci_dev);
    ret = ioread8(pci_capture_data->hwmem + offset);

    return ret;
}

void write_command_to_pci_device(uint32_t command) { 
    struct pci_driver_internal_data *pci_capture_data;
    pci_capture_data = (struct pci_driver_internal_data *) pci_get_drvdata(pci_dev);

    // writing command value to command register of PCI device
    iowrite32(command, pci_capture_data->hwmem + PCI_DEVICE_COMMAND_OFFSET);
}

void write_access_buffer_to_pci_device(uint32_t command) {
    struct pci_driver_internal_data *pci_capture_data;
    pci_capture_data = (struct pci_driver_internal_data *) pci_get_drvdata(pci_dev);

    // writing command value to command register of PCI device
    iowrite32(command, pci_capture_data->hwmem + PCI_DEVICE_ACCESS_BUFFER_OFFSET);
}

int32_t read_width_to_pci_device(void) {
    int32_t width;
    struct pci_driver_internal_data *pci_capture_data;
    pci_capture_data = (struct pci_driver_internal_data *) pci_get_drvdata(pci_dev);

    // writing command value to command register of PCI device
    width = ioread32(pci_capture_data->hwmem + PCI_DEVICE_WIDTH_OFFSET);
    return width;
}

int32_t read_height_to_pci_device(void) {
    int32_t height;
    struct pci_driver_internal_data *pci_capture_data;
    pci_capture_data = (struct pci_driver_internal_data *) pci_get_drvdata(pci_dev);

    // writing command value to command register of PCI device
    height = ioread32(pci_capture_data->hwmem + PCI_DEVICE_HEIGHT_OFFSET);
    return height;
}

int32_t read_value_buffer_to_pci_device(void) {
    int32_t value;
    struct pci_driver_internal_data *pci_capture_data;
    pci_capture_data = (struct pci_driver_internal_data *) pci_get_drvdata(pci_dev);

    // writing command value to command register of PCI device
    value = ioread32(pci_capture_data->hwmem + PCI_DEVICE_VALUE_BUFFER_OFFSET);
    return value;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ernesto Ulate Ramirez <ernesto.ulate.ramirez@intel.com>");
MODULE_DESCRIPTION("Test PCI driver");
MODULE_VERSION("1.0");
MODULE_DEVICE_TABLE(pci, pci_capture_driver_table);

module_init(init_pci_capture_driver);
module_exit(finalize_pci_capture_driver);
