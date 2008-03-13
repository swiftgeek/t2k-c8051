/**********************************************************************************\
  Name:         SPI_handler.h
  Created by:   Brian           Feb/25/2008


  Contents:     SPI protocol


  $Id$
\**********************************************************************************/

#ifndef  _SPI_HANDLER_H
#define  _SPI_HANDLER_H

#define SPI_DELAY 50 //in us

void SPI_Init(void);
void SPI_ClockOnce(void);
void SPI_WriteByte(unsigned char dataToSend);
void SPI_WriteUInt(unsigned int  dataToSend);
unsigned char SPI_ReadByte(void);
unsigned int  SPI_ReadUInt(void);

#endif