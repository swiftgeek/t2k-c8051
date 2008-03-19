/********************************************************************\

  Name:         LTC1669_dac.c
  Created by:   Bahman Sotoodian  								Feb/19/2007


  Contents:     LTC1669 DAC user interface

  $Id$

\********************************************************************/
//  need to have FEB64 defined

#ifdef _LTC1669_

#ifndef _SMB_PROTOCOL_
#define _SMB_PROTOCOL_
#endif

#include "../mscbemb.h"
#include "../Protocols/SMBus_handler.h"
#include "LTC1669_dac.h"

void LTC1669_Init(void) {
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)
}

#if 0 

void LTC1669_Cmd(unsigned char addr, unsigned char cmd) {
	watchdog_refresh(0);

	// Wait for the SMBus to clear
	while(SMB_BUSY);
	SMB_BUSY = 1;

	// Have Command Bytes to send, so set to write to start
	SMB_RW = SMB_WRITE;

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s) and Transmission Length
	SMB_DATA_OUT_LEN = 1;
	SMB_DATA_OUT[0] = cmd;

	// Setup Receive Buffer (Empty)
	SMB_DATA_IN_LEN = 0;
	pSMB_DATA_IN = 0;	

	// Start Communication 
	SFRPAGE = SMB0_PAGE;
	STA = 1;		
}

#endif

void LTC1669_SetDAC(unsigned char addr, unsigned char cmd, unsigned int dataWord) {
	watchdog_refresh(0);

	// Wait for the SMBus to clear
	while(SMB_BUSY);
	SMB_BUSY = 1;

	// Have Command Bytes to send, so set to write to start
	SMB_RW = SMB_WRITE;

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s) and Transmission Length
	SMB_DATA_OUT_LEN = 3;
	SMB_DATA_OUT[0] = cmd;
	SMB_DATA_OUT[1] = (dataWord & 0x00FF);			// LSB
	SMB_DATA_OUT[2] = (dataWord >> 8);				// MSB

	// Setup Receive Buffer (Empty)
	SMB_DATA_IN_LEN = 0;
	pSMB_DATA_IN = 0;	

	// Start Communication 
	SFRPAGE = SMB0_PAGE;
	STA = 1;	
}

#endif