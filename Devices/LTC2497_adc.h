/********************************************************************\

  Name:         LTC2497_adc.h
  Created by:   Bryerton Shaw								Mar/17/2008


  Contents:     This ADC provides the VBias and IBias readbacks,
  					 uses interrupt based SMB protocol.

  $Id: LTC2497_adc.h 104 2008-03-15 01:13:21Z bryerton $

\********************************************************************/

//  Need to have FEB64 defined
#ifdef _LTC2497_

//
//------------------------------------------------------------------------
void LTC2497_Init(void);

//------------------------------------------------------------------------
void LTC2497_Read(unsigned char addr, unsigned char cmdByte, unsigned char* dataBytes, unsigned char dataLen);

#endif