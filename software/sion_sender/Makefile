PROJECT=sion_sender
DATASOURCE=source_sion_uart
#DATASOURCE=source_usbcan
#DATASOURCE=source_log

COMMON=../common
INCLUDE=-I$(COMMON)/include
SOURCEDIR=$(COMMON)/src

CC=arm-none-linux-gnueabi-gcc
DEBUG=-g
OPTIMISATION=-O2
CFLAGS=-Wall -Wextra $(DEBUG) $(OPTIMISATION) $(INCLUDE)
LIBS=-ldl -lpthread
LDFLAGS=$(LIBDIRS) $(LIBS)

BUILDDIR=.
OUTPUTDIR=../build

#get IP addresses and shit
SION_USER="root"
SION_USER_HOME="/root"
SENDER_IP = "192.168.0.20"
SENDER_PORT = "3490"
RECEIVER_IP = "192.168.0.10"
RECEIVER_PORT = "3491"

$(PROJECT): $(PROJECT).o $(DATASOURCE).o sqlitedriver.o socket.o sqlite3.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 
	cp $@ $(OUTPUTDIR)

deploy : $(PROJECT)
	#create remote directories
	#ssh $(SION_USER)@$(SENDER_IP) mkdir /home/$(SION_USER)/sion_sender /home/$(SION_USER)/canlog
	#copy binaries
	scp $(PROJECT) $(SION_USER)@$(SENDER_IP):$(SION_USER_HOME)/sion_sender/


#how to build local .o files
%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

#special instructions to build sqlite3 from amalgamation file
sqlite3.o : $(SOURCEDIR)/sqlite3.c
	$(CC) -c $^ 

#how to build common .o files
%.o : $(SOURCEDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~
	rm -f $(PROJECT) $(OUTPUTDIR)/$(PROJECT)
