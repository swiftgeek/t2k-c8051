/********************************************************************\

  Name:         LTC1669_dac.h
  Created by:     								Jan/15/2007


  Contents:     LTC1669 DAC user interface

  $Id: LTC1669_dac.h 58 2008-02-28 22:40:33Z fgddaq $

\********************************************************************/
//  need to have T2KASUM defined

#ifndef  _LTC1669_DAC_H
#define  _LTC1669_DAC_H

#define LTC1669_READ      1
#define LTC1669_WRITE     0

#define Command 0x04	//Selecting the Internal bandgap as reference so setting 
					      //the full-scale output voltage to 2.5 V

void LTC1669_Init(void);
void LTC1669_Cmd(unsigned char addr, unsigned char datMSB, unsigned char datLSB, bit flag);
void LTC1669_Cmd1(unsigned char addr, unsigned int input, bit flag);
#endif