/********************************************************************\

  Name:         LTC2497_adc.c
  Created by:   Bryerton Shaw								Mar/17/2008


  Contents:     This ADC provides the VBias and IBias readbacks,
  					 uses interrupt based SMB protocol.

  $Id: LTC2497_adc.c 104 2008-03-15 01:13:21Z bryerton $

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
void LTC2497_Read(unsigned char addr, unsigned char cmdByte, unsigned char* dataBytes, unsigned char dataLen) {
	watchdog_refresh(0);

	// Wait for the SMBus to clear
	while(SMB_BUSY);
	SMB_BUSY = 1;

	// Have Command Bytes to send, so set to write to start
	SMB_RW = SMB_WRITE;

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s)
	SMB_DATA_OUT_LEN = 1;
	SMB_DATA_OUT[0] = selectPort;

	// Setup Receive Buffer
	SMB_DATA_IN_LEN = dataLen;
	pSMB_DATA_IN = dataBytes;	

	// Start Communication and Block until completed
	SFRPAGE = SMB0_PAGE;
	STA = 1;
	while(SMB_BUSY);
}

#endif