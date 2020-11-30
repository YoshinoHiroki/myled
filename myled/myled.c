// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Hiroki Yoshino. All rights reserved.
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>

MODULE_AUTHOR("Ryuichi Ueda & Hiroki Yoshino");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

static ssize_t led_write(struct file* flip, const char* buf, size_t count, loff_t* pos)
{
	char c;
	int i;
	if(copy_from_user(&c, buf, sizeof(char)))
		return -EFAULT;

//	printk(KERN_INFO "receive %c\n", c);

	if(c == '0'){
		gpio_base[10] = 1 << 25;
	}else if(c == '1'){
		gpio_base[7] = 1 << 25;
	}else if(c == '2'){
		gpio_base[10] = 1 << 24;
	}else if(c == '3'){
		gpio_base[7] = 1 << 24;
	}else if(c == '4'){
		for(i=0;i<3;i++){
		gpio_base[7] = 1 << 24;
		ssleep(1);
		gpio_base[10] = 1 << 24;
		gpio_base[7] = 1 << 25;
		ssleep(1);
		gpio_base[10] = 1 << 25;
	}
	}else if(c == '5'){
		for(i=0;i<3;i++){
		gpio_base[7] = 1 << 24;
		ssleep(1);
		gpio_base[10] = 1 << 24;
		ssleep(1);
		}
		gpio_base[7] = 1 << 25;
	}	
	
	
	
	return 1;
}

static ssize_t sushi_read(struct file* flip, char* buf, size_t count, loff_t* pos)
{
	int size = 0;
	char sushi[] = {0xf0, 0x9f, 0x80, 0xA3, 0x0a};
	if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){
		printk(KERN_ERR "sushi: copy_to_user failed \n");
		return -EFAULT;
	}
	size += sizeof(sushi);
	return size;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.read = sushi_read
};		


static int __init init_mod(void)
{
	int retval;
	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_reginon failed. \n");
		return retval;
	}

	printk(KERN_INFO "%s is loaded. major:%d\n", __FILE__, MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d monor:%d\n", MAJOR(dev), MINOR(dev));
		return retval;
	}

	cls = class_create(THIS_MODULE, "myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class create failed.");
		return PTR_ERR(cls);
	}

	device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));

	gpio_base = ioremap_nocache(0xfe200000, 0xA0);

	const u32 led = 25;
	const u32 index = led/10;
	const u32 shift = (led%10)*3;
	const u32 mask = ~(0x7<<shift);
	gpio_base[index] = (gpio_base[index] & mask ) | (0x1 << shift);

	const u32 led2 = 24;
	const u32 index2 = led2/10;
	const u32 shift2 = (led2%10)*3;
	const u32 mask2 = ~(0x7<<shift2);
	gpio_base[index2] = (gpio_base[index2] & mask2 ) | (0x1 << shift2);


	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n", __FILE__, MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);

