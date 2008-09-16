/*****************************************************************************
/*
  Name:         LTC1665_dac.h
  Created by:   Bahman Sotoodian                  Jan/21/2008


  Contents:     LTC1665 DAC user interface


\****************************************************************************/
#ifdef  _LTC1665_

#ifndef  _LTC1665_DAC_H
#define  _LTC1665_DAC_H

// Bias DAC CS port assignment
#define BIAS_DAC_CSn1  P0 ^ 6
#define BIAS_DAC_CSn2  P0 ^ 7
#define BIAS_DAC_CSn3  P3 ^ 0
#define BIAS_DAC_CSn4  P3 ^ 1
#define BIAS_DAC_CSn5  P3 ^ 5
#define BIAS_DAC_CSn6  P3 ^ 6
#define BIAS_DAC_CSn7  P2 ^ 5
#define BIAS_DAC_CSn8  P2 ^ 6

/* LTC1665 (8-bit DACs)*/
sbit CSn1 = BIAS_DAC_CSn1;
sbit CSn2 = BIAS_DAC_CSn2;
sbit CSn3 = BIAS_DAC_CSn3;
sbit CSn4 = BIAS_DAC_CSn4;
sbit CSn5 = BIAS_DAC_CSn5;
sbit CSn6 = BIAS_DAC_CSn6;
sbit CSn7 = BIAS_DAC_CSn7;
sbit CSn8 = BIAS_DAC_CSn8;

sbit SPI_SCK   = MSCB_SPI_SCK;     // SPI Protocol Serial Clock

/* LTC1665 related commands */
// Command format: Unused, address, unused, data(input by user)
#define DAC1  1
#define DAC2  2
#define DAC3  3
#define DAC4  4
#define DAC5  5
#define DAC6  6
#define DAC7  7
#define DAC8  8

//The LTC1665 instructions, refere to LCT1665 manual.
#define LTC1665_LOAD_A     0x1
#define LTC1665_LOAD_B     0x2
#define LTC1665_LOAD_C     0x3
#define LTC1665_LOAD_D     0x4
#define LTC1665_LOAD_E     0x5
#define LTC1665_LOAD_F     0x6
#define LTC1665_LOAD_G     0x7
#define LTC1665_LOAD_H     0x8
#define LTC1665_LOAD_ALL   0xF
#define LTC1665_SLEEP      0xE

void LTC1665_Init(void);
void LTC1665_Cmd(unsigned char addr,unsigned char DataBeSent, unsigned char channel);

#endif // _LTC_1665_DAC_H

#endif // _LTC1665_

