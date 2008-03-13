/********************************************************************\

  Name:         adc_internal.h
  Created by:     							


  Contents:     Internal ADCs

  $Id: adc_internal.h 62 2008-03-01 09:18:13Z midas $

\********************************************************************/

#ifndef  _INTERNAL_ADC_H
#define  _INTERNAL_ADC_H

// External Vref
#define VREF       2.43f

// Number of active internal ADCs
#define INTERNAL_N_CHN 8   // Number of Internal ADCs
#define TCHANNEL       8   // Temperature channel
void adc_internal_init(void);
unsigned int adc_read(unsigned char channel);
float read_voltage(unsigned char channel);

#endif