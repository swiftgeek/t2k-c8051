/********************************************************************\

  Name:         PCA9539_io.c
  Created by:   Bahman Sotoodian							Feb/11/2008


  Contents:     PCA9539 I/O register user interface
  				This chip is used to enable different switches for
				the Charge Pump and aslo set the Backplane address

  $Id$

\********************************************************************/
//  Need to have FEB64 defined

#ifdef _PCA9539_

#include "mscbemb.h"
#include "../protocols/SMBus_handler.h"
#include "PCA9539_io.h"

void PCA9539_Init(void)
{
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)
	PCA9539_Cmd(Bias_OUTPUT_ENABLE); 		// Put all pins 0 to Output modes
	PCA9539_Cmd(Bias_ALL_LOW); 				// All Biases are enabled
	PCA9539_Cmd(BackPlane_OUTPUT_ENABLE); 	// Put all pins 1 to Output modes
	PCA9539_Cmd(BackPlane_ALL_LOW); 		   // All Backplane Addr are enabled
}

void PCA9539_Cmd(unsigned char addr, unsigned char cmd, unsigned char datByte, bit flag)
{
	unsigned char readValue = 0;
	watchdog_refresh(0);
	if(flag == PCA9539_READ) //reading
	{
		SMBus_Start(); //make start bit
		SMBus_WriteByte(addr, WRITE);
		if (!AA) return; // No slave ACK. Address is wrong or slave is missing.
		SMBus_WriteByte(cmd, CMD);
		if (!AA) return;
		SMBus_Start(); //make repeated start bit
		SMBus_WriteByte(addr, READ); //send address with a Write flag on 
	    if (!AA) return; // No slave ACK. Address is wrong or slave is missing.
	    readValue = SMBus_ReadByte();  // receive the register's data
		//store to mscb user_data here
	    
		//Added by Bahman Sotoodin to update the D_BiasEn
		datByte = readValue;
		
		SMBus_Stop();
	}
	else //writing
	{		
		SMBus_Start();
		SMBus_WriteByte(addr, WRITE);
//		if(!AA) return;
		SMBus_WriteByte(cmd, CMD);
//		if(!AA) return;
		SMBus_WriteByte(datByte, VAL);
//		if(!AA) return;
		SMBus_Stop();
	}
}
#endif