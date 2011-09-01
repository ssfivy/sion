#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <sys/poll.h>

#include "../canbridge2/microuptime.h"

#include "../canbridge2/packet.h"
#include "../canbridge2/socket.h"
#include "../canbridge2/input.h"
#include "../canbridge2/output.h"

#include "can.h"

int 
main(int argc, char **argv)
{
	char	*localip;
	int	localport;
	char	*remoteip;
	int	remoteport;
	
	union pkt_any in;
	union pkt_any out;
	
	struct can_msg msg;

	int r;

	uint64_t time; 

	if (argc != 5) {
		printf("usage: sender <local ip> <local port> "
		       "<remote ip> <remote port>\n");
		return 0;
	}

	localip = argv[1];
	localport = atoi(argv[2]);
	remoteip = argv[3];
	remoteport = atoi(argv[4]);

	printf("using %s:%d -> %s:%d\n", localip, localport, remoteip, remoteport);

	/* Try to become a daemon */
	//	daemon(0, 0);

	init_can();

	if (socket_init(localip, localport, remoteip, remoteport))
		return -1;

	input_init();
	output_init();

	printf("Successfully completed init, entering main loop....\n");
	/* set up for polling */
	/* main loop */

	for(;;){
		r = can_get_msg(&msg);
		if(r==NO_ERR) {
			printf("Sending CAN message!\n");
			out.sample.type = PKT_SAMPLE;
			out.sample.packet_id = 0; /* overwritten later */
			out.sample.can.id = msg.id;
			out.sample.can.data[0] = msg.data[0];
			out.sample.can.data[1] = msg.data[1];
			out.sample.can.data[2] = msg.data[2];
			out.sample.can.data[3] = msg.data[3];

			/* Ugly horribly godawful hack to make data
			   come out of the PLEB with the same timestamps */ 
			/* Only works for channel messages, but that's 
			   all I care about in the middle of this race
			   at this particular point in time, eh? - Dave */
			//out.sample.can.data[4] = msg.data[4];
			//out.sample.can.data[5] = msg.data[5];
			//out.sample.can.data[6] = msg.data[6];
			//out.sample.can.data[7] = msg.data[7];

			time = microuptime();
			time /= 1000;
			printf("Time: %Ld\n", time); 

			out.sample.can.data[4] = (time >> 24) & 0xFF; 
			out.sample.can.data[5] = (time >> 16) & 0xFF; 
			out.sample.can.data[6] = (time >> 8) & 0xFF; 
			out.sample.can.data[7] = (time) & 0xFF; 

			out.sample.can.length = msg.length;

			output_queue(&out);
		}

		for(;;){
			/* empty the input queue */
			r = input_dequeue(&in);
			if(r!=0)
				break;

			if(in.null.type!=PKT_SAMPLE)
				continue;

			msg.id = in.sample.can.id & 0x1FFFFFFF;
			msg.data[0] = in.sample.can.data[0];
			msg.data[1] = in.sample.can.data[1];
			msg.data[2] = in.sample.can.data[2];
			msg.data[3] = in.sample.can.data[3];
			msg.data[4] = in.sample.can.data[4];
			msg.data[5] = in.sample.can.data[5];
			msg.data[6] = in.sample.can.data[6];
			msg.data[7] = in.sample.can.data[7];
			msg.length = in.sample.can.length;

			r = can_send_msg(&msg, 0);
			if(r!=NO_ERR){
				printf("can_send_msg failed!\n");
			}
		}

		input_recv();
		output_send();
	}
	
	return 0;
}
