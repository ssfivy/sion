/*
* Project: SION (Sunswift IV Observer Node)
* File name: led.c
* Author: David Snowdon
* Modified by: Irving Tjiptowarsono
* Description: 
* LED control
* Exactly the same as the scandal_led.h, 
* except for the typedefs.
* Set up for lpc1768
* 
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* Copyright (C) 2002 David Snowdon
*/
//#include <scandal_types.h>   
#ifndef __LED__
#define __LED__

#include <stdint.h>

void red_led(uint8_t on);
void toggle_red_led(void);

void yellow_led(uint8_t on);
void toggle_yellow_led(void);
#endif
