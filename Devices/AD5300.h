/*****************************************************************************
/*
  Name:         LTC1665_dac.h
  Created by:   Bahman Sotoodian  	          Jan/21/2008


  Contents:     LTC1665 DAC user interface

  $Id$
\****************************************************************************/
// Bias DAC CS port assignment
sbit sync = MSCB_SYNC;

void AD5300_Init(void);
void AD5300_Cmd(unsigned char DataBeSent);
