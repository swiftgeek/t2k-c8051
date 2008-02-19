/**********************************************************************************\
  Name:         SMBus_handler.c
  Created by:   Bahman Sotoodian                Feb/11/2008


  Contents:     SMBus protocol for T2K-FGD experiment
          (using C8051F133's internal SMBus features)

  Version:    Rev 

  

  $Id:$
\**********************************************************************************/
// --------------------------------------------------------
//  Include files
// --------------------------------------------------------

#include "../feb64/mscbemb.h"

#ifdef _SMB_PROTOCOL_

#include "SMBus_handler.h"

// --------------------------------------------------------
//  SMBus_Initialization :
//      Bus Initialization sequence
// --------------------------------------------------------
void  SMBus_Init(void)
{
  // timer 1 is usually initialized by mscbmain.c
  // If you make no change in that file, you do not need to
  // activate the following two lines.
  // TMOD = (TMOD & 0x0F) | 0x20;     // 8 bit auto-reloaded mode
  // TR1 = 0;                         // Activate timer 1
  
  //BS    SMB0CF  = 0x55;     // SMBus configuration
  //BS    SMB0CF |= 0x80;     // Enable SMBus after all setting done.
  // SFRPAGE  = CONFIG_PAGE;
  // P0MDOUT |= 0x0C;

  // SMBus Clock Setup
  // SMBus enable, 
  // Sysclk as clock source
 	 SFRPAGE = TMR3_PAGE;
 	 TMR3CN  = 0x04; 	  //Enabling Timer 3 for Bus Free Timeout detection 	 
	 SFRPAGE = SMB0_PAGE;
 	 SMB0CN  = 0x03;     // SMBus Free timeor enable and SMBus Timeout Enable 
	 SMB0CR  = 0xD2;	 // Setting the SCL to 1MHz and Setting Bus Free Timeout period 18.5 us	  
     SMB0CN |= 0x40;     // Enable SMBus after all setting done.
	 
}

// --------------------------------------------------------
//  SMBus_Write_CD :
//      Write a single command and data word to the SMBus as a Master
//
//      Arguments :
//          slave_address   : 7-bit slave address
//          command         : command word
//          value           : data word
//
//      Return    :
//          0               : success
//          non-zero        : error
// --------------------------------------------------------
void SMBus_Start(void)
{
  SI = 0;
  STA = 1;    // make Start bit
  SMBus_SerialInterrupt(); // wait for SMBus interruption..
  STA = 0;    // clear start bit
}

void SMBus_WriteByte(unsigned char Byte, unsigned char RW_flag)
{
  SFRPAGE = SMB0_PAGE;	
  if(RW_flag == WRITE) //write flag on
  {
    SMB0DAT = (Byte << 1) & 0xFE;
  }
  else if(RW_flag == READ) //read flag on
  {
    SMB0DAT = (Byte << 1) | 0x01;
  }
  else //if it's just writing a value or command (CMD or VAL)
  {
    SMB0DAT = Byte; // Set the SMBus Data to be the desired Byte value
  }
  SI = 0;
  SMBus_SerialInterrupt();  // wait for SMBus interruption. sending the byte..
}

unsigned char SMBus_ReadByte(void)
{
  SI      = 0;  // clear interrupt flag
    SMBus_SerialInterrupt();    // wait SMBus interruption. receiving data word ...
  AA     = 0;        // send back NACK
    return SMB0DAT; // return the Received Byte
}

void SMBus_Stop(void)
{
  STO = 1;    // make Stop bit
  SI = 0;   // clear interrupt flag
}

bit SMBus_SerialInterrupt(void)
{
  unsigned char startTime = 0;

  startTime = time();

  while(SI == 0);
  {
    //if SI doesn't turn on for longer than the defined SIwaitTime
    if((time() - startTime) > SMBUS_SI_WAITTIME)
    {
      return 1; //return 1 as an indication for communication failure
    }
  }

  //if nothing goes wrong and SI does turn on
  return 0; //success
}

#endif
