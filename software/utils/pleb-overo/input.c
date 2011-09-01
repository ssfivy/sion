/* (C) 2005 Luke Macpherson */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "packet.h"
#include "input.h"
#include "output.h"
#include "socket.h"

/* queue contains incoming packets hashed % ID */
union pkt_any *in_queue;
uint32_t in_queue_length;

/* fifo contains pointers into queue in fifo order */
union pkt_any **in_fifo;
uint32_t in_fifo_length;
uint32_t in_fifo_head;
uint32_t in_fifo_tail;

void
input_init(){
	int i;

	/* set up in_queue */
	in_queue_length = INPUT_QUEUE_LENGTH;

	in_queue = malloc(sizeof(union pkt_any)*in_queue_length);
	assert(in_queue!=NULL);

	for(i=0; i<in_queue_length; i++){
		in_queue[i].null.type = PKT_NULL;
		in_queue[i].null.packet_id = ~0;
	}

	/* set up fifo */
	in_fifo_length = INPUT_QUEUE_LENGTH + 1;
	in_fifo_head = in_fifo_tail = 0;

	in_fifo = malloc(sizeof(union pkt_any *)*in_fifo_length);
	assert(in_fifo!=NULL);

	for(i=0; i<in_fifo_length; i++){
		in_fifo[i] = NULL;
	}
}

int
input_enqueue(union pkt_any *s){
	union pkt_any *queue_entry;

	/* calculate queue index */
	queue_entry = &in_queue[s->null.packet_id % in_queue_length];

	/* detect duplicate */
	if(queue_entry->null.packet_id == s->null.packet_id){
//		printf("input_equeue(): duplicate\n");
		return 0;
	}

	/* insert into queue */
	*queue_entry = *s;

	/* insert into in_fifo */
	in_fifo[in_fifo_tail++] = queue_entry;
	in_fifo_tail %= in_fifo_length;

	if(in_fifo_tail==in_fifo_head){
		in_fifo_head++; /* oldest entry dropped from in_fifo */
	}

	return 0;
}

int
input_dequeue(union pkt_any *s){
	union pkt_any *queue_entry;

	if(in_fifo_head==in_fifo_tail){
		return -1; /* in_fifo is empty */
	}

	/* get entry out of in_fifo */
	queue_entry = in_fifo[in_fifo_head++];
	in_fifo_head%=in_fifo_length;

	/* get entry out of queue */
	*s = *queue_entry;

	/* mark queue entry free */
	queue_entry->null.type=PKT_NULL;

	return 0;
}

/* acks are not queued, but sent immediately */
static void
input_generate_ack(union pkt_any *packet){
	uint8_t raw[UDP_MAX_PAYLOAD];
	union pkt_any ack;
	uint32_t bytes;

	ack.ack.type = PKT_ACK;
	ack.ack.packet_id = output_id();
	ack.ack.ack_id = packet->null.packet_id;

	/* generate an ack immediately! */
	bytes = pkt_encode(&ack, raw);
	if(bytes==0)
		return;

	socket_send(raw, bytes);
}

void
input_recv(){
	uint8_t raw[UDP_MAX_PAYLOAD];
	union pkt_any packet;

	while(!socket_recv(raw, UDP_MAX_PAYLOAD)){
		switch(pkt_decode(raw, &packet)){
		case PKT_ACK:
			output_ack(packet.ack.ack_id);
			break;
		default:
			/* enqueue new input packet */
			input_enqueue(&packet);
			/* generate an ack! */
			input_generate_ack(&packet);
		}
	}
}
