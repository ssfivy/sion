/* (C) 2005 Luke Macpherson */

#define PKT_FIRST_ID 0

/* definition of can message */
#ifndef STRUCT_CAN_MSG_DEFINED
#define STRUCT_CAN_MSG_DEFINED
struct can_msg{
	uint32_t id;
	uint8_t  data[8];
	uint8_t  length;
};
#endif

/* definitions for type values */
#define PKT_NULL   0
#define PKT_SAMPLE 1
#define PKT_ACK    0xFF

/* definition of sample packet */
#define PKT_NULL_BYTES 5
struct pkt_null{
	uint8_t type;         /* packet type identifier */
	uint32_t packet_id;   /* unique packet identifier */
};

#define PKT_SAMPLE_BYTES 18
struct pkt_sample{
	uint8_t  type;        /* packet type identifier */
	uint32_t packet_id;   /* unique packet identifier */
	struct can_msg can;   /* can packet */
};

/* definition of ack packet */
#define PKT_ACK_BYTES 9
struct pkt_ack{
	uint8_t  type;        /* packet type identifier */
	uint32_t packet_id;   /* unique packet identifier */
        uint32_t ack_id;      /* packet id to ack */
};

/* enum uniting all packet types */
union pkt_any{
	struct pkt_null   null;
	struct pkt_sample sample;
	struct pkt_ack    ack;
};

uint32_t pkt_encode(union pkt_any *in, uint8_t *out);
uint8_t pkt_decode(uint8_t *in, union pkt_any *out);
