/********************************************************************\

  Name:         loader.h
  Created by:   Bahman Sotoodian				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for External EEPROM of FEB64 test board

  $Id:$

\********************************************************************/
#ifndef  _LOADER_H_
#define  _LOADER_H_



#define PAGE_SIZE			0xC8
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
struct user_data_type {
	unsigned char control;       
	unsigned char eepage;
	unsigned char status;	     
};

struct user_data_type xdata user_data;

struct EEPAGE {

float lVIlimit[8]; // vQ iQ +6Vd +6Va -6Va -6Ia +6Ia +6Id 
float uVIlimit[8];

float luCTlimit, uuCTlimit;
float lSSTlimit, uSSTlimit;

float lVQlimit,  uVQlimit;
float lIQlimit,  uIQlimit;

float lVBiaslimit, uVBiaslimit;
float lIBiaslimit, uIBiaslimit;

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

struct EEPAGE xdata eepage2 ;

struct TEST{
unsigned char Var1;
unsigned char Var2;
};

struct TEST xdata test1 = {
0x33,0x22
};

struct TEST xdata test2;
/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
void user_init(unsigned char init);
void user_loop(void);
void user_write(unsigned char index) reentrant;
unsigned char user_read(unsigned char index);
unsigned char user_func(unsigned char *data_in, unsigned char *data_out);

#endif
