/**********************************************************************************\
  Name:           SMBus_handler.h
  Author:         Bryerton Shaw  
  Created:       March 11, 2008
  Description: SMBus protocol using interrupts, created for FEB64 board

  $Id$
\**********************************************************************************/

#ifndef  _SMBUS_HANDLER_H
#define  _SMBUS_HANDLER_H

#define SMB_WRITE            0x00
#define SMB_READ             0x01
#define SMB_ENABLE_ACKPOLL   0x01
#define SMB_DISABLE_ACKPOLL  0x00

#define SMB_MAX_BUFF_SIZE       8

#define SMB_MTSTA    0xE0
#define SMB_MTDB     0xC0
#define SMB_MRDB     0x80

#define SMB_STATE_START           0x08
#define SMB_STATE_REP_START       0x10

#define SMB_STATE_MT_SLAVE_ACK    0x18
#define SMB_STATE_MT_SLAVE_NACK   0x20
#define SMB_STATE_MT_DATA_ACK     0x28
#define SMB_STATE_MT_DATA_NACK    0x30

#define SMB_STATE_MR_SLAVE_ACK    0x40
#define SMB_STATE_MR_SLAVE_NACK   0x48
#define SMB_STATE_MR_DATA_ACK     0x50
#define SMB_STATE_MR_DATA_NACK    0x58

/***
 * Function: SMBus_Init
 * Purpose:  Initialize SMBus Protocol
 *
 * Notes:  Configures Timer3, assumes Timer1 is set properly
 */
void SMBus_Init(unsigned char force);
void SMBus_EnableACKPoll(void);
void SMBus_SetSlaveAddr(unsigned char slaveAddr);
void SMBus_SetTXBuffer(const unsigned char *pData, unsigned char dataLen);
void SMBus_SetRXBuffer(unsigned char *pData, unsigned char dataLen);
void SMBus_Wait(void);
void SMBus_Start(void);
void SMBus_Clear(void);

#endif // _SMBUS_HANDLER_H
