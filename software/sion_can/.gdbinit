#load elf file; contains mapping etc
file build/sion_can.elf

#connect to openocd daemon
target remote localhost:3333

#set breakpoint limits
set remote hardware-breakpoint-limit 6
set remote hardware-watchpoint-limit 4

#disable IRQ while stepping, may fix some bugs
#define hook-step
#	mon cortex_m3 maskisr on
#end
#define hookpost-step
#	mon cortex_m3 maskisr off
#end

#do a soft reset
monitor soft_reset_halt
