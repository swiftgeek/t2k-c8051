/********************************************************************\
  Name:         feb64.c
  Created by:   Bahman Sotoodian  							Feb/11/2008


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for FEB64 board
					 Use "define" for turning functions ON
					 Possible defines:
					 _PCA_INTERNAL_ : Q pump 
					 _ADC_INTERNAL_ : Misc V/I monitoring
					 _PCA9539_      : Bias switches
					 _ADT7486_      : SST Temperature
                	 _AD5301_       : Q pump DAC

  $Id$

\********************************************************************/
//  need to have FEB64 defined.

#include <stdio.h>
#include <math.h>
#include "mscbemb.h"
#include "feb64.h"

#ifdef _ADC_INTERNAL_
#include "adc_internal.h"
#endif
#ifdef _PCA_INTERNAL_
#include "pca_internal.h"
#endif

#ifdef _PCA9539_
#include "PCA9539_io.h"
#endif

#ifdef _LTC1669_
#include "LTC1669_dac.h"
#endif


// #include "ADT7486A_tsensor.h"
// #include "AD5301_dac.h"
// #include "AD7718_adc.h"

#define N_CHN 8

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

char code node_name[] = "FEB64";

/* Charge Pump */
char qpump;
sbit QPUMP = P0 ^ 4;         

/* Testing the SMBus's Clock */
sbit Clock = P0 ^ 3;

unsigned int adc_read(unsigned char channel);
float        read_voltage(unsigned char channel);

 
struct user_data_type xdata user_data;

/* User Data structure declaration */
MSCB_INFO_VAR code vars[] = {
   1, UNIT_BYTE,            0, 0,           0, "Control1",     &user_data.control1,    // 0
   1, UNIT_BYTE,            0, 0,           0, "Control2",     &user_data.control2,    // 1   
   1, UNIT_BYTE,            0, 0,           0, "Status",       &user_data.status,      // 2
   1, UNIT_BYTE,            0, 0,           0, "BiasEN",       &user_data.BiasEN,      // 3
   2, UNIT_BYTE,            0, 0,           0, "AsumDac",      &user_data.AsumDac,     // 4
   1, UNIT_BYTE,            0, 0,           0, "QpumpDac",     &user_data.QpumpDac,    // 5
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVGbl",   &user_data.BiasVGbl,    // 70 
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIGbl",   &user_data.BiasIGbl,    // 71  
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "pAVMon",     &user_data.pAVMon, 	 // 72   
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "pAIMon",     &user_data.pAIMon, 	 // 73
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "pDVMon",     &user_data.pDVMon, 	 // 74 
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "pDIMon",     &user_data.pDIMon, 	 // 75
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "nAVMon",     &user_data.nAVMon, 	 // 76
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "nAIMon",     &user_data.nAIMon, 	 // 77
/*
   1,UNIT_BYTE,             0, 0,           0, "BiasDa1",     &user_data.BiasDac[0],  // 6
   1,UNIT_BYTE,             0, 0,           0, "BiasDa2",     &user_data.BiasDac[1],  // 7
   1,UNIT_BYTE,             0, 0,           0, "BiasDa3",     &user_data.BiasDac[2],  // 8
   1,UNIT_BYTE,             0, 0,           0, "BiasDa4",     &user_data.BiasDac[3],  // 9
   1,UNIT_BYTE,             0, 0,           0, "BiasDa5",     &user_data.BiasDac[4],  // 10
   1,UNIT_BYTE,             0, 0,           0, "BiasDa6",     &user_data.BiasDac[5],  // 11
   1,UNIT_BYTE,             0, 0,           0, "BiasDa7",     &user_data.BiasDac[6],  // 12
   1,UNIT_BYTE,             0, 0,           0, "BiasDa8",     &user_data.BiasDac[7],  // 13
   1,UNIT_BYTE,             0, 0,           0, "BiasDa9",     &user_data.BiasDac[8],  // 14
   1,UNIT_BYTE,             0, 0,           0, "BiasDa10",    &user_data.BiasDac[9],  // 15
   1,UNIT_BYTE,             0, 0,           0, "BiasDa11",    &user_data.BiasDac[10], // 16
   1,UNIT_BYTE,             0, 0,           0, "BiasDa12",    &user_data.BiasDac[11], // 17
   1,UNIT_BYTE,             0, 0,           0, "BiasDa13",    &user_data.BiasDac[12], // 18
   1,UNIT_BYTE,             0, 0,           0, "BiasDa14",    &user_data.BiasDac[13], // 19
   1,UNIT_BYTE,             0, 0,           0, "BiasDa15",    &user_data.BiasDac[14], // 20
   1,UNIT_BYTE,             0, 0,           0, "BiasDa16",    &user_data.BiasDac[15], // 21
   1,UNIT_BYTE,             0, 0,           0, "BiasDa17",    &user_data.BiasDac[16], // 22
   1,UNIT_BYTE,             0, 0,           0, "BiasDa18",    &user_data.BiasDac[17], // 23
   1,UNIT_BYTE,             0, 0,           0, "BiasDa19",    &user_data.BiasDac[18], // 24
   1,UNIT_BYTE,             0, 0,           0, "BiasDa20",    &user_data.BiasDac[19], // 25
   1,UNIT_BYTE,             0, 0,           0, "BiasDa21",    &user_data.BiasDac[20], // 26
   1,UNIT_BYTE,             0, 0,           0, "BiasDa22",    &user_data.BiasDac[21], // 27 
   1,UNIT_BYTE,             0, 0,           0, "BiasDa23",    &user_data.BiasDac[22], // 28
   1,UNIT_BYTE,             0, 0,           0, "BiasDa24",    &user_data.BiasDac[23], // 29
   1,UNIT_BYTE,             0, 0,           0, "BiasDa25",    &user_data.BiasDac[24], // 30
   1,UNIT_BYTE,             0, 0,           0, "BiasDa26",    &user_data.BiasDac[25], // 31
   1,UNIT_BYTE,             0, 0,           0, "BiasDa27",    &user_data.BiasDac[26], // 32
   1,UNIT_BYTE,             0, 0,           0, "BiasDa28",    &user_data.BiasDac[27], // 33
   1,UNIT_BYTE,             0, 0,           0, "BiasDa29",    &user_data.BiasDac[28], // 34
   1,UNIT_BYTE,             0, 0,           0, "BiasDa30",    &user_data.BiasDac[29], // 35
   1,UNIT_BYTE,             0, 0,           0, "BiasDa31",    &user_data.BiasDac[30], // 36
   1,UNIT_BYTE,             0, 0,           0, "BiasDa32",    &user_data.BiasDac[31], // 37
   1,UNIT_BYTE,             0, 0,           0, "BiasDa33",    &user_data.BiasDac[32], // 38
   1,UNIT_BYTE,             0, 0,           0, "BiasDa34",    &user_data.BiasDac[33], // 39
   1,UNIT_BYTE,             0, 0,           0, "BiasDa35",    &user_data.BiasDac[34], // 40
   1,UNIT_BYTE,             0, 0,           0, "BiasDa36",    &user_data.BiasDac[35], // 41
   1,UNIT_BYTE,             0, 0,           0, "BiasDa37",    &user_data.BiasDac[36], // 42
   1,UNIT_BYTE,             0, 0,           0, "BiasDa38",    &user_data.BiasDac[37], // 43
   1,UNIT_BYTE,             0, 0,           0, "BiasDa39",    &user_data.BiasDac[38], // 44
   1,UNIT_BYTE,             0, 0,           0, "BiasDa40",    &user_data.BiasDac[39], // 45
   1,UNIT_BYTE,             0, 0,           0, "BiasDa41",    &user_data.BiasDac[40], // 46
   1,UNIT_BYTE,             0, 0,           0, "BiasDa42",    &user_data.BiasDac[41], // 47  
   1,UNIT_BYTE,             0, 0,           0, "BiasDa43",    &user_data.BiasDac[42], // 48
   1,UNIT_BYTE,             0, 0,           0, "BiasDa44",    &user_data.BiasDac[43], // 49
   1,UNIT_BYTE,             0, 0,           0, "BiasDa45",    &user_data.BiasDac[44], // 50
   1,UNIT_BYTE,             0, 0,           0, "BiasDa46",    &user_data.BiasDac[45], // 51
   1,UNIT_BYTE,             0, 0,           0, "BiasDa47",    &user_data.BiasDac[46], // 52
   1,UNIT_BYTE,             0, 0,           0, "BiasDa48",    &user_data.BiasDac[47], // 53
   1,UNIT_BYTE,             0, 0,           0, "BiasDa49",    &user_data.BiasDac[48], // 54
   1,UNIT_BYTE,             0, 0,           0, "BiasDa50",    &user_data.BiasDac[49], // 55
   1,UNIT_BYTE,             0, 0,           0, "BiasDa51",    &user_data.BiasDac[50], // 56
   1,UNIT_BYTE,             0, 0,           0, "BiasDa52",    &user_data.BiasDac[51], // 57
   1,UNIT_BYTE,             0, 0,           0, "BiasDa53",    &user_data.BiasDac[52], // 58
   1,UNIT_BYTE,             0, 0,           0, "BiasDa54",    &user_data.BiasDac[53], // 59
   1,UNIT_BYTE,             0, 0,           0, "BiasDa55",    &user_data.BiasDac[54], // 60
   1,UNIT_BYTE,             0, 0,           0, "BiasDa56",    &user_data.BiasDac[55], // 61
   1,UNIT_BYTE,             0, 0,           0, "BiasDa57",    &user_data.BiasDac[56], // 62
   1,UNIT_BYTE,             0, 0,           0, "BiasDa58",    &user_data.BiasDac[57], // 63
   1,UNIT_BYTE,             0, 0,           0, "BiasDa59",    &user_data.BiasDac[58], // 64
   1,UNIT_BYTE,             0, 0,           0, "BiasDa60",    &user_data.BiasDac[59], // 65
   1,UNIT_BYTE,             0, 0,           0, "BiasDa61",    &user_data.BiasDac[60], // 66
   1,UNIT_BYTE,             0, 0,           0, "BiasDa62",    &user_data.BiasDac[61], // 67
   1,UNIT_BYTE,             0, 0,           0, "BiasDa63",    &user_data.BiasDac[62], // 68
   1,UNIT_BYTE,             0, 0,           0, "BiasDa64",    &user_data.BiasDac[63], // 69
   
   
   
   4, UNIT_BYTE,			0, 0,			0,  "BiasVADC",  &user_data.BiasVADC, 	 // 78
   4, UNIT_BYTE,			0, 0,			0,  "BiasIADC",  &user_data.BiasIADC, 	 // 79 
   4, UNIT_BYTE,			0, 0,			0,  "pAVADC",    &user_data.pAVADC, 	 // 80
   4, UNIT_BYTE,			0, 0,			0,  "pAIADC",    &user_data.pAIADC, 	 // 81 
   4, UNIT_BYTE,			0, 0,			0,  "pDVADC",    &user_data.pDVADC, 	 // 82
   4, UNIT_BYTE,			0, 0,			0,  "pDIADC",    &user_data.pDIADC, 	 // 83
   4, UNIT_BYTE,			0, 0,			0,  "nAVADC",    &user_data.nAVADC, 	 // 84 
   4, UNIT_BYTE,	   		0, 0,			0,  "nAIADC",    &user_data.nAIADC, 	 // 85 
   
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp1",       &user_data.Temp[0],    // 86
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp2",       &user_data.Temp[1],    // 87  
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp3",       &user_data.Temp[2],    // 88  
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp4",       &user_data.Temp[3],    // 89  
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp5",       &user_data.Temp[4],    // 90  
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp6",       &user_data.Temp[5],    // 91  
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp7",       &user_data.Temp[6],    // 92  
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp8",       &user_data.Temp[7],    // 93  

   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVgr1",   &user_data.BiasVgrp[0], // 94 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVgr2",   &user_data.BiasVgrp[1], // 95
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVgr3",   &user_data.BiasVgrp[2], // 96
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVgr4",   &user_data.BiasVgrp[3], // 97
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVgr5",   &user_data.BiasVgrp[4], // 98
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVgr6",   &user_data.BiasVgrp[5], // 99
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVgr7",   &user_data.BiasVgrp[6], // 100
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "BiasVgr8",   &user_data.BiasVgrp[7], // 101
   

   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIgr1",   &user_data.BiasIgrp[0], // 102
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIgr2",   &user_data.BiasIgrp[1], // 103
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIgr3",   &user_data.BiasIgrp[2], // 104
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIgr4",   &user_data.BiasIgrp[3], // 105
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIgr5",   &user_data.BiasIgrp[4], // 106
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIgr6",   &user_data.BiasIgrp[5], // 107
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIgr7",   &user_data.BiasIgrp[6], // 108
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "BiasIgr8",   &user_data.BiasIgrp[7], // 109
      
   4, UNIT_BYTE,			0, 0,			0,  "BiasVad1",  &user_data.BiasVadc[0], // 110
   4, UNIT_BYTE,			0, 0,			0,  "BiasVad2",  &user_data.BiasVadc[1], // 111
   4, UNIT_BYTE,			0, 0,			0,  "BiasVad3",  &user_data.BiasVadc[2], // 112
   4, UNIT_BYTE,			0, 0,			0,  "BiasVad4",  &user_data.BiasVadc[3], // 113
   4, UNIT_BYTE,			0, 0,			0,  "BiasVad5",  &user_data.BiasVadc[4], // 114
   4, UNIT_BYTE,			0, 0,			0,  "BiasVad6",  &user_data.BiasVadc[5], // 115
   4, UNIT_BYTE,			0, 0,			0,  "BiasVad7",  &user_data.BiasVadc[6], // 116
   4, UNIT_BYTE,			0, 0,			0,  "BiasVad8",  &user_data.BiasVadc[7], // 117

   4, UNIT_BYTE,			0, 0,			0,  "BiasIad1",  &user_data.BiasIadc[0], // 118
   4, UNIT_BYTE,			0, 0,			0,  "BiasIad2",  &user_data.BiasIadc[1], // 119
   4, UNIT_BYTE,			0, 0,			0,  "BiasIad3",  &user_data.BiasIadc[2], // 120
   4, UNIT_BYTE,			0, 0,			0,  "BiasIad4",  &user_data.BiasIadc[3], // 121
   4, UNIT_BYTE,			0, 0,			0,  "BiasIad5",  &user_data.BiasIadc[4], // 122
   4, UNIT_BYTE,			0, 0,			0,  "BiasIad6",  &user_data.BiasIadc[5], // 123
   4, UNIT_BYTE,			0, 0,			0,  "BiasIad7",  &user_data.BiasIadc[6], // 124
   4, UNIT_BYTE,			0, 0,			0,  "BiasIad8",  &user_data.BiasIadc[7], // 125
*/

   0
};


MSCB_INFO_VAR *variables = vars;
extern SYS_INFO sys_info;

//unsigned char ADT7486A_addrArray[] = {ADT7486A_ADDR_ARRAY};


/********************************************************************\
  Application specific init and in/output routines
\********************************************************************/

/*---- User init function ------------------------------------------*/
void user_init(unsigned char init)
{
   short int add;

  // PAA- to be removed when if (init) is valid
  add = init;
	add = cur_sub_addr();

//
// Initial setting for communitation and overall ports (if needed).
//-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
                      // P0MDOUT contains Tx in push-pull
   P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull

//   P1MDOUT = 0x00;
//   P2MDOUT = 0x00;
   
//	
// default settings, set only when EEPROM is being erased and written
//-----------------------------------------------------------------------------
//	if(init)
//	{
//		user_data.control = 0x7D; //Turn on the charge pump 
//		user_data.status   = 0x00;
//		user_data.biasEn   = 0xFF;
//		user_data.dac_asumThreshold   = 0x80;
//		user_data.dac_chPump   = 0x00; //set to lowest scale, just to be safe
//		user_data.biasDac1   = 0xFF;
//		user_data.biasDac2   = 0xFF;
//		user_data.biasDac3   = 0xFF;
//		user_data.biasDac4   = 0xFF;
//		user_data.biasDac5   = 0xFF;
//		user_data.biasDac6   = 0xFF;
//		user_data.biasDac7   = 0xFF;
//		user_data.biasDac8   = 0xFF;		
//		sys_info.group_addr = 400;
//		sys_info.node_addr = 0xFF00;
//	}   

/*-----------------------------------------------------------------------------------*/
/* set-up / initialize circuit components (order is important)                       */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
//
// Charge Pump frequency setup
//-----------------------------------------------------------------------------
#ifdef _PCA_INTERNAL_
  pca_operation(Q_PUMP_INIT); //Charge Pump initialization (crossbar settings)
  pca_operation(Q_PUMP_ON);   //Initially turn it on	
#endif

//
// Miscellaneous ADCs (V/I Monitoring)
//-----------------------------------------------------------------------------
#ifdef _ADC_INTERNAL_
  adc_internal_init();
#endif

//
// SST Temperatures
//-----------------------------------------------------------------------------
#ifdef _ADT7486_
  ADT7486A_Init(); //Temperature measurements related initialization
#endif

//
// Charge Pump DAC
//-----------------------------------------------------------------------------
#ifdef _LTC1669_	
  LTC1669_Init(); //I2C DAC initialization (D_CH_PUMP)
  user_data.control1 = 0x41; //Set the control bit for Qpump and the Qpump threashold enable bit
#endif

//
// Bias Voltage switches
//-----------------------------------------------------------------------------
#ifdef _PCA9539_
  PCA9539_Init(); //PCA General I/O (Bais Enables and Backplane Addr) initialization	
#endif

//#ifdef _AD5301_
//	AD5301_Init(); 	
//	user_data.control = 0x81;
//#endif
//	AD7718_Init(); //ADC initialization 		 


//
// Physical address retrieval 
//-----------------------------------------------------------------------------

//
// FLASH memory Initialization
//-----------------------------------------------------------------------------

}
/*---- User write function -----------------------------------------*/
#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
   unsigned char command, mask;

   // In case of you changing common control bit (ex. EXT/IN switch bit)
   // the change should be distributed to other channels.
   if (index == 0) {
       // preserve common command bits for all channels
       command = user_data.control1;
       mask    = CONTROL_QPUMP; // common bits

       user_data.control1 &= ~mask;
       user_data.control1 |= (command & mask);
   }
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

/*---- User loop function ------------------------------------------*/
void user_loop(void)
{
  unsigned char channel;
  float volt, *p;


#ifdef _ADC_INTERNAL_
  // If order of ADC needs to be as in the structure, lookup table 
  // has to be implemented (current readout order is hardware driven).
  p = &(user_data.BiasVGbl);
  for (channel=0 ; channel<N_CHN ; channel++) {
    volt = read_voltage(channel);
	DISABLE_INTERRUPTS;
      p[channel] = volt;
    ENABLE_INTERRUPTS;
  }
#endif

	
/*
	if(user_data.control1 & CONTROL_QPUMP)
	{
		pca_operation(Q_PUMP_ON);
	}
	else
	{
		pca_operation(Q_PUMP_OFF);
	}
    
	//Update the Charge Pump Threshold voltage
	if(user_data.control1 & CONTROL_D_CHPUMP)
	{
		#ifdef _LTC1669_	
		LTC1669_Cmd(ADDR_LTC1669, (user_data.QpumpDac >> 4), (user_data.QpumpDac << 4), LTC1669_WRITE);
		#endif 
	}
	

	//Added to turn on and off all the channels	
	if( user_data.control1 & CONTROL_CHANNEL )
	{
    	chNum = 0; 

		#ifdef _PCA9539_
   		PCA9539_Cmd(Bias_ALL_LOW);	
		#endif
	}
*/

delay_ms(10);

}


