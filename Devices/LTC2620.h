/*****************************************************************************
/*
  Name:         LTC2620.h
  Created by:   Bahman Sotoodian  								Feb/27/2008


  Contents:     LTC2620 DAC user interface

  
\****************************************************************************/
#ifdef _LTC2620_

#ifndef  _LTC2620_DAC_H
#define  _LTC2620_DAC_H

sbit DAC_SCK = MSCB_SPI_SCK;    
sbit DAC_DAT = MSCB_SPI_MOSI;   
sbit DAC_CS1 = P0 ^ 1;
sbit DAC_CS2 = P0 ^ 2;

#define WriteTo_Update		 0x3 //Write to and Update command is used for loading DACs 
#define PowerDown				   0x4

/* LTC2620 related commands */
	//Command Format: command, address, MSB data(input by user), LSB data(input by user)
#define LTC2620_LOAD_A		 0x0 
#define LTC2620_LOAD_B		 0x1 
#define LTC2620_LOAD_C		 0x2 
#define LTC2620_LOAD_D		 0x3 
#define LTC2620_LOAD_E		 0x4
#define LTC2620_LOAD_F		 0x5
#define LTC2602_LOAD_G		 0x6
#define LTC2620 LOAD_H		 0x7
#define LTC2620_LOAD_ALL	 0xF

// Macro                            Cmd,  Chip, Allch, data ignored
#define LTC2620_1_PWRDOWN_ALL PowerDown, 	0x1,  0xF,   0x00  //doesn't need input by user
#define LTC2620_2_PWRDOWN_ALL PowerDown, 	0x2,  0xF,   0x00  //doesn't need input by user

void LTC2620_Init(unsigned char chip);
void LTC2620_Cmd(unsigned char cmd, unsigned char chip, unsigned char addr, unsigned int DataBeSent);

#endif // _LTC2620_DAC_H

#endif // _LTC2620_
