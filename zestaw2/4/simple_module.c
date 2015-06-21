#include "simple_module.h"

#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/mount.h>

#define MYBUF_SIZE 1000
#define SIMPLE_MAJOR 199

const char *text = "SIMPLE. Read calls: %d, Write calls: %d\n";

//long mybuf_size = 1024;
//long mybuf_ptr = 0;

int read_count = 0;
int write_count = 0;
char *mybuf;

struct proc_dir_entry *proc_entry = 0;

int __init simple_init(void) {
    int result;

    /* Register an entry in /proc */
    proc_entry = proc_create("simple", 0, NULL, &proc_fops);

    /* Register a device with the given major number */
    result = register_chrdev(SIMPLE_MAJOR, "simple", &simple_fops);
    if (result < 0) {
        printk(KERN_WARNING "Cannot register the /dev/simple device with major number: %d\n", SIMPLE_MAJOR);
        return result;
    }

    mybuf = kmalloc(MYBUF_SIZE, GFP_KERNEL);
    if (!mybuf) {
        result = -ENOMEM;
        simple_exit();
        return result;
    } else {
        memset(mybuf, 0, 1);
        printk(KERN_INFO "The SIMPLE module has been inserted.\n");
        return 0;
    }
}

void simple_exit(void) {
    /* Unregister the device and /proc entry */
    unregister_chrdev(SIMPLE_MAJOR, "simple");
	if (proc_entry) {
		proc_remove(proc_entry);
	}

    /* Free the buffer */
    if (mybuf) {
        kfree(mybuf);
    }

    printk(KERN_INFO "The SIMPLE module has been removed\n");
}

int simple_open(struct inode *inode, struct file *filp) {
    return 0;
}

int simple_release(struct inode *inode, struct file *filp) {
    return 0;
}

ssize_t simple_read(struct file *filp, char __user *user_buf, size_t count, loff_t *f_pos) {
    /* Move one byte to the userspace */
    // copy_to_user(void *to, void* from, int size)
    int err = copy_to_user(user_buf, mybuf, strlen(mybuf));
    if (err) {
		printk(KERN_WARNING "SIMPLE: error occured in simple_read: %d", err);
    }
    read_count++;

    if (*f_pos == 0) {
        *f_pos += strlen(mybuf);
        return strlen(mybuf);
    } else {
        return 0;
    }
}

ssize_t simple_write(struct file *filp, const char __user *user_buf, size_t count, loff_t *f_pos) {
    // copy_from_user(to, from, size)
    int max = count;
    int err;
    
    struct kstat stat;
    struct path p;

    struct vfsmount *mnt = NULL;
    struct dentry *dntr = NULL;

    if (max > MYBUF_SIZE - 1) {
		max = MYBUF_SIZE - 1;
	}
    err = copy_from_user(mybuf, user_buf, max);

    mybuf[max-1] = 0;

    err = kern_path(mybuf, 0, &p);
    
    if(err != 0){
        printk(KERN_INFO "File does not exist");
	return -ENOENT;
    }

    err = vfs_getattr(&p, &stat);
    if(err != 0) {
	printk(KERN_INFO "Cannot get file attrs");
	return -ENOENT;
    }

    mnt = p.mnt;
    if(mnt == NULL){
 	printk(KERN_INFO "File is not mounted :O");
	return -ENOENT;
    }

    dntr = mnt->mnt_root;
    if(dntr == NULL) return -ENOENT;

    snprintf(mybuf, MYBUF_SIZE, "mount point=%s\n", dntr->d_name.name);
 
    write_count++;
    return max;
}

bool copied = false;

ssize_t simple_read_proc(struct file *filp, char *user_buf, size_t count, loff_t *f_pos) {
	return simple_read(filp, user_buf, count, f_pos);       
}
