#include "mysyscall.h"

#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/cred.h>

#define MAX_USERS 10


asmlinkage int sys_mysyscall_1(uid_t* uid)
{
    struct task_struct *task_list;

    struct counter_struct
    {
        int filled;
        uid_t uid;
        unsigned count;
    };

    int i = 0;
    
    struct counter_struct counter[MAX_USERS];
    for(i=0; i<MAX_USERS; i++)
    {
        counter[i].filled = 0;
        counter[i].uid = (uid_t)0;
        counter[i].count = 0;
    }
    
    for_each_process(task_list) {
        // check if process owner is in processes array, if so, increment counter
        for(i=0; i<MAX_USERS; i++)
	{
	    if(counter[i].filled == 0)
	    {
		counter[i].filled = 1;
 		counter[i].uid = task_list->real_cred->uid.val;
		counter[i].count = 1;
	    	break;
	    }

            if(counter[i].uid == task_list->real_cred->uid.val)
            {
 		counter[i].count++;
		break;
	    }
	}
    }

    uid_t max_uid = 17;
    unsigned max_count = 0;
    for(i=0; i<MAX_USERS; i++)
    {
	if(counter[i].filled == 1 && max_count < counter[i].count)
	{
	    max_count = counter[i].count;
	    max_uid = counter[i].uid;
	}
    }

    printk (KERN_EMERG "MAX UID %d MAX PROCESY %d\n", max_uid, max_count);

    return max_count;
}
