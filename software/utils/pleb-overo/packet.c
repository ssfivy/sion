/* (C) 2005 Luke Macpherson */

#include <stdio.h>
#include <stdint.h>
#include "packet.h"

#define BYTE_MASK	0xFF
#define BYTE(x,y)	(((x)>>((y)<<3)) & BYTE_MASK)

void 
pkt_encode_sample(struct pkt_sample *in, uint8_t *out)
{
	/* encode type */
	out[0] = in->type;

	/* encode packet id */
	out[1] = BYTE(in->packet_id, 0);
	out[2] = BYTE(in->packet_id, 1);
	out[3] = BYTE(in->packet_id, 2);
	out[4] = BYTE(in->packet_id, 3);

	/* encode can id */
	out[5] = BYTE(in->can.id, 0);
	out[6] = BYTE(in->can.id, 1);
	out[7] = BYTE(in->can.id, 2);
	out[8] = BYTE(in->can.id, 3);

	/* encode data */
	out[9] = in->can.data[0];
	out[10] = in->can.data[1];
	out[11] = in->can.data[2];
	out[12] = in->can.data[3];
	out[13] = in->can.data[4];
	out[14] = in->can.data[5];
	out[15] = in->can.data[6];
	out[16] = in->can.data[7];

	/* encode length */
	out[17] = in->can.length;
}

void
pkt_encode_ack(struct pkt_ack *in, uint8_t *out){
	/* encode type */
	out[0] = in->type;

	/* encode packet id */
	out[1] = BYTE(in->packet_id, 0);
	out[2] = BYTE(in->packet_id, 1);
	out[3] = BYTE(in->packet_id, 2);
	out[4] = BYTE(in->packet_id, 3);

	/* encode ack id */
	out[5] = BYTE(in->ack_id, 0);
	out[6] = BYTE(in->ack_id, 1);
	out[7] = BYTE(in->ack_id, 2);
	out[8] = BYTE(in->ack_id, 3);
}

/* returns number of bytes encoded */
uint32_t
pkt_encode(union pkt_any *in, uint8_t *out)
{
	switch(in->null.type){
	case PKT_SAMPLE:
		pkt_encode_sample(&(in->sample), out);
		return PKT_SAMPLE_BYTES;
	case PKT_ACK:
		pkt_encode_ack(&(in->ack), out);
		return PKT_ACK_BYTES;
	}

	return 0; /* no packet encoded */
}

/* returns type of packet decoded */
uint8_t
pkt_decode(uint8_t *in, union pkt_any *out)
{
	uint8_t type;

	type = in[0];

	switch(type){
	case PKT_ACK:
		out->ack.type = type;

		out->ack.packet_id =  (uint32_t) in[1]
		                   | ((uint32_t) in[2] << 8)
		                   | ((uint32_t) in[3] << 16)
		                   | ((uint32_t) in[4] << 24);

		out->ack.ack_id =  (uint32_t) in[5]
		                | ((uint32_t) in[6] << 8)
		                | ((uint32_t) in[7] << 16)
		                | ((uint32_t) in[8] << 24);

		break;
	case PKT_SAMPLE:
		out->sample.type = type;

		out->sample.packet_id = (uint32_t)  in[1]
		                      | ((uint32_t) in[2] << 8)
		                      | ((uint32_t) in[3] << 16)
		                      | ((uint32_t) in[4] << 24);

		out->sample.can.id = (uint32_t)  in[5]
		                   | ((uint32_t) in[6] << 8)
		                   | ((uint32_t) in[7] << 16)
		                   | ((uint32_t) in[8] << 24);

		out->sample.can.data[0] = in[9];
		out->sample.can.data[1] = in[10];
		out->sample.can.data[2] = in[11];
		out->sample.can.data[3] = in[12];
		out->sample.can.data[4] = in[13];
		out->sample.can.data[5] = in[14];
		out->sample.can.data[6] = in[15];
		out->sample.can.data[7] = in[16];

		out->sample.can.length = in[17];

		break;
	default:
		fprintf(stderr, "decode(): unknown packet type!\n");
		out->null.type = PKT_NULL;
		type = PKT_NULL;
	}

	return type;
}
