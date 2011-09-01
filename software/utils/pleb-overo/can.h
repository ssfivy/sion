#ifndef __CAN__
#define __CAN__

#define NO_ERR     0
#define NO_MSG_ERR 3

#define CAN_MSG_LENGTH 8

#ifndef STRUCT_CAN_MSG_DEFINED
#define STRUCT_CAN_MSG_DEFINED
struct can_msg {
        uint32_t id;
	uint8_t data[CAN_MSG_LENGTH];
	uint8_t length;
};
#endif

/* This is a subset of the functions defined by scandal_can.h */
void init_can(void);
int can_fd();
uint8_t can_get_msg(struct can_msg *msg);
uint8_t can_send_msg(struct can_msg *msg, uint8_t priority);

#endif
