/********************************************************************\

  Name:         LTC1669_dac.h
  Created by:     								Jan/15/2007


  Contents:     LTC1669 DAC user interface

  $Id$

\********************************************************************/
//  need to have T2KASUM defined

#ifdef _LTC1669_

#ifndef  _LTC1669_DAC_H
#define  _LTC1669_DAC_H

#define LTC1669_UPDATE_ON_SYNC	0x01
#define LTC1669_POWER_DOWN			0x02
#define LTC1669_INT_BG_REF			0x04

void LTC1669_Init(void);
void LTC1669_Cmd(unsigned char addr, unsigned char cmd);
void LTC1669_SetDAC(unsigned char addr, unsigned char cmd, unsigned int dataWord);
void dowhile(char *busy, char location);

#endif // _LTC1669_DAC_H

#endif // _LTC_1669_