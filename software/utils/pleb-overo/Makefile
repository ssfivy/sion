ARCH=
CROSS_COMPILE=arm-none-linux-gnueabi-
#CROSS_COMPILE=
CC=$(CROSS_COMPILE)gcc
INCLUDE=
CFLAGS=-Wall -Wextra -Wno-long-long -g $(INCLUDE)
LIBDIRS=-Wl,-rpath=/home/root/libc/lib:/home/root/libc/usr/lib \
	-Wl,--dynamic-linker=/home/root/libc/lib/ld-linux.so.3
LIBS=
LDFLAGS=$(LIBDIRS) $(LIBS)

OUTPUTDIR=root@192.168.1.20:/home/root
BUILDDIR=.

all: sender
	scp sender $(OUTPUTDIR)/

#### sender: sender.o output.o input.o sample.o socket.o cancode.o serial.o scandal_timer.o

sender: sender.o can.o ../canbridge2/heap.o ../canbridge2/input.o ../canbridge2/microuptime.o ../canbridge2/output.o ../canbridge2/packet.o ../canbridge2/socket.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


hello: helloworld.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
canusb: canusb.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


clean:
	rm -f sender *.o ../canbridge2/*.o
