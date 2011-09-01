/* (C) 2005 Luke Macpherson */

#define INPUT_QUEUE_LENGTH (50000)

void input_init(void);
int input_dequeue(union pkt_any *s);
void input_recv(void);

