#!/bin/bash
#Download kernel and ramdisk and sets them up to be copied to /boot on sd card
#required u-boot to be INSTALLLED, since we need to use mkimage

#SITE=http://ftp.debian.org/debian/
SITE=http://mirror.cse.unsw.edu.au/pub/debian/
SITEPATH=dists/stable/main/installer-armel/current/images/versatile/netboot/
RAMDISK="initrd.gz"
RAMDISKPATH=$SITE$SITEPATH$RAMDISK
BUILD_DIR=../../build
URAMDISK=uInitrd

echo "Downloading files....." 
wget -c $RAMDISKPATH 

#convert ramdisk to u-boot loadable file
mkimage -A arm -O linux -T ramdisk -C gzip -n initramfs -d ./$RAMDISK ./$URAMDISK

#copy to build dir
cp -v $URAMDISK $BUILD_DIR

#cleanup
#rm -v $RAMDISK $URAMDISK
echo "Done."
