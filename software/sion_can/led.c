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
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
 
#include "led.h"
#include "hardware.h"

void yellow_led (uint8_t on) {
	if (on)
		GPIO_ClearValue(YELLOWLEDPORT, YELLOWLEDPIN);
	else
		GPIO_SetValue(YELLOWLEDPORT, YELLOWLEDPIN);
}

void toggle_yellow_led (void) {
	if (GPIO_ReadValue(YELLOWLEDPORT) & YELLOWLEDPIN)
		GPIO_ClearValue(YELLOWLEDPORT, YELLOWLEDPIN);
	else
		GPIO_SetValue(YELLOWLEDPORT, YELLOWLEDPIN);
}

void red_led (uint8_t on) {
	if (on)
		GPIO_ClearValue(REDLEDPORT, REDLEDPIN);
	else
		GPIO_SetValue(REDLEDPORT, REDLEDPIN);
}

void toggle_red_led (void) {
	if (GPIO_ReadValue(REDLEDPORT) & REDLEDPIN)
		GPIO_ClearValue(REDLEDPORT, REDLEDPIN);
	else
		GPIO_SetValue(REDLEDPORT, REDLEDPIN);
}

