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

extern  unsigned long xdata smbdebug;


//
//------------------------------------------------------------------------
void LTC2495_Init(void) {
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)
}

//
//------------------------------------------------------------------------
void LTC2495_StartConversion(unsigned char addr, unsigned char channel, unsigned char gain) {
	unsigned char cmd, cmd2;
	
	watchdog_refresh(0);

	cmd = (channel >> 1) | LTC2495_CMD_SGL | LTC2495_CMD_SELECT;
	if(channel & 0x01) {
		cmd |= 0x08;
	} 

	cmd2= LTC2495_ENABLE2 | gain;
// 	cmd2= LTC2495_ENABLE2 | LTC2495_CMD2_SPD | gain;  //DB - set speed to x2

	// Wait for the SMBus to clear
	dowhile(&SMB_BUSY,2);
	SMB_BUSY = 1;
	SMB_ACKPOLL = 1;

	// Have Command Bytes to send, so set to write to start
	SMB_RW = SMB_WRITE;

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s)
	SMB_DATA_OUT_LEN = 2;
	SMB_DATA_OUT[0] = cmd;
	SMB_DATA_OUT[1] = cmd2;

	// Setup Receive Buffer
	SMB_DATA_IN_LEN = 0;
	pSMB_DATA_IN = 0;	

	// Start Communication and Block until completed
	SFRPAGE = SMB0_PAGE;
	STA = 1;
}

//
//------------------------------------------------------------------------
unsigned char LTC2495_ReadConversion(unsigned char addr, 
unsigned char channel, signed long *pResult, unsigned char gain) {
	unsigned char cmd, cmd2;
	unsigned char validRange = 1;
	signed long value;

	watchdog_refresh(0);

	cmd = (channel >> 1) | LTC2495_CMD_SGL | LTC2495_CMD_SELECT;
	if(channel & 0x01) {
		cmd |= 0x08;
	} 	

//	cmd2= LTC2495_ENABLE2 | gain;
	cmd2= LTC2495_ENABLE2 | LTC2495_CMD2_SPD | gain;  //DB - set speed to x2

	// Wait for the SMBus to clear
//   while(SMB_BUSY);
	dowhile(&SMB_BUSY, 5);
	SMB_BUSY = 1;
	SMB_RW = SMB_WRITE;
	SMB_ACKPOLL = 1; // keep trying until success!

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s)
	SMB_DATA_OUT_LEN = 2;
	SMB_DATA_OUT[0] = cmd;
	SMB_DATA_OUT[1] = cmd2;

	// Setup Receive Buffer
	SMB_DATA_IN_LEN = 3;
	pSMB_DATA_IN = (unsigned char*)&value;	

	// Start Communication and Block until completed
	SFRPAGE = SMB0_PAGE;
	STA = 1;

//  	while(SMB_BUSY);
	dowhile(&SMB_BUSY, 10);

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