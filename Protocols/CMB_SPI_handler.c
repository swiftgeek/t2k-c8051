/**********************************************************************************\
  Name:         CMBSPI_handler.c
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

#ifndef _CMBSPI_PROTOCOL_
#define _CMBSPI_PROTOCOL_
#endif

#include "../mscbemb.h"
#include "CMB_SPI_handler.h"

//
//------------------------------------------------------------------------
/** 
Setting the related pins of SPI protocol
*/
sbit CMBSPI_SCK   = CMB_SPI_SCK;     // SPI Protocol Serial Clock
sbit CMBSPI_MISO  = CMB_SPI_MISO;    // SPI Protocol Master In Slave Out
sbit CMBSPI_MOSI  = CMB_SPI_MOSI;    // SPI Protocol Master Out Slave In

//
//------------------------------------------------------------------------
/** 
Initializing the SPI communication by pulling the SPI Master Output
Slave Input (MOSI) pin to 1.
*/

void CMBSPI_Init(void)
{
  SFRPAGE  = SPI0_PAGE ;
  CMBSPI_MOSI = 1;          //pull the MOSI line high
}

//
//------------------------------------------------------------------------
/**
Generating the SPI communication clock."SFRPAGE = SPI0_PAGE" makes sure that
we are in the right page to properly communicate with the SPI protocol.

@attention	The SPI clock is currently running at 50us. 
*/
void CMBSPI_ClockOnce(void)
{
  delay_us(CMBSPI_DELAY);
  CMBSPI_SCK = 1;
  delay_us(CMBSPI_DELAY);
  CMBSPI_SCK = 0;
}

//
//------------------------------------------------------------------------
/**
Performing SPI write operation.One byte of data transmits in a bit wise fashion

@param dataToSend		One byte of information which has to be sent via SPI protocol.
@return 				   The function does not return any value.
*/
void CMBSPI_WriteByte(unsigned char dataToSend)
{
  signed char i;

  for(i = 7; i >= 0; i--) {
     CMBSPI_MOSI = (dataToSend >> i) & 0x01;
     CMBSPI_ClockOnce();
  }
  CMBSPI_MOSI = 0;
}

 
//
//------------------------------------------------------------------------
unsigned char CMBSPI_ReadByteRising(void)
{
  signed char i = 0;
  unsigned char din = 0;
  unsigned char dataReceived = 0;

  for(i = 7; i >= 0; i--)
  {
	delay_us(CMBSPI_DELAY);
   din = CMBSPI_MISO;
   dataReceived |= (din << i);
  	CMBSPI_SCK = 1;
  	delay_us(CMBSPI_DELAY);
  	CMBSPI_SCK = 0;
 
  }
  return dataReceived;
}
