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


#ifdef L_TEMP36
#define PAGE_SIZE			0x4E
#elif defined(L_FEB64)
#define PAGE_SIZE			0xE7
#elif defined(L_CMB)
#define PAGE_SIZE			0xC8
#elif defined(L_LPB)
#define PAGE_SIZE			0xC8
#endif

#define NUMEBER_PAGES	0x06
#define CLEAR				0x01
#define WRITE				0x00

//Defining the page mapping
unsigned int xdata page_addr[]={0x600,0x0,0x100,0x200,0x300,0x400,0x500};

#define ADDR_PCA9539   0x74
// PCA9539 Macro Definitions
#define BIAS_OUTPUT_ENABLE      ADDR_PCA9539, PCA9539_CONFIG0, PCA9539_ALL_OUTPUT
#define BIAS_DISABLE            ADDR_PCA9539, PCA9539_OUTPUT0, 0xFF
#define BIAS_ENABLE             ADDR_PCA9539, PCA9539_OUTPUT0, 0x00
#define BIAS_READ			        ADDR_PCA9539, PCA9539_INPUT0
#define BIAS_WRITE				  ADDR_PCA9539, PCA9539_OUTPUT0

#define BACKPLANE_INPUT_ENABLE  ADDR_PCA9539, PCA9539_CONFIG1, PCA9539_ALL_INPUT
#define BACKPLANE_READ			  ADDR_PCA9539, PCA9539_INPUT1	 

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct{
	unsigned char control;       
	unsigned char eepage;
	unsigned char status;	
	unsigned long serialN;     
}xdata user_data;

bit EEPROM_FLAG;
#define IDXCTL 1


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
#elif defined(L_FEB64)
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
#elif defined(L_CMB)
struct EEPAGE {
	unsigned long SerialN;
};
struct EEPAGE xdata eepage = {
	 0x0000
};
#elif defined(L_LPB)
struct EEPAGE {
	unsigned long SerialN;
};
struct EEPAGE xdata eepage = {
	 0x0000
};
#endif

struct EEPAGE xdata eepage2 ;


#endif

