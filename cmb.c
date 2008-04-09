/********************************************************************\
 Name:         cmb.c
 Created by:   Bahman Sotoodian                
 Created on:   April/9/2008 
 
 Contents:     Application specific (user) part of
 					Midas Slow Control for CMB board. 					
   
 define(_SST_PROTOCOL_) 
 
 $Id:$
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
	4, UNIT_AMPERE,          0, 0, MSCBF_FLOAT, "pIs4V",      &user_data.pIs4V,      // 0
   4, UNIT_AMPERE, 			 0, 0, MSCBF_FLOAT, "IsSC",       &user_data.IsSC,       // 1
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "A33VMon",    &user_data.A33VMon,    // 2
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "A25VMon",    &user_data.A25VMon,    // 3
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "p15VMon",    &user_data.p15VMon,    // 4
	4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "p18VMon",    &user_data.p18VMon,    // 5	
	4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "p25VMon",    &user_data.p25VMon,    // 6
	4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "p33VMon",    &user_data.p33VMon,    // 7
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "uCTemp",     &user_data.uCTemp,     // 8

   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "ssTemp0",    &user_data.ssTemp[0],  // 9
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "ssTemp1",    &user_data.ssTemp[1],  // 10
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "ssTemp2",    &user_data.ssTemp[2],  // 11
   
	4, UNIT_BYTE,            0, 0, 		0, "rpIs4V",      &user_data.rpIs4V,      // 12
   4, UNIT_BYTE, 			    0, 0, 		0, "rIsSC",       &user_data.rIsSC,       // 13
   4, UNIT_BYTE,            0, 0,		0, "rA33VMon",    &user_data.rA33VMon,    // 14
   4, UNIT_BYTE,            0, 0, 		0, "rA25VMon",    &user_data.rA25VMon,    // 15
   4, UNIT_BYTE,            0, 0, 		0, "rp15VMon",    &user_data.rp15VMon,    // 16
	4, UNIT_BYTE,            0, 0, 		0, "rp18VMon",    &user_data.rp18VMon,    // 17	
	4, UNIT_BYTE,            0, 0, 		0, "rp25VMon",    &user_data.rp25VMon,    // 18
	4, UNIT_BYTE,            0, 0, 		0, "rp33VMon",    &user_data.rp33VMon,    // 19	
	0
 };

 MSCB_INFO_VAR *variables = vars;   // Structure mapping
 // Get sysinfo if necessary
 extern SYS_INFO sys_info;          // For address setting
 

 
 
 /********************************************************************\
 Application specific init and in/output routines
 \********************************************************************/
 
 /*---- User init function ------------------------------------------*/
 void user_init(unsigned char init)
 {
 	char xdata i,add;

 	if (init)
	{
	  for (i=0;i<3;i++)	    	
		user_data.ssTemp[i]  = 0.0;	
		user_data.error = 0;
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

   ADT7486A_Init();
 #endif
	
	user_data.error = 0;
	add = cur_sub_addr();
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
	unsigned int *rpfData,rvolt;

  //
  //-----------------------------------------------------------------------------
  //Internal ADCs monitoring Voltages and Currents based on time
    pfData = &(user_data.pIs4V);
    rpfData = &(user_data.rpIs4V);
    for (channel=0; channel<INTERNAL_N_CHN ; channel++) {
      volt = read_voltage(channel,&rvolt);
      //volt = volt * coeff[channel] + offset[channel];
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
    	if(!ADT7486A_Cmd(ADT7486A_address, GetIntTemp, &temperature)){
		  IntssTT = CLEAR;
        DISABLE_INTERRUPTS;
         user_data.ssTemp[0] = temperature;
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
     if(!ADT7486A_Cmd(ADT7486A_address, GetExt1Temp, &temperature)){
         Ext1ssTT = CLEAR;
         DISABLE_INTERRUPTS;
          user_data.ssTemp[1] = temperature;
          user_data.error   = rESR;
         ENABLE_INTERRUPTS;
     } else {
         DISABLE_INTERRUPTS;
          Ext1ssTT = SET;
          user_data.error   = rESR;
         ENABLE_INTERRUPTS;
     }
    
     if(!ADT7486A_Cmd(ADT7486A_address, GetExt2Temp, &temperature)){
         Ext2ssTT = CLEAR;
         DISABLE_INTERRUPTS;
          user_data.ssTemp[2] = temperature;
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
