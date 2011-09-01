#!/bin/bash 
#grabs kernel source, unzips, adds config file and compiles kernel.

#SITE=http://www.kernel.org/pub/
SITE=http://www.linux-australia.lkams.kernel.org/pub
KERNEL=linux
KERNELVERSION=2.6.38.2
KERNELEXT=tar.bz2
KERNELPATH=linux/kernel/v2.6
BUILD_DIR=../../../build/

CONFIG=kernel_config
IMAGE=uImage
ARCH=arm

echo "Downloading linux kernel...."
wget -c $SITE/$KERNELPATH/$KERNEL-$KERNELVERSION.$KERNELEXT

echo "uncompressing kernel....."
#tar xvjf $KERNEL-$KERNELVERSION.$KERNELEXT

echo "cleaning up just in case..."
cd $KERNEL-$KERNELVERSION
make distclean

echo "loading config file...."
cp -v ../$CONFIG .config

echo "compiling kernel...."
make ARCH=$ARCH -j2 $IMAGE

echo "copying resulting file...."
cp -v arch/$ARCH/boot/$IMAGE $BUILD_DIR
cd ../

#echo "cleaning up...."
#rm -rf $KERNEL-$KERNELVERSION.$KERNELEXT
