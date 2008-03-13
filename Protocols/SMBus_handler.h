/**********************************************************************************\
  Name:			SMBus_handler.h
  Author:   	Bryerton Shaw 	
  Created: 		March 11, 2008
  Description:	SMBus protocol using interrupts, created for FEB64 board

  $Id$
\**********************************************************************************/

#ifndef  _SMBUS_HANDLER_H
#define  _SMBUS_HANDLER_H

// SMBus Defines
#define SMB_FREQUENCY	1000000	// in Hz, allowed range is 10kHz to 100kHz

#define SMB_WRITE	0x00
#define SMB_READ	0x01

#define SMB_MAX_BUFF_SIZE	0x08

#define SMB_MTSTA		0xE0
#define SMB_MTDB		0xC0	
#define SMB_MRDB		0x80
 
// SMBus Global Variables
unsigned char *pSMB_DATA_IN;
unsigned char *pSMB_DATA_OUT;
unsigned char SMB_DATA_LEN;
unsigned char SMB_TARGET;

bit SMB_BUSY;
bit SMB_RW;
bit SMB_SENDWORDADDR;
bit SMB_RANDOMREAD;
bit SMB_ACKPOLL;

// Alias SDA and SCL to defined SMBus ports (see mscbemb.h)
sbit SDA = MSCB_I2C_SDA;
sbit SCL = MSCB_I2C_SCL;

/***
 * Function:	SMBus_Clear
 * Purpose:		Attempt to clear SDA if a slave is holding SDA high due to 
 *					improper SMBus reset or error.
 *
 * Notes:		Call on startup (before SMBus_Init)
 */
void SMBus_Clear(void);

/***
 * Function:	SMBus_Init
 * Purpose:		Initialize SMBus Protocol
 *
 * Notes:		Configures Timer3, assumes Timer1 is set properly
 */
void SMBus_Init(void);



#endif // _SMBUS_HANDLER_H