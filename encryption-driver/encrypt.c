//	File:	encrypt.c
//  Author:	Michael Bruno
//	Descr:	Device driver for ROT-47 encryption.

#include <linux/string.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/slab.h> 
#include <linux/fs.h> 
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>

#define MAX_STR 40

#define MAJOR_NUM 62
#define NAME "encrypt"

#define ROT_MIN '!'
#define ROT_MAX '~'
#define ROT_SIZE (ROT_MAX - ROT_MIN + 1)
#define ROT_HALF (ROT_SIZE / 2)


void print(char *msg) {
	printk(KERN_ALERT "%s: %s", NAME, msg);
}

char *p;
char *strdup(const char *s) {
	p = kmalloc(strlen(s) + 1, GFP_KERNEL);

	if (p)
		strcpy(p, s);

	return p;
 }

char *rotate_encrypt(char *value) {
	char *rv = strdup(value);

	size_t i, len = strlen(rv);

	for (i = 0; i < len; ++i) {
		int ch;

		if (rv[i] < ROT_MIN || rv[i] > ROT_MAX)
			continue;

		ch = rv[i] + ROT_HALF;
		if (ch > ROT_MAX) {
			rv[i] = ch - ROT_SIZE;
		} else {
			rv[i] = ch;
		}
	}
	return rv;
}


MODULE_LICENSE("Dual BSD/GPL");

int encrypt_open(struct inode *inode, struct file *filp);
int encrypt_release(struct inode *inode, struct file *filp);
ssize_t encrypt_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t encrypt_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void encrypt_exit(void);
int encrypt_init(void);


struct file_operations encrypt_fops = {
  read: encrypt_read,
  write: encrypt_write,
  open: encrypt_open,
  release: encrypt_release
};

module_init(encrypt_init);
module_exit(encrypt_exit);


int encrypt_init(void) {
	int result = register_chrdev(MAJOR_NUM, NAME, &encrypt_fops);

	if (result < 0) {
		print("cannot obtain major number.\n");
		return result;
	}

	print("module installed.\n");

	return 0;
}


void encrypt_exit(void) {
  unregister_chrdev(MAJOR_NUM, NAME);
  print("removing module...\n");

  if (p)
  	kfree(p);
 
  print("module removed.\n");
}


int encrypt_open(struct inode *inode, struct file *filp) {
  print("opening module...\n");
  return 0;
}


int encrypt_release(struct inode *inode, struct file *filp) {
	return 0;
}


ssize_t encrypt_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
  int datasize = strlen(filp->private_data);
  int available_to_read;

  available_to_read = datasize - *f_pos;
  printk(KERN_ALERT "%s: count = %ld, available_to_read = %d\n", NAME, count, available_to_read);

  if (available_to_read <= 0) 
	return 0;
	  
  if (count > available_to_read) {
    count = available_to_read;
  }

  copy_to_user(buf,filp->private_data + *f_pos,count);

  *f_pos += count;

  return count; 
}


ssize_t encrypt_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
	char data[MAX_STR];
	char *edat;
        
	if (count > sizeof(data)) { 
		count = sizeof(data);
	}

	printk(KERN_ALERT "%s: count = %ld\n", NAME, count);

	copy_from_user(data,buf,count);

	edat = rotate_encrypt(data);

	filp->private_data = edat;

	*f_pos = 0;

	return count;
}