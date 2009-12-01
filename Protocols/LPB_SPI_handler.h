/**********************************************************************************\
  Name:         LPB_SPI_handler.h
  Created by:   Brian           	
  Modified by:	 Bahman Sotoodian Mar/11/2008

  Contents:     SPI protocol


  $Id$
\**********************************************************************************/


#ifndef  _LPBSPI_HANDLER_H
#define  _LPBSPI_HANDLER_H

#define LPBSPI_DELAY 50 //Defining the SPI protocol speed in us
#define LPBSPI_HALF_DELAY 25
#define LPBSPI_WADDRESS  0x01
#define LPBSPI_RADDRESS  0x02
#define LPBSPI_RSTATUS   0x05

//
//------------------------------------------------------------------------
/**
Defining the functions which will be used within the SPI_handler.c file.
*/
void LPBSPI_Init(void);										//Initializing the SPI protocol
void LPBSPI_ClockOnce(void);								//Defining the SPI protocol clock
void LPBSPI_WriteByte(unsigned char dataToSend);	//Writing one byte of data
														      // edge of clock													
#endif // _LPBSPI_HANDLER_H
