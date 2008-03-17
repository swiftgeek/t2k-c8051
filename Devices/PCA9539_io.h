/********************************************************************\

  Name:         PCA9539_io.h
  Created by:   Bahman Sotoodian 								Feb/11/2008
  Modified by:  Bryerton Shaw										Mar/15/2008

  Contents:     PCA9539 I/O register user interface

  $Id$

\********************************************************************/
//  need to have FEB64 defined

#ifdef _PCA9539_

#ifndef  _PCA9539_IO_H
#define  _PCA9539_IO_H

// Command Bytes
#define PCA9539_INPUT0			  0x00
#define PCA9539_INPUT1			  0x01
#define PCA9539_OUTPUT0			  0x02
#define PCA9539_OUTPUT1			  0x03
#define PCA9539_POLINV0			  0x04
#define PCA9539_POLINV1			  0x05
#define PCA9539_CONFIG0			  0x06
#define PCA9539_CONFIG1			  0x07

// Bit Meanings
#define PCA9539_READ				  1
#define PCA9539_WRITE			  0

#define PCA9539_INVERT			  1
#define PCA9539_NO_INVERT		  0
#define PCA9539_INPUT			  1
#define PCA9539_OUTPUT			  0
#define PCA9539_ALL_INPUT		  0xFF
#define PCA9539_ALL_OUTPUT		  0x00

void PCA9539_Init(void);
void PCA9539_WriteByte(unsigned char addr, unsigned char selectPort, unsigned char dataByte);
void PCA9539_WriteWord(unsigned char addr, unsigned char selectPort, unsigned int dataWord);
void PCA9539_Read(unsigned char addr, unsigned char selectPort, unsigned char* dataBytes, unsigned char dataLen);

#endif // _PCA9539_IO_H

#endif // _PCA9539_