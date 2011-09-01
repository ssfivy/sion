/* source_sion_uart.c
 * lpc1768 interface to SION via uart
 */

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

#include "source.h"
#include "scandal.h"

#define BAUDRATE B115200

int fd;

int
init_source( char *sourcepath){
	/* open the can device - this loads a value into fd and made it meaningful. */
	
	/* oh god how did these end up here im not good with serial ports */
  	struct termios oldtio,newtio;
//  	uint8_t outbuf[255];
	
//	memset(outbuf, 0, sizeof(outbuf));
	fd = open(sourcepath, O_RDWR | O_NOCTTY ); 
	if (fd <0) {
		perror(sourcepath);
		printf("Error opening %s, dying ;_;\n", sourcepath);
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
	newtio.c_cc[VMIN]     = 1;   /* blocking read until x chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	/* Tells the lpc1768 to start sending CAN packets */
	// this is a wee hack as it only works once, not everytime the overo is restarted
	write(fd, "S", 1); 
	printf("SION: Successfully init serial port: %s\n", sourcepath);
	return 0;
}

int source_fd(){
	// calls fd from everywhere, since global variables does not spread across files.
	return fd;
}


int get_can_pkt(can_pkt *pkt) {
	//loads the provided can_msg pointer target with meaningful messages.
	//struct can_msg readmsg;
	uint8_t inbuf[80];
	int ret;

	sion_entry tmp;

	do { 
		do {
			ret = read(fd,inbuf,1);
			//perror("read");
			//printf("Not a delimiter: %d\n", ret);
			//return NO_MSG_ERR;
			if (inbuf[0] == SHUTDOWN_SIGNAL ) {
				return -20;
			}
		} while ( inbuf[0] != 'U' );
		//if we reach here, then we got U - start of a possible delimiter!
		ret = read(fd,inbuf, 2); //nead next two characters....
	} while ((inbuf [0] != '\n') && (inbuf[1] != '\r')); //and see if they're also delimiter characters!
	//if we're here we have an incoming packet!
	ret = read (fd,inbuf, 13);
	
	stringtoentry( inbuf, &tmp );
	entrytocan(&tmp, pkt);
	
	return 0;
}


int send_can_pkt(can_pkt *pkt) {
	//transmits a can packet on the can network. 
	uint8_t outbuf[80];
	sion_entry tmp;
	int ret;
	uint8_t header;

	cantoentry(pkt, &tmp);
	entrytostring(&tmp, outbuf);

	header = 'U';

	ret = write(fd, &header,1);
	ret += write(fd,outbuf,13);

	if(ret<0)
		return -1;
	return ret;
}
