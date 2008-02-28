/**********************************************************************************\
  Name:         SPI_handler.c
  Created by:   Brian Lee                


  Contents:     SPI protocol for FEB64 board


  $Id$
\**********************************************************************************/

#ifdef _SPI_PROTOCOL_

#include "../feb64/mscbemb.h"
#include "SPI_handler.h"

/* SPI Protocl Related Pins */
sbit SPI_SCK   = MSCB_SPI_SCK;     // SPI Protocol Serial Clock
sbit SPI_MISO  = MSCB_SPI_MISO;    // SPI Protocol Master In Slave Out
sbit SPI_MOSI  = MSCB_SPI_MOSI;    // SPI Protocol Master Out Slave In

//
//------------------------------------------------------------------------
void SPI_Init(void)
{
  SFRPAGE  = SPI0_PAGE ;
  SPI_MOSI = 1;          //pull the MOSI line high
}

//
//------------------------------------------------------------------------
void SPI_ClockOnce(void)
{
  SFRPAGE = SPI0_PAGE ;
  delay_us(SPI_DELAY);
  SPI_SCK = 1;
  delay_us(SPI_DELAY);
  SPI_SCK = 0;
}

//
//------------------------------------------------------------------------
void SPI_WriteByte(unsigned char dataToSend)
{
  signed char i;

  SFRPAGE = SPI0_PAGE ;
  for(i = 7; i >= 0; i--) {
	  SPI_MOSI = (dataToSend >> i) & 0x01;
     SPI_ClockOnce();
  }
}

//
//------------------------------------------------------------------------
void SPI_WriteUInt(unsigned int dataToBeSend)
{
	signed char i;
	
  SFRPAGE = SPI0_PAGE ;
  for(i = 15; i >= 0; i--) {       
	  SPI_MOSI = (dataToBeSend >> i) & 0x01;
     SPI_ClockOnce();
  }
}

//
//------------------------------------------------------------------------
unsigned char SPI_ReadByte(void)
{
  signed char i = 0;
  unsigned char din = 0;
  unsigned char dataReceived = 0;

  for(i = 7; i >= 0; i--)
  {
    SPI_ClockOnce();
    din = SPI_MISO;
    dataReceived |= (din << i);
  }
  return dataReceived;
}

#endif