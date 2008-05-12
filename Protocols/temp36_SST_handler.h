/**********************************************************************************\
  Name:         SST_handler.h
  Created by:   Brian Lee                May/11/2007
  Modified by:  Noel Wu                  May/12/2008

  Contents:   SST protocol for temperature sensor array (ADT7486A)

  Version:    Rev 1.2

  $Id: SST_handler.h 94 2008-03-15 01:06:12Z bryerton $
\**********************************************************************************/


#ifndef  TEMP36_SST_HANDLER_H
#define  TEMP36_SST_HANDLER_H

// --------------------------------------------------------
// SST Protocol related definitions and function prototypes
// --------------------------------------------------------

#define T_BIT 16  //Time Bit for both the Address and Message (AT and MT)
#define T_H1 T_BIT * 0.75  //Logic High time bit
#define T_H0 T_BIT * 0.25  //Logic Low time bit
#define SST_CLEAR_DELAY 2 //in milliseconds, not exactly measured, an assumed one that works

//usable from outside
void SST_Clear(int SST_LINE);
void SST_WriteByte(unsigned char datByte, int SST_LINE);
unsigned char SST_ReadByte(int SST_LINE);
void SST_Init(int SST_LINE);
unsigned char FCS_Step(unsigned int msg, unsigned char fcs)  reentrant;

//internal functions
void SST_DrvHigh(int SST_LINE);
void SST_DrvLow(int SST_LINE);
unsigned char SST_DrvClientResponse(int SST_LINE);

#endif // _SST_HANDLER_H
