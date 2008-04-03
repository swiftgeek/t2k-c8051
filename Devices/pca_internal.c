/********************************************************************\
  Name:         pca_internal.c
  Created by:   

  Contents:     Internal PCA functions for F12x-13x
                Midas Slow Control Bus protocol

  $Id$

\********************************************************************/

#include "../mscbemb.h"
#include "pca_internal.h"

/*---- PCA initilalization -----------------------------------------*/
void pca_operation(unsigned char mode)
{
//	signed char i;
	if (mode == 1) {
        // PCA setup for Frequency Output Mode on CEX0
        // Set P0.4 as CEX0, requires RX/TX+SDA/SCK to be enable in the XBAR0
		SFRPAGE  = CONFIG_PAGE;
		P0MDOUT |= 0x10; // Add pin 4 (CHG_PMP_CLK) to PushPull
		XBR0     = 0x0D; // Route SMB (SDA and SCL); Route UART (TX and RX);and Route CEX0(CHG_PMP_CLK) to their pins 
		XBR2     = 0x40; // Enable Xbar

		SFRPAGE  = LEGACY_PAGE;
		PCA0MD   = 0x02;  // Sysclk (default CKCON [2MHz])
		PCA0CPL0 = 0x00;
		PCA0CPM0 = 0x46; // ECM, TOG, PWM
		PCA0CPH0 = 10 ;   // (for ~2MHz)		       
		PCA0CN   = 0x40; // Enable PCA Run Control 

	} else if (mode == 2) {
		SFRPAGE  = LEGACY_PAGE;
		PCA0CN   = 0x00 ;   //Turn off the PCA counter

	} else if (mode == 3) {
		SFRPAGE  = LEGACY_PAGE;
		PCA0CN   = 0x40 ;   //Turn on the PCA counter
    }
	
}
