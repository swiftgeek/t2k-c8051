/**********************************************************************************\
Name:			SMBus_handler.c
Author:   	Bryerton Shaw 	
Created: 		March 11, 2008
Modified:	   Bahman Sotoodian
Description:	SMBus protocol using interrupts, created for FEB64 board

$Id: SMBus310_handler.c 73 2009-01-30 05:16:24Z midas $
\**********************************************************************************/

//-------------------------------------------------------------------
//Include files
//-------------------------------------------------------------------
#include "../mscbemb.h"
#include "SMBus310_handler.h"

// SMBus Global Variables
static unsigned char xdata SMB_DATA_OUT[SMB_MAX_BUFF_SIZE];
static unsigned char xdata SMB_DATA_OUT_LEN;
static unsigned char *pSMB_DATA_IN;
static unsigned char xdata SMB_DATA_IN_LEN;
static unsigned char xdata SMB_TARGET;
static bit SMB_BUSY;
static bit SMB_RW;
static bit SMB_ACKPOLL;

//
//-------------------------------------------------------------------
void SMBus_EnableACKPoll(void) {
  SMB_ACKPOLL = SMB_ENABLE_ACKPOLL;
}

//
//-------------------------------------------------------------------
void SMBus_SetSlaveAddr(unsigned char slaveAddr) {
  SMB_TARGET = slaveAddr;
}

//
//-------------------------------------------------------------------
void SMBus_SetTXBuffer(unsigned char *pData, unsigned char dataLen) {
  unsigned char i;

  for(i = 0; (i < dataLen) && (i < SMB_MAX_BUFF_SIZE); ++i) {
    SMB_DATA_OUT[i] = pData[i];
  }
  
  SMB_DATA_OUT_LEN = i;  
}

//
//-------------------------------------------------------------------
void SMBus_SetRXBuffer(unsigned char *pData, unsigned char dataLen) {
  pSMB_DATA_IN = pData; 
  SMB_DATA_IN_LEN = dataLen;
}

//
//-------------------------------------------------------------------
void SMBus_Wait(void) {
  while(SMB_BUSY);
}

//
//-------------------------------------------------------------------
void SMBus_Clear(void) {
  pSMB_DATA_IN = 0;
  SMB_DATA_IN_LEN = 0;
  SMB_DATA_OUT_LEN = 0;
  SMB_BUSY = 0;
  SMB_ACKPOLL = SMB_DISABLE_ACKPOLL;
}

//
//-------------------------------------------------------------------
void SMBus_Start(void) {
  if(SMB_DATA_OUT_LEN > 0) {
    SMB_RW = SMB_WRITE;
  } else if(SMB_DATA_IN_LEN > 0) {
    SMB_RW = SMB_READ;
  } else {
    return;
  }
  
  SMB_BUSY = 1;
  STA = 1;

  SMBus_Wait();
  SMBus_Clear();
}

//
//-------------------------------------------------------------------
/**
Initializing the SMBus
*/
void SMBus_Init(void) {
  static char init = 0;

  if(!init) {
    init = 1;

    // Configuring the Timer3 Registers
    TMR3CN = 0x00;	// Turn Clock off and SYSCLK / 12

    TMR3RLL = 0x00; /* TIMER 3 RELOAD LOW BYTE */
    TMR3RLH = 0x00; /* TIMER 3 RELOAD HIGH BYTE */
    TMR3L = 0x00;   //Timer3 Low Bytes
    TMR3H = 0x00;   //Timer3 High Bytes
    TMR3CN = 0x04;  // Enable Timer3

    SMB0CF = 0x5D;   // SMbus Setup Hold Extension/Timeout Detection/Free Timeout Detection Enable
    // Timer1 Overflow is set as the clock source
    SMB0CF |= 0x80;  // Enable SMBus after all setting done.


    EIE1 |= 0x81;	   // Enable SMBus interrupts/Enable Timer3 interrupts
    EA = 1;			 // Enable Global interrupts 
    
    SMBus_Clear();    
  }
}

//
//-------------------------------------------------------------------
void SMBus_ISR(void) interrupt 7{
  bit FAIL; 						 // Used by the ISR to flag failed
  // transfers
  static unsigned char data_in;
  static unsigned char data_out;	

  switch (SMB0CN & 0xF0)	    // Status vector
  {
    // Master Transmitter/Receiver: START condition transmitted.
  case SMB_MTSTA:
    SMB0DAT = (SMB_TARGET << 1) | SMB_RW; // Load address of the target slave and R/W bit
    STA = 0;					  // Manually clear START bit
    data_in = 0;
    data_out = 0;
    break;

    // Master Transmitter: Data byte transmitted
  case SMB_MTDB:
    if (ACK) {				     // Slave ACK?
      if (data_out < SMB_DATA_OUT_LEN) {	 	      						 		 
        if (SMB_RW==SMB_WRITE) {// If this transfer is a WRITE,
          SMB0DAT = SMB_DATA_OUT[data_out++]; // send data byte
        } else {
			// If this transfer is a READ, proceed with transfer
			// without writing to SMB0DAT (switch to receive mode)
        }		
      } else if(data_in < SMB_DATA_IN_LEN) {
        SMB_RW = SMB_READ;
        STO = 0;
        STA = 1;      
      } else {
        STO = 1;
        SMBus_Clear();
      }
    } else if (SMB_ACKPOLL) {			 				  
      STO = 1;
      STA = 1;  
    } else {	// If slave NACK, and we are not expecting anything to read
      FAIL = 1;	// Indicate failed transfer and handle at end of ISR											  
    }
    break;

    // Master Receiver: byte received
  case SMB_MRDB:
    *(pSMB_DATA_IN+data_in) = SMB0DAT;
    data_in++;						         					      								      
    if ( data_in >= SMB_DATA_IN_LEN) { 		     						  
      ACK = 0;                 // Send NACK to indicate last byte
      STO = 1;               // Send STOP to terminate transfer
      SMBus_Clear();
    } else {
      ACK = 1;
      STO = 0;
    }
    break;

  default:
    FAIL = 1; // Indicate failed transfer and handle at end of ISR											 
    break;
  } // end switch
	
  // If the transfer failed
  if (FAIL) {							 
    SMB0CN &= ~0x40; // Reset communication
    SMB0CN |= 0x40;
    SMBus_Clear();
    STA = 0;
    STO = 0;
    ACK = 0;
    FAIL = 0;
  }

  SI=0; // clear interrupt flag
}

//
//-------------------------------------------------------------------
void Timer3_ISR(void) interrupt 14 {
  SMB0CN &= ~0x40;	// Disable SMBus
  SMB0CN |= 0x40;	// Re-enable SMBus
  TMR3CN &= ~0x80;	// Clear Timer3 interrupt-pending flag
  SMBus_Clear();
}	
