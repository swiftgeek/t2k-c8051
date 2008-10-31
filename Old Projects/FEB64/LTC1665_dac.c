/*********************************************************************
/*
	Name:         LTC1665_dac.c
	Created by:   Bahman Sotoodian  		Feb/21/2008

  Contents:     LTC1665 DAC user interface 

\********************************************************************/
//  need to have FEB64 defined

#ifdef  _LTC1665_

#include "mscbemb.h"
#include "../protocols/SPI_handler.h"
#include "LTC1665_dac.h"

void LTC1665_Init(void)
{
  SPI_Init();	
	CSn1 = CSn2 = CSn3 = CSn4 = CSn5 = CSn6 = CSn7 = CSn8 = 1;
}

void LTC1665_Cmd(unsigned char addr,unsigned char DataBeSent, unsigned char channel)
{
		SPI_SCK = 0; //SCK must be low before Chip Select is pulled low

		if(channel == DAC1)       CSn1 = 0;	
		else if (channel == DAC2) CSn2 = 0;
		else if (channel == DAC3) CSn3 = 0;
		else if (channel == DAC4) CSn4 = 0;
		else if (channel == DAC5) CSn5 = 0;
		else if (channel == DAC6) CSn6 = 0;
		else if (channel == DAC7) CSn7 = 0;
		else if (channel == DAC8) CSn8 = 0;
		
		SPI_WriteUInt((addr<<12)|(DataBeSent <<4)|(0x00));		
		delay_us(1);

		if(channel == DAC1)       CSn1 = 1;	
		else if (channel == DAC2) CSn2 = 1;
		else if (channel == DAC3) CSn3 = 1;
		else if (channel == DAC4) CSn4 = 1;
		else if (channel == DAC5) CSn5 = 1;
		else if (channel == DAC6) CSn6 = 1;
		else if (channel == DAC7) CSn7 = 1;
		else if (channel == DAC8) CSn8 = 1;
		
		delay_us(1);	
}
#endif