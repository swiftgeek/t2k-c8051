/**********************************************************************************\
Name:   SMBus_handler.c
Author:    Bryerton Shaw  
Created:   March 11, 2008
Description: SMBus protocol using interrupts, created for FEB64 board

$Id$
\**********************************************************************************/

// --------------------------------------------------------
//  Include files
// --------------------------------------------------------

#ifndef _SMB_PROTOCOL_
#define _SMB_PROTOCOL_
#endif

//-------------------------------------------------------------------
//Include files
//-------------------------------------------------------------------
#include "../mscbemb.h"
#include "SMBus_handler.h"


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
void SMBus_SetTXBuffer(const unsigned char *pData, unsigned char dataLen) {
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
  SFRPAGE = SMB0_PAGE;
  STA = 1;

  SMBus_Wait();
  SMBus_Clear();
}

//-------------------------------------------------------------------
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
    SFRPAGE = TMR3_PAGE;
    TMR3CN = 0x00; // Turn Clock off
    TMR3CF = 0x00; // SYSCLK / 12

    RCAP3L = 0x00; /* TIMER 3 CAPTURE/RELOAD LOW BYTE */
    RCAP3H = 0x00; /* TIMER 3 CAPTURE/RELOAD HIGH BYTE */

    TMR3L = 0x00;  //Timer3 Low Bytes
    TMR3H = 0x00;  //Timer3 High Bytes

    TMR3CN = 0x04; // Enable Timer3

    SFRPAGE = SMB0_PAGE;
    SMB0CN = 0x43;
    EIP1 |= 0x02; // SMB High priority 
    EIE1 |= 0x02; // Enable SMBus interrupts
    EIE2 |= 0x01; // Enable Timer3 interrupts
    EA = 1;   // Enable Global interrupts     
    SMBus_Clear();    

  }
}

//
//-------------------------------------------------------------------
void SMBus_ISR(void) interrupt 7 {
	bit FAIL = 0;
	static unsigned char data_in;
	static unsigned char data_out;

	switch(SMB0STA) {
	case SMB_STATE_START:
		data_in = 0;
		data_out = 0;
	case SMB_STATE_REP_START:
		SMB0DAT = (SMB_TARGET << 1) | SMB_RW;
		STA = 0;						// clear START bit
		break;

	case SMB_STATE_MT_SLAVE_ACK:
		if(data_out < SMB_DATA_OUT_LEN) {
			SMB0DAT = SMB_DATA_OUT[data_out++];
		} else {
			FAIL = 1;
		}
		break;

	case SMB_STATE_MT_SLAVE_NACK:
		if(SMB_ACKPOLL) {
			STO = 1;
			STA = 1;
		} else {
			FAIL = 1;
		}
		break;

	case SMB_STATE_MT_DATA_ACK:
		if(data_out < SMB_DATA_OUT_LEN) {
			SMB0DAT = SMB_DATA_OUT[data_out++];
		} else if (data_in < SMB_DATA_IN_LEN) {
			SMB_RW = SMB_READ;
			STO = 0;
			STA = 1;
		} else {
      if(SMB_BUSY) {
			STO = 1;
			SMB_BUSY = 0;
	    }
  	}
		break;

	case SMB_STATE_MT_DATA_NACK:
		// This is the place to add DATA retry
		STO = 1;
		break;

	case SMB_STATE_MR_SLAVE_ACK:
		if(SMB_DATA_IN_LEN == 1) {
			// only expecting one byte, so NACK reply to it to end transfer
			AA = 0;
		} else {
			AA = 1;
		}
		break;

	case SMB_STATE_MR_SLAVE_NACK:
		if(SMB_ACKPOLL) {
			STO = 1;
			STA = 1;
		} else {
			FAIL = 1;
		}
		break;

	case SMB_STATE_MR_DATA_ACK:
		if(data_in == (SMB_DATA_IN_LEN - 1)) {
			// Last Byte, send NACK
			AA = 0;
		} else {
			AA = 1;
		}

		if(data_in < SMB_DATA_IN_LEN) {
			*(pSMB_DATA_IN+data_in) = SMB0DAT;
			data_in++;
		} else {
			FAIL = 1;
		}
		break;

	case SMB_STATE_MR_DATA_NACK:
		// Save last byte received
		*(pSMB_DATA_IN+data_in) = SMB0DAT;
		data_in++;
		// Stop Communication
		STO = 1;
		SMB_BUSY = 0;
		break;

	default:
		FAIL = 1;
		break;

	} // switch()

	if(FAIL) {
		SMB0CN &= ~0x40;	// Disable SMBus
		SMB0CN |= 0x40;	// Re-enable SMBus
		STA = 0;
		STO = 0;
		AA = 0;

		SMB_BUSY = 0;
		FAIL = 0;
	}

	SI = 0;
}

//-------------------------------------------------------------------
//
void Timer3_ISR(void) interrupt 14 {
  SFRPAGE = SMB0_PAGE;
  SMB0CN &= ~0x40; // Disable SMBus
  SMB0CN |= 0x40; // Re-enable SMBus

  SFRPAGE = TMR3_PAGE;
  TMR3CN &= ~0x80; // Clear Timer3 interrupt-pending flag
  SMBus_Clear();
} 
