/********************************************************************\
  Name:         feb64.h
  Created by:   Bahman Sotoodian
   Modified by: Noel Wu, Pierre Amaudruz
 
  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for T2K-FEB64 test board

  $Id$
\********************************************************************/
#ifndef  _FEB64_H
#define  _FEB64_H

/**********************************************************************************/
// ADT7486A
#define ADT7486A_ADDR0 0x49   
#define ADT7486A_ADDR1 0x48	
#define ADT7486A_ADDR2 0x4B
#define ADT7486A_ADDR3 0x4A
//BS Temperature testing
#define TEMP_ThRESHOLD  27
#define SST_TIME			5 //In us

/**********************************************************************************/
// PCA9539
#define BIAS_OUTPUT_ENABLE ADDR_PCA9539, PCA9539_CONFIG0, PCA9539_ALL_OUTPUT
#define BIAS_DISABLE       ADDR_PCA9539, PCA9539_OUTPUT0, 0xFF
#define BIAS_ENABLE        ADDR_PCA9539, PCA9539_OUTPUT0, 0x00
#define BIAS_READ		   ADDR_PCA9539, PCA9539_INPUT0
#define BIAS_WRITE	       ADDR_PCA9539, PCA9539_OUTPUT0

#define BACKPLANE_INPUT_ENABLE  ADDR_PCA9539, PCA9539_CONFIG1, PCA9539_ALL_INPUT
#define BACKPLANE_READ			    ADDR_PCA9539, PCA9539_INPUT1	 

/**********************************************************************************/
// External ADC
#define N_RB_CHANNEL	 16
#define EXT_VREF      1.25f  // 2.4989f actually 1/2 of the number here
#define CURR_MEASURE  0x04       
#define V_A2MTABLE       0
#define I_A2MTABLE       1
#define VOLT_MEASURE  0x04
#define GAIN1			 0 // actually x1
#define GAIN4            1 // actually x4
#define GAIN8            2 // actually x8
#define GAIN16           3 // actually x16
#define GAIN32           4 // actually x32
#define GAIN64           5 // actually x64

#define CONVER_FAC2 65536.0f

/**********************************************************************************/
#ifdef FEB64REV0
//2 of the 3 cards use LTC2497 instead of LTC2495
char code adc_convert[] = { 1, 3, 5, 7, 1, 3, 5, 7, 0, 2, 4, 6, 0, 2, 4, 6 };
// External ADC channel conversion table
struct ADC2MSCB_TABLE {
char mscbIdx;
char current;
float Coef;
unsigned int Offst;
};
struct ADC2MSCB_TABLE xdata adc2mscb_table[16] = {
  {1, V_A2MTABLE, 100, 0.2278}, {3, V_A2MTABLE, 100, 0.2278}
, {5, V_A2MTABLE, 100, 0.2278}, {7, V_A2MTABLE, 100, 0.2278}
, {7, I_A2MTABLE, 1000, 0}, {5, I_A2MTABLE, 1000, 0}
, {3, I_A2MTABLE, 1000, 0}, {1, I_A2MTABLE, 1000, 0}
, {0, V_A2MTABLE, 100, 0.2278}, {2, V_A2MTABLE, 100, 0.2278}
, {4, V_A2MTABLE, 100, 0.2278}, {6, V_A2MTABLE, 100, 0.2278}
, {6, I_A2MTABLE, 1000, 0}, {4, I_A2MTABLE, 1000, 0}
, {2, I_A2MTABLE, 1000, 0}, {0, I_A2MTABLE, 1000, 0}
};
#define CONVER_FAC1  65536.0f
float code Mon_Coef[]={  100, 100, 100, 100
                       , 1000, 1000, 1000, 1000
							  , 100, 100, 100, 100
							  , 1000, 1000, 1000, 1000};


float code Mon_Offst[]={  0.2278, 0.2278, 0.2278, 0.2278
								, 0, 0, 0, 0
								, 0.2278, 0.2278, 0.2278, 0.2278
								, 0, 0, 0, 0};
#elif defined(FEB64REV1)

/**********************************************************************************/
// External ADC channel, gain conversion table
struct ADC2MSCB_TABLE {
char gain;
char mscbIdx;
char current;
float Coef;
unsigned int Offst;
};

struct ADC2MSCB_TABLE xdata adc2mscb_table[16] = {
  {GAIN32, 1, I_A2MTABLE, 10000, 22}, {GAIN32, 3, I_A2MTABLE, 10000, 127}
, {GAIN32, 5, I_A2MTABLE, 10000, 7176}, {GAIN32, 7, I_A2MTABLE, 10000, 1840}
, {GAIN1 , 7, V_A2MTABLE,   101, 55}, {GAIN1 , 5, V_A2MTABLE, 101, 55}
, {GAIN1 , 3, V_A2MTABLE,   101, 55}, {GAIN1 , 1, V_A2MTABLE, 101, 55}
, {GAIN32, 0, I_A2MTABLE, 10000, 3178}, {GAIN32, 2, I_A2MTABLE, 10000, 1800}
, {GAIN32, 4, I_A2MTABLE, 10000, 925}, {GAIN32, 6, I_A2MTABLE, 10000, 4895}
, {GAIN1 , 6, V_A2MTABLE,   101, 55}, {GAIN1 , 4, V_A2MTABLE, 101, 55}
, {GAIN1 , 2, V_A2MTABLE,   101, 55}, {GAIN1 , 0, V_A2MTABLE, 101, 55}
};
#define CONVER_FAC1 0
#endif

/**********************************************************************************/
// charge pump state for PCA control
#define Q_PUMP_INIT          1           
#define Q_PUMP_OFF           2           
#define Q_PUMP_ON            3

/**********************************************************************************/
// Internal ADC channel/gain assignment
#define INT_VREF      2.432f // Internal uC Vref
#define IGAIN1  0
#define IGAIN2  1
#define IGAIN4  2
#define IGAIN8  3
#define IGAIN16 4

struct IADC_TABLE {
  char gain;
  float coeff;
  float offset;
};

/**********************************************************************************/
#ifdef FEB64REV0
struct IADC_TABLE xdata iadc_table[8] = {
  {IGAIN1, 41.448, -0.3464} , {IGAIN1, 2.496587, 0}
, {IGAIN1, 4.025, -0.06}    , {IGAIN1, 4.025, -0.054}
, {IGAIN1 , 8.4534, -18.622}, {IGAIN1 , 0.237, 0}
, {IGAIN1 , 0.475, 0}       , {IGAIN1 , 0.237, 0}
};

/**********************************************************************************/
#elif defined(FEB64REV1)
struct IADC_TABLE xdata iadc_table[8] = {
  {IGAIN2, 50.0,       0.0}, {IGAIN1, 10, 0}
, {IGAIN1, 3.980132,     0}, {IGAIN1, 3.980132, 0}
, {IGAIN1, 8.5,     -18.75}, {IGAIN1, 0.1, 0}
, {IGAIN1, 0.4,          0}, {IGAIN1, 0.1, 0}
};

#endif

/**********************************************************************************/
// External EEPROM variables
unsigned char rEER;
#define EEP_CTRL_KEY	    0x3C000000
#define EEP_CTRL_READ       0x00110000   	
#define EEP_CTRL_WRITE 	    0x00220000
#define EEP_CTRL_INVAL_REQ -100	
#define EEP_CTRL_INVAL_KEY -10	
#define TEMPOFF_INDX 		27
#define TEMPOFF_LAST_INDX   36

// EEPROM structure
struct EEPAGE {
unsigned long SerialN;
unsigned int structsze;
unsigned int rasum[8];
unsigned int rqpump;
unsigned int SWbias;
unsigned int calQpump; 
unsigned char rbias [64];  
// back door access starts @ 92 ->offset=23 (0x17)
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
float iBiasOffset[8];
};

// Default structure 
struct EEPAGE xdata eepage = {
// 0x0 - Card Serial Number
   0x00000000
// - Structure size
   ,110
// - Asum[0..7] threshold
	 ,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff
// - Qpump DAC
	 ,0x0000
// - Qpump switch
	 ,0x0000
// - Qpump DAC value for Calibration
   , 800
// - DACs[0..63]
	 ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	 ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	 ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	 ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	 ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	 ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	 ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	 ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
// 0x17 - LvQ, LiQ,  Lp6Vd, Lp6Va, Ln6Va , Lp6Ia, Lp6Ia , Lp6Id 
   ,30.0, 0.0, 5.5, 5.5, -6.5, 0.0, 0.0, 0.0
// 0x1F - HvQ, HiQ, Hp6Vd, Hp6Va, Hp6Va,  Hp6Ia, Hp6Ia,  Hp6Id 
   ,73.0, 0.1, 6.5, 6.5, -5.5, 0.2, 0.8, 0.2
// 0x27 - LuC Temperature,  HuC Temperature
	 ,18., 50.
// 0x29 - LSST Temperature,  HSST Temperature
   ,18. ,30.
// 0x2B - LVQ, HVQ (V)
	 ,-1.0 ,1.0
// 0x2D - LVQ, HVQ (V)
	 ,-0.1 ,1.0
// 0x2F - LVBias, HVBias (V)
	 ,0.0 ,73.0
// 0x31 - LIBias, LVBias (uA)
	 ,0.0 ,10.0
// 0x33 - SST channel 1 offset[0..3]
	 ,0,0 ,0,0
// 0x35 - SST channel 2 offset[0..3]
	 ,0,0 ,0,0
// 0x37 - Current Bias Offset [0..7]
   , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

// EEPROM page for debugging
struct EEPAGE xdata eepage2; //NW testing
#define PAGE_SIZE  sizeof(eepage)   //0x108         // with the iBiasOffsets

/**********************************************************************************/
// Definitions
#define NCHANNEL_BIAS    64
#define NCHANNEL_ASUM     8
#define NCHANNEL_SST      8
#define NCHANNEL_ADT7486A 4

// Indices for user_write functions
#define IDXCTL          2
#define IDXEER          1
#define IDXQVOLT        6
#define IDXBSWITCH      5
#define IDXASUM        44
#define IDXSST         40
#define IDXBIAS        52
#define IDXEEP_CTL	  141
#define IDXASUM_CTL   142

#define SERIALN_LENGTH 4
#define SERIALN_ADD    (0x600)  // Fixed as sitting at the first byte of the EEPROM  

// EEPROM page assignment (memory offset)
unsigned int xdata PageAddr[]={0x0,0x200,0x400};

#define FIRST_BIAS   IDXBIAS
#define LAST_BIAS    FIRST_BIAS + NCHANNEL_BIAS

#define FIRST_ASUM   IDXASUM
#define LAST_ASUM    FIRST_ASUM + NCHANNEL_ASUM

// Device Address mapping
#define ADDR_LTC1669   0x20
#define ADDR_PCA9539   0x74
#define ADDR_LTC2497   0x14	
#define ADDR_LTC2495   0x14
//NW mirror of the DAC values (for multiple DAC change)
unsigned char xdata ltc1665mirror [64];

// Global ON / OFF definition
#define ON     1
#define DONE   1
#define OFF    0
#define FAILED 0
#define CLEAR  0

 
// Vreg Enable port assignment
#ifdef FEB64REV0
sbit EN_pD5V = P3 ^ 4;
sbit EN_pA5V = P3 ^ 3;
sbit EN_nA5V = P3 ^ 2;
#elif defined(FEB64REV1)
// P3.4, P3.3 are spares
sbit REG_EN = P3 ^ 2;
#endif

// CTL register
unsigned char bdata rCTL;
sbit CPup     = rCTL ^ 0;
sbit CqPump   = rCTL ^ 1;
sbit Ccurrent = rCTL ^ 2; //temporary control the current limits
sbit Ccal     = rCTL ^ 3;
sbit CeeS     = rCTL ^ 4;
sbit CeeR     = rCTL ^ 5;
sbit CeeClr   = rCTL ^ 6;
sbit CmSd     = rCTL ^ 7;


// CSR Register
unsigned char bdata rCSR;
sbit SPup     = rCSR ^ 0;
sbit SqPump   = rCSR ^ 1;
sbit Scal     = rCSR ^ 3;
sbit SeeS     = rCSR ^ 4;
sbit SeeR     = rCSR ^ 5;
sbit SsS      = rCSR ^ 6;
sbit SmSd     = rCSR ^ 7;

// ESR Error Register
//The low and high bytes are switched in the bdata section of the memory
//This is the reason that the sbit declarations do not appear to match
//the documentation but they actually do.
unsigned int bdata rESR;
sbit vQpump   = rESR ^ 8;  //0x1
sbit iQpump   = rESR ^ 9;  //0x2
sbit vReg1    = rESR ^ 10; //0x4
sbit vReg2    = rESR ^ 11; //0x8

sbit vReg3    = rESR ^ 12; //0x10
sbit iReg1    = rESR ^ 13; //0x20
sbit iReg2    = rESR ^ 14; //0x40
sbit iReg3    = rESR ^ 15; //0x80

sbit uCT      = rESR ^ 0;  //0x100
sbit IntssTT  = rESR ^ 1;  //0x200
sbit ExtssTT  = rESR ^ 2;  //0x400
sbit EEPROM   = rESR ^ 3;  //0x800

sbit RdssT	  = rESR ^ 4;  //0x1000
//sbit xxx      = rESR ^ 5 //0x2000
//sbit xxx      = rESR ^ 6 //0x4000
//sbit xxx      = rESR ^ 7 //0x8000


// Shutdown mask
// Shut down the card only if any of the following bits in the rESR register is set
// correspond to the rESR bit assignment
// All the Vreg U/I, uC/Board/FGD Temperature
#define UCTEMPERATURE_MASK   0x0100
#define BTEMPERATURE_MASK    0x0200
#define FGDTEMPERATURE_MASK  0x0400
#define VOLTAGE_MASK         0x001C
#define CURRENT_MASK         0x00E0

// SMBus Port Aliases
sbit SDA		  = MSCB_I2C_SDA;
sbit SCL		  = MSCB_I2C_SCL;

/**********************************************************************************/
// MSCB structure
//Define variable parameters returned to CMD_GET_INFO command
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
// Raw data section
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
// EEPROM back door
	float eepValue;
	unsigned long eeCtrSet;
	unsigned long asumCtl;
	unsigned long watchdog;
	unsigned long debugsmb;
};
struct user_data_type xdata user_data;

// Debugging port
sbit timing = P2 ^ 7;
unsigned long xdata smbdebug;

/********************************************************************/
// Application specific init and inout/output routines
/********************************************************************/
void user_init(unsigned char init);
void user_loop(void);
void user_write(unsigned char index) reentrant;
unsigned char user_read(unsigned char index);
unsigned char user_func(unsigned char *data_in, unsigned char *data_out);
float read_voltage(unsigned char channel,unsigned int *rvalue, float coeff, float offset, unsigned char gain);
void switchonoff(unsigned char command);
int eepageAddrConvert(unsigned int index);
#endif
