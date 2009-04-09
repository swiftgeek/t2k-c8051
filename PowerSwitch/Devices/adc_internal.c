/********************************************************************\
  Name:         adc_internal.c
  Created by:   

  Contents:     Internal ADC functions for F31x

  $Id$
\********************************************************************/

#include "../mscbemb.h"
#include "adc_internal.h"

/*------------------------------------------------------------------*/
void adc_internal_init(void)
{
  ADC0CN = 0xC0;   // ADC0 Enabled
                   // Tracking on request (ADBUSY)
                   // left-justified
  ADC0CF  = 0x80;  // ADC conversion 16 clocks

  REF0CN = 0x0E;   // External Vcc , Temperature, VREF0 Bias
}

/*------------------------------------------------------------------*/
unsigned int adc_read(unsigned char channel)
{
  /* set MUX */
  AMX0P = (channel & 0x1F);
  AMX0N = 0xFF;   // Input 0.. 15 + Temperature in single ended mode

  DISABLE_INTERRUPTS;

  AD0INT = 0;
  AD0BUSY = 1;

  AD0INT = 0;
  AD0BUSY = 1;

  while(AD0BUSY == 1);

  ENABLE_INTERRUPTS;

  return (((ADC0H & 0x03) << 8) + ADC0L);
}

