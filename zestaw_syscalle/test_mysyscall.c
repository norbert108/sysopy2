#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <errno.h>

#define __NR_mysyscall 331

int mysyscall(uid_t* uid) {
  syscall(__NR_mysyscall);
}

int main()
{
    uid_t *uid;
    int result = mysyscall(uid);
    printf("User with the most processes: %d\n", *uid);

    return 0;
}
