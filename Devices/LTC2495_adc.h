/********************************************************************\

  Name:         LTC2495_adc.h
  Created by:   Noel Wu												 Jul/21/2008


  Contents:     This ADC provides the VBias and IBias readbacks,
  					 uses interrupt based SMB protocol.

  $Id$

\********************************************************************/

//  Need to have FEB64 defined
#ifdef _LTC2495_

#define LTC2495_CMD_IGNORE 	0x00 	// Don't care/leave alone
#define LTC2495_CMD_SELECT	0xA0	// Select an Input Channel command
#define LTC2495_CMD_SGL			0x10	// Single Ended (non-differential)
#define LTC2495_ENABLE2			0x80


//
//------------------------------------------------------------------------
void LTC2495_Init(void);

//
//------------------------------------------------------------------------
void LTC2495_StartConversion(unsigned char addr, unsigned char channel, unsigned char gain);

//
//------------------------------------------------------------------------
unsigned char  LTC2495_ReadConversion(unsigned char addr
                                    , unsigned char channel
                                    , signed long *pResult
                                    , unsigned char gain);
void dowhile(char *busy, char location);

#endif