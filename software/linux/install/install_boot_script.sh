#!/bin/bash
#installs boot script into the boot partition
#useful for quickly customising kernel command line parameters

WHEREWASI=`pwd`

if [ -e ./boot.cmd ]; then
	dmesg | tail
	echo -n "Enter the partition for the fat32 on the SD card (without /dev): "
	read fat32

	sudo mkdir -p /mnt/fat32
	sudo mount /dev/$fat32 /mnt/fat32

	sudo cp -v ./boot.cmd /mnt/fat32

	cd /mnt/fat32

	sudo mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "Debian" -d ./boot.cmd ./boot.scr
	
	#don't delete boot.cmd, so we know what the boot.scr actually contains

	#makes a copy just to collect everything in build dir
	cp -v boot.scr $WHEREWASI/../../build

	cd $WHEREWASI
	sync

	echo "waiting for sync to complete...."
	sleep 5

	sudo umount /mnt/fat32
else
	echo "no boot.cmd found in " `pwd`
fi
