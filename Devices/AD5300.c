/*********************************************************************
/*
	Name:         AD5300.c
	Created by:   Noel Wu    		Aug/06/2008

  Contents:     AD5300 DAC user interface 
 $Id$
\********************************************************************/

#ifdef  _AD5300_

#ifndef _SPI_PROTOCOL_
#define _SPI_PROTOCOL_
#endif

//-------------------------------------------------------------------
// Include files
//-------------------------------------------------------------------
#include "../mscbemb.h"
#include "../Protocols/SPI_handler.h"
#include "AD5300.h"

//
//-------------------------------------------------------------------
/**
Initializing the SPI communication protocol and also unselecting all the 8 devices
*/
void AD5300_Init(void)
{
	sync=1;
   SPI_Init();	
}
//
//-------------------------------------------------------------------
/**
Sending data to the requested device
@param addr			Address associated with one of the 8 APD_DACs of each device
@param DataBeSent		One bytes of data to set the APD_DACs
@param channel			The channel will determine which device to be selected
*/
void AD5300_Cmd(unsigned char DataBeSent)
{
	sync = 0;	
	delay_us(1);
	SPI_WriteUInt((DataBeSent <<4)|(0x0000));		
	delay_us(1);
	sync = 1;	
}
#endif