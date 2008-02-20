// --------------------------------------------------------
//  SMBus handling routines for T2K-FGD experiment
//      - Writtern for FEB64 board
//      - Bahman Sotoodian   (Feb/11/2008)
//
//  $Id$
// --------------------------------------------------------
#ifndef  _SMBUS_HANDLER_H
#define  _SMBUS_HANDLER_H

#define WRITE 0
#define READ 1
#define CMD 2
#define VAL 3
#define SMBUS_SI_WAITTIME 100 //in ms

void SMBus_Init(void);
void SMBus_Start(void);
void SMBus_WriteByte(unsigned char Byte, unsigned char RW_flag);
unsigned char SMBus_ReadByte(void);
void SMBus_Stop(void);
bit SMBus_SerialInterrupt(void);

#endif

