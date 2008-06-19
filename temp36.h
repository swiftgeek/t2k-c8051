/********************************************************************\

  Name:         temp36.h
  Created by:   Noel Wu				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for temp 36 board
   $Id$
\********************************************************************/

/*****USER DEFINED DATA*****/
struct {
	unsigned long SerialN;		//Serial Number
	unsigned char control; 		//Writing/Reading the EEPROM
	unsigned char status;  		//Displaying the status of the EEPROM command
	unsigned char eepage;  		//EEPROM page number
	unsigned char terror[5]; 	//Status for reading the temperature
  	float         Temp[36];    //ADT7486A external temperature [degree celsius]
   unsigned long eepValue;		//EEPROM Value to be stored/read
	long 			  eeCtrSet;	   //Initiate changing the offset values
   float			  SHTtemp1;
   float  		  SHThumi1;
   float			  SHTtemp2;
   float  		  SHThumi2;
   unsigned int  DAC[8];
	unsigned int  navg;
	float 		  AT[36];
} xdata user_data;			


/*****TEMPERATURE VARIABLES*****/
#define SST_LINE1 1
#define SST_LINE2 2
//Address for all the ADT7486A sensors on the SST_LINE1/SST_LINE2
unsigned char xdata ADT7486A_addrArray[] = {0x48,0x49,0x4A,0x4B,0x4C,0x4D, 0x4E,0x4F,0x50};
//Every element in Terrorclear indicates a particular binary bit is 0
unsigned char xdata Terrorclear[]={0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
//Every element in Terrorclear indicates a particular binary bit is 1
unsigned char xdata Terrorset[]={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
float xdata Taverage[36][30];
#define TAVGMAX 30

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
//Reading from the eeprom
struct EEPAGE xdata eepage2;


/*****HUMIDITY SENSOR VARIABLES*****/
#define humsen1 1
#define humsen2 2
sbit SHT_GND1=P1^3;
sbit SHT_VDD1=P1^1;
sbit SHT_GND2=P1^7;
sbit SHT_VDD2=P1^5;


/*****DAC VARIABLES******/
#define First_DACIndex    52
#define Last_DACIndex	  59

	 
/*****GENERAL*****/

//Control Register
unsigned char bdata rCTL;
sbit CeeS     = rCTL ^ 0;
sbit CeeR     = rCTL ^ 1;
sbit CeeClr   = rCTL ^ 2;
sbit CHum     = rCTL ^ 3;

// Status Register
unsigned char bdata rCSR;
sbit SeeS     = rCSR ^ 0;
sbit SeeR     = rCSR ^ 1;

//Index value for the user data
#define IDXCTL          	1
#define IDXEEP_CTL	     47
#define IDXNAVG 		  	   4

// Global ON / OFF definition
#define DONE   1
#define FAILED 0
#define SET    1
#define CLEAR  0





