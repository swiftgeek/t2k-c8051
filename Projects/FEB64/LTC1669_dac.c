/********************************************************************\

  Name:         LTC1669_dac.c
  Created by:   Bahman Sotoodian  								Feb/19/2007


  Contents:     LTC1669 DAC user interface

  $Id: LTC1669_dac.c 58 2008-02-28 22:40:33Z fgddaq $

\********************************************************************/
//  need to have FEB64 defined

#ifdef _LTC1669_

#include "mscbemb.h"
#include "../protocols/SMBus_handler.h"
#include "LTC1669_dac.h"

void LTC1669_Init(void)
{
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)
}

void LTC1669_Cmd1(unsigned char addr, unsigned int input, bit flag)
{
	unsigned char xdata readValue = 0;
	watchdog_refresh(0);
	if(flag == LTC1669_READ) //reading
	{
		//The SMBus_WriteByte function would write in the SMBus Data Register and would 
		//specify the next operation. If Read has been passed to it, the slave would be ready for 
		//the read operation and if the Write has been passed in the master would be ready for write operation
		
		SMBus_Start(); //make start bit
		SMBus_WriteByte(addr, READ_CYCLE); //send address with a Read flag on 	    
//		if (!AA) return; // No slave ACK. Address is wrong or slave is missing.	    
		readValue = SMBus_ReadByte();  // set data word
		//store to mscb user_data here
	    SMBus_Stop();
	}
	else //writing
	{		
		SMBus_Start();
		SMBus_WriteByte(addr, WRITE_CYCLE);
//		if(!AA) return;
		SMBus_WriteByte(Command, CMD_CYCLE);
//		if(!AA) return;
		SMBus_WriteByte(input, WRITE_CYCLE);
//		if(!AA) return;
		SMBus_WriteByte((input>>8), WRITE_CYCLE);
//		if(!AA) return;
		SMBus_Stop();
	}
}
#endif