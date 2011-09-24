/*
* Project: SION (Sunswift IV Observer Node)
* File name: sion_can.h
* Author: Irving Tjiptowarsono
* Description: 
* header file for SION can controller
contains various #defines for configuring stuff

* 
* 
* 
* 
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* 
*/

#ifndef __SION_CAN_HEADER__
#define __SION_CAN_HEADER__

/*
Sets the CAN baud rate.

MCP2515 devices:
This is set in mcp2510.c, line 92 - use the default baud rate,
which is 50kbps.
There are also definitions in carsoft/scandal/include/scandal_can.h

LPC11C14 devices:
not done yet.
*/
//#define SCANDAL_CAN_SPEED DEFAULT_BAUD
#define SCANDAL_CAN_SPEED 50000
//#define SCANDAL_CAN_SPEED 125000


/*
Sets how often systick interrupt happens in 1 seconds.
ARM / NXP recommends an interrupt every 10 ms, 
which means 100 interrupts per second.
and that's what we'll use here.
*/
#define SYSTICK_INTERRUPT_FREQUENCY 100

/*
Sets how many systick interrupts must happen before
the LED blinks / changes state. Arbitrary.
*/
#define LED_PERIOD 10

/*
Sets how many systick interrupts must happen before
status messages are re-broadcasted. Arbitrary.
*/
#define BROADCAST_PERIOD 100

/*
Sets how many checks must happen
after shutdown GPIO handler is triggered
before shutdown command is sent. Arbitrary.

There is one check per systick interrupt.
*/
#define SHUTDOWN_CONFIRM_TIME 15

/*
Specifies the size of the static buffer (for CAN packets)
Input buffer should be pretty large, while output can be smaller.
These values should be fine. Too low and you might overwrite stuff, 
too high and you will be out of memory (though compiler will warn you)
*/
#define CAN_IN_BUFFER_SIZE 1300
#define CAN_OUT_BUFFER_SIZE 30




#endif
