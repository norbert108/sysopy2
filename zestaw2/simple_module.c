#include "simple_module.h"

#define MYBUF_SIZE 100
#define SIMPLE_MAJOR 199

const char *text = "SIMPLE. Read calls: %d, Write calls: %d\n";

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
    if (max > MYBUF_SIZE - 1) {
		max = MYBUF_SIZE - 1;
	}
    err = copy_from_user(mybuf, user_buf, max);

    mybuf[max] = 0;

    write_count++;
    return max;
}

bool copied = false;

ssize_t simple_read_proc(struct file *filp, char *user_buf, size_t count, loff_t *f_pos) {
	char *buf;
	int length;
	int err;

	buf = kmalloc(1000, GFP_KERNEL);
	if (!buf) goto out;

	if (!copied) {
		length = snprintf(buf, 1000, text, read_count, write_count);
		if (count >= length) {
			count = length;
		}

		err = copy_to_user(user_buf, buf, count);
		if (err) {
	    	printk(KERN_WARNING "SIMPLE: error occured in simple_read_proc: %d\n", err);
			goto out;
		}
		copied = true;
	} else {
    	count = 0;
		copied = false;
	}

out:
	if (buf) {
		kfree(buf);
	}
	return count;
}
