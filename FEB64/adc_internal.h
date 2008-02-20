/********************************************************************\

  Name:         adc_internal.h
  Created by:     							


  Contents:     Internal ADCs

  $Id$

\********************************************************************/

#ifndef  _INTERNAL_ADC_H
#define  _INTERNAL_ADC_H

#define VREF       2.4f

void adc_internal_init(void);
unsigned int adc_read(unsigned char channel);
float read_voltage(unsigned char channel);

#endif