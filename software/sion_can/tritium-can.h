/*
* Project: SION (Sunswift IV Observer Node)
* File name: tritium-can.h
* Author: Irving Tjiptowarsono
* Description:
* This file contains the code used to convert 
* tritium packets from the wavesculptor
* into scandal packets,
* complete with address, node etc.
*
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* 
*/

#ifndef __TRITIUM_DEFINED__
#define __TRITIUM_DEFINED__

#include "scandal.h" /* scandal.h from SION. */
#include "lpc17xx_can.h"
//#include "scandal_timestamp.h" //also from SION

//#define __INTEGRATE_SCANDAL__
#ifdef __INTEGRATE_SCANDAL__
#include "scandal_devices.h" //from scandal.
#include "scandal_timer.h"

#else

//Scandal node type
#define WS20	36

/* WaveSculptor 20 out channels  / broadcast messages */
//none of them should be scaled at all.
//roughly in the order they were listed in the tritium docs.
#define WS20_TRITIUM_HEARTBEAT		0
#define WS20_ACTIVE_MOTOR			1
#define WS20_ERRORS					2
#define WS20_LIMITS					3
#define WS20_BUSCURRENT				4
#define WS20_BUSVOLT				5
#define WS20_VEHICLE_VELOCITY		6
#define WS20_PHASE_A				7
#define WS20_PHASE_B				8
//motor vector and random voltage isnt useful to us
#define WS20_FAN_SPEED_RPM			9
#define WS20_HEATSINK_TEMP			10
#define WS20_MOTOR_TEMP				11
#define WS20_AMBIENT_AIR_IN_TEMP	12
#define WS20_AMP_HOURS				13
#define WS20_ODOMETER				14


/* In channels */
//most of these should be controlled from the driver controls; 
//eg you should not actually send stuff to them.
//#define WS20_MOTOR_DRIVE_COMMAND	0
//#define WS20_MOTOR_POWER_COMMAND	1
#define WS20_RESET_COMMAND			2

/* Configuration commands */
#define WS20_CHANGE_ACTIVE_MOTOR	0


#endif // __INTEGRATE_SCANDAL__

#define WAVESCULPTOR_SCANDAL_ADDRESS 12
#define SCANDAL_NODE_TYPE	WS20

/* Tritium definitions */

#define MC_CAN_BASE	0x0400
#define MC_STATUS	0x01
#define MC_BUS		0x02
#define MC_VELOCITY	0x03
#define MC_PHASE	0x04
#define MC_V_VECTOR	0x05
#define MC_I_VECTOR	0x06
#define MC_BACKEMF	0x07
#define MC_RAIL_1	0x08
#define MC_RAIL_2	0x09
#define MC_FAN		0x0A
#define MC_TEMP_1	0x0B
#define MC_TEMP_2	0x0C
#define MC_TEMP_3	0x0D
#define MC_ODO		0x0E

//#define WS20_CONFIG_REQUEST(ba)		(ba+18)
//#define WS20_CONFIG_REPLY(ba)		(ba+19)
//#define WS20_CONFIG_DATAOUT(ba)		(ba+20)
//#define WS20_CONFIG_DATAIN(ba)		(ba+21)

/* 
maximum possible number of different scandal channels (information)
contained in a tritium channel - in this case it's 3 
since we want 3 channels out from the status packet.
*/
#define MAX_SC_CHN_PER_TRI_CHN	3

static inline void craft_scandal_packet(can_pkt *sc_pkts, uint8_t index, uint8_t priority, uint16_t channel_number, int32_t value, uint32_t timestamp) {
	can_pkt pkt;
	sion_entry entry;
	entry.priority = priority;
	entry.message_type = 0x00; //channel message
	entry.source_address = WAVESCULPTOR_SCANDAL_ADDRESS;
	entry.specifics = channel_number;
	entry.value = value;
	entry.scandal_timestamp = timestamp;
	entrytocan(&entry, &pkt);
	sc_pkts[index] = pkt;
}

static inline void tritium_to_scandal_packet(CAN_MSG_Type *tri_pkt, can_pkt *sc_pkts, uint8_t *sc_pkts_count) {
	uint32_t i, timestamp;
	union value_t {
	uint8_t data_u8[8];
	uint16_t data_u16[4];
	uint64_t data_u64;
	float data_float[2];
	} value;

	#ifdef __INTEGRATE_SCANDAL__
	timestamp = scandal_get_realtime32();
	#else
	timestamp = 31337; //FIXME: Get proper timing info
	#endif

	*sc_pkts_count = 0; //at first, assume nothing in this packet is useful

	//load tritium packet to temp struct
	
	for (i=0; i<4; i++) {
		value.data_u8[i] = tri_pkt->dataA[i];
	}
	for (i=0; i<4; i++) {
		value.data_u8[i+4] = tri_pkt->dataB[i];
	}


	switch(tri_pkt->id & 0x07FF) {
/*
	case MC_CAN_BASE: //sort of a heartbeat packet
		craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_TRITIUM_HEARTBEAT, value, timestamp);
		*sc_pkts_count = 1;
	break;
*/
	case MC_CAN_BASE + MC_STATUS: //status
		craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_ACTIVE_MOTOR, value.data_u16[2], timestamp);
		craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS20_ERRORS, value.data_u16[1], timestamp);
		craft_scandal_packet(sc_pkts, 2, TELEM_HIGH, WS20_LIMITS, value.data_u16[0], timestamp);
		*sc_pkts_count = 3;
	break;

	case MC_CAN_BASE + MC_BUS: //bus current & voltage
		craft_scandal_packet(sc_pkts, 0, TELEM_HIGH, WS20_BUSVOLT, (int32_t)(value.data_float[0] * 1000.0), timestamp);
		craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS20_BUSCURRENT, (int32_t)(value.data_float[1] * 1000.0), timestamp);
		*sc_pkts_count = 2;
	break;

	case MC_CAN_BASE + MC_VELOCITY: //velocity, converted to km/h
		craft_scandal_packet(sc_pkts, 0, TELEM_HIGH, WS20_VEHICLE_VELOCITY, (int32_t)(value.data_float[1] * 3600.0), timestamp);
		*sc_pkts_count = 1;
	break;

	case MC_CAN_BASE + MC_PHASE: //phase currents
		craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_PHASE_A, (int32_t) (value.data_float[1] *1000.0), timestamp);
		craft_scandal_packet(sc_pkts, 1, TELEM_LOW, WS20_PHASE_B, (int32_t) (value.data_float[0] *1000.0), timestamp);
		*sc_pkts_count = 2;
	break;

	case MC_CAN_BASE + MC_FAN: //fan speed (rpm)
		craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_FAN_SPEED_RPM, (int32_t) (value.data_float[1] * 1000.0), timestamp);
		*sc_pkts_count = 1;
	break;

	case MC_CAN_BASE + MC_TEMP_1: //heatsink & motor temp
		craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_HEATSINK_TEMP, (int32_t) (value.data_float[1] * 1000.0), timestamp);
		craft_scandal_packet(sc_pkts, 1, TELEM_HIGH, WS20_MOTOR_TEMP, (int32_t) (value.data_float[0] * 1000.0), timestamp);
		*sc_pkts_count = 2;
	break;

	case MC_CAN_BASE + MC_TEMP_2: //air inlet temp, essentially ambient temp in the car
		craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_AMBIENT_AIR_IN_TEMP, (int32_t) (value.data_float[1] * 1000.0), timestamp);
		*sc_pkts_count = 1;
	break;

	case MC_CAN_BASE + MC_ODO: //currentint & odometer
		craft_scandal_packet(sc_pkts, 0, TELEM_LOW, WS20_AMP_HOURS, (int32_t) (value.data_float[1] * 1000.0), timestamp);
		craft_scandal_packet(sc_pkts, 1, TELEM_LOW, WS20_ODOMETER, (int32_t) (value.data_float[0] * 1000.0), timestamp);
		*sc_pkts_count = 2;
	break;

	}

}




#endif
