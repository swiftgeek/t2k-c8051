/********************************************************************\

  Name:         PCA9539_io.c
  Created by:   Brian Lee
  Modified by:	 Bahman Sotoodian							Feb/11/2008
  Modified by:	 Bryerton Shaw								Mar/15/2008


  Contents:     PCA9539 I/O register user interface
  				This chip is used to enable different switches for
				the Charge Pump and aslo set the Backplane address

  $Id$

\********************************************************************/
//  Need to have FEB64 defined

#ifdef _PCA9539_

#include "../mscbemb.h"
#include "../Protocols/SMBus_handler.h"
#include "PCA9539_io.h"

//
//------------------------------------------------------------------------
void PCA9539_Init(void) {
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)
}

//
//------------------------------------------------------------------------
void PCA9539_Write(unsigned char addr, unsigned char selectPort, unsigned int dataBytes, unsigned char dataLen) {
	watchdog_refresh(0);

	if(dataLen > 2) dataLen = 2;	// Safety Check

	// Wait for the SMBus to clear
	while(SMB_BUSY);
	SMB_BUSY = 1;

	// Have Command Bytes to send, so set to write to start
	SMB_RW = SMB_WRITE;

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s) and Transmission Length
	SMB_DATA_OUT_LEN = dataLen+1;
	SMB_DATA_OUT[0] = selectPort;

	// Offset one due to Command Byte
	if(dataLen == 1) {
		SMB_DATA_OUT[1] = dataBytes & 0x00FF;	// Take LSB
	} else {
		SMB_DATA_OUT[1] = (dataBytes & 0xFF00) >> 8;	// Take MSB
		SMB_DATA_OUT[2] = (dataBytes & 0x00FF);		// Take LSB
	}

	// Setup Receive Buffer (Empty)
	SMB_DATA_IN_LEN = 0;
	pSMB_DATA_IN = 0;	

	// Start Communication 
	SFRPAGE = SMB0_PAGE;
	STA = 1;
}

//
//------------------------------------------------------------------------
void PCA9539_Read(unsigned char addr, unsigned char selectPort, unsigned char* dataBytes, unsigned char dataLen) {
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