/* Modified can.c to interface with the canusb.*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <strings.h>
#include <string.h>

#include "can.h"

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB0"
int fd;

void
init_can(void){
	/* open the can device - this loads a value into fd and made it meaningful. */
	
	/* oh god how did these end up here im not good with serial ports */
  	struct termios oldtio,newtio;
  	uint8_t outbuf[255];
	
	memset(outbuf, 0, sizeof(outbuf));
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
	if (fd <0) {
		perror(MODEMDEVICE);
		printf("Error opening MODEMDEVICE, dying ;_;\n");
		exit(-1);
	}

	tcgetattr(fd,&oldtio); /* save current port settings */
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 16;   /* blocking read until x chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	/* enter raw mode on the canusb*/
	//strcpy(outbuf, "rrr");
	write(fd, "rrr", 3); 
	printf("Successfully init serial port.\n");
}

int can_fd(){
	// calls fd from everywhere - wut?
	return fd;
}

uint8_t
can_get_msg(struct can_msg *msg){
	//loads the provided can_msg pointer target with meaningful messages.
	//struct can_msg readmsg;
	uint8_t inbuf[255];
	int ret;
	unsigned long i = 0;
	
	uint32_t header;
	
	
	int32_t payload;
	uint32_t time;
	uint8_t priority;
	uint8_t messagetype;
	uint8_t source_address;
	uint16_t specifics;

	do {
		ret = read(fd,inbuf,1);
		//perror("read");
		printf("Not a delimiter: %d\n", ret);
		//return NO_MSG_ERR;
	} while ( inbuf[0] != 'C' );
	
	ret = read (fd,inbuf, 14);
	header = (inbuf[0] << 24) | (inbuf[1] << 16) | (inbuf[2] << 8) | (inbuf[3] << 0);
	
	
	
	payload = (inbuf[4] << 24) | (inbuf[5] << 16) | (inbuf[6] << 8) | (inbuf[7] << 0);
	time = (inbuf[8] << 24) | (inbuf[9] << 16) | (inbuf[10] << 8) | (inbuf[11] << 0);
	
	priority = (header >> 26) & ((1<<3)-1);
	messagetype = (header >> 18) & ((1<<8)-1);
	source_address = (header >> 10) & ((1<<8)-1);
	specifics = (header >> 0) & ((1<<10)-1);

	printf("%Packet:%d\t%d\t%d\t%d\t", priority, messagetype, source_address, specifics);
	printf("%d\t", payload);
	printf("%d\n", time);

	
	msg->id = header;
	for(i=0; i<CAN_MSG_LENGTH; i++)
		msg->data[i] = inbuf[(i+4)];
	msg->length = 8;
	
	  
	return NO_ERR;
}

uint8_t
can_send_msg(struct can_msg *msg, uint8_t priority){
	//transmits a can packet on the can network. 
	/*
	struct can_msg sendmsg={0,{0,},0};
	int i, ret;

	sendmsg.id = msg->id;
	for(i=0; i<CAN_MSG_LENGTH; i++)
		sendmsg.data[i] = msg->data[i];
	sendmsg.length = msg->length;

	ret = write(fd,&sendmsg,sizeof(struct can_msg));
	if(ret<0)
		return NO_MSG_ERR;

	*/
	return NO_ERR;
}
