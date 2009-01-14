/********************************************************************\

  Name:         loader.h
  Created by:   Bahman Sotoodian

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for External EEPROM of FEB64 test board

  $Id$

\********************************************************************/
#ifndef  _LOADER_H_
#define  _LOADER_H_

#define NUMBER_PAGES     3
#define CLEAR         0x01
#define WRITE         0x00

//Defining the page mapping      NP[0..2]  0x600 Protected
unsigned int xdata page_addr[] = {0x000,0x200,0x400,0x600};

#define ADDR_PCA9539   0x74
// PCA9539 Macro Definitions
#define BIAS_OUTPUT_ENABLE     ADDR_PCA9539, PCA9539_CONFIG0, PCA9539_ALL_OUTPUT
#define BIAS_DISABLE           ADDR_PCA9539, PCA9539_OUTPUT0, 0xFF
#define BIAS_ENABLE            ADDR_PCA9539, PCA9539_OUTPUT0, 0x00
#define BIAS_READ              ADDR_PCA9539, PCA9539_INPUT0
#define BIAS_WRITE             ADDR_PCA9539, PCA9539_OUTPUT0

#define BACKPLANE_INPUT_ENABLE ADDR_PCA9539, PCA9539_CONFIG1, PCA9539_ALL_INPUT
#define BACKPLANE_READ         ADDR_PCA9539, PCA9539_INPUT1
#define SERIALN_LENGTH         4

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct{
  unsigned long serialN;
  unsigned char control;
  unsigned char eepage;
  unsigned char status;
  unsigned long serialNWp;
  unsigned int  structszeWp;
  unsigned long serialN0;
  unsigned int  structsze0;
}xdata user_data;

char EEPROM_FLAG;
#define IDXCTL 1

//
// SELECT EEPAGE STRUCTURE BASED ON BOARD
//-----------------------------------------------------------------------------
#ifdef L_TEMP36
// The structure of each EEPAGE
//---------------------------------------------------------------
struct EEPAGE {
unsigned long SerialN;    
unsigned int structsze;
unsigned int debug1;
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
// 0x01 - debug size
 , 123
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

//-----------------------------------------------------------------------------
#elif defined(L_FEB64)
// EEPROM structure should be a COPY of the feb64.h structure
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
 ,0.0, 0.0, 0.0, 0.0
// 0x37 - SST channel 2 offset[0..3]
 ,0.0, 0.0, 0.0, 0.0
// 0x3B - Current Bias Offset [0..7]
 , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
// 0x43 - Voltage Bias Offset [0..7]
 , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.00
 };
unsigned long xdata smbdebug;

//
//-----------------------------------------------------------------------------
#elif defined(L_CMB)
struct EEPAGE {
unsigned long SerialN;    
unsigned int structsze;
unsigned int debug1;
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
// 0x01 - Debug
 , 123
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

//
//-----------------------------------------------------------------------------
#elif defined(L_LPB)

//The structure of each EEPAGE
struct EEPAGE {
  unsigned long SerialN;
  unsigned int structsze;   //  0x1
  float sstOffset[2];
};

//+/- 16 increments corresponds to a +/- 0.25 degrees offset in the ADT7486A chip
//Initial values for eepage
struct EEPAGE xdata eepage={
   0,    // S/N
   0,
   0.0, 0.0  // SST Offset
};
#endif  // L_xxx

//
// Global to ALL boards
// Macro EEPAGE size independently of the BOARD
#define PAGE_SIZE  sizeof(eepage)
// EEPROM page for loader confirmation
struct EEPAGE xdata eepage2;

#endif  // _LOADER_H_