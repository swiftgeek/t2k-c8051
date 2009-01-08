/********************************************************************\

  Name:         PCA9536_io.c
  Created by:   Brian Lee
  Modified by:	 Bahman Sotoodian							Feb/11/2008
  Modified by:	 Bryerton Shaw								Mar/15/2008

  Contents:     PCA9536 I/O register user interface

  $Id: PCA9536.c 62 2008-12-19 00:12:12Z midas $

\********************************************************************/
#ifdef _PCA9536_

#ifndef _SMB_PROTOCOL_
#define _SMB_PROTOCOL_
#endif

#include "../mscbemb.h"
#include "../Protocols/SMBus310_handler.h"
#include "PCA9536.h"

static unsigned long xdata  lcounter=0;

//
//------------------------------------------------------------------------
void PCA9536_Init(void) {
  SMBus_Init(); // SMBus initialization (should be called after pca_operation)
}

//
//------------------------------------------------------------------------
void PCA9536_WriteByte(unsigned char addr, unsigned char selectPort, unsigned char dataByte) {
	unsigned char buffer[2];

  buffer[0] = selectPort;
  buffer[1] = dataByte;

  watchdog_refresh(0);

	// Wait for the SMBus to clear
	SMBus_Wait();
  SMBus_SetSlaveAddr(addr, 0);
  SMBus_SetTXBuffer(buffer, 2);
  SMBus_SetRXBuffer(0, 0);
  SMBus_Start();
}

#if 0
//
//------------------------------------------------------------------------
void PCA9536_WriteWord(unsigned char addr, unsigned char selectPort, unsigned int dataWord) {
	unsigned char buffer[3];
  
  buffer[0] = selectPort;
  buffer[1] = (dataWord >> 8);
  buffer[2] = (dataWord & 0x00FF);

  watchdog_refresh(0);

	// Wait for the SMBus to clear
	SMBus_Wait();
  SMBus_SetSlaveAddr(addr, 0);
  SMBus_SetTXBuffer(buffer, 3);
  SMBus_SetRXBuffer(0, 0);
  SMBus_Start();
}

#endif 

//
//------------------------------------------------------------------------
void PCA9536_Read(unsigned char addr, unsigned char selectPort, unsigned char *pDataBytes, unsigned char dataLen) {
	watchdog_refresh(0);

	SMBus_Wait();
  SMBus_SetSlaveAddr(addr, 0);
  SMBus_SetTXBuffer(&selectPort, 1);
  SMBus_SetRXBuffer(pDataBytes, dataLen);
  SMBus_Start();
}

//
//------------------------------------------------------------------------
void PCA9536_Conversion(unsigned char *conversion){
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