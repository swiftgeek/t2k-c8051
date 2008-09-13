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

//AD5300 DAC
bit AD5300_FLAG;

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

// ESR Error Register
unsigned char bdata rESR;
sbit IntssTT   = rESR ^ 0;
sbit Ext1ssTT  = rESR ^ 1;
sbit Ext2ssTT  = rESR ^ 2;
sbit EEPerror  = rESR ^ 3;

#define IGAIN1  0
#define IGAIN2  1
#define IGAIN4  2
#define IGAIN8  3
#define IGAIN16 4

// Coeff
//  V      R1   R2    (R1+R2)/R1
// D2A     2.2K 6.8K   2.2/9.0
// Vss     2.2K 6.8K   2.2/9.0
// Iss
// Vdd     2.2K 6.8K   2.2/9.0
// Idd
// I5
// I3.3
// I1.8
// D2A Vss Iss Vdd Idd I5 I3.3 I1.8
float code coeff[8]  = {4.091, 4.091, 0.4, 4.091, 0.4, 0.4, 0.4, 0.4};
float code offset[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/*****EEPROM VARIABLES*****/
//The EEPROM is separated into a total of 4 pages, 0x600 is the WP page
unsigned int xdata PageAddr[]={0x0, 0x200, 0x400, 0x600};

// The structure of each EEPAGE
struct EEPAGE {
  unsigned long SerialN;
  unsigned int structsze; 
  unsigned int spare; 
  float sstOffset[2];
};

//+/- 16 increments corresponds to a +/- 0.25 degrees offset in the ADT7486A chip
//Initial values for eepage
struct EEPAGE xdata eepage={
// 0x00 - S/N
   0,   
// 0x01 - 
   0,
   0xaabb,
// 0x02 - SST offset
   0.0, 0.0  
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
#define SERIALN_ADD         0x600

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
unsigned long SerialN;
unsigned char error;
unsigned char control;     //Writing/Reading the EEPROM
unsigned char status;      //Displaying the status of the EEPROM command
unsigned char eepage;      //EEPROM page number
float iadc[8];
float uCTemp;
float IntTemp;
float _58Temp;
float _33Temp;
float  SHTtemp;
float SHThumi;
float riadc[8];
unsigned char rdac;
char spare1;
int  spare2;
float          eepValue;    //EEPROM Value to be stored/read
unsigned long  eeCtrSet;     //Initiate changing the offset values
};
struct user_data_type xdata user_data;

#define IDXDAC      27
#define IDXCTL       2
#define IDXEEP_CTL  31

unsigned char bdata rCTL;
sbit CPup   = rCTL ^ 0;
sbit CeeS   = rCTL ^ 4;
sbit CeeR   = rCTL ^ 5;
sbit CeeClr = rCTL ^ 6;
sbit CmSd   = rCTL ^ 7;

unsigned char bdata rCSR;
sbit SPup   = rCSR ^ 0;
sbit SeeS   = rCSR ^ 4;
sbit SeeR   = rCSR ^ 5;
sbit SmSd   = rCSR ^ 7;

//---------------------------------------------------------------
// P2.7:+1.8En   .6:+3.3En    .5:+5En     .4:SPIMOSI | .3:SPISCK  .2:RAMHLDn  .1:SPIMISO .0:RAMWP
// P1.7:NC       .6:+6ddFlag  .5:R/HClock .4:R/HData | .3:+6ddEN  .2:RAMCS    .1:D2ASync .0:SST_DRV 
//
sbit VCC_EN =  P1 ^ 3;
sbit VREG_5 =  P2 ^ 5;
sbit VREG_3 =  P2 ^ 6;
sbit VREG_1 =  P2 ^ 7;

/*****HUMIDITY SENSOR VARIABLES*****/
#define humsense 1

/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
float read_voltage(unsigned char channel,unsigned int *rvalue, unsigned char gain);
#endif