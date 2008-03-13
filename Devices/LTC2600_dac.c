/********************************************************************\

  Name:         LTC_dac.c
  Created by:   Bahman Sotoodian  								Feb/27/2007


  Contents:     LTC1665/2600 DAC user interface

  $Id:$

\********************************************************************/
//  need to have FEB64 defined

#ifdef _LTC2600_

#include "../mscbemb.h"
#include "../Protocols/SPI_handler.h"
#include "LTC2600_dac.h"


void LTC2600_Init(void)
{
   SUM_CSn  = 1; // Unselect LTC2600   
}

void LTC2600_Cmd(unsigned char cmd, unsigned char addr, unsigned int DataBeSent)
{
		 //Chip Select has to be low before the Clock is driven low 
		DAC_SCK = 0;
		SUM_CSn = 0;
		SPI_WriteByte((cmd << 4) | addr); //Send the command and address
		SPI_WriteUInt(DataBeSent);
		SUM_CSn = 1; //unselect LTC2600 after communication is done	
}

#endif