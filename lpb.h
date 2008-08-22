/********************************************************************\
 Name:         lpb.h
 Created by:   Bahman Sotoodian                
 Created on:   April/9/2008 
 Modified  :   Noel Wu
 
 Contents:     Application specific (user) part of
 					Midas Slow Control for the LPB (Light Pulser Board). 					 
 
 $Id$
 \********************************************************************/

//AD5300 DAC
bit AD5300_FLAG;

// Internal Vref 
#define VREF       2.50f

// ADT7486A temperature addresses
#define ADT7486A_address 0x4C	
#define SST_LINE1 1
// Global definition
#define CLEAR 0
#define SET 1
#define DONE 1
#define FAILED 0
#define ON 1
#define OFF 0
// ESR Error Register
unsigned char bdata rESR;
sbit IntssTT   = rESR ^ 0; 
sbit Ext1ssTT  = rESR ^ 1;	
sbit Ext2ssTT  = rESR ^ 2;	



#define IGAIN1  0
#define IGAIN2  1
#define IGAIN4  2
#define IGAIN8  3
#define IGAIN16 4
// Coeff
//
//  V      R1   R2    (R1+R2)/R1
// 33Mon   2K   4.7K   6.7/4.7
// 25Mon   2K   4.7K   6.7/4.7
// A+33Mon 2K   4.7K   6.7/4.7
// A+25Mon 2K   4.7K   6.7/4.7
// +1.5    no conversion.
// +1.8    no conversion

float code coeff[8] = {1, 3.136, 0.4, 3.136, 0.4, 0.4, 0.4, 0.4};
float code offset[8] = {0,0,0,0,0,0,0,0}; 

/*****EEPROM VARIABLES*****/
//The EEPROM is separated into a total of 6 pages
unsigned int xdata PageAddr[]={0x0,0x100,0x200,0x300,0x400,0x500};
//The structure of each EEPAGE
struct EEPAGE {
	int ext1offset[18];
	int ext2offset[18];

	int control;
	unsigned long SerialN;
};
//+/- 16 increments corresponds to a +/- 0.25 degrees offset in the ADT7486A chip
//Initial values for eepage
struct EEPAGE xdata eepage={
	//Temperature sensor offsets corresponding to the external 1 channel
    //Temp02,04....36
	32,48,32,32,16,16,48,16,0,16,0,0,16,32,16,32,-16,32 ,
	//Temperature sensor offsets corresponding to the external 2 channel
    //Temp01,03....35
	48,32,48,64,64,32,48,64,0,16,16,32,48,48,32,32,16,32,

	0,
	1000
};
//Keys for changing the EEPROM
#define EEP_CTRL_KEY	    0x3C000000
#define EEP_CTRL_READ    0x00110000  	
#define EEP_CTRL_WRITE 	 0x00220000
#define EEP_CTRL_INVAL_REQ -100	
#define EEP_CTRL_INVAL_KEY -10	
#define SERIALN_ADD  0x64A  

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
unsigned long SerialN;
unsigned char error;
unsigned char control; 		//Writing/Reading the EEPROM
unsigned char status;  		//Displaying the status of the EEPROM command
unsigned char eepage;  		//EEPROM page number

float iadc[8]; 
  
float uCTemp;
float IntTemp;  
float _58Temp;
float _33Temp;

float riadc[8];

unsigned char rdac;

char spare1;
int spare2;

unsigned long eepValue;		//EEPROM Value to be stored/read
long 			  eeCtrSet;	   //Initiate changing the offset values
};
struct user_data_type xdata user_data;

#define IDXDAC			25
#define IDXCTL        2
#define IDXEEP_CTL	29

unsigned char bdata rCTL;
sbit CPup   = rCTL ^ 0;
sbit CeeS	= rCTL ^ 4;
sbit CeeR   = rCTL ^ 5;
sbit CeeClr = rCTL ^ 6;
sbit CmSd   = rCTL ^ 7;

unsigned char bdata rCSR;
sbit SPup	= rCSR ^ 0;
sbit SeeS   = rCSR ^ 4;
sbit SeeR   = rCSR ^ 5;
sbit SmSd   = rCSR ^ 7;

sbit VCC_EN =  P1 ^ 3;
sbit VREG_5	=	P2 ^ 5;
sbit VREG_3 =  P2 ^ 6;
sbit VREG_1 =  P2 ^ 7;
/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
float read_voltage(unsigned char channel,unsigned int *rvalue, unsigned char gain);

