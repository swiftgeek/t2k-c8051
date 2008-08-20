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

// External Vrefa 
#define VREF       2.455f

// ADT7486A temperature addresses
#define ADT7486A_address 0x48	
#define SST_LINE1 1
// Global definition
#define CLEAR 0
#define SET 1

// ESR Error Register
unsigned char bdata rESR;
sbit IntssTT   = rESR ^ 0; 
sbit Ext1ssTT  = rESR ^ 1;	
sbit Ext2ssTT  = rESR ^ 2;	

// Coeff
//
//  V      R1   R2    (R1+R2)/R1
// 33Mon   2K   4.7K   6.7/4.7
// 25Mon   2K   4.7K   6.7/4.7
// A+33Mon 2K   4.7K   6.7/4.7
// A+25Mon 2K   4.7K   6.7/4.7
// +1.5    no conversion.
// +1.8    no conversion

// I		  R1	  R2	  (R1+R2)/R2	Rsense 	Isense/Vout
//	Is4     2K	  4.7K	6.7/4.7     0.015    10/(100*0.015)= 3.33     
// IsSC	  2K	  4.7K   6.7/4.7	   0.47		10/(100*4.7) = 0.0106
float code coeff[8] = {1.9,0.304,1.426,1.426,1.0,1.0,1.426,1.426};
float code offset[8] = {0,0,0,0,0,0,0,0}; 


/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
unsigned char error;
unsigned long SerianN;
float iadc[7];   

float InteTemp;  
float FPGATemp;
float VregTemp;

float riadc[7];

unsigned char led;

};
struct user_data_type xdata user_data;

#define IDXLED			19
/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
float read_voltage(unsigned char channel,unsigned int *rvalue);

