/********************************************************************\

  Name:         ExtEEPROM.c
  Created by:   Bahman Sotoodian  								Feb/28/2007


  Contents:     EEPROM (T25160AN-10SU-2.7) user interface

  $Id$

\********************************************************************/
//  need to have FEB64 or TEMP36 defined

#ifdef  _ExtEEPROM_

#ifndef _SPI_PROTOCOL_
#define _SPI_PROTOCOL_
#endif

//-----------------------------------------------------------------
// Include files
//-----------------------------------------------------------------
#include "../mscbemb.h"
#include "ExtEEPROM.h"
#include "../Protocols/SPI_handler.h"

//
//-----------------------------------------------------------------
/**
Initializing the SPI communication protocol
*/
void ExtEEPROM_Init (void){	 
	// Unselecting the device 
	RAM_CSn = 1;

	//Initializing the SPI
	SPI_Init();
}

//
//-----------------------------------------------------------------
/**
Reads data from ext_eeprom.

@param ReadPage	   address of memory which originator wasnts to read from
@param *destination  address of memory which the read data would be stored
@param page_size		number of bytes of data that the originator wants to read from memory
*/
unsigned char ExtEEPROM_Read (unsigned int ReadPage, unsigned char *destination, 
unsigned char page_size) {
		
	unsigned char i=0;
			
	//Making sure that Write Protection pin is high to start doing W/R operation
	RAM_WPn = 1;	
	if (ExtEEPROM_WriteEnable() ==EEP_BUSY)
		return EEP_BUSY;

	RAM_CSn = 0;
	SPI_WriteByte(EEP_READcmd); 				//Sending the raed opcode 
	SPI_WriteUInt(ReadPage);	 			//Sending the memory address 
	
	for (i;i<page_size;i++)	{
		*destination=SPI_ReadByteRising();
		destination++;
   }

	delay_us(EEP_delay);
	RAM_CSn = 1;	
	RAM_WPn = 0;
	return EEP_SUCCESS;
}

//
//-----------------------------------------------------------------
/**
Writes data to the ext_eeprom.
@attention 							 The user should be able to set a bit in control register to request a write or clear
						  			    operation to the ext_eeprom. The value of that bit should be reflected on the "clear" argument.
@param write_addr					 The address of ext_eeprom's memory where the write/clear operation would be perfomred.
@param *source                 The stored data which should be written in ext_eeprom
@param page_size					 Number of bytes of data which will be written into ext_eeprom
@param WC_flag						 Write or Clear flag
@param *flag						 In case of EEP_BUSY return, the value of parameter 
										 "i" would be stored in *flag for next function call
@return EEP_PROTECTED		    Indicating that the memory is write protected and can not be modified
@return EEP_BUSY					 Indiacting that the device is busy performing the internal write cycle
@return EEP_SUCCESS  			 				  
*/


unsigned char ExtEEPROM_Write_Clear(unsigned int write_addr, unsigned char **source, 
unsigned char page_size, unsigned char WC_flag, unsigned char *flag)
{
	unsigned char i,j;
	unsigned int Nblock;
	unsigned char counter = EEP_MAX_BYTE;	

		//Checking if we are trying to write in the write protected block
	if (write_addr+page_size >= WP_START_ADDR)
		return EEP_PROTECTED;

	//Making sure that Write Protection pin is high
	RAM_WPn = 1;
	//Nblock would determine number of 32 bytes blocks
	Nblock = page_size / EEP_MAX_BYTE;

	for (i = *flag; i <= Nblock; i++) {  
		//	To check whether eeprom is ready (not busy) or not
		if(ExtEEPROM_Wait()==EEP_BUSY){
			//Storing the value of counter("i")
			*flag = i;
			return EEP_BUSY;
		}
		//Updating the address of memory 				
		write_addr = write_addr + (i * EEP_MAX_BYTE);					 
		//Checking for the last cycle of write operation		
		if (i == Nblock) {	
			//Determing number of bytes left for the last cycle of write operation 
			counter = (page_size % EEP_MAX_BYTE);
      }
		//Sending the Write Enable instruction 
		if (ExtEEPROM_WriteEnable() == EEP_BUSY){	 //The device will be automatically
			*flag = i;
			return EEP_BUSY;								//returned to the write disable state 
		}													    //at the completion of write cycle
		
	
		RAM_CSn = 0;
	
		SPI_WriteByte(EEP_WRITEcmd); 					//Sending the WRITE opcode 
		SPI_WriteUInt(write_addr);	 			   	//Sending the memory address which 
		                                          //data is going to be stored in;
						
		for (j=0; j<counter; j++) {	
			if (WC_flag == WRITE_EEPROM){
				SPI_WriteByte(**source);
				(*source)++;
			}
			else
			   SPI_WriteByte(CLEAR_EEPROM);		  //Clearing the memory									
		}

		delay_us(EEP_delay);							
		RAM_CSn = 1;	 //Programming will start after the chip select is brought high	 
		delay_us(EEP_delay);		
	}

	RAM_WPn = 0;	
	return EEP_SUCCESS;
}

//
//-----------------------------------------------------------------
/**
Sending the write enable instruction 
*/
unsigned char ExtEEPROM_WriteEnable(void)
{				
		//Enabling the write operation
		RAM_CSn = 0;
		delay_us(EEP_delay);
		SPI_WriteByte(EEP_WRENcmd);  					
		delay_us(EEP_delay);
		RAM_CSn = 1;
		delay_us(EEP_delay);
		if (ExtEEPROM_Wait() ==EEP_BUSY)
			return EEP_BUSY;
      return EEP_READY;
}

//
//-----------------------------------------------------------------
/**
Writing to the status register
@param status	Stuats would determine the value of WPEN,BP0,BP1 
	            bits of status register
*/
unsigned char ExtEEPROM_WriteStatusReg(unsigned char status) {
	ExtEEPROM_WriteEnable();
	RAM_CSn = 0;
	SPI_WriteByte(EEP_WRSRcmd);
	SPI_WriteByte(status);
	delay_us(EEP_delay);
	RAM_CSn = 1;
	delay_us(EEP_delay);
	if (ExtEEPROM_Wait() == EEP_BUSY)
		return EEP_BUSY;
   return EEP_READY;
}

//
//-----------------------------------------------------------------
/**
Reading the status register
@attention	This function is not currently called, but it can be used to check the 
			   state of statue register bits
@return status	The value of status register
*/
unsigned char ExtEEPROM_Status(void)
{	
	unsigned char status;

	RAM_CSn = 0;
	SPI_WriteByte(EEP_RDSRcmd);
	status=SPI_ReadByteRising();
	delay_us(EEP_delay);
	RAM_CSn = 1;
	delay_us(EEP_delay);		

	return status;
}

//
//-----------------------------------------------------------------
/**
Checking to see whether the device is ready or not. 
@return EEP_BUSY 	Indicating the device is busy performing internal write cycle
@return EEP_READY  Indicating the device is ready for the next operation
*/
unsigned char ExtEEPROM_Wait(void) {
	unsigned char status;		
	status = ExtEEPROM_Status();
	if((status & W_INTERNAL) != 0)
		return EEP_BUSY;
return EEP_READY;
}
#endif