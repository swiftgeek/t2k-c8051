/**********************************************************************************\
  Name:         LPBSPI_handler.c
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

#ifndef _LPBSPI_PROTOCOL_
#define _LPBSPI_PROTOCOL_
#endif

#include "../mscbemb.h"
#include "LPB_SPI_handler.h"

//
//------------------------------------------------------------------------
/** 
Setting the related pins of SPI protocol
*/
sbit LPBSPI_SCK   = LPB_SPI_SCK;     // SPI Protocol Serial Clock
sbit LPBSPI_MISO  = LPB_SPI_MISO;    // SPI Protocol Master In Slave Out
sbit LPBSPI_MOSI  = LPB_SPI_MOSI;    // SPI Protocol Master Out Slave In

//
//------------------------------------------------------------------------
/** 
Initializing the SPI communication by pulling the SPI Master Output
Slave Input (MOSI) pin to 1.
*/

void LPBSPI_Init(void)
{
  SFRPAGE  = SPI0_PAGE ;
//  LPBSPI_MOSI = 1;          //pull the MOSI line high
}

//
//------------------------------------------------------------------------
/**
Generating the SPI communication clock."SFRPAGE = SPI0_PAGE" makes sure that
we are in the right page to properly communicate with the SPI protocol.

@attention	The SPI clock is currently running at 50us. 
*/
void LPBSPI_ClockOnce(void)
{
  delay_us(LPBSPI_DELAY);
  LPBSPI_SCK = 1;
  delay_us(LPBSPI_DELAY);
  LPBSPI_SCK = 0;
}

//
//------------------------------------------------------------------------
/**
Performing SPI write operation.One byte of data transmits in a bit wise fashion

@param dataToSend		One byte of information which has to be sent via SPI protocol.
@return 				   The function does not return any value.
*/
void LPBSPI_WriteByte(unsigned char dataToSend)
{
  signed char i;

  for(i = 7; i >= 0; i--) {
     LPBSPI_MOSI = (dataToSend >> i) & 0x01;
     LPBSPI_ClockOnce();
  }
  LPBSPI_MOSI = 0;
}

//
//------------------------------------------------------------------------
unsigned char LPBSPI_ReadByteRising(void)
{
  signed char i = 0;
  unsigned char din = 0;
  unsigned char dataReceived = 0;

  for(i = 7; i >= 0; i--)
  {
	  delay_us(LPBSPI_DELAY);
    din = LPBSPI_MISO;
    dataReceived |= (din << i);
  	LPBSPI_SCK = 1;
  	delay_us(LPBSPI_DELAY);
  	LPBSPI_SCK = 0;
 
  }
  return dataReceived;
}

