/*
* Project: SION (Sunswift IV Observer Node)
* File name: sion_can.c
* Author: Irving Tjiptowarsono
* Description: 
*  main file for the CAN driver interface for SION
* Does not have Scandal installed in it, only a crude CAN parser. 
* Based on CMSIS Cortex-M3 Blinky example v1.03, 
* which is Copyright (C) 2009 ARM Limited. All rights reserved.
* 
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* 
*/

/* Standard CMSIS Definitions. */
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_can.h"
#include "debug_frmwrk.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_exti.h"

/* Local includes. */
#include "sion_can.h"
#include "hardware.h"
#include "led.h"
#include "printf.h" /* only provides sprintf_(), for debugging */
#include "scandal.h"
#include "circbuffer.h"
#include "tritium-can.h"

volatile uint32_t msTicks;                            /* counts 10ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
/* 
While this thing looks like it will eventually overflow, 
for some reason the LED will keep blinking... I'm guessing
the comparison operator checks the overflow bit, or something.

Well, I don't have a problem with it!
*/
}

CAN_MSG_Type tx_msg, rx_msg; //declare buffers
can_pkt pkt, ipkt, opkt;
sion_entry entry, oentry;
uint8_t obuf[80];

/* CAN FIFO circular static buffer stuff */
can_pkt inbuf[CAN_IN_BUFFER_SIZE], outbuf[CAN_OUT_BUFFER_SIZE];
circbuf incount, outcount;

/* Tritium translation... */
can_pkt sc_pkts[MAX_SC_CHN_PER_TRI_CHN]; //see definition...
uint8_t sc_pkts_count, l;

/* Status sending... */
sion_entry bentry;
can_pkt bpkts[SION_NUM_OUT_CHANNELS+1];
uint8_t m;

/* CAN rx interrupt handler */
void CAN_IRQHandler(void) {
	//get packet from register
	CAN_ReceiveMsg(LPC_CAN2, &rx_msg);

	//Check if we have a standard or extended length header (tritium or scandal packet)
	if (rx_msg.format == EXT_ID_FORMAT) {

	//load struct
	ipkt.id = rx_msg.id;
	ipkt.payload = 
		((uint64_t)rx_msg.dataA[0]<<56) 
		|((uint64_t)rx_msg.dataA[1]<<48) 
		|((uint64_t)rx_msg.dataA[2]<<40) 
		|((uint64_t)rx_msg.dataA[3]<<32)
		|(rx_msg.dataB[0]<<24) |(rx_msg.dataB[1]<<16) |(rx_msg.dataB[2]<<8) |(rx_msg.dataB[3]);

		//put stuff thru buffer
		insert_packet(&ipkt, inbuf, &incount, CAN_IN_BUFFER_SIZE);
	}
	else {

		//convert to scandal packet(s)
		tritium_to_scandal_packet(&rx_msg, sc_pkts, &sc_pkts_count);
		//UARTPutDec32(LPC_UART0, (uint32_t) sc_pkts_count);

		/*
		if this packet doesnt have what we want (eg, we dont want random DC voltage)
		then sc_pkts_count will be 0 and the following will be skipped.
		*/

		//disable other things that may use insert_packet on the output message,
		//then inject the packets
		NVIC_DisableIRQ(UART1_IRQn); //untested
		NVIC_DisableIRQ(CAN_IRQn); //untested

		for (l=0;l<sc_pkts_count;l++) {
			//insert packet(s) to output buffer, where it will be re-broadcasted to the
			//CAN bus and also put back into the input queue. This is for driver display etc.
		//UARTPuts(LPC_UART0, "Got Tritium packet!!!\n\r");
			
			//insert_packet(&sc_pkts[l], inbuf, &incount, CAN_IN_BUFFER_SIZE); //working line of code
			insert_packet(&sc_pkts[l], outbuf, &outcount, CAN_OUT_BUFFER_SIZE); //untested
		}
		//turn interrupts back on
		NVIC_EnableIRQ(UART1_IRQn); //untesued
		NVIC_EnableIRQ(CAN_IRQn); //untested
	}

}

/* UART rx intruupt handler */
void UART1_IRQHandler(void) {
	//red_led(1);
	uint8_t rx_count;
	//check for correct header
	if (UART_ReceiveByte((LPC_UART_TypeDef *) LPC_UART1) == CAN_MESSAGE_HEADER ) {
		//whee we have correct header, get the rest of data packet.
		rx_count = UART_Receive((LPC_UART_TypeDef *) LPC_UART1, obuf, 13, BLOCKING);
		if (13 == rx_count) { //check that we have full packet
			obuf[13] = '\0'; //null terminated string
			//load struct
			stringtoentry(obuf, &oentry);
			entrytocan(&oentry, &opkt);
			//put it in buffer
			insert_packet(&opkt, outbuf, &outcount, CAN_OUT_BUFFER_SIZE);
		}
	}
	//red_led(0);
}


/* Shutdown GPIO interrupt handler */
volatile uint8_t potential_shutdown_flag = 0;
volatile uint32_t when_was_shutdown_triggered = 0;
void EINT3_IRQHandler(void) {
	potential_shutdown_flag = 1;
	when_was_shutdown_triggered = msTicks;
	red_led(1);
	GPIO_ClearInt(0, (1<<21));
}

/*
This checks that pin stays LOW after a certain period of time
after the shutdown GPIO interrupt is activated. 

This is to prevent brownouts from triggering shutdown.
*/
__INLINE static void check_shutdown(void) {
	if(potential_shutdown_flag) {
		if (msTicks > (when_was_shutdown_triggered + SHUTDOWN_CONFIRM_TIME)) { //if enough time passed 
			if ( (GPIO_ReadValue(0) & (1<<21)) == 0) {  //if pin stays low
				/* shit, it is not a brownout, send the shutdown command! */
				UART_SendByte((LPC_UART_TypeDef *) LPC_UART1, SHUTDOWN_SIGNAL);
			}
			else if( (GPIO_ReadValue(0) & (1<<21)) != 0){ //if pin goes back up
				potential_shutdown_flag = 0;
				red_led(0);
			}
		}
	}
}

/* Periodic functions */
void Blink_LED_Periodically(uint32_t period) {
	static uint32_t Last_Blink = 0;
	if ((msTicks - Last_Blink) >= period) {
		Last_Blink = msTicks;
		toggle_yellow_led();
	}
}

void Broadcast_Status_Periodically(uint32_t period) {
	static uint32_t Last_Broadcast = 0;

	if ((msTicks - Last_Broadcast) >= period) {
		Last_Broadcast = msTicks;
		//put broadcast stuff here
		make_entry_from_SION(&bentry, SION_DROPPED_PACKET_COUNT, dropped_packet_count, 31337);
		entrytocan(&bentry, &bpkts[0]);

		make_entry_from_SION(&bentry, WS20, 0x0000000A, 31337);
		bentry.priority = 0x06;
		bentry.message_type = 0x02;
		entrytocan(&bentry, &bpkts[1]);

		make_entry_from_SION(&bentry, SION_RECEIVED_PACKET_COUNT, received_packet_count, 31337);
		entrytocan(&bentry, &bpkts[2]);

		//disable other things that may use insert_packet on the output message,
		//then inject the packets
		NVIC_DisableIRQ(UART1_IRQn);
		NVIC_DisableIRQ(CAN_IRQn);
		for (l=0;l<SION_NUM_OUT_CHANNELS;l++) {
			/*
			insert packet(s) to output buffer, where it will be re-broadcasted to the
			CAN bus and also put back into the input queue.
			*/
			insert_packet(&bpkts[l], outbuf, &outcount, CAN_OUT_BUFFER_SIZE);
		}
		NVIC_EnableIRQ(UART1_IRQn);
		NVIC_EnableIRQ(CAN_IRQn);
	}
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	/* Init Clocks */
	SystemInit(); //initialize clocks
	SystemCoreClockUpdate(); //update SystemCoreClock - has to be done, otherwise timing stuff breaks.

	/* Init random stuff */
	if (SysTick_Config(SystemCoreClock / SYSTICK_INTERRUPT_FREQUENCY )) { /* Setup SysTick Timer for 1 msec interrupts  */
		while (1);                                  /* Capture error */
	}
	All_Pin_Init(); //init all pins / pinsel register
	debug_frmwrk_init(); //init UART debugging
	dropped_packet_count = 0;
	received_packet_count = 0;

	/* Initialise all CAN stuff */
	incount.start = 0;
	incount.length = 0;
	outcount.start = 0;
	outcount.length = 0;
	CAN_Init(LPC_CAN2, SCANDAL_CAN_SPEED); //init CAN peripherals
	CAN_IRQCmd(LPC_CAN2, CANINT_RIE, ENABLE); //enable can rx interrupt
	//NVIC_SetPriority(CAN_IRQn, ((0x00<<3)|(0x01)) ); //hightest priority, hope this works.
	NVIC_EnableIRQ(CAN_IRQn); //enable can interrupt
	CAN_SetAFMode(LPC_CANAF, CAN_AccBP); //bypass acceptance filter

	/* Initialise comms to Overo through UART1*/
	UART_CFG_Type UARTConfigStruct; //declare config struct
	UART_ConfigStructInit(&UARTConfigStruct); //set default configs
	UARTConfigStruct.Baud_rate = 115200; //set baud rate
	UART_Init((LPC_UART_TypeDef *) LPC_UART1, &UARTConfigStruct); // init registers
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_FIFOConfig((LPC_UART_TypeDef *) LPC_UART1, &UARTFIFOConfigStruct);
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART1, ENABLE);

	/* Initialise comms to Overo through SSP0 */
	/* Not implemented yet
	SSP_CFG_Type SSP_ConfigStruct;
	SSP_DATA_SETUP_Type xferConfig;

	SSP_ConfigStructInit(&SSP_ConfigStruct);
	SSP_ConfigStruct.Mode= SSP_SLAVE_MODE;
	SSP_Init(LPC_SSP0, &SSP_ConfigStruct);

	NVIC_SetPriority(SSP0_IRQn, ((0x01<<3)|0x01)); //preemption=1, sub-priority=1
	NVIC_EnableIRQ(SSP0_IRQn);
	SSP_Cmd(LPC_SSP0, ENABLE);
	*/

	/* print init message */
	uint8_t buf[80];
	sprintf_(buf, "SION CAN Controller - LPC1768\n\r");
	UARTPuts(LPC_UART0, buf);
	UARTPuts((LPC_UART_TypeDef *)LPC_UART1, buf);
	sprintf_(buf, "SystemCoreClock: %d Hz\n\r", (int32_t) SystemCoreClock);
	UARTPuts(LPC_UART0, buf);
	UARTPuts((LPC_UART_TypeDef *)LPC_UART1, buf);



	/* HACK: do not send data to overo until trigger character is received. */
	/* Hack since this is one-off, it should be hardware triggered so it 
	   can stop sending when overo is rebooted */
	uint8_t count = 0;
	while(1) {
		/* We send nothing and queue CAN packets.... */
		Blink_LED_Periodically(LED_PERIOD);

		/* and check incoming characters in UART for the trigger character...*/
		if( UART_ReceiveByte((LPC_UART_TypeDef *) LPC_UART1) == START_SENDING_CAN_MESSAGES ) {
			count++;
		}
		else {
			count = 0;
		}

		/* until we get the trigger characters... */
		if (1 == count) {
		/* then we break and enter the main loop, when we send stuff. */
			break;
		}
	}

	/* Now we can init UART1 rx interrupts */
	UART_IntConfig((LPC_UART_TypeDef *) LPC_UART1, UART_INTCFG_RBR, ENABLE);
	NVIC_SetPriority(UART1_IRQn, ((0x02<<3)|(0x01)) );
	NVIC_EnableIRQ(UART1_IRQn);

	/* Init shutdown interrupt */
	GPIO_IntCmd(0, (1 << 21), 1); //port 0, pin 21, falling edge
	NVIC_SetPriority(EINT3_IRQn, ((0x01<<3)|(0x01)));
	NVIC_EnableIRQ(EINT3_IRQn);

	red_led(1);



	while(1) {
		Blink_LED_Periodically(LED_PERIOD);
		Broadcast_Status_Periodically(BROADCAST_PERIOD);

		//if possibility of shutdown exists, check state of pin continously
		check_shutdown();

		/* if we have incoming CAN data in buffer, send it to overo */
		if ( incount.length > 0 ) {
			remove_packet(&pkt, inbuf, &incount, CAN_IN_BUFFER_SIZE);
			cantoentry(&pkt, &entry);
			entrytostring(&entry, buf);

			//send data to overo
			UART_SendByte((LPC_UART_TypeDef *) LPC_UART1, CAN_MESSAGE_HEADER);
			UART_SendByte((LPC_UART_TypeDef *) LPC_UART1, '\n');
			UART_SendByte((LPC_UART_TypeDef *) LPC_UART1, '\r');
			UART_Send((LPC_UART_TypeDef *) LPC_UART1, buf, 13, BLOCKING);
		
			//print debug
			#define PRINT_DEBUG
			//UARTPutDec32(LPC_UART0, (uint32_t) n);
			//UARTPuts(LPC_UART0, " <- Dropped packet count.\n\r");
			#ifdef PRINT_DEBUG
			UARTPutDec(LPC_UART0, entry.priority);
			UARTPutChar(LPC_UART0, '\t');
			UARTPutDec(LPC_UART0, entry.message_type);
			UARTPutChar(LPC_UART0, '\t');
			UARTPutDec(LPC_UART0, entry.source_address);
			UARTPutChar(LPC_UART0, '\t');
			UARTPutDec16(LPC_UART0, entry.specifics);
			UARTPutChar(LPC_UART0, '\t');
			sprintf_(buf, "%d", entry.value);
			UARTPuts(LPC_UART0, buf);
			UARTPutChar(LPC_UART0, '\t');
			UARTPutChar(LPC_UART0, '\t');
			UARTPutDec32(LPC_UART0, (uint32_t) (entry.scandal_timestamp & 0xFFFFFFFF));
			UARTPutChar(LPC_UART0, '\n');
			UARTPutChar(LPC_UART0, '\r');
			#endif
		}

		/*if we have outgoing CAN data in buffer, send it to CAN */
		if (outcount.length > 0 ) {
			/* read  buffer */
			remove_packet(&opkt, outbuf, &outcount, CAN_OUT_BUFFER_SIZE);

			tx_msg.id = opkt.id;
			tx_msg.dataA[0] = (uint8_t) (0x000000FF & (opkt.payload >> 56));
			tx_msg.dataA[1] = (uint8_t) (0x000000FF & (opkt.payload >> 48));
			tx_msg.dataA[2] = (uint8_t) (0x000000FF & (opkt.payload >> 40));
			tx_msg.dataA[3] = (uint8_t) (0x000000FF & (opkt.payload >> 32));
			tx_msg.dataB[0] = (uint8_t) (0x000000FF & (opkt.payload >> 24));
			tx_msg.dataB[1] = (uint8_t) (0x000000FF & (opkt.payload >> 16));
			tx_msg.dataB[2] = (uint8_t) (0x000000FF & (opkt.payload >> 8));
			tx_msg.dataB[3] = (uint8_t) (0x000000FF & (opkt.payload >> 0));
			
			//need to load all these parameters too, otherwise CMSIS chokes up and dies.
			tx_msg.len = 0x08; //8 bytes length
			tx_msg.format = EXT_ID_FORMAT; //29 bit header
			tx_msg.type = DATA_FRAME; //data, not remote frame
			
			/* Send it off */
			CAN_SendMsg(LPC_CAN2, &tx_msg);

			/*	Add packet to the input queue, for logging
				We need to disable CAN interrupt here as we don't want this insert_packet to
				be interrupted by CAN (which will call its own insert_packet). 
				I have enough problem debugging this circular buffer, I am not going to make it atomic.
			*/
			NVIC_DisableIRQ(CAN_IRQn);
			insert_packet(&opkt, inbuf, &incount, CAN_IN_BUFFER_SIZE);
			NVIC_EnableIRQ(CAN_IRQn);
			
		}
	}

	return 0;
}

