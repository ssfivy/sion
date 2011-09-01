/* (C) 2005 Luke Macpherson */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "packet.h"
#include "output.h"
#include "heap.h"
#include "microuptime.h"
#include "socket.h"

union pkt_any *queue;
uint32_t queue_length;
struct heap *heap;

/* rate control variables */
uint64_t start_time;
uint64_t total_sends;

/*
 * output strategy:
 * always pick the sample with the fewest number of retransmits
 * prioritises newest packets
 * everything gradually reaches same age if not acked
 * should tend not to retransmit same packet over and over
 */


void
output_init()
{
	int i;

	queue_length = OUTPUT_QUEUE_LENGTH;
	queue = malloc(sizeof(union pkt_any)*queue_length);
	assert(queue!=NULL);

	for(i=0; i<queue_length; i++){
		queue[i].null.type = PKT_NULL;
	}

	heap = heap_init(OUTPUT_QUEUE_LENGTH);
	assert(heap!=NULL);

	/* initialise rate control variables */
	start_time = microuptime();
	total_sends = 0;
}

uint32_t
output_id()
{
	static uint32_t unique_id = PKT_FIRST_ID;
	unique_id++;
	return unique_id;
}

/* find id and set to null packet */
void
output_ack(uint32_t id)
{
	uint32_t index;

	index = id % queue_length;
	if(queue[index].null.packet_id==id){
		queue[index].null.type = PKT_NULL;
	}else{
		printf("output_ack(): packet %d not in queue\n", id);
	}
}

/* add packet to queue */
void
output_queue(union pkt_any *in)
{
	assert(in->null.type!=PKT_ACK);
	assert(in->null.type!=PKT_NULL);

	in->null.packet_id = output_id();
	queue[in->null.packet_id % queue_length] = *in;
	heap_insert(heap, 0, in->null.packet_id);
}

/* pick and send samples */
void
output_send()
{
	/* space for encoding packets */
	uint8_t raw[UDP_MAX_PAYLOAD];

	/* output packet queue */
	uint32_t o_id[MAX_SENDS];
	uint32_t o_tx[MAX_SENDS];
	uint32_t o_entries = 0;
	uint32_t i;

	/* rate control variables */
	uint32_t transmits, id, index, bytes;
	uint64_t remaining, expected;
	uint64_t now, delta;

	/* calculate rate control */
	now = microuptime();
	delta = now - start_time;
	expected = (delta * TRANSMIT_RATE) / US_PER_S;
	remaining = expected - total_sends;

	/* this stops us building up too big a pool */
	if(remaining > MAX_SENDS){
		remaining = MAX_SENDS;
		total_sends = expected - MAX_SENDS;
	}

	while(remaining > 0){
		/* get id of lowest priority packet off heap */
		if(heap_delete(heap, &transmits, &id)!=0){
			break; /* nothing else to send */
		}

		/* find id in queue */
		index = id % queue_length;
		if(queue[index].null.packet_id!=id)
			continue; /* packet has already been overwritten */
		if(queue[index].null.type==PKT_NULL)
			continue; /* packet has already been acked */

		/* queue packet for immediate transmit */
		o_id[o_entries] = id;
		o_tx[o_entries] = transmits;
		o_entries++;
		remaining--;
	}

	/* retransmit */
	for(i=0; i<o_entries; i++){
		transmits = o_tx[i];
		id = o_id[i];
		index = id % queue_length;

		bytes = pkt_encode(&queue[index], raw);
		if(bytes==0)
			continue; /* packet can't be encoded */

		if(socket_send(raw, bytes)==0){
			transmits++;
			total_sends++;
		}

		assert(heap_insert(heap, transmits, id)==0);
	}

}
