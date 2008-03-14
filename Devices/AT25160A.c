/********************************************************************\

  Name:         AT25160A.c
  Created by:   Bahman Sotoodian  								Feb/28/2007


  Contents:     EEPROM (T25160AN-10SU-2.7) user interface

  $Id:$

\********************************************************************/
//  need to have EXT_EEPROM defined

#include "../mscbemb.h"
#include "AT25160A.h"
#include "../Protocols/SPI_handler.h"

signed char ExtEEPROM_Init (unsigned char npages, unsigned char page_size)
{
//	unsigned char i;

//	for (i=0;(i<npages)&&((page_size*i)<WP_START_ADDR);i++)
//	{
//		page_address[i]=page_size*i;
//	}
			 
	// Unselecting the device 
	RAM_CSn = 1;
	delay_us(10);

	//Initializing the SPI
	SPI_Init();

	if((page_size*npages)>WP_START_ADDR)
		return ERROR;

	return 0;
}


signed char ExtEEPROM_Write(unsigned int WritePage, void *source, unsigned char page_size)
{
		unsigned char i,j,WriteStatus;
		unsigned char *src, counter=0;	

		RAM_WPn = 0;
		src = source;
		WriteStatus=ExtEEPROM_Enable(WREN);
		if (WriteStatus!=(WEN|PROTECTION))
			return 5;
		
		for (i = 0; i <=(page_size /MAX_BYTE); i++) 
		{  
			
		  	
			RAM_CSn = 0;
			SPI_WriteByte(WRITE); 					//Sending the Write Data to Memory Array command
			WritePage +=(i* MAX_BYTE);					//Updating the address of mempry 
			SPI_WriteUInt(WritePage);	 					//Sending the address of location that we want to do the write
			
			if (i == page_size /MAX_BYTE)
			{	
				counter = (page_size % MAX_BYTE);
         }
			else
			{
				counter = MAX_BYTE;
			}
			
			for (j=0; j<counter; j++)
			{	
				SPI_WriteByte(*src);
				src++;								
			}

			EEP_SCK = 0;								// Programming will start after the RAM_CSn is brought high. 	
			RAM_CSn = 1;								// The low-to-high transition of the RAM_CSn must occur during 
			delay_us(1);								// the SCK low-time immediately after clocking in the 
															// D0 (LSB) data bit.

			// Enabling the write operation, the device returns to write disable 
			// state at the completion of a write ccycle (writing maximum 32 bytes)
			RAM_CSn = 0;
			WriteStatus=ExtEEPROM_Enable(WREN);
			if (WriteStatus!=WEN)
				return 6;
		}
	return 0;
}

signed char ExtEEPROM_Read (unsigned int ReadPage, void *destination, unsigned char page_size)
{
		
	unsigned char ReadStatus,i=0;
	unsigned char *des;
			
	des = destination;
	
	//Making sure that Write Protection pin is high
//	RAM_WPn = 1;	
	ReadStatus=ExtEEPROM_Enable(WREN);
	
	if (ReadStatus != (WEN|PROTECTION))
		return 4;


	RAM_CSn = 0;
	SPI_WriteByte(READ); 					//Sending the Write Data to Memory Array command
	ReadPage +=(i* MAX_BYTE);					//Updating the address of mempry 
	delay_us(10);
	SPI_WriteUInt(ReadPage);	 					//Sending the address of location that we want to do the write
	delay_us(1);
	
	for (i;i<page_size;i++)
	{
		*des=SPI_ReadByteFalling();
		des++;
		delay_us(10);
   }
	RAM_CSn = 1;	
	return 0;
}

signed char ExtEEPROM_Clear(unsigned int page, unsigned char page_size)
{
	
		unsigned char i,j;
		unsigned char counter=0;	

		//Writing the protection section
		if ( page+page_size > WP_START_ADDR )  
		{
				return ERROR;
		}
		
		for (i = 0; i <=(page_size /MAX_BYTE); i++) 
		{  
			
			// Enabling the write operation, the device returns to write disable state at the 
			// completion of a write ccycle (writing maximum 32 bytes)
			RAM_CSn = 0;
			delay_us(1);
			SPI_WriteByte(WREN);  					//Sending the Write Enable Latch command
			RAM_CSn = 1;	  
		  	delay_us(1);     

			RAM_CSn = 0;
			SPI_WriteByte(WRITE); 					//Sending the Write Data to Memory Array command
			page +=(i* MAX_BYTE);					//Updating the address of mempry 
			SPI_WriteUInt(page);	 					//Sending the address of location that we want to do the write
			
			if (i == page_size /MAX_BYTE)
			{	
				counter = (page_size % MAX_BYTE);
         }
			else
			{
				counter = MAX_BYTE;
			}
			
			for (j=0; j<counter; j++)
			{	
				SPI_WriteByte(0);								
			}

			EEP_SCK = 0;								// Programming will start after the RAM_CSn is brought high. 	
			RAM_CSn = 1;								// The low-to-high transition of the RAM_CSn must occur during 
															// the SCK low-time immediately after clocking in the 
															// D0 (LSB) data bit.
		}

return 0;
}


signed char ExtEEPROM_WriteProtect (void *source,unsigned char page_size)
{
	
		unsigned char i,j,PWriteStatus;
		unsigned int page;
		unsigned char *src, counter=0;	

		page = WP_START_ADDR;
		src = source;
		
		//Making sure that Write Protection pin is high
		RAM_WPn = 1;
		RAM_CSn = 0;
		PWriteStatus=ExtEEPROM_Enable(WREN);
		RAM_CSn = 1;

		//Writing to the Status register
		RAM_CSn = 0;
		SPI_WriteByte(WRSR);
		SPI_WriteByte(WEN);
		RAM_CSn = 1;
		delay_us(1);
		
		RAM_CSn = 0;
		PWriteStatus=ExtEEPROM_Enable(WREN);
	//BS	if (PWriteStatus!=WEN)
	//BS		return 1;
		
		for (i = 0; i <=(page_size /MAX_BYTE); i++) 
		{  
			
		  	
			RAM_CSn = 0;
			SPI_WriteByte(WRITE); 					//Sending the Write Data to Memory Array command
			page +=(i* MAX_BYTE);					//Updating the address of mempry 
			SPI_WriteUInt(page);	 					//Sending the address of location that we want to do the write
			
			if (i == page_size /MAX_BYTE)
			{	
				counter = (page_size % MAX_BYTE);
         }
			else
			{
				counter = MAX_BYTE;
			}
			
			for (j=0; j<counter; j++)
			{	
				SPI_WriteByte(*src);
				src++;								
			}

			EEP_SCK = 0;								// Programming will start after the RAM_CSn is brought high. 	
			RAM_CSn = 1;								// The low-to-high transition of the RAM_CSn must occur during 
			delay_us(1);								// the SCK low-time immediately after clocking in the 
															// D0 (LSB) data bit.

			// Enabling the write operation, the device returns to write disable 
			// state at the completion of a write ccycle (writing maximum 32 bytes)
			RAM_CSn = 0;
			PWriteStatus=ExtEEPROM_Enable(WREN);
			delay_us(10);
			RAM_CSn = 1;

//BS			if (PWriteStatus!=WEN)
//BS				return 2;
		}

	
	/*Defining the protection region here */
	
	//Writing to the Status register
	RAM_CSn = 0;
	SPI_WriteByte(WRSR);
	SPI_WriteByte(PROTECTION);
	RAM_CSn = 1;
	delay_us(10);
	
	RAM_CSn = 0;
	PWriteStatus=ExtEEPROM_Enable(RDSR);
//BS	if (PWriteStatus!=PROTECTION)
//BS		return 3;
	RAM_WPn = 0;
	
	RAM_CSn = 1;
	return 0;
}

unsigned char ExtEEPROM_Enable(unsigned char flag)
{
	unsigned char status;		
	
	if (flag==WREN)
	{	
		//Enabling the write operation
		RAM_CSn = 0;
		delay_us(10);
		SPI_WriteByte(WREN);  					
		delay_us(10);
		RAM_CSn = 1;
		delay_us(10);		
	}
		
	//Checking the status register
	RAM_CSn = 0;
	SPI_WriteByte(RDSR);
	status=SPI_ReadByteFalling();
	RAM_CSn = 1;
	delay_us(1);		
	return status;
}
