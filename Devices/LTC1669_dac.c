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

//-------------------------------------------------------------------
//Include files
//-------------------------------------------------------------------
#include "../mscbemb.h"
#include "../Protocols/SMBus_handler.h"
#include "LTC1669_dac.h"

//
//-------------------------------------------------------------------
/**
Initializing the SMBus protocol communication
*/
//------------------------------------------------------------------------
//
void LTC1669_Init(void) {
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)
}

//------------------------------------------------------------------------
//
// void LTC1669_Cmd(unsigned char addr, unsigned char cmd) {

//------------------------------------------------------------------------
//
void LTC1669_SetDAC(unsigned char addr, unsigned char cmd, unsigned int dataWord) {
  unsigned char buffer[3];

  buffer[0] = cmd;
  buffer[1] = (dataWord & 0x00FF);
  buffer[2] = (dataWord >> 8);

  watchdog_refresh(0);

  // Wait for the SMBus to clear
  SMBus_Wait();
  SMBus_SetSlaveAddr(addr);
  SMBus_SetTXBuffer(buffer, 3);
  SMBus_SetRXBuffer(0, 0);
  SMBus_Start();
}

#endif