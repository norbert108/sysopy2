make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
rmmod simple_module.ko
insmod simple_module.ko
