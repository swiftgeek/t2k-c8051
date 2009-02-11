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
#define ADT7486A_ADDR4 0x48
#define SST_LINE1         1
#define SST_LINE2         2

//BS Temperature testing
#define TEMP_ThRESHOLD  27
#define SST_TIME         5 //In us

/**********************************************************************************/
// PCA9539
#define BIAS_OUTPUT_ENABLE ADDR_PCA9539, PCA9539_CONFIG0, PCA9539_ALL_OUTPUT
#define BIAS_DISABLE       ADDR_PCA9539, PCA9539_OUTPUT0, 0xFF
#define BIAS_ENABLE        ADDR_PCA9539, PCA9539_OUTPUT0, 0x00
#define BIAS_READ          ADDR_PCA9539, PCA9539_INPUT0
#define BIAS_WRITE         ADDR_PCA9539, PCA9539_OUTPUT0

#define BACKPLANE_INPUT_ENABLE  ADDR_PCA9539, PCA9539_CONFIG1, PCA9539_ALL_INPUT
#define BACKPLANE_READ          ADDR_PCA9539, PCA9539_INPUT1

/**********************************************************************************/
// External ADC
#define N_RB_CHANNEL  16
#define EXT_VREF      1.25f  // 2.4989f actually 1/2 of the number here
#define CURR_MEASURE  0x04
#define V_A2MTABLE       0
#define I_A2MTABLE       1
#define VOLT_MEASURE  0x04
#define GAIN1            0 // actually x1
#define GAIN4            1 // actually x4
#define GAIN8            2 // actually x8
#define GAIN16           3 // actually x16
#define GAIN32           4 // actually x32
#define GAIN64           5 // actually x64

#define CONVER_FAC2 65536.0f

/**********************************************************************************/
// External ADC channel, gain conversion table
struct ADC2MSCB_TABLE {
int gain;
int mscbIdx;
int current;
float Coef;
signed int Offst;
};

struct ADC2MSCB_TABLE xdata adc2mscb_table[16] = {
  {GAIN32, 1, I_A2MTABLE, 10000, 22},    {GAIN32, 3, I_A2MTABLE, 10000, 127}
, {GAIN32, 5, I_A2MTABLE, 10000, 7176},  {GAIN32, 7, I_A2MTABLE, 10000, 1840}
, {GAIN1 , 7, V_A2MTABLE,   101.1, 230}, {GAIN1 , 5, V_A2MTABLE, 101.1, 230}
, {GAIN1 , 3, V_A2MTABLE,   101.1, 230}, {GAIN1 , 1, V_A2MTABLE, 101.1, 230}
, {GAIN32, 0, I_A2MTABLE, 10000, 3178},  {GAIN32, 2, I_A2MTABLE, 10000, 1800}
, {GAIN32, 4, I_A2MTABLE, 10000, 925},   {GAIN32, 6, I_A2MTABLE, 10000, 4895}
, {GAIN1 , 6, V_A2MTABLE,   101.1, 24},  {GAIN1 , 4, V_A2MTABLE, 101.1, 230}
, {GAIN1 , 2, V_A2MTABLE,   101.1, 225}, {GAIN1 , 0, V_A2MTABLE, 101.1, 230}
};
#define CONVER_FAC1 0

/**********************************************************************************/
// charge pump state for PCA control
#define Q_PUMP_INIT          1
#define Q_PUMP_OFF           2
#define Q_PUMP_ON            3

/**********************************************************************************/
// Internal ADC channel/gain assignment
#define INT_VREF      2.44f // Internal uC Vref
#define IntGAIN1  0
#define IntGAIN2  1
#define IntGAIN4  2
#define IntGAIN8  3
#define IntGAIN16 4

struct IADC_TABLE {
char gain;
float coeff;
float offset;
};

/**********************************************************************************/
//   LvQ, LiQ,  Lp6Vd, Lp6Va, Ln6Va , Lp6Ia, Lp6Ia , Lp6Id
struct IADC_TABLE xdata iadc_table[8] = {
  {IntGAIN2, 50.5,       0.0}, {IntGAIN8, 10000.0/8.0, 0.0}
, {IntGAIN1, 3.980132,     0}, {IntGAIN1, 3.980132, 0}
, {IntGAIN1, 8.5,    -18.751}, {IntGAIN1, 0.1, 0}
, {IntGAIN1, 0.4,          0}, {IntGAIN1, 0.1, 0}
};
//Qpump  Vbias readback conversion: 1010k/10k     = (101/IntGAIN2) = 50.5
//Qpump  Ibias readback conversion: HV7800 output = [Ibias x 100 ohms] {1ma = 100mv} x 10,000 change units to uA
//pDVmon +6V readback conversion:   24.40k/6.04k  = 3.980132
//pAVmon +6V readback conversion:   24.40k/6.04k  = 3.980132
//nAVmon -6V readback conversion:   17k/2k        = 8.5 [At 0V input -Vin = 2.206V x 8.5 = 18.751]

/**********************************************************************************/
// External EEPROM variables
unsigned char rEER;
//Keys for changing the EEPROM
#define EEP_CTRL_KEY        0x3C000000
#define EEP_CTRL_READ       0x00110000
#define EEP_CTRL_WRITE      0x00220000
#define EEP_CTRL_INVAL_REQ  0xff000000
#define EEP_CTRL_INVAL_KEY  0x00ff0000
#define EEP_CTRL_OFF_RANGE  0x0000ff00
#define PAGE_SIZE  sizeof(eepage)  // 300 for now
#define EEP_RW_IDX          0x06   // (I*4)  but use 0x17 for normal operation
#define EEP_RW_LAST_IDX   PAGE_SIZE/4

// EEPROM structure
struct EEPAGE {
unsigned long SerialN;    
unsigned int structsze;
unsigned int rasum[8];
unsigned int rqpump;
unsigned int SWbias;   
unsigned int calQpump;
unsigned char rbias [64]; 
// 
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
float vBiasOffset[8];
};

// Default structure
struct EEPAGE xdata eepage = {
// 0x00 - Card Serial Number
 0x00000000
// 0x1 - Structure size
 , 110
// - Asum[0..7] threshold
 , 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
// - Qpump DAC
 , 0x0000
// Backdoor option offset 0x06 for Qswitch|DACcal
// 0x06 - Qpump switch
 , 0x0000
// - Qpump DAC value for Calibration
 , 800
// 0x07 - DACs[0..63]
 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
// Backdoor option offset 0x17 in Float
// 0x17 - LvQ, LiQ,  Lp6Vd, Lp6Va, Ln6Va , Lp6Ia, Lp6Ia , Lp6Id
 ,30.0, 0.0, 5.5, 5.5, -6.5, 0.0, 0.0, 0.0
// 0x1F - HvQ, HiQ, Hp6Vd, Hp6Va, Hp6Va,  Hp6Ia, Hp6Ia,  Hp6Id
 ,73.0, 0.1, 6.5, 6.5, -5.5, 0.2, 0.8, 0.2
// 0x27 - LuC Temperature,  HuC Temperature
 ,10., 50.
// 0x29 - LSST Temperature,  HSST Temperature
 ,10. ,30.
// 0x2B - LVQ, HVQ (V)
 ,-1.0 ,1.0
// 0x2D - LVQ, HVQ (V)
 ,-0.1 ,1.0
// 0x2F - LVBias, HVBias (V)
 ,0.0 ,73.0
// 0x31 - LIBias, LVBias (uA)
 ,0.0 ,10.0
// 0x33 - SST channel 1 offset[0..3]
 ,0.0, 0.0, 0.0, 0.0
// 0x37 - SST channel 2 offset[0..3]
 ,0.0, 0.0, 0.0, 0.0
// 0x3B - Current Bias Offset [0..7]
 , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
// 0x43 - Voltage Bias Offset [0..7]
 , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

// EEPROM page for debugging
struct EEPAGE xdata eepage2; //NW testing

/**********************************************************************************/
// Definitions
#define NCHANNEL_BIAS    64
#define NCHANNEL_ASUM     8
#define NCHANNEL_SST      8
#define NCHANNEL_ADT7486A 4

// Indices for user_write functions
#define IDXEER            1
#define IDXCTL            2
#define IDXQVOLT          6
#define IDXBSWITCH        5
#define IDXASUM          47
#define IDXSST           19
#define IDXBIAS          55
#define IDXEEP_CTL      144
#define IDXASUM_CTL     145
 
#define SERIALN_LENGTH 4
#define SERIALN_ADD    (0x600)  // Fixed as sitting at the first byte of the EEPROM

// EEPROM page assignment (memory offset)  page Nr3 is the protected page
unsigned int xdata PageAddr[]={0x000, 0x200, 0x400, 0x600};

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

// CTL register
unsigned char bdata rCTL;
sbit CPup     = rCTL ^ 0;
//sbit CqPump   = rCTL ^ 1;  // Enable qPump with CPup in the right order
sbit CAsum    = rCTL ^ 2;
sbit Ccal     = rCTL ^ 3;
sbit CeeS     = rCTL ^ 4;
sbit CeeR     = rCTL ^ 5;
sbit CeeClr   = rCTL ^ 6;
sbit CmSd     = rCTL ^ 7;


// CSR Register
unsigned char bdata rCSR;
sbit SPup     = rCSR ^ 0;
sbit SqPump   = rCSR ^ 1;
sbit SAsum    = rCSR ^ 2;
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

sbit pcbssTT  = rESR ^ 4;  //0x1000
sbit RdssT    = rESR ^ 5;  //0x2000

//sbit xxx      = rESR ^ 6 //0x4000
//sbit xxx      = rESR ^ 7 //0x8000


// Shutdown mask
// Shut down the card only if any of the following bits in the rESR register is set
// correspond to the rESR bit assignment
// All the Vreg U/I, uC/Board/FGD Temperature
#define UCTEMPERATURE_MASK   0x0100
#define BTEMPERATURE_MASK    0x1200
#define FGDTEMPERATURE_MASK  0x0400
#define VOLTAGE_MASK         0x001C
#define CURRENT_MASK         0x00E0

// SMBus Port Aliases
//sbit SDA    = MSCB_I2C_SDA;
//sbit SCL    = MSCB_I2C_SCL;

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
float adcTemp;
float afterTemp;
float regTemp;
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
