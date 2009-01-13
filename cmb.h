/********************************************************************\
Name:         cmb.h
Created by:   Bahman Sotoodian
Created on:   April/9/2008
Modified  :   Noel Wu

Contents:     Application specific (user) part of
Midas Slow Control for CMB board.

$Id$
\********************************************************************/
#ifndef _CMB_H_
#define _CMB_H_

// External Vrefa
#define VREF       2.455f

// ADT7486A temperature addresses
#define ADT7486A_address    0x48
#define SST_LINE1           1
#define SST_TIME            4 // in second
#define TEMP_TIME           3 // in second

// Global definition
// Global ON / OFF definition
#define ON     1
#define DONE   1
#define OFF    0
#define FAILED 0
#define CLEAR  0
#define SET    1

//Internal ADC
#define IGAIN1  0
#define IGAIN2  1
#define IGAIN4  2
#define IGAIN8  3
#define IGAIN16 4

// EEPROM
#define SERIALN_LENGTH 4
#define SERIALN_ADD    (0x600)  // Fixed as sitting at the first byte of the EEPROM

// EEPROM page assignment (memory offset)  page Nr3 is the protected page
unsigned int xdata PageAddr[]={0x000, 0x200, 0x400, SERIALN_ADD};

//Keys for changing the EEPROM
#define EEP_CTRL_KEY        0x3C000000
#define EEP_CTRL_READ       0x00110000
#define EEP_CTRL_WRITE      0x00220000
#define EEP_CTRL_INVAL_REQ  0xff000000
#define EEP_CTRL_INVAL_KEY  0x00ff0000
#define EEP_CTRL_OFF_RANGE  0x0000ff00
#define PAGE_SIZE  sizeof(eepage)
#define EEP_RW_IDX          0x02

// EEPROM structure
// To be copied to the loader.h
//---------------------------------------------------------------
struct EEPAGE {
unsigned long SerialN;    
unsigned int structsze;
// 
float lVIlimit[8], uVIlimit[8]; // i4 v4 a33 a25 d15 d18 d25 d33
float luCTlimit, uuCTlimit;
float lSSTlimit, uSSTlimit;
float ext1offset[4];
};

// Default structure
struct EEPAGE xdata eepage = {
// 0x00 - Card Serial Number
 0x00000000
// 0x01 - Structure size
 , 110
// 0x02 - Low   i4 v4 a33 a25 d15 d18 d25 d33
 , 0.0, 3.0, 3.0, 2.0, 1.2, 1.5, 2.0, 3.0
// 0x0A - High  i4 v4 a33 a25 d15 d18 d25 d33
 , 4.0, 4.5, 3.7, 2.7, 1.7, 2.0, 2.7, 3.7
// 0x12 - LuC Temperature,  HuC Temperature
 , 10., 50.
// 0x14 - LSST Temperature,  HSST Temperature
 , 10. ,50.
// 0x33 - SST channel 1 offset[0..3]
 , 0.0, 0.0, 0.0, 0.0
};

// To be copied to the loader.h
//---------------------------------------------------------------

// EEPROM page for debugging
struct EEPAGE xdata eepage2; //NW testing

// Indices for user_write functions
#define IDXEER            1
#define IDXCTL            2

// CTL register
unsigned char bdata rCTL;
sbit CPup     = rCTL ^ 0;
sbit CXclk    = rCTL ^ 1;
sbit CeeS     = rCTL ^ 4;
sbit CeeR     = rCTL ^ 5;
sbit CeeClr   = rCTL ^ 6;
sbit CmSd     = rCTL ^ 7;

// CSR Register
unsigned char bdata rCSR;
sbit SPup     = rCSR ^ 0;
sbit SXclk    = rCSR ^ 1;
sbit SLos     = rCSR ^ 2;
sbit Sdeg1    = rCSR ^ 3;
sbit SeeS     = rCSR ^ 4;
sbit SeeR     = rCSR ^ 5;
sbit SsS      = rCSR ^ 6;
sbit SmSd     = rCSR ^ 7;

// ESR Error Register
//The low and high bytes are switched in the bdata section of the memory
//This is the reason that the sbit declarations do not appear to match
//the documentation but they actually do.
unsigned int bdata rESR;
sbit DI4mon    = rESR ^ 8;  //0x1
sbit DV4mon    = rESR ^ 9;  //0x2
sbit AV33mon   = rESR ^ 10; //0x4
sbit AV25mon   = rESR ^ 11; //0x8

sbit DV15mon   = rESR ^ 12; //0x10
sbit DV18mon   = rESR ^ 13; //0x20
sbit DV25mon   = rESR ^ 14; //0x40
sbit DV33mon   = rESR ^ 15; //0x80

sbit uCT       = rESR ^ 0;  //0x100
sbit FPGAssTT  = rESR ^ 1;  //0x200
sbit Vreg1ssTT = rESR ^ 2;  //0x400
sbit Vreg2ssTT = rESR ^ 3;  //0x800

sbit RdssT     = rESR ^ 4;  //0x1000
sbit EEPROM    = rESR ^ 5;  //0x2000
//sbit xxx      = rESR ^ 6; //0x4000
sbit V4_OC     = rESR ^ 7; //0x8000

// Shutdown mask
// Shut down the card only if any of the following bits in the rESR register is set
// correspond to the rESR bit assignment
#define UFTEMPERATURE_MASK (unsigned int) 0x0300
#define BTEMPERATURE_MASK  (unsigned int) 0x0C00
#define VOLTAGE_MASK       (unsigned int) 0x00FE
#define CURRENT_MASK       (unsigned int) 0x0001

// Coeff
// I		   R1	  R2	  (R1+R2)/R2	Rsense 	Isense/Vout
// Is4     2K	  4.7K	 6.7/4.7    0.015   10/(100*0.015)= 3.33
//  V      R1   R2    (R1+R2)/R1
// 40Mon   4.7  4.7K   9.4/4.7
// 33Mon   2K   4.7K   6.7/4.7
// 25Mon   2K   4.7K   6.7/4.7
// +1.5    no conversion.
// +1.8    no conversion.
// A+33Mon 2K   4.7K   6.7/4.7
// A+25Mon 2K   4.7K   6.7/4.7
float code coeff[8] =  {0.973, 2.000 , 1.426, 1.426, 1.0, 1.0, 1.426, 1.426};
float code offset[8] = {0.000,    0.0,   0.0, 0.0  , 0.0, 0.0, 0.0  , 0.0};


/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
  unsigned long SerialN;
  unsigned int  error;
  unsigned char control;
  unsigned char status;
  unsigned char eepage;
  unsigned char spare;

  float pDI4Mon; 
  float pDV4Mon;
  float pAV33Mon;
  float pAV25Mon;
  float pDV15Mon;
  float pDV18Mon;
  float pDV25Mon;
  float pDV33Mon;

  float uCTemp;
  float FPGATemp;
  float Vrg1Temp;
  float Vrg2Temp;

  // Raw data section
  unsigned int rpDI4Mon; 
  unsigned int rpDV4Mon; 
  unsigned int rpAV33Mon;
  unsigned int rA25VMon; 
  unsigned int rpDV15Mon;
  unsigned int rpDV18Mon;
  unsigned int rpDV25Mon;
  unsigned int rpDV33Mon;

  // EEPROM back door
  float eepValue;
  unsigned long eeCtrSet; 
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
float read_voltage(unsigned char channel,unsigned int *rvalue, unsigned char gain);

#endif