/*
* Project: SION (Sunswift IV Observer Node)
* File name: tritium-can.h
* Author: Irving Tjiptowarsono
* Description:
* This file contains the code used to convert 
* tritium packets from the wavesculptor
* into scandal packets,
* complete with address, node etc.
* Warning: This will NOT handle two motor controllers in the car at the same time!
*
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* Using part of codes written by Tritium Pty Ltd:
*
* Copyright (C) 2008 Tritium Pty Ltd. All rights reserved. (www.tritium.com.au)
*
* Redistribution and use in source and binary forms, with or without modification, 
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer 
*	  in the documentation and/or other materials provided with the distribution.
*	- Neither the name of Tritium Pty Ltd nor the names of its contributors may be used to endorse or promote products 
*	  derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
* OF SUCH DAMAGE. 
* 
*/

#ifndef __TRITIUM_DEFINED__
#define __TRITIUM_DEFINED__

#include "scandal.h" /* scandal.h from SION. */
#include "arch/can.h"
#include "arch/debug_frmwrk.h"

/* Scandal node type */
#define WS20	36 //WaveSculptor 20
#define WS22	37 //WaveSculptor 22

/* Scandal node address */
#define WAVESCULPTOR_SCANDAL_ADDRESS 12

/* WaveSculptor 20 Translated Packets */
	/* Out Channels */
	#define WS20_WAVESCULPTOR_TYPE				0 //Wavesculptor type, either 20 or 22. Will be 20 here.
	#define WS20_ACTIVE_MOTOR				1 //integer
	#define WS20_ERRORS					2 //bit flags
	#define WS20_LIMITS					3 //bit flags
	#define WS20_BUSCURRENT					4 //mV
	#define WS20_BUSVOLT					5 //mA
	#define WS20_VEHICLE_VELOCITY				6 //km/h
	#define WS20_PHASE_A					7 //mA
	#define WS20_PHASE_B					8 //mA
	//motor vector and random voltage isnt useful to us
	#define WS20_FAN_SPEED_RPM				9  //rpm
	#define WS20_HEATSINK_TEMP				10 //milidegree celsius
	#define WS20_MOTOR_TEMP					11 //milidegree celsius
	#define WS20_AMBIENT_AIR_IN_TEMP			12 //milidegree celsius
	#define WS20_AMP_HOURS					13 //miliamp hours
	#define WS20_ODOMETER					14 //metres
	#define WS20_NUM_OUT_CHANNELS				15

	/* In channels */
	//There's no code to handle these. They are placeholder only.
	#define WS20_MOTOR_DRIVE_COMMAND		0
	#define WS20_MOTOR_POWER_COMMAND		1
	#define WS20_RESET_COMMAND			2
	// Configuration commands
	#define WS20_CHANGE_ACTIVE_MOTOR		3
	#define WS20_NUM_IN_CHANNELS			4

	/* Original Tritium definitions for WS20 */
	#define WS20_MC_CAN_BASE	0x0400
	#define WS20_MC_STATUS	0x01
	#define WS20_MC_BUS		0x02
	#define WS20_MC_VELOCITY	0x03
	#define WS20_MC_PHASE	0x04
	#define WS20_MC_V_VECTOR	0x05
	#define WS20_MC_I_VECTOR	0x06
	#define WS20_MC_BACKEMF	0x07
	#define WS20_MC_RAIL_1	0x08
	#define WS20_MC_RAIL_2	0x09
	#define WS20_MC_FAN		0x0A
	#define WS20_MC_TEMP_1	0x0B
	#define WS20_MC_TEMP_2	0x0C
	#define WS20_MC_TEMP_3	0x0D
	#define WS20_MC_ODO		0x0E


/* WaveSculptor 22 Translated Packets */
	/* Out Channels */
	#define WS22_WAVESCULPTOR_TYPE				0 //Wavesculptor type, either 20 or 22. Will be 22 here.
	#define WS22_ACTIVE_MOTOR				1 //integer
	#define WS22_ERRORS					2 //bit flags
	#define WS22_LIMITS					3 //bit flags
	#define WS22_BUSCURRENT					4 //mV
	#define WS22_BUSVOLT					5 //mA
	#define WS22_VEHICLE_VELOCITY				6 //km/h
	#define WS22_PHASE_A					7 //mA
	#define WS22_PHASE_B					8 //mA
	//motor vector and random voltage isnt useful to us
	//#define WS22_FAN_SPEED_RPM				9  //not present in WS22
	//#define WS22_HEATSINK_TEMP				10 //not present in WS22
	#define WS22_MOTOR_TEMP					11 //milidegree celsius
	//#define WS22_AMBIENT_AIR_IN_TEMP			12 //not present in WS22
	#define WS22_AMP_HOURS					13 //miliamp hours
	#define WS22_ODOMETER					14 //metres
	#define WS22_IPM_PHASEA_TEMP					15 //milidegree celsius
	#define WS22_IPM_PHASEB_TEMP					16 //milidegree celsius
	#define WS22_IPM_PHASEC_TEMP					17 //milidegree celsius
	#define WS22_DSP_BOARD_TEMP					18 //milidegree celsius
	#define WS22_CAN_RX_ERROR_COUNT				19 //uint
	#define WS22_CAN_TX_ERROR_COUNT				20 //uint
	#define WS22_NUM_OUT_CHANNELS				18 //18 since we add 6 channels and remove 3

	/* In channels */
	//There's no code to handle these, they are placeholder only.
	#define WS20_MOTOR_DRIVE_COMMAND		0
	#define WS20_MOTOR_POWER_COMMAND		1
	#define WS22_RESET_COMMAND			2
	// Configuration commands
	#define WS22_CHANGE_ACTIVE_MOTOR		3 
	#define WS22_NUM_IN_CHANNELS			4

	/* Original Tritium definitions for WS22, slightly modified. */
	#define	WS22_MC_CAN_BASE		0x400		// High = Serial Number             Low = "T088" string
	#define WS22_MC_STATUS		0x01		// High = Active Motor/CAN counts   Low = Error & Limit flags
	#define	WS22_MC_BUS			0x02		// High = Bus Current               Low = Bus Voltage
	#define WS22_MC_VELOCITY		0x03		// High = Velocity (m/s)            Low = Velocity (rpm)
	#define WS22_MC_PHASE		0x04		// High = Phase A Current           Low = Phase B Current
	#define WS22_MC_V_VECTOR		0x05		// High = Vd vector                 Low = Vq vector
	#define WS22_MC_I_VECTOR		0x06		// High = Id vector                 Low = Iq vector
	#define WS22_MC_BEMF_VECTOR	0x07		// High = BEMFd vector              Low = BEMFq vector
	#define WS22_MC_RAIL1		0x08		// High = 15V                       Low = Unused
	#define WS22_MC_RAIL2		0x09		// High = 3.3V                      Low = 1.9V
	#define WS22_MC_FAN			0x0A		// High = Reserved                  Low = Reserved
	#define WS22_MC_TEMP1		0x0B		// High = Heatsink Phase C Temp     Low = Motor Temp
	#define WS22_MC_TEMP2		0x0C		// High = Heatsink Phase B Temp     Low = CPU Temp
	#define WS22_MC_TEMP3		0x0D		// High = Heatsink Phase A Temp     Low = Unused
	#define WS22_MC_CUMULATIVE	0x0E		// High = DC Bus AmpHours           Low = Odometer

/* 
maximum possible number of different scandal channels (information)
contained in a tritium channel - in this case it's 5 
since there's 5 items in the status packet.
*/
#define MAX_SC_CHN_PER_TRI_CHN	5


typedef union _value {
	float 		data_float[2];
	uint8_t		data_u8[8];
	uint16_t	data_u16[4];
	uint32_t 	data_u32[2];
	uint64_t	data_u64;
} value_t;

can_pkt pkt;
sion_entry entry;

void craft_scandal_packet(can_pkt *sc_pkts, uint8_t index, uint8_t priority, uint16_t channel_number, int32_t value, uint32_t timestamp) {

	entry.priority = priority;
	entry.message_type = 0x00; //channel message
	entry.source_address = WAVESCULPTOR_SCANDAL_ADDRESS;
	entry.specifics = channel_number;
	entry.value = value;
	entry.scandal_timestamp = timestamp;

	entrytocan(&entry, &pkt);
	sc_pkts[index] = pkt;
}


void tritium_to_scandal_packet(CAN_MSG_Type *tri_pkt, can_pkt *sc_pkts, uint8_t *sc_pkts_count) {
	uint32_t i, timestamp;
	value_t value;
	static uint8_t tritium_device_type = 0; //by default, no device

	//timestamp = scandal_get_realtime32();
	//timestamp = 31337; //FIXME: Get proper timing info
	timestamp = (LPC_TIM0->TC) + timestamp_offset;

	*sc_pkts_count = 0; //at first, assume nothing in this packet is useful

	/* load tritium packet to temp struct */
	for (i=0; i<4; i++) {
		value.data_u8[i] = tri_pkt->dataA[i];
	}
	for (i=0; i<4; i++) {
		value.data_u8[i+4] = tri_pkt->dataB[i];
	}

	/* We need to determine the type of Wavesculptor present */
	if ((tri_pkt->id & 0x07FF) == WS20_MC_CAN_BASE) {
		//check that a wavesculptor 20 is present
		if ((value.data_u8[0] == 'T') &&
			(value.data_u8[1] == 'R') &&
			(value.data_u8[2] == 'I') &&
			(value.data_u8[3] == 'a') ){
			tritium_device_type = WS20;
		}
	}
	if ((tri_pkt->id & 0x07FF) == WS22_MC_CAN_BASE) {
		//check that a wavesculptor 22 is present
		if ((value.data_u8[0] == 'T') &&
			(value.data_u8[1] == '0') &&
			(value.data_u8[2] == '8') &&
			(value.data_u8[3] == '8') ){
			tritium_device_type = WS22;
		}	
	}

	/* If we have a WaveSculptor 20 */
	if (tritium_device_type == WS20) {
		switch(tri_pkt->id & 0x07FF) { //Pick a handling code based on device+channel combination
		case WS20_MC_CAN_BASE: //sort of a heartbeat packet
			/* Send the WS ID*/
			value.data_u32[0] = 0x00000000; //make sure there's no leftover data, clear all the bits
			value.data_u32[0] = 20; //for WaveSculptor 20
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_WAVESCULPTOR_TYPE, value.data_u32[0], timestamp);

			/* also craft a scandal heartbeat packet so scanalysis can detect it*/
			entry.priority = 0x06; //NETWORK_LOW
			entry.message_type = 0x02; //channel message
			entry.source_address = WAVESCULPTOR_SCANDAL_ADDRESS;
			entry.specifics = WS20; //Wavesculptor 20
			entry.value = 2560; //2560 integer = whatever all other nodes' heartbeats sent
			entry.scandal_timestamp = timestamp;
			entrytocan(&entry, &pkt);
			sc_pkts[1] = pkt;
			*sc_pkts_count = 2;
		break;

		case (WS20_MC_CAN_BASE + WS20_MC_STATUS): //status
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_ACTIVE_MOTOR, value.data_u16[2], timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS20_ERRORS, value.data_u16[1], timestamp);
			craft_scandal_packet(sc_pkts, 2, TELEM_HIGH, WS20_LIMITS, value.data_u16[0], timestamp);
			*sc_pkts_count = 3;
		break;

		case (WS20_MC_CAN_BASE + WS20_MC_BUS): //bus current & voltage
			craft_scandal_packet(sc_pkts, 0, TELEM_HIGH, WS20_BUSVOLT, (int32_t) (value.data_float[0] * 1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS20_BUSCURRENT, (int32_t)(value.data_float[1] * 1000.0), timestamp);
			*sc_pkts_count = 2;
		break;

		case WS20_MC_CAN_BASE + WS20_MC_VELOCITY: //velocity, converted to km/h
			craft_scandal_packet(sc_pkts, 0, TELEM_HIGH, WS20_VEHICLE_VELOCITY, (int32_t)(value.data_float[1] * 3600.0), timestamp);
			*sc_pkts_count = 1;
		break;

		case WS20_MC_CAN_BASE + WS20_MC_PHASE: //phase currents
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_PHASE_A, (int32_t) (value.data_float[1] *1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_LOW, WS20_PHASE_B, (int32_t) (value.data_float[0] *1000.0), timestamp);
			*sc_pkts_count = 2;
		break;

		case WS20_MC_CAN_BASE + WS20_MC_FAN: //fan speed (rpm)
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_FAN_SPEED_RPM, (int32_t) (value.data_float[1] * 1000.0), timestamp);
			*sc_pkts_count = 1;
		break;

		case WS20_MC_CAN_BASE + WS20_MC_TEMP_1: //heatsink & motor temp
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_HEATSINK_TEMP, (int32_t) (value.data_float[1] * 1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS20_MOTOR_TEMP, (int32_t) (value.data_float[0] * 1000.0), timestamp);
			*sc_pkts_count = 2;
		break;

		case WS20_MC_CAN_BASE + WS20_MC_TEMP_2: //air inlet temp, essentially ambient temp in the car
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_AMBIENT_AIR_IN_TEMP, (int32_t) (value.data_float[1] * 1000.0), timestamp);
			*sc_pkts_count = 1;
		break;

		case WS20_MC_CAN_BASE + WS20_MC_ODO: //currentint & odometer
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_AMP_HOURS, (int32_t) (value.data_float[1] * 1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_LOW, WS20_ODOMETER, (int32_t) (value.data_float[0]), timestamp);
			*sc_pkts_count = 2;
		break;
		}
	} //endif WS20

	/* If we have a WaveSculptor 22 */
	else if (tritium_device_type == WS22) {
		switch(tri_pkt->id & 0x07FF) { //Pick a handling code based on device+channel combination
		case WS22_MC_CAN_BASE: //sort of a heartbeat packet
			/* Send the WS ID*/
			value.data_u32[0] = 0x00000000; //make sure there's no leftover data, clear all the bits
			value.data_u32[0] = 22; //for WaveSculptor 22
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS22_WAVESCULPTOR_TYPE, value.data_u32[0], timestamp);

			/* also craft a scandal heartbeat packet so scanalysis can detect it*/
			entry.priority = 0x06; //NETWORK_LOW
			entry.message_type = 0x02; //channel message
			entry.source_address = WAVESCULPTOR_SCANDAL_ADDRESS;
			entry.specifics = WS22; //Wavesculptor 22
			entry.value = 2560; //2560 integer = whatever all other nodes' heartbeats sent
			entry.scandal_timestamp = timestamp;
			entrytocan(&entry, &pkt);
			sc_pkts[1] = pkt;
			*sc_pkts_count = 2;
		break;

		case (WS22_MC_CAN_BASE + WS22_MC_STATUS): //status
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS22_ACTIVE_MOTOR, value.data_u16[2], timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS22_ERRORS, value.data_u16[1], timestamp);
			craft_scandal_packet(sc_pkts, 2, TELEM_HIGH, WS22_LIMITS, value.data_u16[0], timestamp);
			craft_scandal_packet(sc_pkts, 3, TELEM_LOW, WS22_CAN_RX_ERROR_COUNT, value.data_u8[7], timestamp);
			craft_scandal_packet(sc_pkts, 4, TELEM_LOW, WS22_CAN_TX_ERROR_COUNT, value.data_u8[6], timestamp);
			*sc_pkts_count = 5;
		break;

		case (WS22_MC_CAN_BASE + WS22_MC_BUS): //bus current & voltage
			craft_scandal_packet(sc_pkts, 0, TELEM_HIGH, WS22_BUSVOLT, (int32_t) (value.data_float[0] * 1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS22_BUSCURRENT, (int32_t)(value.data_float[1] * 1000.0), timestamp);
			*sc_pkts_count = 2;
		break;

		case WS22_MC_CAN_BASE + WS22_MC_VELOCITY: //velocity, converted to km/h
			craft_scandal_packet(sc_pkts, 0, TELEM_HIGH, WS22_VEHICLE_VELOCITY, (int32_t)(value.data_float[1] * 3600.0), timestamp);
			*sc_pkts_count = 1;
		break;

		case WS22_MC_CAN_BASE + WS22_MC_PHASE: //phase currents
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS22_PHASE_A, (int32_t) (value.data_float[1] *1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_LOW, WS22_PHASE_B, (int32_t) (value.data_float[0] *1000.0), timestamp);
			*sc_pkts_count = 2;
		break;


		case WS22_MC_CAN_BASE + WS22_MC_TEMP1: //phase C & motor temp
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS22_IPM_PHASEC_TEMP, (int32_t) (value.data_float[1] * 1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS22_MOTOR_TEMP, (int32_t) (value.data_float[0] * 1000.0), timestamp);
			*sc_pkts_count = 2;
		break;

		case WS22_MC_CAN_BASE + WS22_MC_TEMP2: //phase B & dsp board
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS22_IPM_PHASEB_TEMP, (int32_t) (value.data_float[1] * 1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_LOW, WS22_DSP_BOARD_TEMP, (int32_t) (value.data_float[0] * 1000.0), timestamp);
			*sc_pkts_count = 2;
		break;

		case WS22_MC_CAN_BASE + WS22_MC_TEMP3: //phase a temp
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS22_IPM_PHASEA_TEMP, (int32_t) (value.data_float[1] * 1000.0), timestamp);
			*sc_pkts_count = 1;
		break;

		case WS22_MC_CAN_BASE + WS22_MC_CUMULATIVE: //currentint & odometer
			craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS22_AMP_HOURS, (int32_t) (value.data_float[1] * 1000.0), timestamp);
			craft_scandal_packet(sc_pkts, 1, TELEM_LOW, WS22_ODOMETER, (int32_t) (value.data_float[0]), timestamp);
			*sc_pkts_count = 2;
		break;
		}
	} //endif WS22
}




#endif
