/********************************************************************\

  Name:         feb64.h
  Created by:   Bahman Sotoodian				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for T2K-FEB64 test board

  $Id$

\********************************************************************/
#ifndef  _FEB64_H
#define  _FEB64_H


/*************ADT7486A***************/
/************************************/
#define ADT7486A_ADDR0 0x49   
#define ADT7486A_ADDR1 0x48	
#define ADT7486A_ADDR2 0x4B
#define ADT7486A_ADDR3 0x4A
//BS Temperature testing
#define TEMP_ThRESHOLD  27
#define SST_TIME			5 //In us
/*************************************/
/*************************************/

/*************PCA9539****************/
/************************************/
#define BIAS_OUTPUT_ENABLE      ADDR_PCA9539, PCA9539_CONFIG0, PCA9539_ALL_OUTPUT
#define BIAS_DISABLE            ADDR_PCA9539, PCA9539_OUTPUT0, 0xFF
#define BIAS_ENABLE             ADDR_PCA9539, PCA9539_OUTPUT0, 0x00
#define BIAS_READ			        ADDR_PCA9539, PCA9539_INPUT0
#define BIAS_WRITE				  ADDR_PCA9539, PCA9539_OUTPUT0

#define BACKPLANE_INPUT_ENABLE  ADDR_PCA9539, PCA9539_CONFIG1, PCA9539_ALL_INPUT
#define BACKPLANE_READ			  ADDR_PCA9539, PCA9539_INPUT1	 
/*************************************/
/*************************************/

/**********External ADC**************/
/************************************/
#define N_RB_CHANNEL	 16
#define EXT_VREF      1.25f //2.4989f
#define CURR_MEASURE  0x04
#define VOLT_MEASURE  0x04
#define GAIN1			 0
// External ADC channel conversion table
char code adc_convert[] = { 1, 3, 5, 7, 1, 3, 5, 7, 0, 2, 4, 6, 0, 2, 4, 6 };
#define CONVER_FAC2 65536.0f
// #define CONVER_FAC2	131072.0f	// 65536 x 2
//Fix  conversion coeff for V/I external ADC
						 // VBMon1,VBMon3,VBMon5,VBMon7,
						 // IBMon1,IBMon3,IBMon5,IBMon7,
						 // VBMon0,VBMon2,VBMon4,VBMon6,
						 // IBMon0,VBMon2,IBMon4,IBMon6,

#ifdef FEB64REV0
#define CONVER_FAC1  65536.0f
float code Mon_Coef[]={  100, 100, 100, 100
                       , 1000, 1000, 1000, 1000
							  , 100, 100, 100, 100
							  , 1000, 1000, 1000, 1000};


float code Mon_Offst[]={  0.2278, 0.2278, 0.2278, 0.2278
								, 0, 0, 0, 0
								, 0.2278, 0.2278, 0.2278, 0.2278
								, 0, 0, 0, 0};
#else
#define CONVER_FAC1 0.0f
float code Mon_Coef[]={  101, 101, 101, 101
                       , 0.01, 0.01, 0.01, 0.01
							  , 101, 101, 101, 101
							  , 0.01, 0.01, 0.01, 0.01};


float code Mon_Offst[]={  0.2278, 0.2278, 0.2278, 0.2278
								, 0, 0, 0, 0
								, 0.2278, 0.2278, 0.2278, 0.2278
								, 0, 0, 0, 0};
#endif
/*
float code Mon_Coef[]={41.249, 41.286, 41.231, 40.263,
                       62.5   ,62.5,	62.5,	  62.5,
                       41.27 , 41.269, 41.263, 40.278,
							  62.5,	 62.5,	62.5,	  62.5};
float code Mon_Offst[]={0.0107, 0.0019, 0.0147,-0.0009,
								0,		  0,	    0,		0,
							  -0.0086,-0.0082,-0.0125, 0.0003,
							   0,		  0,		 0,		0};
*/
/*************************************/
/*************************************/


// charge pump state for PCA control
#define Q_PUMP_INIT          1           
#define Q_PUMP_OFF           2           
#define Q_PUMP_ON            3           


/**********Internal ADC**************/
/************************************/
//+6Va,+6Vd are off roughly by 5mV, -6Va is off roughly by -30mV
// Fix conversion coeef for V/I internal ADC
							 // Vb        Vbi   		+6Vd     +6Va     -6Va  	 -6Ia   +6Ia  	  +6Id 
//float code  coeff[8]  = {41.448   ,2.496587   ,4.025   ,4.025   ,8.4534   ,0.237  ,0.475  ,0.237};
//float code  offset[8] = {-0.2813  ,0.         ,-0.06   ,-0.054  ,-18.622  ,0     ,0.     ,0.  };
#ifdef FEB64REV0
float code  coeff[8]  = {41.448   ,2.496587   ,4.025   ,4.025   ,8.4534   ,0.237  ,0.475  ,0.237};
float code  offset[8] = {-0.3464 ,0.         ,-0.06   ,-0.054  ,-18.622  ,0     ,0.     ,0.  };
#else
//offset still needs to be calibrated
float code  coeff[8]  = {1000   ,10   ,3.980132, 3.980132   ,8.5   ,0.1  ,0.4  ,0.1};
float code  offset[8] = {0 ,0.         , 0   , 0  ,-18.75  ,0     ,0.     ,0.  };
#endif
// External Vref
#define VREF       2.432f
/*************************************/
/*************************************/

/**********EXTERNAL EEPROM***********/
/************************************/
unsigned char rEER;
#define EEP_CTRL_KEY	    0x3C000000
#define EEP_CTRL_READ    0x00110000   	
#define EEP_CTRL_WRITE 	 0x00220000
#define EEP_CTRL_INVAL_REQ -100	
#define EEP_CTRL_INVAL_KEY -10	

struct EEPAGE {

float lVIlimit[8]; // vQ iQ +6Vd +6Va -6Va -6Ia +6Ia +6Id 
float uVIlimit[8];

float luCTlimit, uuCTlimit;
float lSSTlimit, uSSTlimit;

float lVQlimit,  uVQlimit;
float lIQlimit,  uIQlimit;

float lVBiaslimit, uVBiaslimit;
float lIBiaslimit, uIBiaslimit;

float ext1offset[4];
float ext2offset[4];

unsigned long SerialN;
unsigned int rasum[8];
unsigned int rqpump;
unsigned char SWbias;
unsigned char rbias [64];
};

//   LvQ   LiQ   Lp6Vd  Lp6Va Ln6Va  Lp6Ia Lp6Ia  Lp6Id 
//   HvQ   HiQ   Hp6Vd  Hp6Va Hp6Va  Hp6Ia Hp6Ia  Hp6Id 
//   LuC Temperature,  HuC Temperature
//   LSST Temperature, LSST Temperature
//   LVQ, HVQ (V)
//   LIQ, HIQ (uA)
//	  LVBias, HVBias (V)
//	  LIBias, LVBias (uA)
//	  SST channel 1 offset
//   SST channel 2 offset
//   SerialN
//	  rasum
//	  rpump
//	  SW
//	  DAQ
struct EEPAGE xdata eepage = {
     30.0, 0.0,  5.5,   5.5, -6.5,  20.0,  5.0,   5.0
    ,73.0, 0.1,  6.5,   6.5, -5.5,  1.0,   100.0, 200.0
	 ,23., 45.
    ,20., 30.
	 ,-1.0,1.0
	 ,-0.1,1.0
	 ,0.0,73.0
	 ,0.0,10.0
	 ,0,0,0,0
	 ,0,0,0,0
	 ,0x00000000
	 ,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff
	 ,0x0000
	 ,0x00
	 ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	 ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	 ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	 ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	 ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	 ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	 ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	 ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

//BS	For testing the EXTEEPROM
struct EEPAGE xdata eepage2;
/*************************************/
/*************************************/

#define NCHANNEL_BIAS    64
#define NCHANNEL_ASUM     8
#define NCHANNEL_SST      8
#define NCHANNEL_ADT7486A 4


#define IDXCTL         2
#define IDXEER         1
#define IDXQVOLT       6
#define IDXBSWITCH     5
#define IDXASUM       44
#define IDXSST        40
#define IDXBIAS       52
#define IDXEEP_CTL	 141
#define IDXASUM_CTL  142

#define SERIALN_LENGTH 4
#define SERIALN_ADD  0x690    

unsigned int xdata PageAddr[]={0x0,0x100,0x200,0x300,0x400,0x500};

#define FIRST_BIAS   IDXBIAS
#define LAST_BIAS    FIRST_BIAS + NCHANNEL_BIAS

#define FIRST_ASUM   IDXASUM
#define LAST_ASUM    FIRST_ASUM + NCHANNEL_ASUM

// Device Address mapping
#define ADDR_LTC1669   0x20
#define ADDR_PCA9539   0x74
#define ADDR_LTC2497	  0x14	
#define ADDR_LTC2495   0x14
//NW mirror of the DAC values 
unsigned char xdata ltc1665mirror [64];

// Global ON / OFF definition
#define ON     1
#define OFF    0
#define DONE   1
#define FAILED 0
#define CLEAR  0
#define SET		1


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
sbit CeeClr   = rCTL ^ 6;
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
//The low and high bytes are switched in the bdata section of the memory
//This is the reason that the sbit declarations do not appear to match
//the documentation but they actually do.
unsigned int bdata rESR;
sbit vQpump   = rESR ^ 8; 
sbit iQpump   = rESR ^ 9;
sbit vReg1    = rESR ^ 10;
sbit vReg2    = rESR ^ 11;
sbit vReg3    = rESR ^ 12;
sbit iReg1    = rESR ^ 13;
sbit iReg2    = rESR ^ 14;
sbit iReg3    = rESR ^ 15;
sbit uCT      = rESR ^ 0;
sbit IntssTT  = rESR ^ 1; 
sbit ExtssTT  = rESR ^ 2;	
sbit EEPROM   = rESR ^ 3;

// SMBus Port Aliases
sbit SDA		  = MSCB_I2C_SDA;
sbit SCL		  = MSCB_I2C_SCL;

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
	unsigned long SerialN;
	unsigned int  error;         
	unsigned char control;       
	unsigned char status;		 
	unsigned char eepage;	     
	unsigned char swBias;	     
	unsigned int  rQpump;
	unsigned int  spare;
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
	float VBMon[8];
	float IBMon[8];
	float ssTemp[4];
	unsigned int  rAsum[8];
	unsigned char rBias [64];

	unsigned int rVBias;
	unsigned int rIBias;
	unsigned int rpDV;
	unsigned int rpAV;
	unsigned int rnAV;
	unsigned int rnAI;
	unsigned int rpAI;
	unsigned int rpDI;	
	signed long rVBMon[8];
	signed long rIBMon[8];
	float eepValue;
	unsigned long eeCtrSet;
	unsigned long asumCtl;
};
struct user_data_type xdata user_data;
   		

/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
void user_init(unsigned char init);
void user_loop(void);
void user_write(unsigned char index) reentrant;
unsigned char user_read(unsigned char index);
unsigned char user_func(unsigned char *data_in, unsigned char *data_out);
float read_voltage(unsigned char channel,unsigned int *rvalue);
int switchlowhigh(int number);
#endif
