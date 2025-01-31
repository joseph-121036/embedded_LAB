#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/ioport.h> 
#include <asm/uaccess.h> //for get_user, put_user function 
#include <asm/io.h> 
#include "address_map_arm.h" 
#define LICENSE "KAU_EMBEDDED"
#define AUTHOR "DH_SM" 
#define DESCRIPTION "KAU EMBEDDED SYSTEMS LAB LEDR Driver"

#define LEDR_MAJOR 0 
#define LEDR_NAME "LEDR"
#define LEDR_MODULE_VERSION "LEDR v0.1" 

static int LEDR_major = 0; 
static volatile int *LEDR_ptr; //virtual address for the LEDR port

int LEDR_open(struct inode *minode, struct file *mfile){ 
	LEDR_ptr = ioremap_nocache (LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + LEDR_BASE;
	printk(KERN_INFO "[LEDR_open]\n");
	return 0;
}


int LEDR_release(struct inode *minode, struct file *mfile){
	printk(KERN_INFO "[LEDR_release]\n");
	return 0;
}
/*

ssize_t LEDR_write_byte(struct file *inode, const char *gdata, size_t length, loff_t *off_what){
	unsigned short c;  
	copy_from_user(&c, gdata, sizeof(unsigned short));
	  
	*LEDR_ptr = c;
	printk(KERN_INFO "[LEDR_write_byte] %d\n", c);
	return length;
}
*/

ssize_t LEDR_write_byte(struct file *inode, const char *gdata, size_t length, loff_t *off_what){
    unsigned short c;

    if (length < sizeof(unsigned short)) {
        printk(KERN_ERR "[LEDR_write_byte] Data size is too small.\n");
        return -EINVAL; // Invalid argument error
    }

    if (copy_from_user(&c, gdata, sizeof(unsigned short)) != 0) {
        printk(KERN_ERR "[LEDR_write_byte] Failed to copy data from user space.\n");
        return -EFAULT; // Bad address error
    }

    *LEDR_ptr = c;
    printk(KERN_INFO "[LEDR_write_byte] %d\n", c);
    return length;
}



static struct file_operations LEDR_fops = { 
	.owner = THIS_MODULE, 
	.write = LEDR_write_byte,
	.open = LEDR_open, // call-back functions for the file IOs.
	.release = LEDR_release,
};


int LEDR_init(void){
	int result = register_chrdev(LEDR_MAJOR, LEDR_NAME, &LEDR_fops); 
	if(result<0){
		printk(KERN_WARNING "Can't get any major\n");
		return result;
	}
	LEDR_major = result;
	printk(KERN_INFO "[LEDR_init] major number : %d\n", result);
	return 0;
}

void LEDR_exit(void){
	printk(KERN_INFO "[LEDR_exit]\n");
	unregister_chrdev(LEDR_major, LEDR_NAME); 
}


module_init(LEDR_init);
module_exit(LEDR_exit);

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
