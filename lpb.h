/********************************************************************\
 Name:         lpb.h
 Created by:   Bahman Sotoodian
 Created on:   April/9/2008
 Modified  :   Noel Wu

 Contents:     Application specific (user) part of
                Midas Slow Control for the LPB (Light Pulser Board).

 $Id$
 \********************************************************************/

#ifndef _LPB_H_
#define _LPB_H_

// Internal Vref
#define VREF       2.50f

// ADT7486A temperature addresses
#define ADT7486A_ADDR0    0x4C
#define NCHANNEL_ADT7486A    1
#define SST_LINE1            1

// Global definition
#define CLEAR    0
#define SET      1
#define DONE     1
#define FAILED   0
#define ON       1
#define OFF      0

#define IntGAIN1  0
#define IntGAIN2  1
#define IntGAIN4  2
#define IntGAIN8  3
#define IntGAIN16 4

// Coeff
// Input    V         R1      R2      (R1+R2)/R1  
// AIN0.0  D2A       1.3K    4.7K     6.0/1.3 (4.615)
// AIN0.1  V+6ss     2.2K    4.7K     6.9/2.2 (3.136)
// AIN0.2  Iss       400mA/V          0.4
// AIN0.3  V+6dd     2.2K    4.7K     6.9/2.2 (3.136)
// AIN0.4  Idd       400mA/V          0.4      
// AIN0.5  I5        400mA/V          0.4 
// AIN0.6  I3.3      400mA/V          0.4
// AIN0.7  I1.8      400mA/V          0.4

//                       D2A    Vss    Iss Vdd    Idd   I5  I3.3 I1.8
float code coeff[8]  = {4.615, 3.136, 0.4, 3.136, 0.4, 0.4, 0.4, 0.4};
float code offset[8] = {    0,     0,   0,     0,   0,   0,   0,   0};
unsigned char xdata ltc2620mirror[16];

/*****EEPROM VARIABLES*****/
#define SERIALN_LENGTH 4
#define SERIALN_ADD    (0x600)  // Fixed as sitting at the first byte of the EEPROM

// EEPROM page assignment (memory offset)  page Nr3 is the protected page
unsigned int xdata PageAddr[]={0x000, 0x200, 0x400, 0x600};

// The structure of each EEPAGE
struct EEPAGE {
  unsigned long SerialN;
  unsigned int structsze; 
  unsigned int spare; 
  unsigned int rdac[16];
  float lVIlimit[8];
  float uVIlimit[8];
  float luCTlimit, uuCTlimit;
  float lSSTlimit, uSSTlimit;
};

//+/- 16 increments corresponds to a +/- 0.25 degrees offset in the ADT7486A chip
//Initial values for eepage
struct EEPAGE xdata eepage={
// 0x00 - S/N
     0x00000000   
// 0x01 - Struct size, spare
   , 0x0000, 0x0000
// 0x02 - rdacs
   , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
   , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
// 0x0a - lVIlimit 
//   D2A  Vss  Iss  Vdd  Idd  I5   I3.3  I1.8
   , 0.0, 5.5, 0.1, 5.5, 0.0, 0.0, 0.0,  0.0
// 0x12 - uVIlimit 
//   D2A  Vss  Iss  Vdd  Idd  I5   I3.3  I1.8
   , 6.0, 6.5, 0.4, 6.5, 0.6, 0.2, 0.5,  0.2  
// 0x1a - LuC Temperature,  HuC Temperature
   , 10., 50.
// 0x1c - LSST Temperature,  HSST Temperature
   , 10., 50.
};

#define PAGE_SIZE sizeof(eepage)

//Keys for changing the EEPROM
#define EEP_CTRL_KEY        0x3C000000
#define EEP_CTRL_READ       0x00110000
#define EEP_CTRL_WRITE      0x00220000
#define EEP_CTRL_INVAL_REQ  0xff000000
#define EEP_CTRL_INVAL_KEY  0x00ff0000
#define EEP_CTRL_OFF_RANGE  0x0000ff00
#define EEP_RW_IDX          0x02   // (I*4) 

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
unsigned long SerialN;
unsigned int  error;
unsigned char control;     //Writing/Reading the EEPROM
unsigned char status;      //Displaying the status of the EEPROM command
unsigned char eepage;      //EEPROM page number
unsigned char spare;
float iadc[8];
float uCTemp;
float IntTemp;
float Temp58;
float Temp33;
float SHTtemp;
float SHThumid;
unsigned int rdac[16];
unsigned int riadc[8];
unsigned int rSHTemp;
unsigned int rSHhumid;
char spare1;
int  spare2;
float          eepValue;    //EEPROM Value to be stored/read
unsigned long  eeCtrSet;     //Initiate changing the offset values
};
struct user_data_type xdata user_data;

#define IDXDAC      19
#define IDXCTL       2
#define IDXEEP_CTL  46
#define IDXDELAY     5

unsigned char bdata rCTL;
sbit CPup   = rCTL ^ 0;
sbit CeeS   = rCTL ^ 4;
sbit CeeR   = rCTL ^ 5;
sbit CeeClr = rCTL ^ 6;
sbit CmSd   = rCTL ^ 7;

unsigned char bdata rCSR;
sbit SPup   = rCSR ^ 0;
sbit S6dd   = rCSR ^ 1;
sbit SeeS   = rCSR ^ 4;
sbit SeeR   = rCSR ^ 5;
sbit SsS    = rCSR ^ 6;
sbit SmSd   = rCSR ^ 7;

// ESR Error Register
//The low and high bytes are switched in the bdata section of the memory
//This is the reason that the sbit declarations do not appear to match
//the documentation but they actually do.
//D2A    Vss    Iss Vdd    Idd   I5  I3.3 I1.8
unsigned int bdata rESR;
sbit Vss   = rESR ^ 8;  //0x1  
sbit Iss   = rESR ^ 9;  //0x2
sbit Vdd   = rESR ^ 10; //0x4
sbit Idd   = rESR ^ 11; //0x8

sbit I5    = rESR ^ 12; //0x10 
sbit I33   = rESR ^ 13; //0x20 
sbit I18   = rESR ^ 14; //0x40 
sbit uCT   = rESR ^ 15; //0x80 

sbit IntssTT  = rESR ^ 0;  //0x100
sbit Ext1ssTT = rESR ^ 1;  //0x200
sbit Ext2ssTT = rESR ^ 2;  //0x400
sbit EEPROM   = rESR ^ 3;  //0x800

sbit pcbssTT  = rESR ^ 4;    //0x1000
sbit RdssT    = rESR ^ 5;    //0x2000
//sbit xxx      = rESR ^ 6;  //0x4000
sbit V6Fault  = rESR ^ 7;  //0x8000


// Shutdown mask
// Shut down the card only if any of the following bits in the rESR register is set
// correspond to the rESR bit assignment
// All the Vreg U/I, uC/Board/FGD Temperature
#define UCTEMPERATURE_MASK   0x0180
#define BTEMPERATURE_MASK    0x0600
#define VOLTAGE_MASK         0x0001
#define CURRENT_MASK         0x0000
#define MAIN_CURRENT_MASK    0x8000

//---------------------------------------------------------------
// P2.7:+1.8En   .6:+3.3En    .5:+5En     .4:SPIMOSI | .3:SPISCK  .2:RAMHLDn  .1:SPIMISO .0:RAMWP
// P1.7:NC       .6:+6ddFlag  .5:R/HClock .4:R/HData | .3:+6ddEN  .2:RAMCS    .1:NC      .0:SST_DRV 
//
sbit VCC_EN    = P1 ^ 3;
sbit VREG_5    = P2 ^ 5;
sbit VREG_3    = P2 ^ 6;
sbit VREG_1    = P2 ^ 7;
sbit V6ddFlag  = P1 ^ 6;

sbit DELAY_0   = P0 ^ 6;
sbit DELAY_1   = P0 ^ 7;

/*****HUMIDITY SENSOR VARIABLES*****/
#define humsense 1

/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
float read_voltage(unsigned char channel,unsigned int *rvalue, unsigned char gain);
#endif