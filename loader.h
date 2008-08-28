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

#define NUMBER_PAGES	   3
#define CLEAR				  0x01
#define WRITE				  0x00

//Defining the page mapping     Protected  Non protected
unsigned int xdata page_addr[]={0x600,     0x0,0x200,0x400};

#define ADDR_PCA9539   0x74
// PCA9539 Macro Definitions
#define BIAS_OUTPUT_ENABLE     ADDR_PCA9539, PCA9539_CONFIG0, PCA9539_ALL_OUTPUT
#define BIAS_DISABLE           ADDR_PCA9539, PCA9539_OUTPUT0, 0xFF
#define BIAS_ENABLE            ADDR_PCA9539, PCA9539_OUTPUT0, 0x00
#define BIAS_READ			         ADDR_PCA9539, PCA9539_INPUT0
#define BIAS_WRITE				     ADDR_PCA9539, PCA9539_OUTPUT0

#define BACKPLANE_INPUT_ENABLE ADDR_PCA9539, PCA9539_CONFIG1, PCA9539_ALL_INPUT
#define BACKPLANE_READ			   ADDR_PCA9539, PCA9539_INPUT1	 

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct{
	unsigned char control;       
	unsigned char eepage;
	unsigned char status;	
	unsigned long serialN;     
}xdata user_data;

bit EEPROM_FLAG;
#define IDXCTL 1

//
// SELECT EEPAGE STRUCTURE BASED ON BOARD
//-----------------------------------------------------------------------------
#ifdef L_TEMP36
struct EEPAGE {
	int ext1offset[18];
	int ext2offset[18];

	int control;
	unsigned long SerialN;
};
struct EEPAGE xdata eepage = {
	//Temperature sensor offsets corresponding to the external 1 channel
    //Temp02,04....36
	32,48,32,32,16,16,48,16,0,16,0,0,16,32,16,32,-16,32 ,
	//Temperature sensor offsets corresponding to the external 2 channel
    //Temp01,03....35
	48,32,48,64,64,32,48,64,0,16,16,32,48,48,32,32,16,32,

	0,
	1000
};

//-----------------------------------------------------------------------------
#elif defined(L_FEB64)
// EEPROM structure should be a COPY of the feb64.h structure
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

unsigned long xdata smbdebug;

//
//-----------------------------------------------------------------------------
#elif defined(L_CMB)
struct EEPAGE {
	unsigned long SerialN;
};
struct EEPAGE xdata eepage = {
	 0x0000
};

//
//-----------------------------------------------------------------------------
#elif defined(L_LPB)
struct EEPAGE {
	unsigned long SerialN;
};
struct EEPAGE xdata eepage = {
	 0x0000
};
#endif

// EEPROM page for loader confirmation
struct EEPAGE xdata eepage2;

// Macro EEPAGE size independently of the BOARD
#define PAGE_SIZE  sizeof(eepage)

#endif

