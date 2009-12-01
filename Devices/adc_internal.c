/********************************************************************\
  Name:         adc_internal.c
  Created by:   

  Contents:     Internal ADC functions for F12x-13x
                Midas Slow Control Bus protocol

  $Id$

\********************************************************************/

#include "../mscbemb.h"
#include "adc_internal.h"

/*------------------------------------------------------------------*/
void adc_internal_init(void)
{
#ifdef CPU_C8051F120
  SFRPAGE = ADC0_PAGE;
  ADC0CN = 0xC0;   // ADC0 Enabled
                   // Tracking on request (ADBUSY)
                   // left-justified
  ADC0CF  = 0x80;  // ADC conversion 16 clocks
  ADC0CF |= 0x00;  // PGA gain = 1

  REF0CN = 0x07;   // Temperature, VREF0 Bias & Buffer enable
#endif
#ifdef CPU_C8051F310
  ADC0CN = 0xC0;   // ADC0 Enabled
                   // Tracking on request (ADBUSY)
                   // left-justified
  ADC0CF  = 0x80;  // ADC conversion 16 clocks
  REF0CN = 0x0E;   // External Vcc , Temperature, VREF0 Bias
#endif
}

/*------------------------------------------------------------------*/
unsigned int adc_read(unsigned char channel, unsigned char gain)
{
#ifdef CPU_C8051F120
  /* set MUX */
  SFRPAGE = ADC0_PAGE;
  AMX0SL = (channel & 0x0F);
  ADC0CF = ((ADC0CF&0xF8)|gain);
  AMX0CF = 0x00;   // Input 0.. 7 + Temperature in single ended mode
#endif
#ifdef CPU_C8051F310
  /* set MUX */
  AMX0P = (channel & 0x1F);
  AMX0N = 0xFF;   // Input 0.. 15 + Temperature in single ended mode
#endif

  DISABLE_INTERRUPTS;

  AD0INT = 0;
  AD0BUSY = 1;

  AD0INT = 0;
  AD0BUSY = 1;

  while(AD0BUSY == 1);

  ENABLE_INTERRUPTS;

  return (((ADC0H & 0x03) << 8) + ADC0L);
}


