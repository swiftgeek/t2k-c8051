/********************************************************************\
 Name:         cmb.c
 Created by:   Bahman Sotoodian                
 Created on:   April/9/2008 
 
 Contents:     Application specific (user) part of
 					Midas Slow Control for CMB board. 					
   
 define(_SST_PROTOCOL_) 
 
 $Id$
 \********************************************************************/
 
 #include <stdio.h>
 #include <math.h>
 #include "mscbemb.h"
 #include "cmb.h"

 #include "Devices/adc_internal.h"

 #ifdef   _ADT7486A_
 #include "Devices/ADT7486A_tsensor.h"
 #endif

 
 //
 // Global declarations
 //-----------------------------------------------------------------------------
 char code  node_name[] = "cmb";
 
 //
 // Declare globally the number of sub-addresses to framework
 unsigned char idata _n_sub_addr = 1;

 //
 unsigned char channel;
	
 // User Data structure declaration
 //-----------------------------------------------------------------------------
 MSCB_INFO_VAR code vars[] = {
	1, UNIT_BYTE,            0, 0, 			  0, "Error",      &user_data.error,      // 0
	4, UNIT_BYTE,            0, 0, 			  0, "SerialN",    &user_data.SerianN,    // 1
	4, UNIT_AMPERE,          0, 0, MSCBF_FLOAT, "pIs4V",      &user_data.pIs4V,      // 2
   4, UNIT_AMPERE, 			 0, 0, MSCBF_FLOAT, "IsSC",       &user_data.IsSC,       // 3
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "A33VMon",    &user_data.A33VMon,    // 4
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "A25VMon",    &user_data.A25VMon,    // 5
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "p15VMon",    &user_data.p15VMon,    // 6
	4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "p18VMon",    &user_data.p18VMon,    // 7	
	4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "p25VMon",    &user_data.p25VMon,    // 8
	4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "p33VMon",    &user_data.p33VMon,    // 9
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "uCTemp",     &user_data.uCTemp,     // 10

   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "InteTemp",   &user_data.InteTemp,  // 11

   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "FPGATemp",   &user_data.FPGATemp,  // 12
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "VregTemp",   &user_data.VregTemp,  // 13
   
	2, UNIT_BYTE,            0, 0, 		0, "rpIs4V",      &user_data.rpIs4V,      // 14
   2, UNIT_BYTE, 			    0, 0, 		0, "rIsSC",       &user_data.rIsSC,       // 15
   2, UNIT_BYTE,            0, 0,		0, "rA33VMon",    &user_data.rA33VMon,    // 16
   2, UNIT_BYTE,            0, 0, 		0, "rA25VMon",    &user_data.rA25VMon,    // 17
   2, UNIT_BYTE,            0, 0, 		0, "rp15VMon",    &user_data.rp15VMon,    // 18
	2, UNIT_BYTE,            0, 0, 		0, "rp18VMon",    &user_data.rp18VMon,    // 19	
	2, UNIT_BYTE,            0, 0, 		0, "rp25VMon",    &user_data.rp25VMon,    // 20
	2, UNIT_BYTE,            0, 0, 		0, "rp33VMon",    &user_data.rp33VMon,    // 21	
	0
 };

 MSCB_INFO_VAR *variables = vars;   // Structure mapping
 // Get sysinfo if necessary
 extern SYS_INFO sys_info;          // For address setting
 

 //
 // 
 //-----------------------------------------------------------------------------
float read_voltage(unsigned char channel,unsigned int *rvalue)
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
  *rvalue =  rawsum/10;
  voltage = (float)  *rvalue;                  // averaging
  voltage = (float)  voltage / 1024.0 * VREF;  // conversion
  if ( channel != TCHANNEL)
	  voltage = voltage * coeff[channel] + offset[channel];

  return voltage;
}

 
 /********************************************************************\
 Application specific init and in/output routines
 \********************************************************************/
 
 /*---- User init function ------------------------------------------*/
 void user_init(unsigned char init)
 {
   char xdata pca_add=0;
	unsigned int xdata crate_add=0, board_address=0;

 	if (init){
    	user_data.InteTemp = 0;	
		user_data.FPGATemp = 0;
		user_data.VregTemp = 0;
		user_data.error = 0;
		user_data.SerianN = 0x0;
	}
 
 //
 // Initial setting for communication and overall ports (if needed).
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   // P0MDOUT already contains Tx in push-pull
   P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull

 //
 // uC Miscellaneous ADCs (V/I Monitoring)
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   P3MDOUT |= 0x1C; //Setting the Regulators control pins to push pull (3 Vreg)
   adc_internal_init();


 //
 // SST Temperatures
 //-----------------------------------------------------------------------------
 #ifdef _ADT7486A_
   SFRPAGE  = CONFIG_PAGE;
   P1MDOUT |= 0x01; // Setting the SST_DRV (SST) to push pull
   SFRPAGE  = CPT1_PAGE;
   CPT1CN  |= 0x80; // Enable the Comparator 1
   CPT1MD   = 0x03; //Comparator1 Mode Selection
   //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

   ADT7486A_Init(SST_LINE1);
 #endif
	
	user_data.error = 0;
	
	//Configure and read the address
   //C C C C C C 0 B B is the MSCB Addr[8..0], 9 bits
   //Modifying what the board reads from the PCA 
	SFRPAGE = CONFIG_PAGE;
	P3MDOUT = 0x00;
	P3=0xFF;
	pca_add= P3;
   crate_add= ((~pca_add)<<3)  & 0x01F8;
   board_address=(crate_add &  0x01FC) | 0x0004;
   sys_info.node_addr   = board_address; 
 }

/*---- User write function -----------------------------------------*/
 #pragma NOAREGS

void user_write(unsigned char index) reentrant
{
	if(index);
}
 
 /*---- User read function ------------------------------------------*/
 unsigned char user_read(unsigned char index)
 {
   if (index);
   return 0;
 }
 
 /*---- User function called via CMD_USER command -------------------*/
 unsigned char user_func(unsigned char *data_in, unsigned char *data_out)
 {
   /* echo input data */
   data_out[0] = data_in[0];
   data_out[1] = data_in[1];
   return 2;
 }
 
//-----------------------------------------------------------------------------
void user_loop(void) {

	float volt,temperature, *pfData;
	unsigned int *xdata rpfData;
	unsigned int xdata rvolt;

  //
  //-----------------------------------------------------------------------------
  //Internal ADCs monitoring Voltages and Currents based on time
    pfData = &(user_data.pIs4V);
    rpfData = &(user_data.rpIs4V);
    for (channel=0; channel<INTERNAL_N_CHN ; channel++) {
      volt = read_voltage(channel,&rvolt);
      DISABLE_INTERRUPTS;
      pfData[channel] = volt;
    	rpfData[channel]= rvolt;
      ENABLE_INTERRUPTS;	
	 }
 //	
 //-----------------------------------------------------------------------------
 // Read uC temperature    
    volt = read_voltage(TCHANNEL,&rvolt);
    /* convert to deg. C */
    temperature = 1000 * (volt - 0.776) / 2.86;   // Needs calibration
    /* strip to 0.1 digits */
    temperature = ((int) (temperature * 10 + 0.5)) / 10.0;
    DISABLE_INTERRUPTS;
     user_data.uCTemp = (float) temperature;
    ENABLE_INTERRUPTS;	
		 
	 
  //
  //-----------------------------------------------------------------------------
  // Internal temperature reading
	 #ifdef _ADT7486A_
    	if(!ADT7486A_Cmd(ADT7486A_address, GetIntTemp, SST_LINE1, &temperature)){
		  IntssTT = CLEAR;
        DISABLE_INTERRUPTS;
         user_data.InteTemp = temperature;
         user_data.error   = rESR;
        ENABLE_INTERRUPTS;
    	} else {
        DISABLE_INTERRUPTS;
         IntssTT = SET;
         user_data.error   = rESR;
        ENABLE_INTERRUPTS;
    	}
  
  //
  //-----------------------------------------------------------------------------
  // External temperature readings
     if(!ADT7486A_Cmd(ADT7486A_address, GetExt1Temp, SST_LINE1, &temperature)){
         Ext1ssTT = CLEAR;
         DISABLE_INTERRUPTS;
          user_data.FPGATemp = temperature;
          user_data.error   = rESR;
         ENABLE_INTERRUPTS;
     } else {
         DISABLE_INTERRUPTS;
          Ext1ssTT = SET;
          user_data.error   = rESR;
         ENABLE_INTERRUPTS;
     }
    
     if(!ADT7486A_Cmd(ADT7486A_address, GetExt2Temp, SST_LINE1, &temperature)){
         Ext2ssTT = CLEAR;
         DISABLE_INTERRUPTS;
          user_data.VregTemp = temperature;
          user_data.error   = rESR;
         ENABLE_INTERRUPTS;
     } else {
         DISABLE_INTERRUPTS;
          Ext2ssTT = SET;
          user_data.error   = rESR;
         ENABLE_INTERRUPTS;
      }
   #endif
	 								
//	yield();
} 
