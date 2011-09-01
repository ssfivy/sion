#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#include "packet.h"
#include "socket.h"
#include "input.h"
#include "output.h"

int 
main(int argc, char **argv)
{
	char	*localip;
	int	localport;
	char*	remoteip;
	int	remoteport;
	int     iteration=0;
	int     booger=0;

	union pkt_any out;
	union pkt_any in;

	if (argc != 5) {
		printf("usage: dummysender <local ip> <local port> "
		       "<remote ip> <remote port>\n");
		return 0;
	}

	localip = argv[1];
	localport = atoi(argv[2]);
	remoteip = argv[3];
	remoteport = atoi(argv[4]);
	printf("using %s:%d -> %s:%d\n", localip, localport, remoteip, remoteport);

	if (socket_init(localip, localport, remoteip, remoteport))
		return -1;

	input_init();
	output_init();

	for(;;){
		printf("main loop iteration %d...\n", iteration++);

		out.sample.type = PKT_SAMPLE;
		out.sample.can.id = (0<<18) | (1 << 10) | 1;
		out.sample.can.data[0] = 0;
		out.sample.can.data[1] = 1;
		out.sample.can.data[2] = 2;
		out.sample.can.data[3] = 3;
		out.sample.can.data[4] = 4;
		out.sample.can.data[5] = 5;
		out.sample.can.data[6] = 6;
		out.sample.can.data[7] = 7;
		out.sample.can.length = 8;

		/* simulate slow can / fast net */
		if((booger++)%10==0)
			output_queue(&out);
		
		while(input_dequeue(&in)==0){
		  printf("Recieved "
				"packet_type=%d "
				"packet_id=%d "
				"can_id=%x "
				"can.data=[%x %x %x %x %x %x %x %x] "
				"can_length=%d\n",
				in.sample.type,
				in.sample.packet_id,
				in.sample.can.id & 0x1FFFFFFF,
				in.sample.can.data[0],
				in.sample.can.data[1],
				in.sample.can.data[2],
				in.sample.can.data[3],
				in.sample.can.data[4],
				in.sample.can.data[5],
				in.sample.can.data[6],
				in.sample.can.data[7],
				in.sample.can.length
				);
		}

		output_send();
		input_recv();

		usleep(100000);
	}

	return 0;
}
