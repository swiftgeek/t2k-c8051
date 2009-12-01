/********************************************************************\
  Name:         LTC2489_adc.c
  Created by:   Bahman Sotoodian								
  Date:         Apr/21/2008

  Contents:     This ADC uses interrupt based SMB protocol.
  $Id: LTC2489_adc.c 61 2008-12-19 00:11:12Z midas $
\********************************************************************/
//  Need to have Generic_IO defined

#ifdef _LTC2489_

#include "../mscbemb.h"
#include "../Protocols/SMBus310_handler.h"
#include "LTC2489_adc.h"

//
//------------------------------------------------------------------------
void LTC2489_Init(void) {
	SMBus_Init(0); // SMBus initialization (should be called after pca_operation)
}

//
//------------------------------------------------------------------------
void LTC2489_StartConversion(unsigned char addr, unsigned char channel) {
	unsigned char cmd;
	
	watchdog_refresh(0);

	cmd = (channel >> 1) | LTC2489_CMD_SGL | LTC2489_CMD_SELECT;
	if(channel & 0x01) {
		cmd |= 0x08;
	} 

	SMBus_Wait();
  SMBus_SetSlaveAddr(addr, 1);
  SMBus_SetTXBuffer(&cmd, 1);
  SMBus_SetRXBuffer(0, 0);
  SMBus_Start();
}

//
//------------------------------------------------------------------------
unsigned char LTC2489_ReadConversion(unsigned char addr, 
unsigned char channel, signed long *pResult) {
	unsigned char cmd;
	unsigned char validRange = 1;
	signed long value;

	watchdog_refresh(0);

	cmd = (channel >> 1) | LTC2489_CMD_SGL | LTC2489_CMD_SELECT;
	if(channel & 0x01) {
		cmd |= 0x08;
	} 	

	SMBus_Wait();
  SMBus_SetSlaveAddr(addr, 1);
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