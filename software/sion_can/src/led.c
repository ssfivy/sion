/*
* Project: SION (Sunswift IV Observer Node)
* File name: led.c
* Author: David Snowdon
* Modified by: Irving Tjiptowarsono
* Description:
* LED control for LPC1768
* 
* 
* 
* 
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* Copyright (C) 2003 David Snowdon
* 
*/
#include <cmsis/LPC17xx.h>
#include <arch/pinsel.h>
#include <arch/gpio.h>
 
#include "project/led.h"
#include "project/hardware.h"

void yellow_led (uint8_t on) {
	if (on)
		GPIO_SetValue(YELLOWLEDPORT, YELLOWLEDPIN, 0);
	else
		GPIO_SetValue(YELLOWLEDPORT, YELLOWLEDPIN, 1);
}

void toggle_yellow_led (void) {
		GPIO_ToggleValue(YELLOWLEDPORT, YELLOWLEDPIN);
}

void red_led (uint8_t on) {
	if (on)
		GPIO_SetValue(REDLEDPORT, REDLEDPIN, 0);
	else
		GPIO_SetValue(REDLEDPORT, REDLEDPIN, 1);
}

void toggle_red_led (void) {
	GPIO_ToggleValue(REDLEDPORT, REDLEDPIN);
}

