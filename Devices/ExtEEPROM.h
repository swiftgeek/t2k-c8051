/*****************************************************************************
/*
  Name:         ExtEEPROM.h
  Created by:   Bahman Sotoodian                                Feb/28/2008


  Contents:     EEPROM user interface

\****************************************************************************/

#ifndef  _ExtEEPROM_H_
#define  _ExtEEPROM_H_

//ExtEEPROM port assignment
#ifdef FEB64
#define RAM_CHIP_SELECT    P3 ^ 7
#define RAM_HOLD_DOWN      P2 ^ 2
#define RAM_WRITE_PROTECT  P2 ^ 0

#elif defined(TEMP36)   //NW temp36 board
#define RAM_CHIP_SELECT    P2 ^ 0
#define RAM_HOLD_DOWN      P2 ^ 3
#define RAM_WRITE_PROTECT  P2 ^ 2

//#define PAGE_SIZE           0x4E         //Would define the length of our pages
//#define NUMEBER_PAGES       0x06         //Would define number of pages  

#elif defined(LPB)   //NW temp36 board
#define RAM_CHIP_SELECT    P1 ^ 2
#define RAM_HOLD_DOWN      P2 ^ 2
#define RAM_WRITE_PROTECT  P2 ^ 0

#define PAGE_SIZE           0x4E         //Would define the length of our pages
#define NUMEBER_PAGES       0x04         //Would define number of pages  
#endif

sbit RAM_CSn  = RAM_CHIP_SELECT;     // The Chip Select Signal would select/unselect the chip
sbit RAM_HLDn = RAM_HOLD_DOWN;       // The Hold Down Signal would puase the serial communication           
sbit RAM_WPn  = RAM_WRITE_PROTECT;   // The Write Protection signal would enable/disable write to 
                                                 // the status register

    

//The EEP_CLEAR and EEP_WRITE would be passed to the ExtEEPROM_Write_Clear() function
//to determine whether to perform the "clear" operation or the "write" one.
#define CLEAR_EEPROM                0x00         
#define WRITE_EEPROM                0x01

// The Instruction Set for the AT25160A Device 
#define EEP_WRENcmd  0x06            //Set Write Enable Latch
#define EEP_WRDIcmd  0x04            //Reset Write Enable Latch
#define EEP_RDSRcmd  0x05            //Read Status Register
#define EEP_WRSRcmd  0x01            //Write Status Register
#define EEP_READcmd  0x03            //Read Data from Memory Array
#define EEP_WRITEcmd 0x02            //Write Data to Memory Array

//When the BP1 and BP0 has been set properly to 0 and 1 respectively 
//(refere to AT25160AN manual)
#define WP_START_ADDR   0x0600      //The first address of write protected block 
#define WP_FINAL_ADDR   0x07FF      //The last address of write protected block


#define EEP_MAX_BYTE 32             //Maximum number of bytes that can be written to memory 
                                            //during each write cycle.
#define EEP_PROTECTION 0x84     //Setting the bits of status register: 
                                            //Setting the BP0, WEN and WPEN to 1 and BP1 to 0 to write 
                                            //protect the last quarter of memory

#define EEP_ERROR        0              
#define EEP_SUCCESS      1
#define EEP_READY        1                      
#define EEP_PROTECTED    2  
#define EEP_BUSY         3
#define EEP_delay       25
#define W_INTERNAL    0x71        //Checking bit 3-6 of status register to see
                                          //whether device is busy performing the internal write 
                                          //cycle or is ready   

void ExtEEPROM_Init (void);
unsigned char ExtEEPROM_Read (unsigned int ReadPage
                            , unsigned char *destination
									 , unsigned int page_size);

unsigned char ExtEEPROM_Write_Clear(unsigned int WritePage
                                  , unsigned char **source
                                  , unsigned int  page_size
                                  , unsigned char WC_flag
                                  , unsigned char *flag);
unsigned char ExtEEPROM_WriteEnable(void);
unsigned char ExtEEPROM_WriteStatusReg(unsigned char status);
unsigned char ExtEEPROM_Status(void);
unsigned char ExtEEPROM_Wait(void);

#endif