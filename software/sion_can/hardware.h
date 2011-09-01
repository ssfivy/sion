/*
* Project: SION (Sunswift IV Observer Node)
* File name: hardware.h
* Author: Irving Tjiptowarsono
* Description: 
* Defines hardware ports in LPC1768.
* Most stuff is defined using CMSIS definitions
* 
* 
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* 
*/

/*
Conditionals, to prevent double definitions when linking

*/
#ifndef __HARDWARE__
#define __HARDWARE__

/*--------------------
  LED stuff
--------------------- */

#define YELLOWLEDPORT 0
#define REDLEDPORT 2
#define YELLOWLEDPIN (1<<11)
#define REDLEDPIN (1<<13)

/*-------------------
  Pinsel configurations
---------------------*/
__INLINE static void LED_Init_Pin(void) {
	//set both LED pins to output
	GPIO_SetDir( REDLEDPORT, REDLEDPIN, 1);
	GPIO_SetDir( YELLOWLEDPORT, YELLOWLEDPIN, 1);
	// clear led, since their default state is on
	GPIO_SetValue(REDLEDPORT, REDLEDPIN);
	GPIO_SetValue(YELLOWLEDPORT, YELLOWLEDPIN);
}

__INLINE static void CLKOUT_Init_Pin(void) {
	PINSEL_CFG_Type PinCfg;
	PinCfg.Portnum = PINSEL_PORT_1;
	PinCfg.Funcnum = PINSEL_FUNC_1;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinCfg.Pinnum = PINSEL_PIN_27;
	PINSEL_ConfigPin(&PinCfg);
}

__INLINE static void SSP0_Init_Pin(void){
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 17;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 18;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);

}

__INLINE static void UART1_Init_Pin(void){

	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = PINSEL_FUNC_2;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Pinnum = PINSEL_PIN_0; 
	PinCfg.Portnum = PINSEL_PORT_2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = PINSEL_PIN_1; 
	PINSEL_ConfigPin(&PinCfg);

}

__INLINE static void CAN2_Init_Pin(void) {
	// Pin configuration for CAN2
	PINSEL_CFG_Type PinCfg;

	/* Initialize CAN2 pin connect */
	PinCfg.Funcnum = PINSEL_FUNC_1;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Pinnum = PINSEL_PIN_7;
	PinCfg.Portnum = PINSEL_PORT_2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = PINSEL_PIN_8;
	PINSEL_ConfigPin(&PinCfg);
}

__INLINE static void Shutdown_Init_Pin(void) {
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = PINSEL_FUNC_0;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinCfg.Pinnum = PINSEL_PIN_21;
	PinCfg.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir( 0, 21, 0);

}

__INLINE static void All_Pin_Init (void) {
	LED_Init_Pin();
	CLKOUT_Init_Pin();
	UART1_Init_Pin();
	CAN2_Init_Pin();
	SSP0_Init_Pin();
	Shutdown_Init_Pin();
}




#endif
