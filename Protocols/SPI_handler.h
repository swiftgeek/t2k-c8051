/**********************************************************************************\
  Name:         SPI_handler.h
  Created by:   Brian           	
  Modified by:	 Bahman Sotoodian Mar/11/2008

  Contents:     SPI protocol


  $Id$
\**********************************************************************************/

#ifndef  _SPI_HANDLER_H
#define  _SPI_HANDLER_H

#define SPI_DELAY 50 //Defining the SPI protocol speed in us

//
//------------------------------------------------------------------------
/**
Defining the functions which will be used within the SPI_handler.c file.
*/
void SPI_Init(void);										//Initializing the SPI protocol
void SPI_ClockOnce(void);								//Defining the SPI protocol clock
void SPI_WriteByte(unsigned char dataToSend);	//Writing one byte of data
void SPI_WriteUInt(unsigned int  dataToSend);	//Writing two bytes of data
unsigned char SPI_ReadByteFalling(void);			//Reading one byte of data on falling 
														      // edge of clock
unsigned char SPI_ReadByteRising(void);		   //Reading one byte of data on rising 
														      // edge of clock													


#endif