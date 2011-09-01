#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#include "packet.h"
#include "socket.h"
#include "input.h"
#include "output.h"

#include "../scandal/include/scandal_types.h"

/* HACK! FIXME! these definitions come from ../scandal/include/scandal_message.h */ 

/* Priority definitions */
#define CRITICAL_PRIORITY		0
#define NETWORK_HIGH			1
#define CONTROL_HIGH			2
#define TELEM_HIGH			3
#define CONTROL_LOW			5
#define NETWORK_LOW			6
#define TELEM_LOW			7

/* Message type definitions */
#define CHANNEL_TYPE  	                0
#define CONFIG_TYPE			1
#define HEARTBEAT_TYPE 	                2
#define SCANDAL_ERROR_TYPE		3
#define USER_ERROR_TYPE                 4
#define RESET_TYPE			5                
#define USER_CONFIG_TYPE		6 
#define COMMAND_TYPE			7

/* Fields common to all frames */
#define PRI_BITS		        3
#define PRI_OFFSET		        26
#define MSG_PRIORITY(msg)               ((msg->id >> PRI_OFFSET) & ((1<<PRI_BITS) -1))

#define TYPE_BITS		        8
#define TYPE_OFFSET		        18
#define MSG_TYPE(msg)                   ((msg->id >> TYPE_OFFSET) & ((1<<TYPE_BITS) - 1))

/* Channel messages */
#define CHANNEL_SOURCE_ADDR_BITS	8
#define CHANNEL_SOURCE_ADDR_OFFSET	10
#define CHANNEL_MSG_ADDR(msg)           ((msg->id >> CHANNEL_SOURCE_ADDR_OFFSET) &\
					 ((1<<CHANNEL_SOURCE_ADDR_BITS) - 1))

#define CHANNEL_NUM_BITS	        10
#define CHANNEL_NUM_OFFSET  	        0
#define CHANNEL_MSG_CHAN_NUM(msg)       ((msg->id >> CHANNEL_NUM_OFFSET) &\
					 ((1<<CHANNEL_NUM_BITS) - 1))

#define CHANNEL_MSG_VALUE(msg)          ((((u32)((msg)->data[0] & 0xFF)) << 24) |\
					 (((u32)((msg)->data[1] & 0xFF)) << 16) |\
					 (((u32)((msg)->data[2] & 0xFF)) << 8) |\
					 (((u32)((msg)->data[3] & 0xFF))))
#define CHANNEL_MSG_TIME(msg)           ((((u32)(msg)->data[4] & 0xFF) << 24) |\
					 (((u32)((msg)->data[5] & 0xFF) << 16) |\
					 (((u32)(msg)->data[6] & 0xFF) << 8) |\
					 (((u32)(msg)->data[7] & 0xFF) << 0)))




/* Configuration messages (both user and scandal) */
#define CONFIG_NODE_ADDR_BITS	        8
#define CONFIG_NODE_ADDR_OFFSET	        10
#define CONFIG_MSG_ADDR(msg)            ((msg->id >> CONFIG_NODE_ADDR_OFFSET) &\
					 ((1<<CONFIG_NODE_ADDR_BITS) - 1))

#define CONFIG_PARAM_BITS	        8
#define CONFIG_PARAM_OFFSET	        0
#define CONFIG_MSG_PARAM(msg)           ((msg->id >> CONFIG_PARAM_OFFSET) &\
					 ((1<<CONFIG_PARAM_BITS) - 1))



/* Heartbeat messages */
#define HEARTBEAT_NODE_ADDR_BITS	8
#define HEARTBEAT_NODE_ADDR_OFFSET	10
#define HEARTBEAT_MSG_NODE_ADDR(msg)    ((msg->id >> HEARTBEAT_NODE_ADDR_OFFSET) &\
					 ((1<<HEARTBEAT_NODE_ADDR_BITS) - 1))

#define HEARTBEAT_NODE_TYPE_BITS	10
#define HEARTBEAT_NODE_TYPE_OFFSET      0
#define HEARTBEAT_MSG_NODE_TYPE(msg)    ((msg->id >> HEARTBEAT_NODE_TYPE_OFFSET) &\
					 ((1<<HEARTBEAT_NODE_TYPE_BITS) - 1))

#define HEARTBEAT_LAST_SCANDAL_ERROR_BYTE        0
#define HEARTBEAT_MSG_LAST_SCANDAL_ERROR(msg)    (msg->data[HEARTBEAT_LAST_SCANDAL_ERROR_BYTE])

#define HEARTBEAT_LAST_USER_ERROR_BYTE           1
#define HEARTBEAT_MSG_LAST_USER_ERROR(msg)       (msg->data[HEARTBEAT_LAST_USER_ERROR_BYTE])

#define HEARTBEAT_SCVERSION_BYTE                 2
#define HEARTBEAT_MSG_SCVERSION(msg)             (msg->data[HEARTBEAT_SCVERSION_BYTE])

#define HEARTBEAT_NUMERRORS_BYTE                 3
#define HEARTBEAT_MSG_NUMERRORS(msg)             (msg->data[HEARTBEAT_NUMERRORS_BYTE])

#define HEARTBEAT_MSG_TIME(msg)     ((((u32)msg->data[4] & 0xFF) << 24) |\
					     (((u32)msg->data[5] & 0xFF) << 16) |\
					     (((u32)msg->data[6] & 0xFF) << 8) |\
					     (((u32)msg->data[7] & 0xFF) << 0))


/*
Heartbeat consists of the following:
	ID: 	3 Bits Priority
			8 Bits Message type
			8 Bits Node address
			10 Bits Node type
	Data:	8 bits last scandal error
	        8 bits last user error
		8 bits number of errors
		8 bits scandal version
		32 bits time
*/

/* Reset messages */
#define RESET_NODE_ADDR_BITS		8
#define RESET_NODE_ADDR_OFFSET		10
#define RESET_MSG_NODE_ADDR(msg)        ((msg->id >> RESET_NODE_ADDR_OFFSET) &\
					 ((1<<RESET_NODE_ADDR_BITS) - 1))


/* Scandal error messages */
#define SCANDAL_ERROR_NODE_ADDR_BITS	8
#define SCANDAL_ERROR_NODE_ADDR_OFFSET	10
#define SCANDAL_ERROR_MSG_NODE_ADDR(msg) ((msg->id >> SCANDAL_ERROR_NODE_ADDR_OFFSET) &\
					 ((1<<SCANDAL_ERROR_NODE_ADDR_BITS) - 1))


#define SCANDAL_ERROR_NODE_TYPE_BITS	10
#define SCANDAL_ERROR_NODE_TYPE_OFFSET  0
#define SCANDAL_ERROR_MSG_NODE_TYPE(msg) ((msg->id >> SCANDAL_ERROR_NODE_TYPE_OFFSET) &\
					 ((1<<SCANDAL_ERROR_NODE_TYPE_BITS) - 1))


/* User error messages */
#define USER_ERROR_NODE_ADDR_BITS	8
#define USER_ERROR_NODE_ADDR_OFFSET	10
#define USER_ERROR_MSG_NODE_ADDR(msg)   ((msg->id >> USER_ERROR_NODE_ADDR_OFFSET) &\
					 ((1<<USER_ERROR_NODE_ADDR_BITS) - 1))

#define USER_ERROR_NODE_TYPE_BITS	10
#define USER_ERROR_NODE_TYPE_OFFSET     0
#define USER_ERROR_MSG_NODE_ADDR(msg)   ((msg->id >> USER_ERROR_NODE_ADDR_OFFSET) &\
					 ((1<<USER_ERROR_NODE_ADDR_BITS) - 1))



/* Configuration parameters */
#define CONFIG_ADDR 			0	/* Data: 8 bits of address */
#define CONFIG_IN_CHAN_SOURCE 		1	/* Data: 16 bits channel number, 8 bits source node, 16 bits source num */
#define CONFIG_OUT_CHAN_M		2	/* Data: 16 bits channel number, 32 bits new M */
#define CONFIG_OUT_CHAN_B		3	/* Data: 16 bits channel number, 32 bits new B */


/* Generic utility macros */
#define FIRST_32_BITS(msg) ((((u32)((msg)->data[0] & 0xFF)) << 24) |\
					       (((u32)((msg)->data[1] & 0xFF)) << 16) |\
					       (((u32)((msg)->data[2] & 0xFF)) << 8) |\
					       (((u32)((msg)->data[3] & 0xFF)) << 0))

#define SECOND_32_BITS(msg) ((((u32)msg->data[4] & 0xFF) << 24) |\
					       (((u32)msg->data[5] & 0xFF) << 16) |\
					       (((u32)msg->data[6] & 0xFF) << 8) |\
					       (((u32)msg->data[7] & 0xFF) << 0))


/* Heartbeat message */
#define SCANDAL_HEARTBEAT_MSG_ADDR(msg)    ((msg->id >> HEARTBEAT_NODE_ADDR_OFFSET) &\
					    ((1<<HEARTBEAT_NODE_ADDR_BITS) - 1))
#define SCANDAL_HEARTBEAT_MSG_NODETYPE(msg) ((msg->id >> HEARTBEAT_NODE_TYPE_OFFSET) &\
					     ((1<<HEARTBEAT_NODE_TYPE_BITS) - 1))
#define SCANDAL_HEARTBEAT_MSG_LAST_SCANDAL_ERROR(msg) (msg->data[HEARTBEAT_LAST_SCANDAL_ERROR_BYTE])
#define SCANDAL_HEARTBEAT_MSG_LAST_USER_ERROR(msg) (msg->data[HEARTBEAT_LAST_USER_ERROR_BYTE])
#define SCANDAL_HEARTBEAT_MSG_SCVERSION(msg) (msg->data[HEARTBEAT_SCVERSION_BYTE])
#define SCANDAL_HEARTBEAT_MSG_NUMERRORS(msg) (msg->data[HEARTBEAT_NUMERRORS_BYTE])


u32 scandal_mk_channel_id(u08 priority, u08 source, u16 channel_num){
        return( ((u32)(priority & 0x07) << PRI_OFFSET) |
                ((u32)CHANNEL_TYPE << TYPE_OFFSET) |
                ((u32)(source & 0xFF) << CHANNEL_SOURCE_ADDR_OFFSET) |
                ((u32)(channel_num & 0x03FF) << CHANNEL_NUM_OFFSET));
}

u32     fake_heartbeat_id(int node, int type){
        return( ((u32)(NETWORK_LOW & 0x07) << PRI_OFFSET) |
                ((u32)HEARTBEAT_TYPE << TYPE_OFFSET) |
                ((u32)(node & 0xFF) << HEARTBEAT_NODE_ADDR_OFFSET) |
                ((u32)type << HEARTBEAT_NODE_TYPE_OFFSET));
}

int 
main(int argc, char **argv)
{
	char	*localip;
	int	localport;
	char*	remoteip;
	int	remoteport;

	FILE*   logfile; 
	char*   logfile_name; 
	struct  stat logfile_stats; 
	long    logfile_len; 

	union pkt_any out;
	union pkt_any in;

	if (argc != 6) {
		printf("usage: dummysender <local ip> <local port> "
		       "<remote ip> <remote port> <logfile name>\n");
		return 0;
	}

	localip = argv[1];
	localport = atoi(argv[2]);
	remoteip = argv[3];
	remoteport = atoi(argv[4]);
	logfile_name = argv[5];
	printf("using %s:%d -> %s:%d\n", localip, localport, remoteip, remoteport);
	printf("Sending logfile: %s\n", logfile_name); 

	logfile = fopen(logfile_name, "r"); 
	if(logfile == NULL){
		perror("fopen"); 
		fprintf(stderr, "Error opening log file for sending\n"); 
		exit(1); 
	}

	stat(logfile_name, &logfile_stats); 
	logfile_len = logfile_stats.st_size; 

	if (socket_init(localip, localport, remoteip, remoteport))
		return -1;

	input_init();
	output_init();

	while(!feof(logfile)){
		//int priority, node, channel;// value; 
		//unsigned int timestamp; 
	//	unsigned long long otherstamp; 
		
		uint32_t header;
		uint64_t payload;
		long double scanalysis_time;

		fscanf(logfile, "%x %Lx %Lf", 
		       &header, 
		       &payload, 
		       &scanalysis_time);  

		out.sample.type = PKT_SAMPLE;
		out.sample.can.id = header; 
		
		out.sample.can.data[0] = (payload >> 56) & 0xFF;
		out.sample.can.data[1] = (payload >> 48) & 0xFF;
		out.sample.can.data[2] = (payload >> 40 ) & 0xFF;
		out.sample.can.data[3] = (payload >> 32 ) & 0xFF;
		out.sample.can.data[4] = (payload >> 24) & 0xFF;
		out.sample.can.data[5] = (payload >> 16) & 0xFF;
		out.sample.can.data[6] = (payload >> 8 ) & 0xFF;
		out.sample.can.data[7] = (payload >> 0 ) & 0xFF;
		out.sample.can.length = 8;

		output_queue(&out);

	
		out.sample.type = PKT_SAMPLE;
		
		/* WHAT THE FUCK IS THIS SHIT
		switch(node){
		case 5:
		  out.sample.can.id = fake_heartbeat_id(node, 28);
		  break; 
		  
		case 26:
		  out.sample.can.id = fake_heartbeat_id(node, 26);
		  break; 
		  

		case 111:
		case 113:
		  out.sample.can.id = fake_heartbeat_id(node, 25);
		  break; 
		  
		case 40:
		  out.sample.can.id = fake_heartbeat_id(node, 13);
		  break; 
		  
		case 30:
		  out.sample.can.id = fake_heartbeat_id(node, 23);
		  break; 
		  
		case 155:
		case 160:
		case 165:
		case 170:
		case 175:
		case 180: 
		case 185:
		  out.sample.can.id = fake_heartbeat_id(node, 22);
		  break; 
		}
		*/  
		
		/* again, wtf
		out.sample.can.data[0] = 0;
		out.sample.can.data[1] = 0;
		out.sample.can.data[2] = 0;
		out.sample.can.data[3] = 0;
		out.sample.can.data[4] = (timestamp >> 24) & 0xFF;
		out.sample.can.data[5] = (timestamp >> 16) & 0xFF;
		out.sample.can.data[6] = (timestamp >> 8 ) & 0xFF;
		out.sample.can.data[7] = (timestamp >> 0 ) & 0xFF;
		out.sample.can.length = 8;

		output_queue(&out);	
		*/
		fprintf(stdout, 
			"\r%d%%\r", 
			(int)(100.0 * ((float)ftell(logfile)) 
			      / (float)logfile_len));
		
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
		
		usleep(5000);
	}

	return 0;
}
