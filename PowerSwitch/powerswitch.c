/********************************************************************\

Name:     powerswitch.c   
Author:   Pierre-André Amaudruz 
$Id$

// P0.0 ON6 pp
// P0.1 ON3 pp
// P0.2 ON4 pp
// P0.3 485_Enable pp
// P0.4 485 Tx pp
// P0.5 485 Rx od
// P0.6 ON1 pp 
// P0.0 ON2 pp

// P1.0 Vmon2 AMX0P 0x00 a
// P1.1 Vmon1 AMX0P 0x01 a
// P1.2 Vmon4 AMX0P 0x02 a
// P1.3 Vmon3 AMX0P 0x03 a
// P1.4 Vmon6 AMX0P 0x04 a
// P1.5 Vmon5 AMX0P 0x05 a  
// P1.6 Imon2 AMX0P 0x06 a
// P1.7 Imon1 AMX0P 0x07 a

// P2.0 Imon4   AMX0P 0x08 a 
// P2.1 Imon3   AMX0P 0x09 a 
// P2.2 Imon6   AMX0P 0x0a a
// P2.3 Imon5   AMX0P 0x0b a
// P2.4 VmonIN1 AMX0P 0x0c a
// P2.5 VmonIN2 AMX0P 0x0d a
// P2.6 SST_IO od
// P2.7 SST_REF o

// P3.0 LED2 od
// P3.1 ON5 pp
// P3.2 SST_DRV pp
// P3.3 Fault od
// P3.4 VmonIN3 AMPX0P 0x14 a

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mscbemb.h"

#include "powerswitch.h"
#include "Devices/adc_internal.h"

#ifdef _ADT7486A_
#include "Devices/ADT7486A_tsensor.h"
#endif

char code node_name[] = "PWRSWITCH";
char idata svn_rev_code[] = "$Rev$";

// declare number of sub-addresses to framework
unsigned char idata _n_sub_addr = 1;
extern SYS_INFO sys_info;

// defining the address
unsigned char xdata ADT7486A_addrArray[] = {0x48, 0x49, 0x4A};

#define VREF         3.293f

MSCB_INFO_VAR code vars[] = {
  1, UNIT_BYTE,     0, 0, 0,           "Shutdown"  , &user_data.error,         // 0
  1, UNIT_BYTE,     0, 0, 0,           "Control"   , &user_data.control,       // 1
  1, UNIT_BYTE,     0, 0, 0,           "Status"    , &user_data.status,        // 2 

  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "in Volt1"  , &user_data.value[0],     // 3 2.4 0x0c
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "in Volt2"  , &user_data.value[1],     // 4 2.5 0x0d
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "in Volt3"  , &user_data.value[2],     // 5 3.4 0x14

  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "outVolt1"  , &user_data.value[3],    // 6  1.1 0x01
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "outVolt2"  , &user_data.value[4],    // 7  1.0 0x00
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "outVolt3"  , &user_data.value[5],    // 8  1.3 0x03
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "outVolt4"  , &user_data.value[6],    // 9  1.2 0x02
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "outVolt5"  , &user_data.value[7],    // 10 1.5 0x05
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "outVolt6"  , &user_data.value[8],    // 11 1.4 0x04

  4, UNIT_AMPERE,     0, 0, MSCBF_FLOAT, "outcur1"  ,   &user_data.value[9],    // 12 1.7 0x07
  4, UNIT_AMPERE,     0, 0, MSCBF_FLOAT, "outCur2"  ,   &user_data.value[10],   // 13 1.6 0x06
  4, UNIT_AMPERE,     0, 0, MSCBF_FLOAT, "outCur3"  ,   &user_data.value[11],   // 14 2.1 0x09
  4, UNIT_AMPERE,     0, 0, MSCBF_FLOAT, "outCur4"  ,   &user_data.value[12],   // 15 2.0 0x08
  4, UNIT_AMPERE,     0, 0, MSCBF_FLOAT, "outCur5"  ,   &user_data.value[13],   // 16 2.3 0x0b
  4, UNIT_AMPERE,     0, 0, MSCBF_FLOAT, "outCur6"  ,   &user_data.value[14],   // 17 2.2 0x0a

  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "Temp1"  ,   &user_data.temperature[0], // 18
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "Temp2"  ,   &user_data.temperature[1], // 19 
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "Temp3"  ,   &user_data.temperature[2], // 20
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "Temp4"  ,   &user_data.temperature[3], // 21
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "Temp5"  ,   &user_data.temperature[4], // 22
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "Temp6"  ,   &user_data.temperature[5], // 23

  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "inTemp1"  ,   &user_data.localtemp[0], // 24
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "inTemp2"  ,   &user_data.localtemp[1], // 25
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "inTemp3"  ,   &user_data.localtemp[2], // 26

  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "LTemp1"  ,   &user_data.limit[0], // 27
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "LTemp2"  ,   &user_data.limit[1], // 28 
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "LTemp3"  ,   &user_data.limit[2], // 29
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "LTemp4"  ,   &user_data.limit[3], // 30
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "LTemp5"  ,   &user_data.limit[4], // 31
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "LTemp6"  ,   &user_data.limit[5], // 32

  0
};

MSCB_INFO_VAR *variables = vars;

/********************************************************************\

Application specific init and inout/output routines

\********************************************************************/

sbit LED_RED   = LED_0;
sbit SST = MSCB_SST1;

sbit ON6 = P0 ^ 0;    // Pin 13
sbit ON3 = P0 ^ 1;    // Pin 25
sbit ON4 = P0 ^ 2;    // Pin 12
sbit ON5 = P3 ^ 1;    // Pin 24
sbit ON1 = P0 ^ 6;    // Pin 11
sbit ON2 = P0 ^ 7;    // Pin 23
sbit SHDWN = P3 ^ 3;   // Pin  9

// #pragma NOAREGS
void user_write(unsigned char index) reentrant;
float read_voltage(unsigned char channel, float coeff, float offset);

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

  if (init) {
    user_data.error = 0;
    user_data.control = 0;
    user_data.status = 0;
    sys_info.node_addr = cur_sub_addr();
    for (i=0;i<6;i++)
     user_data.limit[i] = 50.0;
  }

  // 0: open-drain, 1: push-pull
  P0MDOUT = 0xFF; // P0 .4/Tx Rx?
  // 0: Analog, 1: digital
  // Configure port as Analog input port
  P1MDIN = 0x00;   // Analog
  P2MDIN = 0xC0;   // SST_IO/REF digital, rest analog
  P3MDIN = 0x0F;   // Set P3.4 to analog, rest digital
  P3MDOUT = 0x06; // Fault(od) SST_DRV(pp) ON5(pp) LED2(pp)

  // Setting the cross bar
  //  XBR1 |=0x40;   // Enable XBAR
  //  P0SKIP = 0x0f; //Skipping P0.0 (VREF), P0.1 P0.2, P0.3  (TX_EN)

  // Comparator 1 Settings as P2.6, P2.7 are using in this case
  CPT1CN = 0x80; //Enable Comparator1 (functional, the one above is only for CrossBar)
  CPT1MX = 0x33; //Comparator1 MUX selection
  // (P2 ^ 6 is the SST1_IO, so we want to compare SST1 with the threshold voltage
  //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)
  // of !~0.8V on SST_REF (P2 ^ 7)
  CPT1MD = 0x02; //Comparator1 Mode Selection
 
  // VREF selection
//   REF0CN = 0xA;

  // Init internal ADC
  adc_internal_init();

#ifdef _ADT7486A_
  ADT7486A_Init(SST_LINE1); //Temperature measurements related initialization
#endif

  // Initialize variables
  for (i=0;i<N_POWER_CHANNELS;i++) 
    user_data.value[i] = 0;
  for (i=0;i<6;i++) 
    user_data.temperature[i] = 0.0;
  for (i=0;i<3;i++) 
    user_data.temperature[i] = 0.0;

   user_data.control = 0;
   user_data.status  = 0;
   user_data.error   = 0;
}

/*---- User write function -----------------------------------------*/

void user_write(unsigned char index) reentrant
{
  if (index==1) {
    ON2 = !(user_data.control & 0x1);
    ON1 = !(user_data.control & 0x2);
    ON4 = !(user_data.control & 0x4);
    ON3 = !(user_data.control & 0x8);
    ON6 = !(user_data.control & 0x10);
    ON5 = !(user_data.control & 0x20);
    user_data.status = user_data.control;
    user_data.control = 0;
    user_data.error   = 0;
    rESR = 0;
  }
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

//-----------------------------------------------------------------------------
//
float read_voltage(unsigned char channel, float coeff, float offset)
{
  unsigned int  xdata i, rvalue;
  float         xdata voltage;
  unsigned int  xdata rawbin;
  unsigned long xdata rawsum = 0;

  // Averaging on 10 measurements for now.
  for (i=0 ; i<10 ; i++) {
    rawbin = adc_read(channel);
    rawsum += rawbin;
    yield();
  }

  /* convert to V */
  rvalue =  rawsum/10;
  voltage = (float)  rvalue;                  // averaging
  voltage = (float)  voltage / 1024.0 * VREF;  // conversion
  if ( channel != TCHANNEL)
    voltage = voltage * coeff + offset;

  return voltage;
}

//
//----------------------------------------------------------------------------------
//Main user loop
//Read 6 ADT7486A SST diode Temperature
//Read 15 voltages

unsigned char code adc_index[] = {0xc, 0xd, 0x14, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb};
float code            coeff[]  = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 5.00, 5.00, 5.00, 5.00, 5.00, 5.00}; 
float code           offset[]  = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

/*---- User loop function ------------------------------------------*/
void user_loop(void)
{
  unsigned i, j, channel;
  float xdata value[15];

#ifdef _ADT7486A_
// External Temperature reading
  for (i=0 , j=0 ; j<3 ; i+=2, j++) {
    ADT7486A_Cmd(ADT7486A_addrArray[j], GetExt1Temp, SST_LINE1, &user_data.temperature[i+0]);
    ADT7486A_Cmd(ADT7486A_addrArray[j], GetExt2Temp, SST_LINE1, &user_data.temperature[i+1]);
  }

  for (i=0;i<6;i++) {
    if ((user_data.temperature[i] > user_data.limit[i]) && (user_data.temperature[i] < 200.))
        rESR |= (1<<(i+2));
  }
#endif

#ifdef _ADT7486A_
// Internal Temperature reading
  for (i=0;i<3;i++) {
    ADT7486A_Cmd(ADT7486A_addrArray[i], GetIntTemp, SST_LINE1, &user_data.localtemp[i]);
  }
#endif

// Read All ADCs
  for (channel=0 ; channel<N_POWER_CHANNELS ; channel++) {
    value[channel] = read_voltage(adc_index[channel], coeff[channel], offset[channel]);
   }
// Publish data
  DISABLE_INTERRUPTS;
  for (channel=0 ; channel<=N_POWER_CHANNELS ; channel++) {
    user_data.value[channel] = value[channel];
   }
  ENABLE_INTERRUPTS;

// Report hardware shutdown 
  rESR |= !SHDWN;

// Publish error status
  DISABLE_INTERRUPTS;
  user_data.error = rESR; 
  ENABLE_INTERRUPTS;

// Shutdown board if temperature too high
  if (rESR) {
    ON1 = ON2 = ON3 = ON4 = ON5 = ON6 = 1;
    DISABLE_INTERRUPTS;
    user_data.status = 0;
    ENABLE_INTERRUPTS;
  }
 
}
