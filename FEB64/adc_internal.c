/********************************************************************\
  Name:         adc_internal.c
  Created by:   

  Contents:     Internal ADC functions for F12x-13x
                Midas Slow Control Bus protocol

  $Id$

\********************************************************************/

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

  REF0CN = 0x07;   // Temperature, VREF0 Bias & Buffer enable
}

/*------------------------------------------------------------------*/
unsigned int adc_read(unsigned char channel)
{
  /* set MUX */
  SFRPAGE = ADC0_PAGE;
  AMX0SL = (channel & 0x0F);

  AMX0CF = 0x00;   // Input 0.. 7 + Temperature in single ended mode


  DISABLE_INTERRUPTS;

  AD0INT = 0;
  AD0BUSY = 1;

  AD0INT = 0;
  AD0BUSY = 1;

  while(AD0BUSY == 1);

  ENABLE_INTERRUPTS;

  return (((ADC0H & 0x03) << 8) + ADC0L);
}

/*------------------------------------------------------------------*/
float read_voltage(unsigned char channel)
{
  unsigned int  xdata i;
  float         xdata voltage;
  unsigned int  xdata rawbin;
  unsigned long xdata rawsum = 0;

// Averaging on 10 measurements for now.
  for (i=0 ; i<10 ; i++) {
    rawbin = adc_read(channel);
    rawsum += rawbin;
    yield();
  }

  /* convert to V */
  voltage = (float)  rawsum / 10;              // averaging
  voltage = (float)  voltage / 1024.0 * VREF;  // conversion

  return voltage;
}

