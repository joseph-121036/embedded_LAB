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
#define DESCRIPTION "KAU EMBEDDED SYSTEMS LAB KEY Driver "
#define KEY_MAJOR 0 
#define KEY_NAME "KEY"
#define KEY_MODULE_VERSION "KEY v0.1"

static int KEY_major = 0; 
static volatile int *KEY_ptr; //virtual address for the HEX display port

int KEY_open(struct inode *minode, struct file *mfile){ 
	KEY_ptr = ioremap_nocache (LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + KEY_BASE;
	printk(KERN_INFO "[KEY_open]\n");
	return 0;
}



int KEY_release(struct inode *minode, struct file *mfile){
	printk(KERN_INFO "[KEY_release]\n"); 
	return 0;
}


//key read
ssize_t KEY_read_byte(struct file *inode, const char *gdata, size_t length, loff_t *off_what){
	int c; 
	c = *KEY_ptr & 0xF; 
	put_user(c,gdata); //kernel space -> user space (c : value to copy to user space)
	printk(KERN_INFO "[KEY_read_byte] %d\n",c);
	return length;
} 


static struct file_operations KEY_fops = { 
	.owner = THIS_MODULE, 
	.read = KEY_read_byte,
	.open = KEY_open, // call-back functions for the file IOs.
	.release = KEY_release,
};



int KEY_init(void){
	int result = register_chrdev(KEY_MAJOR, KEY_NAME, &KEY_fops);
	if(result<0){
	printk(KERN_WARNING "Can't get any major\n");
	return result;
	}
	KEY_major = result;
	printk(KERN_INFO "[KEY_init] major number : %d\n", result);
	return 0;
}


void KEY_exit(void){
	printk(KERN_INFO "[KEY_exit]\n");
	unregister_chrdev(KEY_major, KEY_NAME); 
}


module_init(KEY_init);
module_exit(KEY_exit);
MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);




