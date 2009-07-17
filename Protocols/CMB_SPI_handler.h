/**********************************************************************************\
  Name:         CMB_SPI_handler.h
  Created by:   Brian           	
  Modified by:	 Bahman Sotoodian Mar/11/2008

  Contents:     SPI protocol


  $Id$
\**********************************************************************************/


#ifndef  _CMBSPI_HANDLER_H
#define  _CMBSPI_HANDLER_H

#define CMBSPI_DELAY 50 //Defining the SPI protocol speed in us
#define CMBSPI_HALF_DELAY 25
#define CMBSPI_WADDRESS  0x01
#define CMBSPI_RADDRESS  0x02
#define CMBSPI_RSTATUS   0x05

//
//------------------------------------------------------------------------
/**
Defining the functions which will be used within the SPI_handler.c file.
*/
void CMBSPI_Init(void);										//Initializing the SPI protocol
void CMBSPI_ClockOnce(void);								//Defining the SPI protocol clock
void CMBSPI_WriteByte(unsigned char dataToSend);	//Writing one byte of data
unsigned char CMBSPI_ReadByteRising(void);		   //Reading one byte of data on rising 
														      // edge of clock													
#endif // _CMBSPI_HANDLER_H
