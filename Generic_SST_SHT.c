 /********************************************************************\

  Name:         Generic_SST_SHT.c
  Created by:   Pierre
                    For FGD and CTM Temperature monitoring
?PR?UPGRADE?MSCBMAIN (0x3000)
  Contents:     General Application with the 310
                Midas Slow Control Bus protocol

  $Id$

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mscbemb.h"

#ifdef _HUMSEN_
#include "Devices/Humidity_sensor.h"
#define humsense 1
#endif

#ifdef _ADT7486A_
#include "Devices/ADT7486A_tsensor.h"
#endif

float avge(float temperature, char chan, int idx);

//Control definitions
extern unsigned long _systime;
// Node Name
char code node_name[] = "GENERIC310_SST";
char idata svn_rev_code[] = "$Rev$";

//NW SST
#define SST_LINE 1

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;
extern SYS_INFO sys_info;
unsigned char ADT7486A_addrArray[] = {0x48, 0x4a};

#ifdef _HUMSEN_
//Humidity variables
unsigned long xdata currentTime=0;
unsigned char xdata status, i1=0, i2=0, i3=0, i4=0;
float         xdata humidity, htemperature, atemp[4][30];
unsigned int  xdata rSHTtemp1, rSHThumi1, tcnt[4];
unsigned char xdata FCSorig1, FCSdevi1;
#endif

/*---- Define channels and configuration parameters returned to
       the CMD_GET_INFO command                                 ----*/

/* data buffer (mirrored in EEPROM) */
struct
{
  unsigned char status;
  unsigned char error;
  float  		 internal[2]; // ADT7486A internal temperature [degree celsius]
  char       ncount;
  float	   	 temp[8]; // ADT7486A external2 temperature [degree celsius]
  float			 SHTtemp;
  float  		 SHThumid;
  unsigned int	 rSHTtemp;
  unsigned int  rSHThumid;
  unsigned char FCSorig;
  unsigned char FCSdevi;
  float	ext01off, ext02off, ext11off, ext12off;
} xdata user_data;

MSCB_INFO_VAR code vars[] = {
   1, UNIT_BYTE,          0, 0,           0, "Status",   &user_data.status,      	 // 1
   1, UNIT_BYTE,          0, 0,           0, "ERROR",    &user_data.error,   	     // 2
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "IntTemp1", &user_data.internal[0], 	 // 5
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "IntTemp2", &user_data.internal[1], 	 // 6

   1, UNIT_BYTE,          0, 0,           0, "ncount",   &user_data.ncount,   	     // 2

   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "Temp1", 	 &user_data.temp[0],       // 7
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "Temp2", 	 &user_data.temp[1],       // 8
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "Temp3",		 &user_data.temp[2],       // 9
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "Temp4",		 &user_data.temp[3],       // 10


   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "AvgTemp1", 	 &user_data.temp[4],       // 7
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "AvgTemp2", 	 &user_data.temp[5],       // 8
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "AvgTemp3",   &user_data.temp[6],       // 9
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "AvgTemp4",	 &user_data.temp[7],       // 10
   4, UNIT_CELSIUS,       0, 0, MSCBF_FLOAT, "SHTtemp",   &user_data.SHTtemp,      // 11
   4, UNIT_PERCENT,       0, 0, MSCBF_FLOAT, "RHumid",    &user_data.SHThumid,     // 12

   2, UNIT_BYTE,          0, 0,           0, "rSHTtemp",  &user_data.rSHTtemp,     // 13
   2, UNIT_BYTE,          0, 0,           0, "rSHThumi",  &user_data.rSHThumid,    // 14
   1, UNIT_BYTE,          0, 0,           0, "FCSorig",   &user_data.FCSorig,   	 // 15
	 1, UNIT_BYTE,          0, 0,           0, "FCSdevi",   &user_data.FCSdevi,      // 16
   4, UNIT_BYTE,          0, 0, MSCBF_FLOAT, "ext01off",  &user_data.ext01off,     // 17
   4, UNIT_BYTE,          0, 0, MSCBF_FLOAT, "ext02off",  &user_data.ext02off,     // 18
   4, UNIT_BYTE,          0, 0, MSCBF_FLOAT, "ext11off",  &user_data.ext11off,     // 19
   4, UNIT_BYTE,          0, 0, MSCBF_FLOAT, "ext12off",  &user_data.ext12off,     // 20
	0
};

MSCB_INFO_VAR *variables = vars;

char flag = 0;

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
sbit RH_VCC = P1 ^ 2;
sbit P1_3  = P1 ^ 3;    // Pin 24
sbit P1_4  = P1 ^ 4;    // Pin 11
sbit RH_GND = P1 ^ 4;
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

sbit P1_OEn   = P0 ^ 7;
sbit P2_OEn   = P3 ^ 4;

// #pragma NOAREGS
void user_write(unsigned char index) reentrant;

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
  idata char i, j;
  char xdata add;
//  float temperature;
	
  if (init) {
    user_data.status = 0;
    user_data.error = 0x0;   // 33250A
	  user_data.SHTtemp = 0;
	  user_data.SHThumid = 0;
	  user_data.rSHTtemp = 0;
	  user_data.rSHThumid = 0;
	  user_data.FCSorig = 0;
	  user_data.FCSdevi = 0;
    user_data.ext01off =0;
    user_data.ext02off =0;
    user_data.ext11off =0;
    user_data.ext12off =0;
    user_data.ncount = 10;

//    sys_info.node_addr = 0x100;
 }

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
	add = cur_sub_addr();	

  // 0:analog 1:digital
  P0MDIN = 0xFF; // P0 all digital pins
  P1MDIN = 0xFF; // P1 all digital pins
  P2MDIN = 0x3F; // P2 all digital pins ( Ain: .6.7 for SST)
  P3MDIN = 0xFF; // P3 all digital pins

  // 0: open-drain, 1: push-pull
  P0MDOUT = 0x90; // P0 .7/P1_OEn .4/Tx
//  P1MDOUT = 0xFF; // P1 Push-Pull
  P2MDOUT = 0x3F; // P2 Push-Pull (OC:.6.7 for SST_IO, SST_REF)
  P3MDOUT = 0x18; // P3.3 (SST_DRV) P3 .4 (P2_OEn)

  // Reference Voltage on P0.0 Enable
  REF0CN = 0x00;  // Int Bias off, Int Temp off, VREF input on

  // Buffer Output Enable
  P1_OEn   = 0;
  P2_OEn   = 0;

  // Comparator 1 Settings as P2.6, P2.7 are using in this case
  CPT1CN = 0x80; //Enable Comparator1 (functional, the one above is only for CrossBar)

  CPT1MX = 0x33; //Comparator1 MUX selection
  //Negative input is set to P2 ^ 7, and Positive input is set to P2 ^ 6
  // (P2 ^ 6 is the SST1_IO, so we want to compare SST1 with the threshold voltage
  //of !~0.8V on SST_REF (P2 ^ 7)
  CPT1MD = 0x02; //Comparator1 Mode Selection
  //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

  // Set SST_REF & SST_IO P2 ^ 6/7 to Open-Drain and Analog Input 
  // so that it accepts the ~650mV set by voltage divider.
  P2MDOUT &= 0x3F;
  P2MDIN  &= 0x3F;

#ifdef _ADT7486A_
  /* set-up / initialize circuit components (order is important) */
  //NW modified SST to support more than one line
  ADT7486A_Init(SST_LINE); //Temperature measurements related initialization
/*
  delay_ms(400);
  ADT7486A_Cmd(ADT7486A_addrArray[0], SetExt1Offset
    , (user_data.ext01off>>8), user_data.ext01off
    , SST_LINE, &temperature);
    delay_ms(200);
  ADT7486A_Cmd(ADT7486A_addrArray[0], SetExt2Offset
    , (user_data.ext02off>>8), user_data.ext02off
    , SST_LINE, &temperature);
    delay_ms(200);
  ADT7486A_Cmd(ADT7486A_addrArray[1], SetExt1Offset
    , (user_data.ext11off>>8), user_data.ext11off
    , SST_LINE, &temperature);
    delay_ms(200);
  ADT7486A_Cmd(ADT7486A_addrArray[1], SetExt2Offset
    , (user_data.ext12off>>8), user_data.ext12off
    , SST_LINE, &temperature);
*/

  if (user_data.ncount > 30) user_data.ncount = 30;
   for (j=0;j<4;j++)
     for (i=0;i<30;i++) atemp[j][i] = -500;
#endif

//
//---------------------------------------------------------------
//Humidity sensor initialization
#ifdef _HUMSEN_
  P1MDOUT |= 0x15;  // P1.0/CLK(PP) P1.2/VCC(PP), P1.4/GND(PP), P1.6:DATA 10K PullUp (OD)
  RH_VCC = 1;
  RH_GND = 0;
  // Initializing the SHTxx communication
  HumiSensor_Init(humsense);
  // temporary humidity sensor
  user_data.SHTtemp = 0;
  user_data.SHThumid = 0;
#endif

  memset (&user_data.internal[0], 0, sizeof(user_data.internal));
  memset (&user_data.temp[0], 0, sizeof(user_data.temp));
  LED_RED = 0;
}

/*---- User write function -----------------------------------------*/
void user_write(unsigned char index) reentrant
{
  led_blink(0, 2, 250);    // Red
  if(index == 0) flag = 1;

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

//--------------------------------------------------------------------
float avge(float temperature, char chan, int idx) {
  int l, divider;
  float sumtemp;

  atemp[chan][idx++] = temperature;
  sumtemp =0;
  for (divider=0,l=0;l<user_data.ncount;l++) {
    if (atemp[chan][l] != -500.f) {
      sumtemp += atemp[chan][l];
      divider++;
    }
  }
  return (sumtemp / divider);
}

/*---- User loop function ------------------------------------------*/
void user_loop(void)
{
  char i;
  float xdata humidity, temperature, avgetemp;	
  unsigned char status;
  	
  user_data.status += 1;

#ifdef _ADT7486A_
//-----------------------------------------------------------------------------
//
  for (i=0;i<2;i++) {
      if(!ADT7486A_Cmd(ADT7486A_addrArray[i], GetIntTemp, SST_LINE, &temperature)){
         DISABLE_INTERRUPTS;
         user_data.internal[i] = temperature;
         ENABLE_INTERRUPTS;
      }
  }

   if(!ADT7486A_Cmd(ADT7486A_addrArray[0], GetExt1Temp, SST_LINE, &temperature)){
     i1 %= user_data.ncount;
     avgetemp = avge(temperature, 0, i1);
     i1++;
     DISABLE_INTERRUPTS;
     user_data.temp[0] = temperature + user_data.ext01off;
     user_data.temp[0+4] = avgetemp;
     ENABLE_INTERRUPTS;
   }
   if(!ADT7486A_Cmd(ADT7486A_addrArray[0], GetExt2Temp, SST_LINE, &temperature)){
     i2 %= user_data.ncount; avgetemp = avge(temperature, 1, i2);  i2++;
     DISABLE_INTERRUPTS;
     user_data.temp[1] = temperature + user_data.ext02off;
     user_data.temp[1+4] = avgetemp;
     ENABLE_INTERRUPTS;
   }
   if(!ADT7486A_Cmd(ADT7486A_addrArray[1], GetExt1Temp, SST_LINE, &temperature)){
     i3 %= user_data.ncount; avgetemp = avge(temperature, 2, i3);  i3++;
     DISABLE_INTERRUPTS;
     user_data.temp[2] = temperature + user_data.ext11off;
     user_data.temp[2+4] = avgetemp;
     ENABLE_INTERRUPTS;
   }
   if(!ADT7486A_Cmd(ADT7486A_addrArray[1], GetExt2Temp, SST_LINE, &temperature)){
     i4 %= user_data.ncount; avgetemp = avge(temperature, 3, i4);  i4++;
     DISABLE_INTERRUPTS;
     user_data.temp[3] = temperature + user_data.ext12off;
     user_data.temp[3+4] = avgetemp;
     ENABLE_INTERRUPTS;
   }
#endif

#ifdef _HUMSEN_
//-----------------------------------------------------------------------------
//
// Measuring the humidity and temperature
  if ((uptime() - currentTime) > 1)
   {
    status = HumidSensor_Cmd (&rSHThumi1
                             ,&rSHTtemp1
                             ,&humidity
                             ,&htemperature
                             ,&FCSorig1
                             ,&FCSdevi1
                             ,humsense);
    if (status == DONE){	 
      DISABLE_INTERRUPTS;
      user_data.SHThumid = humidity;
      user_data.SHTtemp = htemperature;
      user_data.FCSorig = FCSorig1;
      user_data.FCSdevi = FCSdevi1;
      ENABLE_INTERRUPTS;
    }
    currentTime = uptime();
  }
#endif 
  delay_ms(200);
  led_blink(1, 1, 100);
}
