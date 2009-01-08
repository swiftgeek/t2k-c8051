/********************************************************************\
  Name:         LTC2489_adc.h
  Created by:   Bahman Sotoodian								
  Date:         Apr/21/2008

  Contents:     This ADC uses interrupt based SMB protocol.
  $Id: LTC2489_adc.h 61 2008-12-19 00:11:12Z midas $
\********************************************************************/
// Need to have Generic_IO defines

#ifdef _LTC2489_

#define LTC2489_CMD_IGNORE 	    0x00 	// Don't care/leave alone
#define LTC2489_CMD_SELECT		0xA0	// Select an Input Channel command
#define LTC2489_CMD_SGL			0x10	// Single Ended (non-differential)

//
//------------------------------------------------------------------------
void LTC2489_Init(void);

//
//------------------------------------------------------------------------
void LTC2489_StartConversion(unsigned char addr, unsigned char channel);

//
//------------------------------------------------------------------------
unsigned char LTC2489_ReadConversion(unsigned char addr, unsigned char channel, signed long *pResult); 

#endif