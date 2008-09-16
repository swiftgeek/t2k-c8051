/*****************************************************************************
/*
  Name:         AT25160A.h
  Created by:   Bahman Sotoodian  								Feb/28/2008


  Contents:     EEPROM user interface

  
\****************************************************************************/

#ifndef  _EEPROM_H_
#define  _EEPROM_H_

#ifndef  _SPI_PROTOCOL_
#define  _SPI_PROTOCOL_

// SPI Protocol Signals
sbit EEP_SCK  = MSCB_SPI_SCK;
sbit EEP_MOSI = MSCB_SPI_MISO;
sbit EEP_MISO = MSCB_SPI_MOSI;

sbit RAM_CSn  = RAM_CHIP_SELECT;     // The Chip Select Signal to select the chip
sbit RAM_HLDn = RAM_HOLD_DOWN;		 // The Hold Down Signal to puase the serial communication			
sbit RAM_WPn  = RAM_WRITE_PROTECT;   // The Write Protection signal to enable/disable write to 
												 // the status register

// The Instruction Set for the AT25160A Device 
#define AT2516_WREN  0x06  						//Set Write Enable Latch
#define AT2516_WRDI  0x04  						//Reset Write Enable Latch
#define AT2516_RDSR  0x05  						//Read Status Register
#define AT2516_WRSR  0x01  						//Write Status Register
#define AT2516_READ  0x03  						//Read Data from Memory Array
#define AT2516_WRITE 0x02  						//Write Data to Memory Array

#define MAX_PAGE_SIZE      0x200
#define WP_START_ADDR	     0x0600
#define WP_FINAL_ADDR	     0x07FF

#define AT2516_MAX_BYTE 32
#define AT2516_PROTECTION 0x84					//Setting the bits of Status register: 
												//Setting the BP0, WEN and WPEN to 1 and BP1 to 0.

//unsigned int page_address[10] =0;

// Defing the Status register's bits
#define AT2516_WEN   0x02
#define AT2516_WPEN  0x80

#define ERROR -1

signed char ExtEEPROM_Init (unsigned int page_size);
signed char ExtEEPROM_Read (unsigned int ReadPage, unsigned char *destination, 
unsigned int page_size);
signed char ExtEEPROM_Write_Clear(unsigned int WritePage, unsigned char *source, 
unsigned int page_size, unsigned char clear);
signed char ExtEEPROM_WriteProtect (unsigned char *source,unsigned int page_size);
void ExtEEPROM_WriteEnable(void);
void ExtEEPROM_WriteStatusReg(unsigned char status);
unsigned char ExtEEPROM_Status(void);
void ExtEEPROM_Wait(void);

#endif   //_SPI_PROTOCOL_
#endif   //_EEPROM_H_
