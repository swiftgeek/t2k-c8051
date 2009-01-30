/**********************************************************************************\
  Name:			SMBus_handler.h
  Author:   	Bryerton Shaw 	
  Created: 		March 11, 2008
  Description:	SMBus protocol using interrupts, created for FEB64 board

  $Id: SMBus310_handler.h 73 2009-01-30 05:16:24Z midas $
\**********************************************************************************/


#ifndef  _SMBUS_HANDLER_H
#define  _SMBUS_HANDLER_H

#define SMB_WRITE				0x00
#define SMB_READ				0x01
#define SMB_ENABLE_ACKPOLL		0x01
#define SMB_DISABLE_ACKPOLL		0x00

#define SMB_MAX_BUFF_SIZE		8

#define SMB_MTSTA				0xE0
#define SMB_MTDB				0xC0	
#define SMB_MRDB				0x80
 
#define SMB_STATE_START 		0xE0	//Check the most 6bits
#define SMB_STATE_MT_ACK	 	0xC2  //Check the most 6bits
#define SMB_STATE_MT_NACK		0xC0  //Check the most 7bits

#define SMB_STATE_MR_DATA_ACK	0x88  //Check the most 7bits
#define SMB_STATE_SR_ADDR_ACK   0x2C  //Check the most 6bits
#define SMB_STATE_ST_DATA_NACK  0x40  //Check the most 6bits
#define MASTER_LOST_ARBIT		0x2C  //Check the most 6bits
#define REP_START_LOST_ARBIT	0x24  //Check the most 6bits
#define STOP_LOST_ARBIT         0x14  //Check the most 6bits
#define MT_LOST_ARBIT           0x0C  //Check the most 6bits

/***
 * Function:	SMBus_Init
 * Purpose:		Initialize SMBus Protocol
 *
 * Notes:		Configures Timer3, assumes Timer1 is set properly
 */
void SMBus_Init(void);
void SMBus_EnableACKPoll(void);
void SMBus_SetSlaveAddr(unsigned char slaveAddr);
void SMBus_SetTXBuffer(unsigned char *pData, unsigned char dataLen);
void SMBus_SetRXBuffer(unsigned char *pData, unsigned char dataLen);
void SMBus_Wait(void);
void SMBus_Start(void);
void SMBus_Clear(void);

#endif // _SMBUS_HANDLER_H

