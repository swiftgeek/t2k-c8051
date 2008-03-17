/**********************************************************************************\
  Name:			SMBus_handler.h
  Author:   	Bryerton Shaw 	
  Created: 		March 11, 2008
  Description:	SMBus protocol using interrupts, created for FEB64 board

  $Id$
\**********************************************************************************/

#ifdef _SMB_PROTOCOL_

#ifndef  _SMBUS_HANDLER_H
#define  _SMBUS_HANDLER_H


#define SMB_WRITE	0x00
#define SMB_READ	0x01

#define SMB_MAX_BUFF_SIZE 8

#define SMB_MTSTA		0xE0
#define SMB_MTDB		0xC0	
#define SMB_MRDB		0x80
 
#define SMB_STATE_START 				0x08
#define SMB_STATE_REP_START 			0x10

#define SMB_STATE_MT_SLAVE_ACK		0x18
#define SMB_STATE_MT_SLAVE_NACK		0x20
#define SMB_STATE_MT_DATA_ACK			0x28
#define SMB_STATE_MT_DATA_NACK		0x30

#define SMB_STATE_MR_SLAVE_ACK		0x40
#define SMB_STATE_MR_SLAVE_NACK		0x48
#define SMB_STATE_MR_DATA_ACK			0x50
#define SMB_STATE_MR_DATA_NACK		0x58


// SMBus Global Variables
extern unsigned char xdata SMB_DATA_OUT[SMB_MAX_BUFF_SIZE];
extern unsigned char xdata SMB_DATA_OUT_LEN;

extern unsigned char *pSMB_DATA_IN;
extern unsigned char xdata SMB_DATA_IN_LEN;

extern unsigned char xdata SMB_TARGET;

extern bit SMB_BUSY;
extern bit SMB_RW;
extern bit SMB_ACKPOLL;


/***
 * Function:	SMBus_Init
 * Purpose:		Initialize SMBus Protocol
 *
 * Notes:		Configures Timer3, assumes Timer1 is set properly
 */
void SMBus_Init(void);

#endif // _SMBUS_HANDLER_H

#endif // _SMB_PROTOCOL_