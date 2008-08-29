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

	// Wait for the SMBus to clear
//	while(SMB_BUSY);
	dowhile(&SMB_BUSY, 9);
	SMB_BUSY = 1;
	SMB_ACKPOLL = 1;

	// Have Command Bytes to send, so set to write to start
	SMB_RW = SMB_WRITE;

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s)
	SMB_DATA_OUT_LEN = 1;
	SMB_DATA_OUT[0] = cmd;

	// Setup Receive Buffer
	SMB_DATA_IN_LEN = 0;
	pSMB_DATA_IN = 0;	

	// Start Communication and Block until completed
	SFRPAGE = SMB0_PAGE;
	STA = 1;
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

	// Wait for the SMBus to clear
//	while(SMB_BUSY);
	dowhile(&SMB_BUSY, 8);
	SMB_BUSY = 1;

	SMB_RW = SMB_WRITE;
	SMB_ACKPOLL = 1; // keep trying until success!

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s)
	SMB_DATA_OUT_LEN = 1;
	SMB_DATA_OUT[0] = cmd;

	// Setup Receive Buffer
	SMB_DATA_IN_LEN = 3;
	pSMB_DATA_IN = (unsigned char*)&value;	

	// Start Communication and Block until completed
	SFRPAGE = SMB0_PAGE;
	STA = 1;

//     	while(SMB_BUSY);
	dowhile(&SMB_BUSY, 11);

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