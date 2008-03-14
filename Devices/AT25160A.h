/*****************************************************************************
/*
  Name:         AT25160A.h
  Created by:   Bahman Sotoodian  								Feb/28/2008


  Contents:     EEPROM user interface

  
\****************************************************************************/

#ifndef  _EEPROM_H_
#define  _EEPROM_H_

// SPI Protocol Signals
sbit EEP_SCK  = MSCB_SPI_SCK;
sbit EEP_MOSI = MSCB_SPI_MISO;
sbit EEP_MISO = MSCB_SPI_MOSI;


#define RAM_CHIP_SELECT    P3 ^ 7
#define RAM_HOLD_DOWN      P2 ^ 2
#define RAM_WRITE_PROTECT  P2 ^ 0

sbit RAM_CSn  = RAM_CHIP_SELECT;     // The Chip Select Signal to select the chip
sbit RAM_HLDn = RAM_HOLD_DOWN;		 // The Hold Down Signal to puase the serial communication			
sbit RAM_WPn  = RAM_WRITE_PROTECT;   // The Write Protection signal to enable/disable write to 
												 // the status register

#define WP_START_ADDR	0x0600
#define WP_FINAL_ADDR	0x07FF

// The Instruction Set for the AT25160A Device 
#define WREN  0x06  						//Set Write Enable Latch
#define WRDI  0x04  						//Reset Write Enable Latch
#define RDSR  0x05  						//Read Status Register
#define WRSR  0x01  						//Write Status Register
#define READ  0x03  						//Read Data from Memory Array
#define WRITE 0x02  						//Write Data to Memory Array


#define MAX_BYTE 32
#define PROTECTION 0x84					//Setting the bits of Status register: 
												//Setting the BP0, WEN and WPEN to 1 and BP1 to 0.

//unsigned int page_address[10] =0;

// Defing the Status register's bits
#define WEN   0x02
#define WPEN  0x80

#define ERROR -1

signed char ExtEEPROM_Init (unsigned char npages, unsigned char page_size);
signed char ExtEEPROM_Read (unsigned int ReadPage, void *destination, unsigned char page_size);
signed char ExtEEPROM_Write(unsigned int WritePage, void *source, unsigned char page_size);
signed char ExtEEPROM_Clear(unsigned int ClearPage, unsigned char page_size);
signed char ExtEEPROM_WriteProtect (void *source,unsigned char page_size);
unsigned char ExtEEPROM_Enable(unsigned char flag);


#endif