/********************************************************************\

  Name:         feb64.h
  Created by:   Bahman Sotoodian				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for T2K-FEB64 test board

  $Id$

\********************************************************************/
#ifndef _FEB64_H
#define  _FEB64_H

// Device Address mapping
#define ADDR_LTC1669   0x20
#define NCHANNEL_BIAS    64
#define NCHANNEL_ASUM     8
#define NCHANNEL_SST      8

#define IDXCTL       0
#define IDXEER       1
#define IDXQVOLT     4
#define IDXBSWITCH   5
#define IDXASUM      6
#define IDXBIAS     31
#define IDXSST      23 

#define FIRST_BIAS   IDXBIAS
#define LAST_BIAS    FIRST_BIAS + NCHANNEL_BIAS

#define FIRST_ASUM   IDXASUM
#define LAST_ASUM    FIRST_ASUM + NCHANNEL_ASUM

// Global ON / OFF definition
#define ON     1
#define OFF    0
#define DONE   1
#define FAILED 0

// charge pump state for PCA control
#define Q_PUMP_INIT          1           
#define Q_PUMP_OFF           2           
#define Q_PUMP_ON            3           

// Mapping of the channels for the FEB64 card
unsigned char code internal_adc_map[8] = {0,1,4,2,6,7,3,5};

// Shutdwon mask
#define SHUTDOWN_MASK   0x3F

// Vreg Enable port assignment
sbit EN_pD5V = P3 ^ 4;
sbit EN_pA5V = P3 ^ 3;
sbit EN_nA5V = P3 ^ 2;

// CTL register
char bdata rCTL;
sbit CPup     = rCTL ^ 1;
sbit CqPump   = rCTL ^ 2;
sbit CeeS     = rCTL ^ 4;
sbit CeeR     = rCTL ^ 5;
sbit CmSd     = rCTL ^ 7;

// CSR Register
char bdata rCSR;
sbit SPup     = rCSR ^ 1;
sbit SqPump   = rCSR ^ 1;
sbit SeeS     = rCSR ^ 4;
sbit SeeR     = rCSR ^ 5;
sbit SsS      = rCSR ^ 6;
sbit SmSd     = rCSR ^ 7;

// EER Error Register
unsigned long bdata ESR;
sbit vReg1   = ESR ^ 1;
sbit vReg2   = ESR ^ 2;
sbit vReg3   = ESR ^ 3;
sbit iReg1   = ESR ^ 4;
sbit iReg2   = ESR ^ 5;
sbit iReg3   = ESR ^ 6;
sbit vQpump  = ESR ^ 7;
sbit iQpump  = ESR ^ 8;
sbit uCT     = ESR ^ 9;
sbit ssTT    = ESR ^ 10; 

struct EEPAGE {
float lVIlimit[8];
float uVIlimit[8];
float lSSTlimit[8];
float uSSTlimit[8];
float luCTlimit;
float uuCTlimit;
float lVQlimit;
float uVQlimit;
float lIQlimit;
float uIQlimit;
float lVBiaslimit;
float uVBiaslimit;
float lIBiaslimit;
float uIBiaslimit;
};
struct EEPAGE xdata eepage;

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
	unsigned char control;       
	unsigned char status;		 
	unsigned long error;         
	unsigned char eepage;	     
	unsigned int  rQpump;
	unsigned char swBiasEN;	     
	unsigned int  rAsum[8];

	unsigned int  rBPlane;

	float VBias;
	float IBias;
	float pAVMon;
	float pAIMon;
	float pDVMon;
	float pDIMon;
	float nAVMon;
	float nAIMon;
	float uCTemp;
	float Temp[8];               
	unsigned char rBias [64];
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
