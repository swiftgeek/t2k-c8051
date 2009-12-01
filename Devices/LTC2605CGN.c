/********************************************************************\

  Name:         LTC2605CGN.c
  Created by:	 Bahman Sotoodian							Apr/18/2008

  Contents: LTC2605CGN DAQ user interface
  				
  $Id: LTC2605CGN.c 61 2008-12-19 00:11:12Z midas $

\********************************************************************/

#ifdef _LTC2605_

#ifndef _SMB_PROTOCOL_
#define _SMB_PROTOCOL_
#endif

#include "../mscbemb.h"
#include "../Protocols/SMBus310_handler.h"
#include "LTC2605CGN.h"

//
//------------------------------------------------------------------------
void LTC2605_Init(void) {
	SMBus_Init(0); // SMBus initialization (should be called after pca_operation)
}

//
//------------------------------------------------------------------------
void LTC2605_WriteByte(unsigned char addr, unsigned char selectPort, unsigned int dataByte) {
	//BS I am not sure!
//	watchdog_refresh(0);
  unsigned char buffer[3];

  buffer[0] = selectPort;
  buffer[1] = (unsigned char )(dataByte >> 8);
  buffer[2] = (unsigned char)(dataByte & 0x00FF);

	// Wait for the SMBus to clear
//  watchdog_disable();
	SMBus_Wait();
  SMBus_SetSlaveAddr(addr, 0);
  SMBus_SetTXBuffer(buffer, 3);
  SMBus_SetRXBuffer(0, 0);
  SMBus_Start();
}

#endif 