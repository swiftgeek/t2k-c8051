/**********************************************************************************\
  Name:         SMBus_handler.c
  Created by:   Brian Lee                
  Modified by:  Bahman Sotoodian				Feb/11/2008

  Contents:     SMBus protocol for T2K-FGD experiment
          (using C8051F133's internal SMBus features)

  Version:    Rev 

  

  $Id:$
\**********************************************************************************/
// --------------------------------------------------------
//  Include files
// --------------------------------------------------------

#if defined(T2KASUM)
#include "../asum/mscbemb.h"
#endif
#if defined(FEB64)
#include "../feb64/mscbemb.h"
#endif

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

  // SMBus Clock Setup
  // SMBus enable, 
  // Sysclk as clock source
#if defined(T2KASUM)
      SMB0CF  = 0x55;     // SMBus configuration
      SMB0CF |= 0x80;     // Enable SMBus after all setting done.
#endif

#if defined(FEB64)
	 SFRPAGE  = CONFIG_PAGE;
    P0MDOUT |= 0x0C;
	 SFRPAGE = SMB0_PAGE;
	 SMB0CN  = 0x06;     // Free timeout detection 
	 SMB0CR  = 0xDF;	   // Setting the SCL to 1MHz and Setting Bus Free Timeout period 18.5 us	  
    SMB0CN |= 0x40;     // Enable SMBus after all setting done.
#endif
	 
}

// --------------------------------------------------------
//  SMBus_Write_CD :
//      Write a single command and data word to the SMBus as a Master
//
//          Arguments 			:
//          slave_address   : 7-bit slave address
//          command         : command word
//          value           : data word
//
//          Return   		 :
//          0               : success
//          non-zero        : error
// --------------------------------------------------------
//
//
//------------------------------------------------------------------------
bit SMBus_Start(void)
{
  bit ok;

  SFRPAGE = SMB0_PAGE;	
  SI = 0;
  STA = 1;    // make Start bit
  ok = SMBus_SerialInterruptCheck(); // wait for SMBus interruption.. 
  STA = 0;    // clear Start bit
  return ok;
}

//
//------------------------------------------------------------------------
bit SMBus_WriteByte(unsigned char Byte, unsigned char RW_flag)
{
  SFRPAGE = SMB0_PAGE;	
  if(RW_flag == ADD_WRITE) {
  // Address in Write mode
	 SMB0DAT = (Byte << 1) & 0xFE;
  } else if(RW_flag == ADD_READ) {
	// Address in read mode
    SMB0DAT = (Byte << 1) | 0x01;
  } else  {
  // Command or data
    SMB0DAT = Byte; 
  }

  SI = 0;
  return SMBus_SerialInterruptCheck(); // wait for SMBus interruption.. 	 
  
}

//
//------------------------------------------------------------------------
unsigned char SMBus_ReadByte(void)
{
  SFRPAGE = SMB0_PAGE;
  SI      = 0;  // clear interrupt flag
  SMBus_SerialInterruptCheck(); // wait for SMBus interruption..
  return SMB0DAT; // return the Received Byte
}

//
//------------------------------------------------------------------------
void SMBus_Stop(void)
{
  SFRPAGE = SMB0_PAGE;
  STO = 1;    // make Stop bit
  SI = 0;     // clear interrupt flag
}

//
//------------------------------------------------------------------------
bit SMBus_SerialInterruptCheck(void)
{
  unsigned char startTime = 0;

  startTime = time();

  SFRPAGE = SMB0_PAGE;	
  while (SI == 0) {
    //if SI doesn't turn on for longer than the defined SIwaitTime
    if((time() - startTime) > SMBUS_SI_WAITTIME) {
      //return 1 as an indication for communication failure
      return 1; 
    }
  }
  return 0; //success
}

#endif
