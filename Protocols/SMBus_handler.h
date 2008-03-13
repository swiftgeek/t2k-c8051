// --------------------------------------------------------
//  SMBus handling routines for T2K-FGD experiment
//      - Writtern for T2K-ASUM test board
//      - Brian Lee   (Jun/08/2006)
//
//  $Id$
// --------------------------------------------------------
#ifndef  _SMBUS_HANDLER_H
#define  _SMBUS_HANDLER_H

#define WRITE_CYCLE 0
#define READ_CYCLE  1
#define ADD_READ    2
#define ADD_WRITE   3
#define CMD_CYCLE   4

#define SMBUS_SI_WAITTIME 100 //in ms

void SMBus_Init(void);
bit SMBus_Start(void);
bit SMBus_WriteByte(unsigned char Byte, unsigned char RW_flag);
unsigned char SMBus_ReadByte(void);
void SMBus_Stop(void);
bit SMBus_SerialInterruptCheck(void);

#endif

