/**********************************************************************************\
  Name:			SMBus_handler.c
  Author:   	Bryerton Shaw 	
  Created: 		March 11, 2008
  Modified:	   Bahman Sotoodian
  Description:	SMBus protocol using interrupts, created for FEB64 board

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
unsigned char xdata SMB_DATA_OUT[SMB_MAX_BUFF_SIZE];
unsigned char xdata SMB_DATA_OUT_LEN;

unsigned char *pSMB_DATA_IN;
unsigned char xdata SMB_DATA_IN_LEN;

unsigned char xdata SMB_TARGET;

char SMB_BUSY;
bit SMB_RW;
bit SMB_ACKPOLL;

//
//-------------------------------------------------------------------
/**
Initializing the SMBus
*/
void SMBus_Init(void) {
	static char init = 0;

	if(!init) {
		init = 1;
		SMB_BUSY = 0;

		// Configuring the Timer3 Registers
		SFRPAGE = TMR3_PAGE;
		TMR3CN = 0x00;	// Turn Clock off
		TMR3CF = 0x00;	// SYSCLK / 12

		RCAP3L = 0x00; /* TIMER 3 CAPTURE/RELOAD LOW BYTE */
		RCAP3H = 0x00; /* TIMER 3 CAPTURE/RELOAD HIGH BYTE */

		TMR3L = 0x00;  //Timer3 Low Bytes
		TMR3H = 0x00;  //Timer3 High Bytes

		TMR3CN = 0x04; // Enable Timer3
   
		SFRPAGE = SMB0_PAGE;
		SMB0CN = 0x43;
    EIP1 |= 0x02; // SMB High priority 
		EIE1 |= 0x02;	// Enable SMBus interrupts
		EIE2 |= 0x01;	// Enable Timer3 interrupts
		EA = 1;			// Enable Global interrupts     
	}
}

void SMBus_ISR(void) interrupt 7 {
	bit FAIL = 0;
	static char data_in;
	static char data_out;

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
			STO = 1;
			SMB_BUSY = 0;
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

void Timer3_ISR(void) interrupt 14 {
	SFRPAGE = SMB0_PAGE;
	SMB0CN &= ~0x40;	// Disable SMBus
	SMB0CN |= 0x40;	// Re-enable SMBus

	SFRPAGE = TMR3_PAGE;
	TMR3CN &= ~0x80;	// Clear Timer3 interrupt-pending flag
	SMB_BUSY = 0;		// Free SMBus
}	
