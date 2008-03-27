/********************************************************************\

  Name:         PCA9539_io.c
  Created by:   Brian Lee
  Modified by:	 Bahman Sotoodian							Feb/11/2008
  Modified by:	 Bryerton Shaw								Mar/15/2008


  Contents:     PCA9539 I/O register user interface
  				This chip is used to enable different switches for
				the Charge Pump and also set the Backplane address

  $Id$

\********************************************************************/
//  Need to have FEB64 defined

#ifdef _PCA9539_

#ifndef _SMB_PROTOCOL_
#define _SMB_PROTOCOL_
#endif

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
void PCA9539_WriteByte(unsigned char addr, unsigned char selectPort, unsigned char dataByte) {
	watchdog_refresh(0);

	// Wait for the SMBus to clear
	while(SMB_BUSY);
	SMB_BUSY = 1;

	// Have Command Bytes to send, so set to write to start
	SMB_RW = SMB_WRITE;

	// Set Slave Address
	SMB_TARGET = addr;

	// Setup Command Byte(s) and Transmission Length
	SMB_DATA_OUT_LEN = 2;
	SMB_DATA_OUT[0] = selectPort;

	// Offset one due to Command Byte
	SMB_DATA_OUT[1] = dataByte;

	// Setup Receive Buffer (Empty)
	SMB_DATA_IN_LEN = 0;
	pSMB_DATA_IN = 0;	

	// Start Communication 
	SFRPAGE = SMB0_PAGE;
	STA = 1;
}


#if 0

//
//------------------------------------------------------------------------
void PCA9539_WriteWord(unsigned char addr, unsigned char selectPort, unsigned int dataWord) {
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
	SMB_DATA_OUT[0] = selectPort;

	// Offset one due to Command Byte
	SMB_DATA_OUT[1] = (dataWord >> 8);
	SMB_DATA_OUT[2] = (dataWord & 0x00FF);

	// Setup Receive Buffer (Empty)
	SMB_DATA_IN_LEN = 0;
	pSMB_DATA_IN = 0;	

	// Start Communication 
	SFRPAGE = SMB0_PAGE;
	STA = 1;
}

#endif 

//
//------------------------------------------------------------------------
void PCA9539_Read(unsigned char addr, unsigned char selectPort, unsigned char *pDataBytes, unsigned char dataLen) {
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
	pSMB_DATA_IN = pDataBytes;	

	// Start Communication and Block until completed
	SFRPAGE = SMB0_PAGE;
	STA = 1;
	while(SMB_BUSY);
}
void PCA9539_Conversion(unsigned char *conversion){
	unsigned char xdata temporary=0;
	 
	if(*conversion & 0x01) temporary |= 0x80;
	if(*conversion & 0x02) temporary |= 0x04;
	if(*conversion & 0x04) temporary |= 0x10;
	if(*conversion & 0x08) temporary |= 0x01;
	if(*conversion & 0x10) temporary |= 0x40;
	if(*conversion & 0x20) temporary |= 0x08;
	if(*conversion & 0x40) temporary |= 0x20;
	if(*conversion & 0x80) temporary |= 0x02;
	
	*conversion = temporary;
}
#endif