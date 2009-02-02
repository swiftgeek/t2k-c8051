/********************************************************************\

  Name:         LTC2495_adc.c
  Created by:   Noel Wu												Jul/21/2008

  Contents:     This ADC provides the VBias and IBias readbacks,
  					 uses interrupt based SMB protocol.

  $Id$

\********************************************************************/
//  Need to have FEB64 defined

#ifdef _LTC2495_

#ifndef _SMB_PROTOCOL_
#define _SMB_PROTOCOL_
#endif 

#include "../mscbemb.h"
#include "../Protocols/SMBus_handler.h"
#include "LTC2495_adc.h"

//------------------------------------------------------------------------
//
void LTC2495_Init(void) {
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)
}

//------------------------------------------------------------------------
//
void LTC2495_StartConversion(unsigned char addr, unsigned char channel, unsigned char gain) {
  unsigned char xdata buffer[2];
	
	watchdog_refresh(0);

	buffer[0] = (channel >> 1) | LTC2495_CMD_SGL | LTC2495_CMD_SELECT;
	if(channel & 0x01) buffer[0] |= 0x08; 

  //	buffer[1] = LTC2495_ENABLE2 | gain;
 	buffer[1] = LTC2495_ENABLE2 | LTC2495_CMD2_SPD | gain;  //DB - set speed to x2

  watchdog_refresh(0);

  // Wait for the SMBus to clear
  SMBus_Wait();
  SMBus_SetSlaveAddr(addr);   
  SMBus_SetTXBuffer(buffer, 2);  // 2 cmds
  SMBus_SetRXBuffer(0, 0);
  SMBus_EnableACKPoll();
  SMBus_Start();
}

//------------------------------------------------------------------------
//
unsigned char LTC2495_ReadConversion(unsigned char addr
                                   , unsigned char channel
                                   , signed long *pResult
                                   , unsigned char gain) {
  unsigned char xdata buffer[2];
	unsigned char validRange = 1;
	signed long value = 0;

	watchdog_refresh(0);

	buffer[0] = (channel >> 1) | LTC2495_CMD_SGL | LTC2495_CMD_SELECT;
	if(channel & 0x01) buffer[0] |= 0x08;

  // buffer[1] = LTC2495_ENABLE2 | gain;
	buffer[1] = LTC2495_ENABLE2 | LTC2495_CMD2_SPD | gain;  //DB - set speed to x2

  // Wait for the SMBus to clear
  SMBus_Wait();
  SMBus_SetSlaveAddr(addr);   
  SMBus_SetTXBuffer(buffer, 2);  // 2 cmds
  SMBus_SetRXBuffer((unsigned char *)&value, 3);
  SMBus_EnableACKPoll();
  SMBus_Start();

/*
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
*/
  // shift 8 for 32 to 24 filled from the left
  // shift 6 for zero of the data register
	*pResult = value>>14;
  validRange = 1;
	return validRange;
}
#endif
