/**********************************************************************************\
  Name:         SPI_handler.c
  Created by:   Brian Lee                
  Modified by:  Bahman Sotoodian	

  Contents:     SPI protocol for FEB64


  $Id$
\**********************************************************************************/

//
//------------------------------------------------------------------------
/** 
Using the SPI protocol for the 64 channel FrontEnd board
*/

#ifndef _SPI_PROTOCOL_
#define _SPI_PROTOCOL_
#endif

#include "../mscbemb.h"
#include "SPI_handler.h"

//
//------------------------------------------------------------------------
/** 
Setting the related pins of SPI protocol
*/
sbit SPI_SCK   = MSCB_SPI_SCK;     // SPI Protocol Serial Clock
sbit SPI_MISO  = MSCB_SPI_MISO;    // SPI Protocol Master In Slave Out
sbit SPI_MOSI  = MSCB_SPI_MOSI;    // SPI Protocol Master Out Slave In

//
//------------------------------------------------------------------------
/** 
Initializing the SPI communication by pulling the SPI Master Output
Slave Input (MOSI) pin to 1.
*/

void SPI_Init(void)
{
  SFRPAGE  = SPI0_PAGE ;
  SPI_MOSI = 1;          //pull the MOSI line high
#ifdef AD5300
  SPI_SCK = 1;
#endif
}

//
//------------------------------------------------------------------------
/**
Generating the SPI communication clock."SFRPAGE = SPI0_PAGE" makes sure that
we are in the right page to properly communicate with the SPI protocol.

@attention	The SPI clock is currently running at 50us. 
*/
void SPI_ClockOnce(void)
{
  SFRPAGE = SPI0_PAGE;
  delay_us(SPI_DELAY);
  SPI_SCK = 1;
  delay_us(SPI_DELAY);
  SPI_SCK = 0;
}

//
//------------------------------------------------------------------------
/**
Performing SPI write operation.One byte of data transmits in a bit wise fashion

@param dataToSend		One byte of information which has to be sent via SPI protocol.
@return 				   The function does not return any value.
*/

void SPI_WriteByte(unsigned char dataToSend)
{
  signed char i;

  SFRPAGE = SPI0_PAGE ;
  for(i = 7; i >= 0; i--) {
     SPI_MOSI = (dataToSend >> i) & 0x01;
     SPI_ClockOnce();
  }
  SPI_MOSI = 0;
}


//
//------------------------------------------------------------------------
/**
Performing SPI write operation.Two bytes of data transmits in a bit wise fashion

@param dataToSend		Two byte of information which has to be sent via SPI protocol.
@return 				   The function does not return any value.
*/

void SPI_WriteUInt(unsigned int dataToBeSend)
{
	signed char i;
	
  SFRPAGE = SPI0_PAGE ;
  
  for(i = 15; i >= 0; i--) {       
	  SPI_MOSI = (dataToBeSend >> i) & 0x01;
     SPI_ClockOnce();
  }
  SPI_MOSI = 0;
}

// Disabled to remove UNCALLED SEGMENT warning
#if 0

//
//------------------------------------------------------------------------
/**
Performing SPI read operation on the falling edge of clock.One byte of data is received in a bit wise fashion

@param						The function does not have any argument
@return dataReceived 	The function returns one byte of information which has been 
								received via SPI protocol
*/
unsigned char SPI_ReadByteFalling(void)
{
  signed char i = 0;
  unsigned char din = 0;
  unsigned char dataReceived = 0;

  SFRPAGE = SPI0_PAGE ;
  for(i = 7; i >= 0; i--)
  {
    din = SPI_MISO;
    dataReceived |= (din << i);
	 SPI_ClockOnce();
  }
  return dataReceived;
}

#endif
 
//
//------------------------------------------------------------------------
unsigned char SPI_ReadByteRising(void)
{
  signed char i = 0;
  unsigned char din = 0;
  unsigned char dataReceived = 0;

  SFRPAGE = SPI0_PAGE ;
  for(i = 7; i >= 0; i--)
  {
	delay_us(SPI_DELAY);
   din = SPI_MISO;
   dataReceived |= (din << i);
  	SPI_SCK = 1;
  	delay_us(SPI_DELAY);
  	SPI_SCK = 0;
 
  }
  return dataReceived;
}
