#!/bin/bash
#installs bootloaders, kernel, ramfs into the boot partition

PWD=`pwd`
BUILD_DIR=../../build

#determine & mount boot partition
dmesg | tail
echo -n "Enter the partition for the fat32 on the SD card (without /dev): "
read fat32
sudo mkdir -p /mnt/fat32
sudo mount /dev/$fat32 /mnt/fat32

cd $BUILD_DIR
echo "changing directory to " `pwd`

#copy x-loader, uboot
echo -n "do you want to copy bootloaders Y/n? (default is n):"
read doCopyBL

if [$doCopyBL = "Y"]; then
	ls -l
	echo -n "Enter the name of x-loader binary: "
	read mlo
#	sudo cp -v $mlo /mnt/fat32/MLO
	echo -n "Enter the name of u-boot binary:"
	read uboot
#	sudo cp -v $uboot /mnt/fat32/u-boot.bin
else
	echo "Not copying bootloader."
fi

#copy kernel + ramfs
sudo cp -v uImage uInitrd /mnt/fat32

#unmount, sync, etc
cd $PWD
sync
echo "waiting for sync to complete...."
sleep 5
sudo umount /mnt/fat32
