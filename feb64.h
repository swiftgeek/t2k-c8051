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
#define ADDR_PCA9539   0x74
#define ADDR_LTC2497	  0x45	// may be 0010100 (0x14)

//ADT7486A temperature addresses. 2 sensors per chip, so each address is repeated twice
#define ADT7486A_ADDR0 0x49   
#define ADT7486A_ADDR1 0x48	
#define ADT7486A_ADDR2 0x4B
#define ADT7486A_ADDR3 0x4A

#define NCHANNEL_BIAS    64
#define NCHANNEL_ASUM     8
#define NCHANNEL_SST      8
#define NCHANNEL_ADT7486A 4
#define IDXCTL       1
#define IDXEER       0
#define IDXQVOLT     5
#define IDXBSWITCH   4
#define IDXASUM      6
#define IDXSST      23 
#define IDXBIAS     31

#define FIRST_BIAS   IDXBIAS
#define LAST_BIAS    FIRST_BIAS + NCHANNEL_BIAS

#define FIRST_ASUM   IDXASUM
#define LAST_ASUM    FIRST_ASUM + NCHANNEL_ASUM

// PCA9539 Macro Definitions
#define BIAS_OUTPUT_ENABLE      ADDR_PCA9539, PCA9539_CONFIG0, PCA9539_ALL_OUTPUT
#define BIAS_ALL_HIGH           ADDR_PCA9539, PCA9539_OUTPUT0, 0xFF
#define BIAS_ALL_LOW            ADDR_PCA9539, PCA9539_OUTPUT0, 0x00
#define BIAS_READ			        ADDR_PCA9539, PCA9539_INPUT0
#define BIAS_WRITE				  ADDR_PCA9539, PCA9539_OUTPUT0

#define BACKPLANE_INPUT_ENABLE  ADDR_PCA9539, PCA9539_CONFIG1, PCA9539_ALL_INPUT
#define BACKPLANE_READ			  ADDR_PCA9539, PCA9539_INPUT1	 

// Global ON / OFF definition
#define ON     1
#define OFF    0
#define DONE   1
#define FAILED 0

//BS Temperature testing
#define TEMP_ThRESHOLD  27

// charge pump state for PCA control
#define Q_PUMP_INIT          1           
#define Q_PUMP_OFF           2           
#define Q_PUMP_ON            3           

// Fix conversion coeef for V/I internal ADC
//                      Vb      Vbi   +6Vd      +6Va     -6Va  -6Ia   +6Ia  +6Id 
//float code   coeff[8] = {1.  ,1.   ,1.   ,1.   ,1.   ,1.  ,1.  ,1.};
float code   coeff[8] = {40.   ,50.   ,4.025   ,4.025   ,-4.025   ,100.  ,50.  ,100.};
float code  offset[8] = {0.     ,0.   ,0.   ,0.   ,0.    ,-4.5    ,0.   ,0.  };

// Shutdown mask
#define SHUTDOWN_MASK   0xFC

// Vreg Enable port assignment
sbit EN_pD5V = P3 ^ 4;
sbit EN_pA5V = P3 ^ 3;
sbit EN_nA5V = P3 ^ 2;

// CTL register
unsigned char bdata rCTL;
sbit CPup     = rCTL ^ 0;
sbit CqPump   = rCTL ^ 1;
sbit CeeS     = rCTL ^ 4;
sbit CeeR     = rCTL ^ 5;
sbit CmSd     = rCTL ^ 7;

// CSR Register
unsigned char bdata rCSR;
sbit SPup     = rCSR ^ 0;
sbit SqPump   = rCSR ^ 1;
sbit SeeS     = rCSR ^ 4;
sbit SeeR     = rCSR ^ 5;
sbit SsS      = rCSR ^ 6;
sbit SmSd     = rCSR ^ 7;

// ESR Error Register
unsigned long bdata rESR;
sbit vQpump   = rESR ^ 0;
sbit iQpump   = rESR ^ 1;
sbit vReg1    = rESR ^ 2;
sbit vReg2    = rESR ^ 3;
sbit vReg3    = rESR ^ 4;
sbit iReg1    = rESR ^ 5;
sbit iReg2    = rESR ^ 6;
sbit iReg3    = rESR ^ 7;
sbit uCT      = rESR ^ 8;
sbit ssTT     = rESR ^ 9; 

// SMBus Port Aliases
sbit SDA		  = MSCB_I2C_SDA;
sbit SCL		  = MSCB_I2C_SCL;

// EER EEPROM register
unsigned char rEER;

struct EEPAGE {
float lVIlimit[8]; // vQ iQ +6Vd +6Va -6Va -6Ia +6Ia +6Id 
float uVIlimit[8];
float luCTlimit;
float uuCTlimit;
float lSSTlimit;
float uSSTlimit;
float lVQlimit;
float uVQlimit;
float lIQlimit;
float uIQlimit;
float lVBiaslimit;
float uVBiaslimit;
float lIBiaslimit;
float uIBiaslimit;
};

//   vQ   iQ  +6Vd  +6Va  -6Va  -6Ia  +6Ia  +6Id 
// uC Temperature
// SST Temperature
//   vQ    iQ  +6Vd  +6Va -6Va  -6Ia   +6Ia  +6Id 
struct EEPAGE xdata eepage = {
     30.0, 0.0, 5.5, 5.5, -6.5, -20.0, 5.0,   5.0
    ,73.0, 0.1, 6.5, 6.5, -5.5, -1.0, 100.0, 200.0
	 ,23., 45.
    ,20., 30.
};

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
	unsigned long error;         
	unsigned char control;       
	unsigned char status;		 
	unsigned char eepage;	     
	unsigned char swBias;	     
	unsigned int  rQpump;
	unsigned int  rAsum[8];

	float VBias;
	float IBias;
	float pDVMon;
	float pAVMon;
	float nAVMon;
	float nAIMon;
	float pAIMon;
	float pDIMon;
	float uCTemp;
	float Temp[8];               
	unsigned char rBias [64];
	unsigned char NTemFail;
	float FailTemp;
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
