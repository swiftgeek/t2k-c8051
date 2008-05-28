/********************************************************************\

  Name:         temp36.h
  Created by:   Noel Wu				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for temp 36 board
   $Id$
\********************************************************************/

//Address for all the ADT7486A sensors on the SST_LINE1/SST_LINE2
unsigned char ADT7486A_addrArray[] = {0x48,0x49,0x4A,0x4B,0x4C,0x4D, 0x4E,0x4F,0x50};
//The EEPROM is separated into a total of 6 pages
unsigned int xdata PageAddr[]={0x0,0x100,0x200,0x300,0x400,0x500};
//Every element in Terrorclear indicates a particular binary bit is 0
unsigned char xdata Terrorclear[]={0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
//Every element in Terrorclear indicates a particular binary bit is 1
unsigned char xdata Terrorset[]={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
//+/- 16 increments corresponds to a +/- 0.25 degrees offset in the ADT7486A chip
signed int xdata Text1offset[]={-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278};
signed int xdata Text2offset[]={-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278,-1278};
//Temporary storage for the temperature
float xdata temperature;
//User defined data
struct {
	unsigned long SerialN;		//Serial Number
	unsigned char control; 		//Writing/Reading the EEPROM
	unsigned char status;  		//Displaying the status of the EEPROM command
	unsigned char eepage;  		//EEPROM page number
	unsigned char terror[5]; 	//Status for reading the temperature
  	float         Temp[36];    //ADT7486A external temperature [degree celsius]
   unsigned long eepValue;		//EEPROM Value to be stored/read
	long eeCtrSet;					//Initiate changing the offset values
	int ext1offset[18];			//Display the values stored in the EEPROM
	int ext2offset[18];
} xdata user_data;			

#define SST_LINE1 1
#define SST_LINE2 2

//Index value for the user data
#define IDXCTL       	1
#define IDXEEP_CTL	  46

// Control register
unsigned char bdata rCTL;
sbit CeeS     = rCTL ^ 0;
sbit CeeR     = rCTL ^ 1;
sbit CeeClr   = rCTL ^ 2;


// Status Register
unsigned char bdata rCSR;
sbit SeeS     = rCSR ^ 0;
sbit SeeR     = rCSR ^ 1;


// Global ON / OFF definition
#define DONE   1
#define FAILED 0
#define SET    1
#define CLEAR  0

//The structure of each EEPAGE
struct EEPAGE {
	int ext1offset[18];
	int ext2offset[18];

	unsigned long SerialN;
};

//Initial values for eepage
struct EEPAGE xdata eepage={
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
	19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,
	1000
};

//Reading from the eeprom
struct EEPAGE xdata eepage2;

//Keys for changing the EEPROM
#define EEP_CTRL_KEY	    0x3C000000
#define EEP_CTRL_READ    0x00110000  	
#define EEP_CTRL_WRITE 	 0x00220000
#define EEP_CTRL_INVAL_REQ -100	
#define EEP_CTRL_INVAL_KEY -10	
#define EEP_CTRL_INVAL_OVERLIMIT -1000 
#define SERIALN_ADD  0x648  