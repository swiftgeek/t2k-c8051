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

//------------------------------------------------------------------------
//
void PCA9539_Init(void) {
  SMBus_Init(0); // SMBus initialization (should be called after pca_operation)
}

//------------------------------------------------------------------------
//
void PCA9539_WriteByte(unsigned char addr, unsigned char selectPort, unsigned char dataByte) {
  unsigned char buffer[2];

  buffer[0] = selectPort;
  buffer[1] = dataByte;

  watchdog_refresh(0);

  // Wait for the SMBus to clear
  SMBus_Wait();
  SMBus_SetSlaveAddr(addr);
  SMBus_SetTXBuffer(buffer, 2);
  SMBus_SetRXBuffer(0, 0);
  SMBus_Start();
}

//------------------------------------------------------------------------
//
//void PCA9539_WriteWord(unsigned char addr, unsigned char selectPort, unsigned int dataWord) {

//------------------------------------------------------------------------
//
void PCA9539_Read(unsigned char addr, unsigned char selectPort, unsigned char *pDataBytes, unsigned char dataLen) {
  watchdog_refresh(0);

  SMBus_Wait();
  SMBus_SetSlaveAddr(addr);
  SMBus_SetTXBuffer(&selectPort, 1);
  SMBus_SetRXBuffer(pDataBytes, dataLen);
  SMBus_Start();
}

//------------------------------------------------------------------------
//
void PCA9539_Conversion(unsigned char *conversion) {
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