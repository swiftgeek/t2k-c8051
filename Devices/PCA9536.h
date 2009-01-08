/********************************************************************\

  Name:         PCA9536.h
  Created by:   Bahman Sotoodian 								Feb/11/2008
  Modified by:  Bryerton Shaw										Mar/15/2008

  Contents:     PCA9536 I/O register user interface

  $Id: PCA9536.h 62 2008-12-19 00:12:12Z midas $

\********************************************************************/
//  need to have FEB64 defined

#ifdef _PCA9536_


#ifndef  _PCA9536_IO_H
#define  _PCA9536_IO_H

// Command Bytes
#define PCA9536_INPUT		      0x00
#define PCA9536_OUTPUT			  0x01
#define PCA9536_POLINV			  0x02
#define PCA9536_CONFIG			  0x03

// Bit Meanings
#define PCA9536_READ				  1
#define PCA9536_WRITE			    0

#define PCA9536_INVERT			  1
#define PCA9536_NO_INVERT		  0
#define PCA9536_ALL_INPUT		  0xFF
#define PCA9536_ALL_OUTPUT		0x00

void PCA9536_Init(void);
void PCA9536_WriteByte(unsigned char addr, unsigned char selectPort, unsigned char dataByte);
void PCA9536_WriteWord(unsigned char addr, unsigned char selectPort, unsigned int dataWord);
void PCA9536_Read(unsigned char addr, unsigned char selectPort, unsigned char* dataBytes, unsigned char dataLen);
void PCA9536_Conversion(unsigned char *conversion);

#endif // _PCA9536_H

#endif // _PCA9536_
