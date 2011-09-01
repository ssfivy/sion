setenv mpurate 500
setenv mmcroot /dev/mmcblk0p2 rw
setenv mmcrootfstype ext3 rootwait
setenv loadaddr 0x82000000
setenv bootdelay 1
setenv console ttyO2,115200n8
setenv serialtty ttyO2
#saveenv

fatload mmc 0 ${loadaddr} uImage

setenv bootargs console=${console} mpurate=${mpurate} root=${mmcroot} rootfstype=${mmcrootfstype} cpufreq.debug=7

bootm ${loadaddr}
