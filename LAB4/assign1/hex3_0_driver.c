#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/ioport.h> 
#include <asm/uaccess.h> //for get_user, put_user function 
#include <asm/io.h> 
#include "address_map_arm.h" 
#define LICENSE "KAU_EMBEDDED"
#define AUTHOR "DH_SM" //Daehong, Somyeong
#define DESCRIPTION "KAU EMBEDDED SYSTEMS LAB HEX3_0 Driver "
#define HEX3_0_MAJOR 0 
#define HEX3_0_NAME "HEX3_0"
#define HEX3_0_MODULE_VERSION "HEX3_0 v0.1" 


static int HEX3_0_major = 0; 
static volatile int *HEX3_0_ptr; //virtual address for the HEX display port

int HEX3_0_open(struct inode *minode, struct file *mfile){ 
	HEX3_0_ptr = ioremap_nocache (LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + HEX3_HEX0_BASE;
	printk(KERN_INFO "[HEX3_0_open]\n");
	return 0;

}



int HEX3_0_release(struct inode *minode, struct file *mfile){
	printk(KERN_INFO "[HEX3_0_release]\n");
	return 0;
}




ssize_t HEX3_0_write_byte(struct file *inode, const char *gdata, size_t length, loff_t *off_what){
	unsigned char a,b,c,d; //char = 1byte = 8bit size 
	unsigned int hex; 
	get_user(a,gdata); //copy gdata -> a (user space to kernel space)

	gdata++; 
	get_user(b,gdata); 
	gdata++;
	get_user(c,gdata);
	gdata++;
	get_user(d,gdata);
	printk(KERN_INFO "[HEX3_0 write a b c d] %d %d %d %d\n",a,b,c,d);

	hex = d;
	hex = hex << 8;
	hex = hex | c ;
	hex = hex << 8;
	hex = hex | b;
	hex = hex << 8;
	hex = hex | a;
	*HEX3_0_ptr = hex; 
	printk(KERN_INFO "[HEX3_0_write_byte] %d\n",hex); 
	return 4; 
}



static struct file_operations HEX3_0_fops = { 
	.owner = THIS_MODULE, 
	.write = HEX3_0_write_byte,
	.open = HEX3_0_open, // call-back functions for the file IOs.
	.release = HEX3_0_release,
};


int HEX3_0_init(void){
	int result = register_chrdev(HEX3_0_MAJOR, HEX3_0_NAME, &HEX3_0_fops); 
	if(result<0){
		printk(KERN_WARNING "Can't get any major\n");
		return result;
	}
	HEX3_0_major = result;
	printk(KERN_INFO "[HEX3_0_init] major number : %d\n", result);
	return 0;
}
 


void HEX3_0_exit(void){
	printk(KERN_INFO "[HEX3_0_exit]\n");
	unregister_chrdev(HEX3_0_major, HEX3_0_NAME);
}


module_init(HEX3_0_init);
module_exit(HEX3_0_exit);
MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);


