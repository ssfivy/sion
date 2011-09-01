/* (C) 2005 Luke Mapherson */

/* 1mbit ~= 6944 pps (not including udp & ip overhead) */
#define TRANSMIT_RATE 6000 /* packets per second to transmit */
#define MAX_SENDS 100      /* max packets to send in one go */

#define OUTPUT_QUEUE_LENGTH (500000)

void output_init(void);
uint32_t output_id(void);
void output_ack(uint32_t id);
void output_queue(union pkt_any *in);
void output_send(void);
