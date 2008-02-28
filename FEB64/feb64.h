/********************************************************************\

  Name:         feb64.h
  Created by:   Bahman Sotoodian				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for T2K-FEB64 test board

  $Id$

\********************************************************************/
//  need to have FEB64 defined

#ifndef _FEB64_H
#define  _FEB64_H

// Device Address mapping
#define ADDR_LTC1669   0x20

// Global ON / OFF definition
#define ON  1
#define OFF 0

// charge pump state for PCA control
#define Q_PUMP_INIT          1           
#define Q_PUMP_OFF           2           
#define Q_PUMP_ON            3           


// Mapping of the channels for the FEB64 card
unsigned char code internal_adc_map[8] = {0,1,4,2,6,7,3,5};

// Vreg Enable port assignment
sbit EN_pD5V = P3 ^ 4;
sbit EN_pA5V = P3 ^ 3;
sbit EN_nA5V = P3 ^ 2;

// =======================================================================
//      [State machine transition]
//  
// -------------------- Control Register Map -----------------------------
// Bit: Name        Default Description     
// [LSB]
//  0 : CONTROL_QPUMP      0   Internal Q-Pump    		 0 : disable, 1 : enable
//  1 : CONTROL_TEMP_MEAS  1   Ext/In temp measurment  0 : disable, 1 : enable   
//  2 : CONTROL_ADC_MEAS   1   Int-bias control bit 	 0 : disable, 1 : enable
//  3 : CONTROL_BIAS_EN    1	 Switch Enable bit		 0 : disable, 1 : enable
//  4 : CONTROL_BIAS_DAC   1   APD_DAQ Enable bit      0 : disable, 1 : enable
//  5 : CONTROL_ASUM_DAC    1   ASUM DAQ Enable bit     0 : disable, 1 : enable
//  6 : CONTROL_D_CHPUMP   1   Qpump DAQ 					 0 : disable, 1 : enable 
//  7 : CONTROL_CHANNEL    0   Master Switch Control	 0	: disable, 1 : enable
// [MSB]
// -----------------------------------------------------------------------
//
// -------------------- Status Register Map ------------------------------
// Bit: Name                Description     
// [LSB]
//  0 : STATUS_QPUMP        Q-Pump status       0 : stopped, 1 : running
//  1 : STATUS_INT_BIAS     INT bias            0 : disable, 1 : enable
//  2 : STATUS_EXT_BIAS     EXT bias            0 : disable, 1 : enable
//  3 : STATUS_IB_CTL       INT bias control    0 : disable, 1 : enable
//  4 : (not assigned))
//  5 : STATUS_ILIMIT       Current Limit       0 :          1 : observed
//  6 : STATUS_RAMP_UP      HV Rumping up       0 : idle,    1 : processing
//  7 : STATUS_RAMP_DOWN    HV Rumping down     0 : idle,    1 : processing
// [MSB]
// -----------------------------------------------------------------------
//
// [Abbreviations]
//  QP  := CONTROL_QPUMP or STATUS_QPUMP (depending on the contents)
//  E/I := CONTROL_EXT_IN
//  ICN := CONTROL_IB_CTL or STATUS_IB_CTL 
//  INT := STATUS_INT_BIAS
//  EXT := STATUS_EXT_BIAS
// -----------------------------------------------------------------------
//
//           STATE TABLE
//
//           |----------------------------------------------------|
//           |      CONTROL        |       STATUS                 |
//           |---------------------|------------------------------|
//           |   ICN    E/I    QP  |    ICN    EXT    INT    QP   |
//           |---------------------|------------------------------|
//  STATE-1  |   X      0      0   |    0      0      0      0    |
//  STATE-2  |   0      0      1   |    0      0      1      1    |
//  STATE-3  |   X      1      X   |    0      1      0      0    |     
//  STATE-4  |   1      0      1   |    1      1      1      1    |     
//           |----------------------------------------------------|
//   
// =======================================================================

/* CSR control bits */
#define CONTROL_QPUMP     (1<<0) //The control bit for charge pump
#define CONTROL_TEMP_MEAS (1<<1) //If the bit is set, the temp measurements is enabled
#define CONTROL_ADC_MEAS  (1<<2) //Setting the bit, enable the internal readbacks.
#define CONTROL_BIAS_EN   (1<<3) //The switch enable bit, when it is set, user can control eash switch individually
								 			//through "D_BiasEn" 
#define CONTROL_BIAS_DAC  (1<<4) //The DAC enable bit
#define CONTROL_ASUM_DAC  (1<<5) //The ASUM DAQ enable bit
#define CONTROL_D_CHPUMP  (1<<6) //The charge pump threashold enable bit
#define CONTROL_CHANNEL   (1<<7) //Added by Bahman Sotoodian to turn on all the channels
								 			//When charge pump is on and this bit is set, all the switches would be on
								 			//If the bit is not set, the switches functionality would be determined based on 
								 			//"CONTROL_BIAS_EN"	

/* CSR status bits */
#define STATUS_QPUMP      (1<<0)
#define STATUS_TEMP_MEAS  (1<<1)
#define STATUS_ADC_MEAS   (1<<2)
#define STATUS_BIAS_EN    (1<<3)
#define STATUS_BIAS_DAC   (1<<4)
#define STATUS_ASUM_TH    (1<<5)
#define STATUS_D_CHPUMP   (1<<6)
#define STATUS_IDLE       (1<<7)

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

struct user_data_type {
	unsigned char control1;       // CSR control1 bits
	unsigned char control2;		   // CSR control2 bits
	unsigned char status;         // CSR status bits
	unsigned char swBiasEN;	      // Bias Enable Switch control bits	

	unsigned int  rBPlane;
	unsigned int  rAsum;
	unsigned int  rQpump;

	float VBias;
	float IBias;
	float pAVMon;
	float pAIMon;
	float pDVMon;
	float pDIMon;
	float nAVMon;
	float nAIMon;
	float uCTemp;
	unsigned char rBias [64];
	float Temp[8]; // ADT7486A external temperature [degree celsius]
};
 
struct user_data_type xdata user_data;
   	
/*	unsigned long rVBias;
	unsigned long rIBias;
	unsigned long rpAV;
	unsigned long rpAI;
	unsigned long rpDV;
	unsigned long rpDI;
	unsigned long rnAV;
	unsigned long rnAI;	
	
	float Temp[8]; // ADT7486A external temperature [degree celsius]

	float VBMon[8];
	float IBMon[8];

	unsigned long rVBMon[8];
	unsigned long rIBMon[8];

*/	

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_init(unsigned char init);
void user_loop(void);
void user_write(unsigned char index) reentrant;
unsigned char user_read(unsigned char index);
unsigned char user_func(unsigned char *data_in, unsigned char *data_out);

#endif
