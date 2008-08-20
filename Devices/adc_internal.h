/********************************************************************\

  Name:         adc_internal.h
  Created by:     							


  Contents:     Internal ADCs

  $Id$

\********************************************************************/

#ifndef  _INTERNAL_ADC_H
#define  _INTERNAL_ADC_H

// Number of active internal ADCs
#define INTERNAL_N_CHN 8   // Number of Internal ADCs
#define TCHANNEL       8   // Temperature channel
void adc_internal_init(void);
unsigned int adc_read(unsigned char channel, unsigned char gain);

#endif