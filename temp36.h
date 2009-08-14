/********************************************************************\

  Name:         temp36.h
  Created by:   Noel Wu

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for temp 36 board
   $Id$
\********************************************************************/

#ifndef  _TEMP36_H_
#define  _TEMP36_H_

/*****USER DEFINED DATA*****/
struct {
  unsigned long SerialN;    // Serial Number
  unsigned int  error;      // General Error register
  unsigned char control;    // Writing/Reading the EEPROM
  unsigned char status;     // Displaying the status of the EEPROM command
  unsigned char eepage;     // EEPROM page number
  unsigned int  navge;      // Number of samples for averging temperature
  unsigned char terror[5];  // Status for reading the temperature
  float         Temp[36];   // ADT7486A external temperature [degree celsius]
  float         ref;
  float         AT[36];
  unsigned long eepValue;   // EEPROM Value to be stored/read
  long          eeCtrSet;   // Initiate changing the offset values
  float         SHTtemp1;
  float         SHThumi1;
  float         SHTtemp2;
  float         SHThumi2;
  unsigned int  DAC[8];
} xdata user_data;


/*****TEMPERATURE VARIABLES*****/
#define SST_LINE1 1
#define SST_LINE2 2
//Address for all the ADT7486A sensors on the SST_LINE1/SST_LINE2
unsigned char xdata ADT7486A_addrArray[] = {0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50};

//Every element in Terrorclear indicates a particular binary bit is 0
unsigned char xdata Terrorclear[]={0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};

//Every element in Terrorclear indicates a particular binary bit is 1
unsigned char xdata Terrorset[]={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
#define TAVGMAX (int) 30
float xdata Taverage[36][TAVGMAX];

// EEPROM page assignment (memory offset)  page Nr3 is the protected page
unsigned int xdata PageAddr[]={0x000, 0x200, 0x400, 0x600};
#define SERIALN_LENGTH 4
#define SERIALN_ADD    (PageAddr[3])  // Fixed as sitting at the first byte of the EEPROM

// The structure of each EEPAGE
struct EEPAGE {
  unsigned long SerialN;    
  unsigned int structsze;
  unsigned int  navge;
  int ext1offset[18];
  int ext2offset[18];
};

//+/- 16 increments corresponds to a +/- 0.25 degrees offset in the ADT7486A chip
//Initial values for eepage
struct EEPAGE xdata eepage={
  0,     // S/N
  1000,  // Structure size
  10,    // Average value
  //Temperature sensor offsets corresponding to the external 1 channel
    //Temp02,04....36
  32, 48, 32, 32, 16, 16, 48, 16, 0, 16, 0, 0, 16, 32, 16, 32, -16, 32,
  //Temperature sensor offsets corresponding to the external 2 channel
  //Temp01,03....35
  48, 32, 48, 64, 64, 32, 48, 64, 0, 16, 16, 32, 48, 48, 32, 32, 16, 32,
};

#define PAGE_SIZE  sizeof(eepage)  // Macro

//Keys for changing the EEPROM
#define EEP_CTRL_KEY        0x3C000000
#define EEP_CTRL_READ       0x00110000
#define EEP_CTRL_WRITE      0x00220000
#define EEP_CTRL_INVAL_REQ -100
#define EEP_CTRL_INVAL_KEY -10
#define TEMPOFF_LAST_INDX   36

/*****HUMIDITY SENSOR VARIABLES*****/
#define humsen1 1
#define humsen2 2
sbit SHT_GND1=P1^3;
sbit SHT_VDD1=P1^1;
sbit SHT_GND2=P1^7;
sbit SHT_VDD2=P1^5;

/*****DAC VARIABLES******/
#define First_DACIndex   90
#define Last_DACIndex    97

/*****GENERAL*****/
//Control Register
unsigned char bdata rCTL;
sbit Cref     = rCTL ^ 1;
sbit CHum     = rCTL ^ 2;
sbit CeeS     = rCTL ^ 4;
sbit CeeR     = rCTL ^ 5;
sbit CeeClr   = rCTL ^ 6;

// Status Register
unsigned char bdata rCSR;
sbit Sref     = rCSR ^ 1;
sbit SeeS     = rCSR ^ 4;
sbit SeeR     = rCSR ^ 5;

// ESR Error Register
//The low and high bytes are switched in the bdata section of the memory
//This is the reason that the sbit declarations do not appear to match
//the documentation but they actually do.
unsigned int bdata rESR;
sbit err1      = rESR ^ 8;  //0x1
sbit err2      = rESR ^ 9;  //0x2
sbit err3      = rESR ^ 10; //0x4
sbit err4      = rESR ^ 11; //0x8

sbit err5      = rESR ^ 12; //0x10
sbit err6      = rESR ^ 13; //0x20
sbit err7      = rESR ^ 14; //0x40
sbit err8      = rESR ^ 15; //0x80

sbit uCT       = rESR ^ 0;  //0x100
sbit err10     = rESR ^ 1;  //0x200
sbit err11     = rESR ^ 2;  //0x400
sbit err12     = rESR ^ 3;  //0x800

sbit RdssT     = rESR ^ 4;  //0x1000
sbit EEPROM    = rESR ^ 5;  //0x2000
sbit err15     = rESR ^ 6;  //0x4000 1= any of 4 not lock
sbit err16     = rESR ^ 7;  //0x8000

//Index value for the user data
#define IDXCTL         2
#define IDXEEP_CTL    85
#define IDXNAVG        5
#define IDXREF        47

// Global ON / OFF definition
#define DONE   1
#define FAILED 0
#define SET    1
#define CLEAR  0
#endif



