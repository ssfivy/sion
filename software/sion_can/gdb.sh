#!/bin/sh
#starts openocd in the background, then starts gdb
make #we need the elf file to be present before debug
#sudo openocd  #start 
#bg #put openocd in the background
arm-none-eabi-gdb #start gdb
