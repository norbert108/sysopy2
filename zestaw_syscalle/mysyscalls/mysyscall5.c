#include "mysyscall.h"

#include <linux/sched.h>
#include <linux/proc_fs.h>


asmlinkage long sys_mysyscall_5(void)
{
/*    struct task_struct *task_list;

    char buf[10000];
    int len = 0;

    for_each_process(task_list) {
        len  += sprintf(buf+len, "\n %s %d \n",task_list->comm,task_list->pid);
    }

    printk (KERN_EMERG "%s\n", buf);
*/
    return 0;
}
