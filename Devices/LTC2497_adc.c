/********************************************************************\

  Name:         LTC2497_adc.c
  Created by:   Bryerton Shaw								Mar/17/2008


  Contents:     This ADC provides the VBias and IBias readbacks,
  					 uses interrupt based SMB protocol.

  $Id$

\********************************************************************/
//  Need to have FEB64 defined

#ifdef _LTC2497_

#ifndef _SMB_PROTOCOL_
#define _SMB_PROTOCOL_
#endif 

#include "../mscbemb.h"
#include "../Protocols/SMBus_handler.h"
#include "LTC2497_adc.h"

//
//------------------------------------------------------------------------
void LTC2497_Init(void) {
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)
}

//
//------------------------------------------------------------------------
void LTC2497_StartConversion(unsigned char addr, unsigned char channel) {
	unsigned char cmd;
	
	watchdog_refresh(0);

	cmd = (channel >> 1) | LTC2497_CMD_SGL | LTC2497_CMD_SELECT;
	if(channel & 0x01) {
		cmd |= 0x08;
	} 

  SMBus_Wait();
  SMBus_EnableACKPoll();
  SMBus_SetSlaveAddr(addr);
  SMBus_SetTXBuffer(&cmd, 1);
  SMBus_SetRXBuffer(0, 0);
  SMBus_Start();
}

//
//------------------------------------------------------------------------
unsigned char LTC2497_ReadConversion(unsigned char addr, 
unsigned char channel, signed long *pResult) {
	unsigned char cmd;
	unsigned char validRange = 1;
	signed long value;

	watchdog_refresh(0);

	cmd = (channel >> 1) | LTC2497_CMD_SGL | LTC2497_CMD_SELECT;
	if(channel & 0x01) {
		cmd |= 0x08;
	} 	

  SMBus_Wait();
  SMBus_EnableACKPoll();
  SMBus_SetSlaveAddr(addr);
  SMBus_SetTXBuffer(&cmd, 1);
  SMBus_SetRXBuffer(&value, 3);
  SMBus_Start();

	if(((value & 0xC0000000) == 0xC0000000) || ((value & 0xC0000000) == 0x00000000)) {
		// Over/Under-range
		validRange = 0;
	}

	if(!(value & 0x80000000)) {
		// extend sign
		value = value >> 14;
		value |= 0xFFFF0000;
	} else {
		value = value >> 14;
		value &= 0x0000FFFF;
	}

	*pResult = value;

	return validRange;
}


#endif