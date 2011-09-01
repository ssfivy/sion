#!/bin/bash
#overwrites all default config files with the one that works for SION
#should be run on the target machine as root, not chrooted
#yay unix, only requiring file replacement for changing setup.

#who's going to run sion etc? you guessed it, the super user.
USER=root
USERHOME=/root

WHEREAMI=`pwd`

#check permissions
if [ "$(id -u)" != "0" ]; then
	echo "You are not root!" 1>&2
	exit 1
fi
echo "You are root.. proceed" 

########## Adds non-free repository
# If you're the next person working on this,
# make sure you change this to whatever debian version is there.
echo deb http://mirror.aarnet.edu.au/debian/ squeeze non-free >> /etc/apt/sources.list

##########install default packages
apt-get install vim minicom python-serial openssh-server wireless-tools lshw libertas-firmware wpasupplicant psmisc w3m

#########make directories
mkdir -pv $USERHOME/sion_sender $USERHOME/canlog
#chown -v $USER:$USER $USERHOME/sion_sender $USERHOME/canlog
chmod -v 755 $USERHOME/sion_sender $USERHOME/canlog

#ensure everything is executable before copying
chmod -v 755 *

#########place busybox in place
cp -v busybox-armv4tl $USERHOME

#########ssh
cp -v sshd_config /etc/ssh/sshd_config

#########Wireless stuff
#/etc/network/interfaces
cp -v interfaces /etc/network/interfaces

#wpa supplicant
cp -v wpa_supplicant.conf /etc/wpa_supplicant.conf

#########startup script
#inittab
cp -v inittab /etc/inittab

#start rtc
#not needed, keep default debian hwclock
#for restoring rtc
#cp -v rtc /etc/init.d/rtc
#update-rc.d rtc defaults 05

#start wireless
cp -v sion-wireless /etc/init.d/sion-wireless
cd /etc/rc2.d/
ln -s -T ../init.d/sion-wireless S25sion-wireless
cd $WHEREAMI

#start sion
cp -v sion-startup /etc/init.d/sion-startup
cd /etc/rc2.d/
ln -s -T ../init.d/sion-startup S75sion-startup
cd $WHEREAMI

#and we're done!
echo "Done setting up system, please reboot"
