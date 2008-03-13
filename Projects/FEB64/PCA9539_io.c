/********************************************************************\

  Name:         PCA9539_io.c
  Created by:   Brian Lee
  Modified by:	 Bahman Sotoodian							Feb/11/2008


  Contents:     PCA9539 I/O register user interface
  				This chip is used to enable different switches for
				the Charge Pump and aslo set the Backplane address

            -PAA- Currently no action is taken based on the SI return!!!!

  $Id: PCA9539_io.c 58 2008-02-28 22:40:33Z fgddaq $

\********************************************************************/
//  Need to have FEB64 defined

#ifdef _PCA9539_

#include "mscbemb.h"
#include "../protocols/SMBus_handler.h"
#include "PCA9539_io.h"

//
//------------------------------------------------------------------------
void PCA9539_Init(void)
{
	SMBus_Init(); // SMBus initialization (should be called after pca_operation)

//
//-PAA- This next 3 calls should be in the init() not in the driver as its
//      configuration specific.
//	PCA9539_Cmd(Bias_OUTPUT_ENABLE); 		// Put all pins 0 to Output modes
//	PCA9539_Cmd(Bias_ALL_LOW); 				// All Biases are enabled
	PCA9539_Cmd(BackPlane_INPUT_ENABLE); 	// Put all pins 1 to Input modes

}

//
//------------------------------------------------------------------------
void PCA9539_Cmd(unsigned char addr, unsigned char cmd, unsigned char datByte,unsigned char *varToBeWritten, bit flag)
{
	unsigned char xdata readValue = 0;
  bit nok;
	unsigned char xdata smberrflag = 0;
	watchdog_refresh(0);
	if(flag == PCA9539_READ) {
	   // Reading
	  	SMBus_Start(); 				    //make start bit
      delay_us(10);
		SMBus_WriteByte(addr, ADD_WRITE);
      delay_us(10);
		SMBus_WriteByte(cmd, CMD_CYCLE);
//    delay_us(10);
		SMBus_Stop();   					 // Clear communication
      delay_us(10);
		SMBus_Start(); 					 // Start read
      delay_us(10);
		SMBus_WriteByte(addr, ADD_READ);   // Send address with a Read Flag 
      delay_us(10);
		AA = 1; //Send ACK from Master for multiple read request
 		readValue = SMBus_ReadByte();  // receive the register's data
      *varToBeWritten = readValue;   // Receiving the first set of data from Port1
		delay_us(10);
      AA = 0;
 		readValue = SMBus_ReadByte();  // receive the register's data
      SMBus_Stop();
	} else {
	   // Writing 		
		nok = SMBus_Start();
      if (nok) { smberrflag = 1; goto err; }
	   delay_us(10);
	
		nok = SMBus_WriteByte(addr, ADD_WRITE);
      if (nok) { smberrflag = 2; goto err; }
      delay_us(10);

 		if (smberrflag == 0) nok=SMBus_WriteByte(cmd, CMD_CYCLE);
      if (nok) { smberrflag = 3; goto err; }
      delay_us(10);

//		AA = 1; //Send ACK from Master for multiple read request
		nok = SMBus_WriteByte(datByte, WRITE_CYCLE);
      if (nok) { smberrflag = 4; goto err; }
      delay_us(10);

//		AA = 0; //Send ACK from Master for multiple read request
 		nok = SMBus_WriteByte(datByte, WRITE_CYCLE);
      if (nok) { smberrflag = 5; goto err; }
      
		SMBus_Stop();
      delay_us(10);
err:		if (smberrflag != 0 )
   		delay_us(10);
	}

}
#endif