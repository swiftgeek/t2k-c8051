/**********************************************************************************\
  Name:			SMBus_handler.c
  Author:   	Bryerton Shaw 	
  Created: 		March 11, 2008
  Description:	SMBus protocol using interrupts, created for FEB64 board

  $Id$
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

#include "SMBus_int_handler.h"

// --------------------------------------------------------
//  SMBus_Initialization :
//      Bus Initialization sequence
// --------------------------------------------------------
void SMBus_Clear(void) {
	
}

void SMBus_Init(void) {
	// Timer3 Registers
	SFRPAGE = TMR3CN;
	TMR3CN = 0x00;	// Turn Clock off
	TMR3CF = 0x00;	// SYSCLK / 12

	RCAP3L = 0x00; /* TIMER 3 CAPTURE/RELOAD LOW BYTE */
	RCAP3H = 0x00; /* TIMER 3 CAPTURE/RELOAD HIGH BYTE */

	TMR3L = 0x00;
	TMR3H = 0x00;

	TMR3CN = 0x02; // Enable Timer3
   
	SFRPAGE = SMB0_PAGE;
	SMB0CN = 0x41;
	EIE1 |= 0x02;	// Enable SMBus interrupts
	EIE2 |= 0x01;	// Enable Timer3 interrupts
	EA = 1;			// Enable Global interrupts
}

void SMBus_ISR(void) interrupt 7 {
	bit FAIL = 0;
	static char i;
	static bit SEND_START = 0;

	switch(SMB0STA) {
	case SMB_MTSTA:
		SMB0DAT = (SMB_TARGET & 0xFE) | SMB_RW;
		i = 0;						// reset data byte counter
		STA = 0;						// clear START bit
		break;

	case SMB_MTDB:
		if(AA) {
			if(SEND_START) {
				STA = 1;
				SEND_START = 0;
				break;
			}

			if(SMB_SENDWORDADDR) {
				SMB_SENDWORDADDR = 0;
				SMB0DAT = SMB_WORD_ADDR;
			
				if(SMB_RANDOMREAD) {
					SEND_START = 1;
					SMB_RW = SMB_READ;
				}

				break;
			}

			if(SMB_RW == SMB_WRITE) {
				if(i < SMB_DATA_LEN) {
					SMB0DAT = SMB_DATA_OUT[i];
					i++;
				} else {
					STO = 1;
					SMB_BUSY = 0;
				}
			}
		} else {
			if(SMB_ACKPOLL) {
				STA = 1;
			} else {
				FAIL = 1;
			}
		}
		break;

	case SMB_MRDB:
		if(i < SMB_DATA_LEN) {
			*(pSMB_DATA_IN+i) = SMB0DAT;
			i++;
			AA = 1;
		}

		if(i == SMB_DATA_LEN) {
			SMB_BUSY = 0;
			AA = 0;
			STO = 1;
		}

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

#endif // _SMB_PROTOCOL_
