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
#define DESCRIPTION "KAU EMBEDDED SYSTEMS LAB HEX5_4 Driver "
#define HEX5_4_MAJOR 0 
#define HEX5_4_NAME "HEX5_4"
#define HEX5_4_MODULE_VERSION "HEX5_4 v0.1" 


static int HEX5_4_major = 0; 
static volatile int *HEX5_4_ptr; //virtual address for the HEX display port

int HEX5_4_open(struct inode *minode, struct file *mfile){ 
	HEX5_4_ptr = ioremap_nocache (LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + HEX5_HEX4_BASE;
	printk(KERN_INFO "[HEX5_4_open]\n");
	return 0;

}



int HEX5_4_release(struct inode *minode, struct file *mfile){
	printk(KERN_INFO "[HEX5_4_release]\n");
	return 0;
}




ssize_t HEX5_4_write_byte(struct file *inode, const char *gdata, size_t length, loff_t *off_what){
	unsigned char a,b,c,d; //char = 1byte = 8bit size 
	unsigned int hex; 
	get_user(a,gdata); //copy gdata -> a (user space to kernel space)

	gdata++; 
	get_user(b,gdata); 
	gdata++;
	get_user(c,gdata);
	gdata++;
	get_user(d,gdata);
	printk(KERN_INFO "[HEX5_4 write a b c d] %d %d %d %d\n",a,b,c,d);

	hex = d;
	hex = hex << 8;
	hex = hex | c ;
	hex = hex << 8;
	hex = hex | b;
	hex = hex << 8;
	hex = hex | a;
	*HEX5_4_ptr = hex; 
	printk(KERN_INFO "[HEX5_4_write_byte] %d\n",hex); 
	return 4; 
}



static struct file_operations HEX5_4_fops = { 
	.owner = THIS_MODULE, 
	.write = HEX5_4_write_byte,
	.open = HEX5_4_open, // call-back functions for the file IOs.
	.release = HEX5_4_release,
};


int HEX5_4_init(void){
	int result = register_chrdev(HEX5_4_MAJOR, HEX5_4_NAME, &HEX5_4_fops); 
	if(result<0){
		printk(KERN_WARNING "Can't get any major\n");
		return result;
	}
	HEX5_4_major = result;
	printk(KERN_INFO "[HEX5_4_init] major number : %d\n", result);
	return 0;
}
 


void HEX5_4_exit(void){
	printk(KERN_INFO "[HEX5_4_exit]\n");
	unregister_chrdev(HEX5_4_major, HEX5_4_NAME);
}


module_init(HEX5_4_init);
module_exit(HEX5_4_exit);
MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);


