/********************************************************************\

  Name:         feb64.h
  Created by:   Bahman Sotoodian				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for T2K-FEB64 test board

  $Id$

\********************************************************************/
//  need to have FEB64 defined

#ifndef  _FEB64_H
#define  _FEB64_H

// charge pump state for PCA control
#define Q_PUMP_INIT          1           
#define Q_PUMP_OFF           2           
#define Q_PUMP_ON            3           

// =======================================================================
//      [State machine transition]
//  
// -------------------- Control Register Map -----------------------------
// Bit: Name        Default Description     
// [MSB]
//  7 : CONTROL_CHANNEL (0) (force CPU idle)    			 1 : enable
//  6 : (not assigned)  X
//  5 : CONTROL_ISENSOR 1   I measurement       0 : disable, 1 : enable
//  4 : CONTROL_TSENSOR 1   T measurement       0 : disable, 1 : enable
//  3 : (not assigned)  X
//  2 : CONTROL_IB_CTL  0   Int-bias cntrl mode 0 : disable, 1 : enable
//  1 : CONTROL_EXT_IN  0   Ext/In bias switch  0 : INTRNL,  1 : EXTRNL   
//  0 : CONTROL_QPUMP   0   Internal Q-Pump     0 : disable, 1 : enable
// [LSB]
// -----------------------------------------------------------------------
//
// -------------------- Status Register Map ------------------------------
// Bit: Name                Description     
// [MSB]
//  7 : STATUS_RAMP_DOWN    HV Rumping down     0 : idle,    1 : processing
//  6 : STATUS_RAMP_UP      HV Rumping up       0 : idle,    1 : processing
//  5 : STATUS_ILIMIT       Current Limit       0 :          1 : observed
//  4 : (not assigned))
//  3 : STATUS_IB_CTL       INT bias control    0 : disable, 1 : enable
//  2 : STATUS_EXT_BIAS     EXT bias            0 : disable, 1 : enable
//  1 : STATUS_INT_BIAS     INT bias            0 : disable, 1 : enable
//  0 : STATUS_QPUMP        Q-Pump status       0 : stopped, 1 : running
// [LSB]
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
#define CONTROL_ADC_MEAS  (1<<2) 
#define CONTROL_BIAS_EN   (1<<3) //The switch enable bit, when it is set, user can control eash switch individually
								 //through "D_BiasEn" 
#define CONTROL_BIAS_DAC  (1<<4) //The DAC enable bit
#define CONTROL_ASUM_TH   (1<<5)
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
	unsigned char control2;		  // CSR control2 bits
	unsigned char status;         // CSR status bits
	unsigned char BiasEN;	      // Bias Enable Switch control bits	
	unsigned int AsumDac;
	unsigned char QpumpDac;

	float BiasVGbl;
	float BiasIGbl;
	float pAVMon;
	float pAIMon;
	float pDVMon;
	float pDIMon;
	float nAVMon;
	float nAIMon;

/*
	unsigned char BiasDac [64];
	
   	
	unsigned long BiasVADC;
	unsigned long BiasIADC;
	unsigned long pAVADC;
	unsigned long pAIADC;
	unsigned long pDVADC;
	unsigned long pDIADC;
	unsigned long nAVADC;
	unsigned long nAIADC;	
	
	float Temp[8]; // ADT7486A external temperature [degree celsius]

	float BiasVgrp[8];
	float BiasIgrp[8];

	unsigned long BiasVadc[8];
	unsigned long BiasIadc[8];
*/
	
}; 

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_init(unsigned char init);
void user_loop(void);
void user_write(unsigned char index) reentrant;
unsigned char user_read(unsigned char index);
unsigned char user_func(unsigned char *data_in, unsigned char *data_out);
void pca_operation(unsigned char mode);
void set_current_limit(float value);
void Hardware_Update(void);

#endif
