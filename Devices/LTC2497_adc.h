/********************************************************************\

  Name:         LTC2497_adc.h
  Created by:   Bryerton Shaw								Mar/17/2008


  Contents:     This ADC provides the VBias and IBias readbacks,
  					 uses interrupt based SMB protocol.

  $Id$

\********************************************************************/

//  Need to have FEB64 defined
#ifdef _LTC2497_

#define LTC2497_CMD_IGNORE 	0x00 	// Don't care/leave alone
#define LTC2497_CMD_SELECT		0xA0	// Select an Input Channel command
#define LTC2497_CMD_SGL			0x10	// Single Ended (non-differential)

//
//------------------------------------------------------------------------
void LTC2497_Init(void);

//
//------------------------------------------------------------------------
void LTC2497_StartConversion(unsigned char addr, unsigned char channel);

//
//------------------------------------------------------------------------
unsigned char  LTC2497_ReadConversion(unsigned char addr, unsigned char channel, 
signed long *pResult);

#endif