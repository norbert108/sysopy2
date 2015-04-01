#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

int simple_open(struct inode *inode, struct file *filp);
int simple_release(struct inode *inode, struct file *filp);
ssize_t simple_read(struct file *filp, char *user_buf, size_t count, loff_t *f_pos);
ssize_t simple_write(struct file *filp, const char *user_buf, size_t count, loff_t *f_pos);
ssize_t simple_read_proc(struct file *filp, char *user_buf, size_t count, loff_t *f_pos);
void simple_exit(void);
int simple_init(void);

/* Operations for /dev/simple */
struct file_operations simple_fops = {
    read: simple_read,
    write: simple_write,
    open: simple_open,
    release: simple_release
};

/* Operations for /proc/simple */
struct file_operations proc_fops = {
	read: simple_read_proc,
};

module_init(simple_init);
module_exit(simple_exit);
