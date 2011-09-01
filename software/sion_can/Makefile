#  Project stuff
PROJECT=sion_can
BUILD_DIR=.
ROOT_DIR=../..
OUTPUT_DIR=../build
COMMON=../common

#  List of the objects files to be compiled/assembled
OBJECTS  = startup_LPC17xx.o core_cm3.o system_LPC17xx.o sion_can.o
OBJECTS += lpc17xx_can.o lpc17xx_libcfg.o lpc17xx_clkpwr.o
OBJECTS += lpc17xx_pinsel.o lpc17xx_gpio.o lpc17xx_uart.o debug_frmwrk.o
OBJECTS += led.o printf.o itoa.o lpc17xx_ssp.o circbuffer.o lpc17xx_exti.o

# lpc1768 files, modified from cmsis
LPCFILES=$(ROOT_DIR)/arch/lpc1768env

# where to find CMSIS stuff (unmodified)
CMSISROOT=$(LPCFILES)/LPC1700CMSIS
CMSISCORE=$(CMSISROOT)/Core/CM3/CoreSupport
CMSISDEVICE=$(CMSISROOT)/Core/CM3/DeviceSupport/NXP/LPC17xx
CMSISDRIVERINC=$(CMSISROOT)/Drivers/include
CMSISDRIVERSRC=$(CMSISROOT)/Drivers/source


#where to find scandal stuff. Not yet used.


#linker script to use - feeds into gnu linker.
LSCRIPT=$(LPCFILES)/LPC17xx.ld

OPTIMIZATION = 0
DEBUG = -g
#LISTING = -ahls

#  Compiler Options
GCFLAGS = -Wall -fno-common -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) $(DEBUG)
#GCFLAGS += -Wcast-align -Wcast-qual -Wimplicit -Wpointer-arith -Wswitch
#GCFLAGS += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
GCFLAGS += -I. -I$(CMSISCORE) -I$(CMSISDEVICE) -I$(CMSISDRIVERINC) -I$(COMMON)/include/
LDFLAGS = -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) -nostartfiles -Wl,-Map=$(PROJECT).map -T$(LSCRIPT)
ASFLAGS = $(LISTING) -mcpu=cortex-m3

#  Compiler/Assembler/Linker Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = arm-none-eabi-size

# Programmer, JTAG or otherwise.
PROGRAM = sudo openocd
PROGRAMARGS = 	-f openocd.cfg \
				-c "reset halt" \
				-c "flash write_image erase $(OUTPUT_DIR)/$(PROJECT).bin 0x0 bin" \
				-c reset \
				-c shutdown
#flash write does auto reset, but just to be sure let's do another reset before shutdown.


#########################################################################

all:: $(PROJECT).hex $(PROJECT).bin
	cp $(PROJECT).bin $(OUTPUT_DIR)

program: all
	$(PROGRAM) $(PROGRAMARGS)

$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary -j .text -j .data $(PROJECT).elf $(PROJECT).bin

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -R .stack -O ihex $(PROJECT).elf $(PROJECT).hex

$(PROJECT).elf: $(OBJECTS)
	$(GCC) $(LDFLAGS) $(OBJECTS) -o $(PROJECT).elf

stats: $(PROJECT).elf
	$(SIZE) $(PROJECT).elf

clean:
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(PROJECT).hex
	$(REMOVE) $(PROJECT).elf
	$(REMOVE) $(PROJECT).map
	$(REMOVE) $(PROJECT).bin $(OUTPUT_DIR)/$(PROJECT).bin
	$(REMOVE) *~
	$(REMOVE) *.o *.lst

#########################################################################
#  Default rules to compile .c and .cpp file to .o
#  and assemble .s files to .o

.c.o :
	$(GCC) $(GCFLAGS) -c $<

.cpp.o :
	$(GCC) $(GCFLAGS) -c $<

.S.o :
	$(AS) $(ASFLAGS) -o $(PROJECT)_crt.o $< > $(PROJECT)_crt.lst

#########################################################################
# How to build individual
$(BUILD_DIR)/core_cm3.o : $(CMSISCORE)/core_cm3.c
	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/startup_LPC17xx.o: $(LPCFILES)/startup_LPC17xx.s
	$(AS) $(ASFLAGS) -o startup_LPC17xx.o $< > startup_LPC17xx_crt.lst

#$(BUILD_DIR)/system_LPC17xx.o : $(LPCFILES)/system_LPC17xx.c
#	$(GCC) $(GCFLAGS) -c $<

#$(BUILD_DIR)/snprintf.o : $()/snprintf.c
#	$(GCC) $(GCFLAGS) -c $<

#$(BUILD_DIR)/lpc17xx_libcfg_default.o : $(CMSISDRIVERSRC)/lpc17xx_libcfg_default.c
#	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/lpc17xx_clkpwr.o : $(CMSISDRIVERSRC)/lpc17xx_clkpwr.c
	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/lpc17xx_can.o : $(CMSISDRIVERSRC)/lpc17xx_can.c
	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/lpc17xx_pinsel.o : $(CMSISDRIVERSRC)/lpc17xx_pinsel.c
	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/lpc17xx_gpio.o : $(CMSISDRIVERSRC)/lpc17xx_gpio.c
	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/lpc17xx_uart.o : $(CMSISDRIVERSRC)/lpc17xx_uart.c
	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/debug_frmwrk.o : $(CMSISDRIVERSRC)/debug_frmwrk.c
	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/lpc17xx_ssp.o : $(CMSISDRIVERSRC)/lpc17xx_ssp.c
	$(GCC) $(GCFLAGS) -c $<

$(BUILD_DIR)/lpc17xx_exti.o : $(CMSISDRIVERSRC)/lpc17xx_exti.c
	$(GCC) $(GCFLAGS) -c $<


$(BUILD_DIR)/circbuffer.o : $(COMMON)/src/circbuffer.c
	$(GCC) $(GCFLAGS) -c $<

