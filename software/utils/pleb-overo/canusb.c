//can-usb standalone parser
//test file, i never played with serials on c before
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
 
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
  
volatile int STOP=FALSE; 
  
int main()
{
	/* oh god how did these end up here im not good with serial ports */
	int fd, res;
  	struct termios oldtio,newtio;
  	uint8_t inbuf[255];
  	uint8_t outbuf[255];

	uint32_t header;
	int32_t payload;
	uint32_t time;

	uint8_t priority;
	uint8_t messagetype;
	uint8_t source_address;
	uint16_t specifics;

	memset(outbuf, 0, sizeof(outbuf));
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
	if (fd <0) {perror(MODEMDEVICE); exit(-1); }




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

	while (STOP==FALSE) {       /* loop for input */
		res = read(fd,inbuf,1);
		//inbuf[res]=0;               /* aka "\0" aka NULL, so we can printf w/o buffer overflows */
		//printf("%c", inbuf[0]);
		
		if (inbuf[0] == 'C' ) {
			//printf("Delimiter found! Data is : ");
			res = read (fd,inbuf, 14);
			header = (inbuf[0] << 24) | (inbuf[1] << 16) | (inbuf[2] << 8) | (inbuf[3] << 0);
			payload = (inbuf[4] << 24) | (inbuf[5] << 16) | (inbuf[6] << 8) | (inbuf[7] << 0);
			time = (inbuf[8] << 24) | (inbuf[9] << 16) | (inbuf[10] << 8) | (inbuf[11] << 0);
			
			priority = (header >> 26) & ((1<<3)-1);
        	messagetype = (header >> 18) & ((1<<8)-1);
    	    source_address = (header >> 10) & ((1<<8)-1);
	        specifics = (header >> 0) & ((1<<10)-1);

			printf("%d\t%d\t%d\t%d\t", priority, messagetype, source_address, specifics);
			printf("%d\t", payload);
			printf("%d\n", time);
		}
		
	
	}
	tcsetattr(fd,TCSANOW,&oldtio);
	return 0;
}
