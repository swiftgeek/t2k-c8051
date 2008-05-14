/********************************************************************\

  Name:         temp36.h
  Created by:   Noel Wu				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for temp 36 board
   $Id$
\********************************************************************/

unsigned char ADT7486A_addrArray[] = {0x50,0x4F,0x4E,0x4D,0x4C,0x4B, 0x4A,0x49,0x48};
unsigned int xdata PageAddr[]={0x0,0x100,0x200,0x300,0x400,0x500};
float xdata temperature;
struct
{
  	unsigned long error;
	unsigned long error2;
	unsigned long SerialN;
	unsigned char control;     
	unsigned char eepage;   
	unsigned char status;	
  	float         Temp[36];    // ADT7486A external2 temperature [degree celsius]
}xdata user_data;

#define SST_LINE1 1
#define SST_LINE2 2

//Index value for the user data
#define IDXCTL 	2

// CTL register
unsigned char bdata rCTL;
sbit CeeS     = rCTL ^ 0;
sbit CeeR     = rCTL ^ 1;
sbit CeeClr   = rCTL ^ 2;


// CSR Register
unsigned char bdata rCSR;
sbit SeeS     = rCSR ^ 0;
sbit SeeR     = rCSR ^ 1;


// Global ON / OFF definition
#define CLEAR  0
#define SET		1
#define DONE   1
#define FAILED 0

struct EEPAGE {
	float exttemp1[18];
	float exttemp2[18];
};

struct EEPAGE xdata eepage={
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
	20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
};

//BS	For testing the EXTEEPROM
struct EEPAGE xdata eepage2;