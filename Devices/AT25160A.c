/********************************************************************\

Name:         AT25160A.c
Created by:   Bahman Sotoodian        Feb/28/2007


Contents:     EEPROM (T25160AN-10SU-2.7) user interface
Differs from the Ext_EEPROM function as this
one include write operation to the Protective page

$Id$

\********************************************************************/
//  need to have EXT_EEPROM defined

#include "../mscbemb.h"
#include "AT25160A.h"
#include "../Protocols/SPI_handler.h"

//
//
//--------------------------------------------------------------------
signed char ExtEEPROM_Init (unsigned int page_size)
{
  // Unselecting the device
  RAM_CSn = 1;

  //Initializing the SPI
  SPI_Init();

  if(page_size > MAX_PAGE_SIZE)
  return ERROR;

  ExtEEPROM_Wait();

  return 0;
}

//--------------------------------------------------------------------
signed char ExtEEPROM_Read (unsigned int ReadPage
                          , unsigned char *destination
                          , unsigned int page_size)
{
  unsigned int i=0;

  //Making sure that Write Protection pin is high
  RAM_WPn = 1;
  ExtEEPROM_WriteEnable();

  RAM_CSn = 0;
  SPI_WriteByte(AT2516_READ);      //Sending the Write Data to Memory Array command
  SPI_WriteUInt(ReadPage);         //Sending the address of location that we want to do the write

  for (i;i<page_size;i++)
  {
    *destination=SPI_ReadByteRising();
    destination++;

  }
  delay_us(25);
  RAM_CSn = 1;
  RAM_WPn = 0;
  return 0;
}

//--------------------------------------------------------------------
signed char ExtEEPROM_WriteProtect(unsigned char *source, unsigned int page_size)
{
  unsigned int i,j;
  unsigned int eeprom_addr, blockSize;
  unsigned int counter = AT2516_MAX_BYTE;

  //Making sure that Write Protection pin is high
  RAM_WPn = 1;

  ExtEEPROM_WriteStatusReg(0x0);

  blockSize = page_size / AT2516_MAX_BYTE;

  for (i = 0; i <= blockSize; i++) {
    eeprom_addr = WP_START_ADDR + (i * AT2516_MAX_BYTE);    //Updating the address of memory
    if (i == blockSize) {
      counter = (page_size % AT2516_MAX_BYTE);
    }

    ExtEEPROM_WriteEnable();


    RAM_CSn = 0;

    SPI_WriteByte(AT2516_WRITE);    // Sending the Write Data to Memory Array command
    SPI_WriteUInt(eeprom_addr);     // Sending the address of location that we want to do the write

    for (j=0; j<counter; j++) {
      SPI_WriteByte(*source);
      source++;
    }

    delay_us(25);
    RAM_CSn = 1;                // The low-to-high transition of the RAM_CSn must occur during
    delay_us(25);
    ExtEEPROM_Wait();
  }

  ExtEEPROM_WriteStatusReg(AT2516_PROTECTION);
  RAM_WPn = 0;
  return 0;
}

//--------------------------------------------------------------------
signed char ExtEEPROM_Write_Clear(unsigned int write_addr
                                , unsigned char *source
                                , unsigned int page_size
                                , unsigned char clear)
{
  unsigned int i,j;
  unsigned int blockSize;
  unsigned int counter = AT2516_MAX_BYTE;

  //Making sure that Write Protection pin is high
  RAM_WPn = 1;
  blockSize = page_size / AT2516_MAX_BYTE;

  for (i = 0; i <= blockSize; i++) {
    //NW
    write_addr = i * AT2516_MAX_BYTE;          //Updating the address of mempry
    if (i == blockSize) {
      counter = (page_size % AT2516_MAX_BYTE);
    }

    ExtEEPROM_WriteEnable();

    RAM_CSn = 0;

    SPI_WriteByte(AT2516_WRITE);           //Sending the Write Data to Memory Array command
    SPI_WriteUInt(write_addr);             //Sending the address of location that we want to do the write

    for (j=0; j<counter; j++) {
      if (!clear){
        SPI_WriteByte(*source);
        source++;
      }
      else
      SPI_WriteByte(0x00);
    }

    delay_us(25);
    RAM_CSn = 1;                // The low-to-high transition of the RAM_CSn must occur during
    delay_us(25);
    ExtEEPROM_Wait();
  }

  RAM_WPn = 0;
  return 0;
}

//--------------------------------------------------------------------
void ExtEEPROM_WriteEnable(void)
{
  //Enabling the write operation
  RAM_CSn = 0;
  delay_us(10);
  SPI_WriteByte(AT2516_WREN);
  delay_us(25);
  RAM_CSn = 1;
  delay_us(25);
  ExtEEPROM_Wait();
}

//--------------------------------------------------------------------
void ExtEEPROM_WriteStatusReg(unsigned char status) {
  ExtEEPROM_WriteEnable();
  RAM_CSn = 0;
  SPI_WriteByte(AT2516_WRSR);
  SPI_WriteByte(status);
  delay_us(25);
  RAM_CSn = 1;
  delay_us(25);
  ExtEEPROM_Wait();
}

//--------------------------------------------------------------------
unsigned char ExtEEPROM_Status(void)
{
  unsigned char status;

  RAM_CSn = 0;
  SPI_WriteByte(AT2516_RDSR);
  status=SPI_ReadByteRising();
  delay_us(25);
  RAM_CSn = 1;
  delay_us(25);

  return status;
}

//--------------------------------------------------------------------
void ExtEEPROM_Wait(void) {
  unsigned char status;
  do {
    status = ExtEEPROM_Status();
  } while((status & 0x71) != 0);
}
