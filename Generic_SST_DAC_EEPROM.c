/********************************************************************\

  Name:     Generic_SST_DAC_EEPROM
  Author:   Bahman Sotoodian     
  Modified by: Noel Wu 
  Date:     August 6, 2008
  $Id:$

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mscbemb.h"

#ifdef _ADT7486A_
#include "Devices/ADT7486A_tsensor.h"
#endif

#ifdef _LTC2605_
#include "Devices/LTC2605CGN.h"
#endif

#ifdef _LTC2489_
#include "Devices/LTC2489_adc.h"
#endif

#ifdef _PCA9536_
#include "Devices/PCA9536.h"
#endif


/*
//Control definitions
extern unsigned long _systime;
// Node Name
*/

char code node_name[] = "GENERIC310_REV1";
char idata svn_rev_code[] = "$Rev: 57 $";

// declare number of sub-addresses to framework 
unsigned char idata _n_sub_addr = 1;
extern SYS_INFO sys_info;

// defining the address
unsigned char xdata ADT7486A_addrArray[] = {0x48, 0x4A};
unsigned char xdata ADDR_LTC2489 [] = {0x24, 0x17};
unsigned char xdata DAQ_ADDRESS = 0x42;
unsigned char xdata PCA9536_ADD = 0x41;

unsigned char xdata FCS_Mismatch;
unsigned char xdata BiasIndex;

#define FIRST_DAQ 18
#define LAST_DAQ  FIRST_DAQ + 8
#define N_CHANNEL 4              //Number of ADC Channels
#define VREF         2.4986f
#define CONVER_FAC1  65536.0f
#define CONVER_FAC2	131072.0f
#define CLEAR 0
#define PCA_OUT 0x00
#define PCA_INDEX 1
#define SST_LINE 1
 // Global bit register
unsigned char bdata bChange;
sbit LTC2605_Flag = bChange ^ 0;
sbit PCA9536_Flag = bChange ^ 1;

//Error Register                                                                                                                  
unsigned char bdata rErr;
sbit adc     = rErr ^ 0;
sbit INTtemp = rErr ^ 1;
sbit EXTtemp = rErr ^ 2;

/*---- Define channels and configuration parameters returned to
       the CMD_GET_INFO command                                 ----*/

/* data buffer (mirrored in EEPROM) */
struct
{
  unsigned char error;
  unsigned char pca9536;
  float         volt[8];
  signed long 	adc[8];
  unsigned int  daq[8];
  float         intemp[2];  // ADT7486A internal temperature [degree celsius]
  float         temp[4];    // ADT7486A external2 temperature [degree celsius]
} xdata user_data;

MSCB_INFO_VAR code vars[] = {
   1, UNIT_BYTE,       0, 0, 0,           "Error"  ,  &user_data.error,   // 0
   1, UNIT_BYTE,       0, 0, 0,           "MUX"  ,    &user_data.pca9536, // 1   
   4, UNIT_VOLT,       0, 0, MSCBF_FLOAT, "Volt0"  ,  &user_data.volt[0], // 2
   4, UNIT_VOLT,       0, 0, MSCBF_FLOAT, "Volt1"  ,  &user_data.volt[1], // 3
   4, UNIT_VOLT,       0, 0, MSCBF_FLOAT, "Volt2"  ,  &user_data.volt[2], // 4
   4, UNIT_VOLT,       0, 0, MSCBF_FLOAT, "Volt3"  ,  &user_data.volt[3], // 5
   4, UNIT_VOLT,       0, 0, MSCBF_FLOAT, "Volt4"  ,  &user_data.volt[4], // 6
   4, UNIT_VOLT,       0, 0, MSCBF_FLOAT, "Volt5"  ,  &user_data.volt[5], // 7
   4, UNIT_VOLT,       0, 0, MSCBF_FLOAT, "Volt6"  ,  &user_data.volt[6], // 8
   4, UNIT_VOLT,       0, 0, MSCBF_FLOAT, "Volt7"  ,  &user_data.volt[7], // 9
   4, UNIT_BYTE,       0, 0, 0,           "ADC0"   ,  &user_data.adc[0],  // 10
   4, UNIT_BYTE,       0, 0, 0,           "ADC1"   ,  &user_data.adc[1],  // 11
   4, UNIT_BYTE,       0, 0, 0,           "ADC2"   ,  &user_data.adc[2],  // 12
   4, UNIT_BYTE,       0, 0, 0,           "ADC3"   ,  &user_data.adc[3],  // 13
   4, UNIT_BYTE,       0, 0, 0,           "ADC4"   ,  &user_data.adc[4],  // 14
   4, UNIT_BYTE,       0, 0, 0,           "ADC5"   ,  &user_data.adc[5],  // 15
   4, UNIT_BYTE,       0, 0, 0,           "ADC6"   ,  &user_data.adc[6],  // 16
   4, UNIT_BYTE,       0, 0, 0,           "ADC7"   ,  &user_data.adc[7],  // 17
   2, UNIT_BYTE,       0, 0, 0,           "DAQ0"   ,  &user_data.daq[0],  // 18
   2, UNIT_BYTE,       0, 0, 0,           "DAQ1"   ,  &user_data.daq[1],  // 19
   2, UNIT_BYTE,       0, 0, 0,           "DAQ2"   ,  &user_data.daq[2],  // 20
   2, UNIT_BYTE,       0, 0, 0,           "DAQ3"   ,  &user_data.daq[3],  // 21
   2, UNIT_BYTE,       0, 0, 0,           "DAQ4"   ,  &user_data.daq[4],  // 22
   2, UNIT_BYTE,       0, 0, 0,           "DAQ5"   ,  &user_data.daq[5],  // 23
   2, UNIT_BYTE,       0, 0, 0,           "DAQ6"   ,  &user_data.daq[6],  // 24
   2, UNIT_BYTE,       0, 0, 0,           "DAQ7"   ,  &user_data.daq[7],  // 25
   4, UNIT_CELSIUS,    0, 0, MSCBF_FLOAT, "T0int",  &user_data.intemp[0], // 26
   4, UNIT_CELSIUS,    0, 0, MSCBF_FLOAT, "T1int",  &user_data.intemp[1], // 27
   4, UNIT_CELSIUS,    0, 0, MSCBF_FLOAT, "T00ext", &user_data.temp[0],   // 28
   4, UNIT_CELSIUS,    0, 0, MSCBF_FLOAT, "T01ext", &user_data.temp[1],   // 29
   4, UNIT_CELSIUS,    0, 0, MSCBF_FLOAT, "T10ext", &user_data.temp[2],   // 30
   4, UNIT_CELSIUS,    0, 0, MSCBF_FLOAT, "T11ext", &user_data.temp[3],   // 31
   0
};

MSCB_INFO_VAR *variables = vars;

char xdata flag = 0;

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

/* 8 data bits to LPT */
#define P1_DATA    P1
sbit LED_GREEN = LED_1;
sbit LED_RED   = LED_0;
sbit SST = MSCB_SST1;

sbit P1_0  = P1 ^ 0;    // Pin 13
sbit P1_1  = P1 ^ 1;    // Pin 25
sbit P1_2  = P1 ^ 2;    // Pin 12
sbit P1_3  = P1 ^ 3;    // Pin 24
sbit P1_4  = P1 ^ 4;    // Pin 11
sbit P1_5  = P1 ^ 5;    // Pin 23
sbit P1_6  = P1 ^ 6;    // Pin 10
sbit P1_7  = P1 ^ 7;    // Pin 22

sbit P2_0  = P2 ^ 0;    // Pin 09
sbit P2_1  = P2 ^ 1;    // Pin 21
sbit P2_2  = P2 ^ 2;    // Pin 08
sbit P2_3  = P2 ^ 3;    // Pin 20
sbit P2_4  = P2 ^ 4;    // Pin 19
sbit P2_5  = P2 ^ 5;    // Pin 06
sbit P0_1  = P0 ^ 1;    // Pin 18
sbit P0_2  = P0 ^ 2;    // Pin 05


// #pragma NOAREGS
void user_write(unsigned char index) reentrant;

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
  idata char i;

  /* Format the SVN and store this code SVN revision into the system */
  for (i=0;i<4;i++) {
  	 if (svn_rev_code[6+i] < 48) {
	    svn_rev_code[6+i] = '0';
    }
  }

   sys_info.svn_revision = (svn_rev_code[6]-'0')*1000+
                           (svn_rev_code[7]-'0')*100+
                           (svn_rev_code[8]-'0')*10+
                           (svn_rev_code[9]-'0');

  // Reference Voltage on P0.0 Enable
  REF0CN = 0x00;  // Int Bias off, Int Temp off, VREF input on

  //BS I am not sure
  // 0:analog 1:digital
  P0MDIN = 0xFF; // P0 all digital pins
  P1MDIN = 0xFF; // P1 all digital pins
  P2MDIN = 0x3F; // P2 all digital pins ( Ain: .6.7 for SST)
  P3MDIN = 0xFF; // P3 all digital pins

  //BS I am not sure
  // 0: open-drain, 1: push-pull
  P0MDOUT = 0x10; // P0 .4/Tx
  P1MDOUT = 0xFF; // P1 Push-Pull
  P2MDOUT = 0x3F; // P2 Push-Pull (OC:.6.7 for SST_IO, SST_REF)
  P3MDOUT = 0x10; // P3.4 (SST_DRV) 
	
  // Setting the cross bar	
  XBR0 |=0x04;	// Enable SMBus pins
  XBR1 |=0x40; // Enable XBAR
  P0SKIP = 0x0f; //Skipping P0.0 (VREF), P0.1 P0.2, P0.3  (TX_EN)
   
  // Comparator 1 Settings as P2.6, P2.7 are using in this case
  CPT1CN = 0x80; //Enable Comparator1 (functional, the one above is only for CrossBar)
  CPT1MX = 0x33; //Comparator1 MUX selection

 
  // Negative input is set to P2 ^ 7, and Positive input is set to P2 ^ 6
  // (P2 ^ 6 is the SST1_IO, so we want to compare SST1 with the threshold voltage
  // of !~0.8V on SST_REF (P2 ^ 7)
  CPT1MD = 0x02; //Comparator1 Mode Selection
  //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

  // Set SST_REF & SST_IO P2 ^ 6/7 to Open-Drain and Analog Input 
  // so that it accepts the ~650mV set by voltage divider.
  P2MDOUT &= 0x3F;
  P2MDIN  &= 0x3F;

  /* set-up / initialize circuit components (order is important) */
  //NW modified SST to support more than one line
  ADT7486A_Init(SST_LINE); //Temperature measurements related initialization
  
  #ifdef _LTC2489_
        LTC2489_Init();
  #endif 
    
  #ifdef _LTC2605_
		LTC2605_Init();
  #endif

  #ifdef _PCA9536_
        PCA9536_Init();
        PCA9536_WriteByte (PCA9536_ADD,CMD_CONFIG_REG,PCA_OUT);
  #endif 


  if (init) {
   }

  for (i=0;i<8;i++)
  		user_data.adc [i] = 0.0;
  rErr = 0x00;
  user_data.error = rErr;  
  user_data.pca9536 = 0;
  // Temperature initialization   
   user_data.intemp[0] = 0;
   user_data.intemp[1] = 0;
   memset (&user_data.temp[0], 0, sizeof(user_data.temp));
}

/*---- User write function -----------------------------------------*/

void user_write(unsigned char index) reentrant{
  
//  led_blink(0, 2, 250);    // Red
  
  // -- Index Bias Dac
  if ((index >= FIRST_DAQ) && (index < LAST_DAQ)) {
     BiasIndex = (index - FIRST_DAQ);
	  LTC2605_Flag = 1;
    }
  
  if (index == PCA_INDEX) 	
      PCA9536_Flag = 1;
  return;
}

/*---- User read function ------------------------------------------*/

unsigned char user_read(unsigned char index)
{
  if(index);

   return 0;
}

/*---- User function called vid CMD_USER command -------------------*/

unsigned char user_func(unsigned char *data_in, unsigned char *data_out)
{
   /* echo input data */
   data_out[0] = data_in[0];
   data_out[1] = data_in[1];
   return 2;
}


//
//----------------------------------------------------------------------------------
//Main user loop
//Read 6 ADT7486A SST diode Temperature 

/*---- User loop function ------------------------------------------*/
void user_loop(void)
{
  unsigned char xdata i,channel;
  float xdata volt1, volt2;
  signed long xdata result1, result2;
  static unsigned char xdata adcChannel = N_CHANNEL;
  
  if(adcChannel == N_CHANNEL) {
      adcChannel = CLEAR;
      LTC2489_StartConversion(ADDR_LTC2489[0], adcChannel);
      LTC2489_StartConversion(ADDR_LTC2489[1], adcChannel);
    } else {
      adcChannel++;
      adcChannel %= N_CHANNEL;
      if (!LTC2489_ReadConversion(ADDR_LTC2489[0], adcChannel, &result1)){
          result1 = -CONVER_FAC1;
          adc = 1;
      }
      if (!LTC2489_ReadConversion(ADDR_LTC2489[1], adcChannel, &result2)){
            result2 = -CONVER_FAC1;
            adc = 1;
      }
      volt1 = ((float)(result1 + CONVER_FAC1) * (float)(VREF /CONVER_FAC2));
      volt2 = ((float)(result2 + CONVER_FAC1) * (float)(VREF /CONVER_FAC2));
      channel = (adcChannel + (N_CHANNEL-1)) % N_CHANNEL;    
      DISABLE_INTERRUPTS;
        user_data.volt[channel]  = volt1;
        user_data.volt[channel+4]= volt2;
        user_data.adc[channel]   = result1;
        user_data.adc[channel+4] = result2;
      ENABLE_INTERRUPTS;
    }

  for (i=0;i<2;i++) {
      if (ADT7486A_Cmd(ADT7486A_addrArray[i], GetIntTemp, SST_LINE, &user_data.intemp[i])!= SUCCESS) //NW
            INTtemp = 1;
    }
  if (ADT7486A_Cmd(ADT7486A_addrArray[0], GetExt1Temp, SST_LINE, &user_data.temp[0])!= SUCCESS)  //NW
  		EXTtemp = 1;
  if(ADT7486A_Cmd(ADT7486A_addrArray[0], GetExt2Temp, SST_LINE, &user_data.temp[1])!= SUCCESS)  //NW
		EXTtemp = 1;
  if(ADT7486A_Cmd(ADT7486A_addrArray[1], GetExt1Temp, SST_LINE, &user_data.temp[2])!= SUCCESS)  //NW
  	   EXTtemp = 1;
  if(ADT7486A_Cmd(ADT7486A_addrArray[1], GetExt2Temp, SST_LINE, &user_data.temp[3]) != SUCCESS)  //NW
      EXTtemp = 1;      
 
#ifdef _LTC2605_
 if (LTC2605_Flag){
 	 LTC2605_WriteByte(DAQ_ADDRESS,((WREG_n_UPDATE_n << 4) | BiasIndex),user_data.daq[BiasIndex]);
	 LTC2605_Flag = 0;
 }
#endif

 #ifdef _PCA9536_
 if (PCA9536_Flag){
     PCA9536_WriteByte (PCA9536_ADD,CMD_OUTPUT_REG,user_data.pca9536);
     PCA9536_Flag = 0;
 }
#endif

 led_blink(1, 1, 250);

}
