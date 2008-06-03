/**********************************************************************************\
  Name:			SHT7x_protocol.h
  Author:   	Bahman Sotoodian 	
  Created: 		March 28, 2008
  Description:	Humidity and Temperature measurements

  $Id:$
\**********************************************************************************/

#ifndef _SHT7x_PROTOCOL_H
#define _SHT7x_PROTOCOL_H

sbit SHT7x_DATA = SHT_DATA;
sbit SHT7x_SCK = SHT_SCK;

//#define SHT7x_DELAY	1 //in us
#define SHT7x_DELAY       5
#define SHT7x_SHORT_DELAY 0.3
#define  ACK  0
#define NACK  1
#define ERROR 0
#define DONE  1

void SHT7x_Init(void); 
void SHT7x_ClockOnce(void);
void SHT7x_ConnectionReset(void);
void SHT7x_TransStart(void);
unsigned char SHT7x_FCS_Step(unsigned int msg, unsigned char fcs) reentrant; 
unsigned char SHT7x_WriteByte(unsigned char dataToSend);
unsigned char SHT7x_ReadByte(unsigned char read_flag);
//NW Reading from the Data line, read before the falling edge to 
//compensate for the slow rise time of the weak pull up resistor
void SHT7x_ClockOnce2(void);
#endif