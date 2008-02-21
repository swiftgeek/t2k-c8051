/********************************************************************\
  Name:         adc_internal.c
  Created by:   

  Contents:     Internal ADC functions for F12x-13x
                Midas Slow Control Bus protocol

  $Id$

\********************************************************************/

#ifdef _ADC_INTERNAL_

#include "mscbemb.h"
#include "adc_internal.h"

/*------------------------------------------------------------------*/
void adc_internal_init(void)
{
  SFRPAGE = ADC0_PAGE;
  ADC0CN = 0xC0;   // ADC0 Enabled
                   // Tracking on request (ADBUSY)
                   // left-justified
  ADC0CF  = 0x80;  // ADC conversion 16 clocks
  ADC0CF |= 0x00;  // PGA gain = 1

  REF0CN = 0x03;   // VREF0 Bias & Buffer enable
}

/*------------------------------------------------------------------*/
unsigned int adc_read(unsigned char channel)
{
  /* set MUX */
  SFRPAGE = ADC0_PAGE;
  AMX0SL = (channel & 0x0F);

  ADC0CF  = 0x80;  // ADC conversion 16 clocks
  ADC0CF |= 0x00;  // PGA gain = 1 (0x06 = 0.5)
  AMX0CF = 0x00;   // Input 0..8 in single ended mode


  DISABLE_INTERRUPTS;

  AD0INT = 0;
  AD0BUSY = 1;

  while(AD0BUSY == 1);

  ENABLE_INTERRUPTS;

  return (ADC0L + ((ADC0H & 0x03) * 256));
}

/*------------------------------------------------------------------*/
float read_voltage(unsigned char channel)
{
  unsigned int  i;
  float         voltage;
  unsigned long uv_average;
  signed long   vsum_average;

  vsum_average = 0;

// Averaging on 10 measurements for now.
  for (i=0 ; i<10 ; i++) {
    uv_average = adc_read(channel);
    vsum_average = (uv_average & 0x0800) ? vsum_average - ((~uv_average + 0x0001) & 0x0FFF) : vsum_average + uv_average;
    yield();
  }

  /* convert to V */
  voltage = (float)  vsum_average / 10;         // averaging
  voltage = (float)  voltage / 1024.0 * VREF;   // conversion

  return voltage;
}
#endif

