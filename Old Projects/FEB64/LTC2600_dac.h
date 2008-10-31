/*****************************************************************************
/*
  Name:         LTC2600.h
  Created by:   Bahman Sotoodian  								Feb/27/2008


  Contents:     LTC2600 DAC user interface

  
\****************************************************************************/
#ifndef  _LTC2600_DAC_H
#define  _LTC2600_DAC_H

//Pin assignment
#define SUM_DAC_CSn P1 ^ 4

sbit DAC_SCK = MSCB_SPI_SCK;    // // SPI Protocol Serial Clock
sbit SUM_CSn = SUM_DAC_CSn;


#define WriteTo_Update		 0x3 //Write to and Update command is used for loading DACs 
#define PowerDown				 0x4

/* LTC2600 related commands */
	//Command Format: command, address, MSB data(input by user), LSB data(input by user)
#define LTC2600_LOAD_A		 WriteTo_Update, 0x0 
#define LTC2600_LOAD_B		 WriteTo_Update, 0x1 
#define LTC2600_LOAD_C		 WriteTo_Update, 0x2 
#define LTC2600_LOAD_D		 WriteTo_Update, 0x3 
#define LTC2600_LOAD_E		 WriteTo_Update, 0x4
#define LTC2600_LOAD_F		 WriteTo_Update, 0x5
#define LTC2600_LOAD_G		 WriteTo_Update, 0x6
#define LTC2600_LOAD_H		 WriteTo_Update, 0x7
#define LTC2600_LOAD_ALL	 WriteTo_Update, 0xF
#define LTC2600_PWRDOWN_ALL PowerDown, 	  0xF, 0x00, 0x00 //doesn't need input by user

void LTC2600_Init(void);
void LTC2600_Cmd(unsigned char cmd, unsigned char addr, unsigned int DataBeSent);

#endif