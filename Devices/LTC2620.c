/********************************************************************\

  Name:         LTC_dac.c
  Created by:   Bahman Sotoodian  								Feb/27/2007


  Contents:     LTC2620 DAC user interface

  $Id$

\********************************************************************/
#ifdef _LTC2620_

#ifndef _SPI_PROTOCOL_
#define _SPI_PROTOCOL_
#endif

#include "../mscbemb.h"
#include "../Protocols/SPI_handler.h"
#include "LTC2620.h"

//
//-----------------------------------------------------------------------
void LTC2620_Init(char chip)
{
   if (chip == 1) DAC_CS1 = 1; // Unselect LTC2620   
   if (chip == 2) DAC_CS2 = 1; // Unselect LTC2620   
}

//
//-----------------------------------------------------------------------
void LTC2620_Cmd(unsigned char cmd, unsigned char chip, unsigned char addr, unsigned int DataBeSent)
{
	  // Chip Select has to be low before the Clock is driven low 
		DAC_SCK = 0;
		if (chip == 1) DAC_CS1 = 0;
		if (chip == 2) DAC_CS2 = 0;
		SPI_WriteByte((cmd << 4) | addr);  // Send the command and address
		SPI_WriteUInt(DataBeSent << 4);    // 12bit DAC (LSB ignored) 
		if (chip == 1) DAC_CS1 = 1;
		if (chip == 2) DAC_CS2 = 1;
}
#endif