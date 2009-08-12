/********************************************************************\
Name:         lpb.c
Created by:   Noel Wu
Created on:   August/6/2008

Contents:     Application specific (user) part of
              Midas Slow Control for LPB board.
              Board Revision 2 (Mike)

// P3.7:A7       .6:A6        .5:A5       .4:A4      | .3:A3      .2:A2       .1:A1      .0:A0 
// P2.7:+1.8En   .6:+3.3En    .5:+5En     .4:SPIMOSI | .3:SPISCK  .2:RAMHLDn  .1:SPIMISO .0:RAMWP
// P1.7:NC       .6:+6ddFlag  .5:R/HClock .4:R/HData | .3:+6ddEN  .2:RAMCS    .1:NC      .0:SST_DRV 
// P0.7:DEL_CLK  .6:DEL_DATA  .5:485TXEN  .4:NC      | .3:DELCS1  .2:DELCS2   .1:Rx      .0:Tx 

Program Size: data=165.1 xdata=482 code=17448  Aug 04/2009

$Id$
\********************************************************************/

#include <stdio.h>
#include <math.h>
#include "mscbemb.h"
#include "LPB.h"

#include "Devices/adc_internal.h"

#ifdef _HUMSEN_
#include "Devices/Humidity_sensor.h"
#endif

#ifdef   _ADT7486A_
#include "Devices/ADT7486A_tsensor.h"
#endif

#ifdef   _LTC2620_
#include "Devices/LTC2620.h"
#endif

#ifdef _ExtEEPROM_
#include "Devices/ExtEEPROM.h"
#endif
//
// Global declarations
//-----------------------------------------------------------------------------
char code  node_name[] = "LPB";
char idata svn_rev_code[] = "$Rev$";

//
// Declare globally the number of sub-addresses to framework
unsigned char idata _n_sub_addr = 1;

// Local flag
// Global bit register
unsigned char bdata bChange;

// Local flag
sbit bCPupdoitNOW   = bChange ^ 0;
sbit bDacdoitNOW    = bChange ^ 1;

//Defining the user_write() flag actions
sbit LTC2620_Flag   = bChange ^ 3;
sbit EEP_CTR_Flag   = bChange ^ 4;

sbit SPI_SCK   = MSCB_SPI_SCK;     // SPI Protocol Serial Clock 
sbit SPI_MOSI  = MSCB_SPI_MOSI;    // SPI Protocol Master Out Slave In

unsigned char channel;
unsigned long xdata humidTime=0, currentTime=0;

//Humidity variables
unsigned char xdata status;
float         xdata humidity, htemperature;
unsigned int  xdata rSHTtemp1, rSHThumi1;
unsigned char xdata FCSorig1, FCSdevi1;

//EEPROM variables
float* xdata eep_address;
static unsigned char tcounter;
static unsigned char xdata eeprom_flag=CLEAR;
unsigned char xdata eeprom_wstatus, eeprom_rstatus;
static unsigned int  xdata eeptemp_addr;
unsigned char xdata * eeptemp_source;
unsigned char xdata eep_request;

// User Data structure declaration
//-----------------------------------------------------------------------------
MSCB_INFO_VAR code vars[] = {
  4, UNIT_BYTE,      0, 0,           0, "SerialN",               &user_data.SerialN,   // 0
  2, UNIT_BYTE,      0, 0,           0, "Error",                 &user_data.error,     // 1
  1, UNIT_BYTE,      0, 0,           0, "Control",               &user_data.control,   // 2
  1, UNIT_BYTE,      0, 0,           0, "Status",                &user_data.status,    // 3 
  1, UNIT_BYTE,      0, 0,           0, "eepage",                &user_data.eepage,    // 4
  1, UNIT_BYTE,      0, 0,           0, "Delay",                 &user_data.spare,     // 5
  4, UNIT_VOLT,      0, 0, MSCBF_FLOAT, "D2Ashft" ,              &user_data.iadc[0],   // 6
  4, UNIT_VOLT,      0, 0, MSCBF_FLOAT, "pDVsc",                 &user_data.iadc[1],   // 7
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDIsc",                 &user_data.iadc[2],   // 8
  4, UNIT_VOLT,      0, 0, MSCBF_FLOAT, "pDV6",                  &user_data.iadc[3],   // 9
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDI6",                  &user_data.iadc[4],   // 10
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDI5",                  &user_data.iadc[5],   // 11
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDI33",                 &user_data.iadc[6],   // 12
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDI18",                 &user_data.iadc[7],   // 13
  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "uCTemp",                &user_data.uCTemp,    // 14

  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "IntTemp",               &user_data.IntTemp,   // 15
  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "Temp58",                &user_data.Temp58,    // 16
  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "Temp33",                &user_data.Temp33,    // 17

  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "SHTtemp",               &user_data.SHTtemp,   // 18
  4, UNIT_PERCENT,   0, 0, MSCBF_FLOAT, "RHumid",                &user_data.SHThumid,  // 19

  2, UNIT_BYTE,      0, 0,           0, "rdac0",                 &user_data.rdac[0],   // 20
  2, UNIT_BYTE,      0, 0,           0, "rdac4",                 &user_data.rdac[1],   // 21
  2, UNIT_BYTE,      0, 0,           0, "rdac8",                 &user_data.rdac[2],   // 22
  2, UNIT_BYTE,      0, 0,           0, "rdac12",                &user_data.rdac[3],   // 23
  2, UNIT_BYTE,      0, 0,           0, "rdac16",                &user_data.rdac[4],   // 24
  2, UNIT_BYTE,      0, 0,           0, "rdac20",                &user_data.rdac[5],   // 25
  2, UNIT_BYTE,      0, 0,           0, "rdac24",                &user_data.rdac[6],   // 26
  2, UNIT_BYTE,      0, 0,           0, "rdac28",                &user_data.rdac[7],   // 27
  2, UNIT_BYTE,      0, 0,           0, "rdac32",                &user_data.rdac[8],   // 28
  2, UNIT_BYTE,      0, 0,           0, "rdac36",                &user_data.rdac[9],   // 29
  2, UNIT_BYTE,      0, 0,           0, "rdac40",                &user_data.rdac[10],  // 30
  2, UNIT_BYTE,      0, 0,           0, "rdac44",                &user_data.rdac[11],  // 31
  2, UNIT_BYTE,      0, 0,           0, "rdac48",                &user_data.rdac[12],  // 32
  2, UNIT_BYTE,      0, 0,           0, "rdac52",                &user_data.rdac[13],  // 33
  2, UNIT_BYTE,      0, 0,           0, "rdac56",                &user_data.rdac[14],  // 34
  2, UNIT_BYTE,      0, 0,           0, "rdacsp",                &user_data.rdac[15],  // 35

  2, UNIT_BYTE,      0, 0,           0, "rD2Ashft",              &user_data.riadc[0],  // 36
  2, UNIT_BYTE,      0, 0,           0, "rpDVsc",                &user_data.riadc[1],  // 37
  2, UNIT_BYTE,      0, 0,           0, "rpDIsc",                &user_data.riadc[2],  // 38
  2, UNIT_BYTE,      0, 0,           0, "rpDV6",                 &user_data.riadc[3],  // 39  
  2, UNIT_BYTE,      0, 0,           0, "rpDI6",                 &user_data.riadc[4],  // 40
  2, UNIT_BYTE,      0, 0,           0, "rpDI5",                 &user_data.riadc[5],  // 41
  2, UNIT_BYTE,      0, 0,           0, "rpDI33",                &user_data.riadc[6],  // 42
  2, UNIT_BYTE,      0, 0,           0, "rpDI18",                &user_data.riadc[7],  // 43
  2, UNIT_BYTE,      0, 0,           0, "rSHTemp",               &user_data.rSHTemp,   // 44
  2, UNIT_BYTE,      0, 0,           0, "rRHumid",               &user_data.rSHhumid,  // 45

  1, UNIT_BYTE,      0, 0,           0, "watchdog",              &user_data.spare1,    // 46
  2, UNIT_BYTE,      0, 0,           0, "spare2",                &user_data.spare2,    // 47

  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "eepValue",              &user_data.eepValue,  // 48
  4, UNIT_BYTE,      0, 0,           0, "eeCtrSet",              &user_data.eeCtrSet,  // 49
  0
};

MSCB_INFO_VAR *variables = vars;   // Structure mapping
// Get sysinfo if necessary
extern SYS_INFO sys_info;          // For address setting

//-----------------------------------------------------------------------------
//
void publishCtlCsr(void) {
  DISABLE_INTERRUPTS;
  user_data.control = rCTL;
  user_data.status  = rCSR;
  ENABLE_INTERRUPTS;
}

//-----------------------------------------------------------------------------
//
void publishErr(bit errbit) {
    DISABLE_INTERRUPTS;
    errbit = ON;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
}

//-----------------------------------------------------------------------------
//
void publishAll() {
  user_data.control = rCTL;
  user_data.status  = rCSR;
  user_data.error   = rESR;
  ENABLE_INTERRUPTS;
}

//-----------------------------------------------------------------------------
//
void PublishVariable(float xdata * pvarDest, float varSrce, bit errbit) {
  DISABLE_INTERRUPTS;
  *pvarDest = varSrce;
  if (errbit) user_data.error = rESR;
  ENABLE_INTERRUPTS;
}

//-----------------------------------------------------------------------------
//
float read_voltage(unsigned char channel,unsigned int *rvalue, unsigned char gain)
{
  unsigned int  xdata i;
  float         xdata voltage;
  unsigned int  xdata rawbin;
  unsigned long xdata rawsum = 0;

  // Averaging on 10 measurements for now.
  for (i=0 ; i<10 ; i++) {
    rawbin = adc_read(channel, gain);
    rawsum += rawbin;
    yield();
  }

  /* convert to V */
  *rvalue =  rawsum/10;
  voltage = (float)  *rvalue;                  // averaging
  voltage = (float)  voltage / 1024.0 * VREF;  // conversion
  if ( channel != TCHANNEL)
    voltage = voltage * coeff[channel] + offset[channel];

  return voltage;
}


//-----------------------------------------------------------------------------
//
void switchonoff(unsigned char command)
{
  char xdata i;

  if(command==ON)
  {
    SFRPAGE  = CONFIG_PAGE;
    VCC_EN = ON;
    VREG_5 = VREG_3 = VREG_1 = ON;
    P0MDOUT |= 0xCC;   // DACs_CS + DELAY_x PP

    // Activate 16 dac settings
    // user_data contains correct values, force update 
    // by setting mirror to 0xff 
    for(i=0;i<16;i++) ltc2620mirror[i] = 0x001;

    // Needs power for DAC update, by now it should be ON
    // Force DAC updates
    LTC2620_Flag = ON;

  } else if(command==OFF) {
    // Switch all the ports to open drain except for...
    SFRPAGE  = CONFIG_PAGE;
    VCC_EN = OFF;
    VREG_5 = VREG_3 = VREG_1 = OFF;

// P3.7:A7       .6:A6        .5:A5       .4:A4      | .3:A3      .2:A2       .1:A1      .0:A0 
// P2.7:+1.8En   .6:+3.3En    .5:+5En     .4:SPIMOSI | .3:SPISCK  .2:RAMHLDn  .1:SPIMISO .0:RAMWP
// P1.7:NC       .6:+6ddFlag  .5:R/HClock .4:R/HData | .3:+6ddEN  .2:RAMCS    .1:NC      .0:SST_DRV 
// P0.7:DEL_CLK  .6:DEL_DATA  .5:485TXEN  .4:NC      | .3:DELCS1  .2:DELCS2   .1:Rx      .0:Tx 
    P0MDOUT &= ~0xCC;   // // DACs_CS + DELAY_x OD

  }
}

/********************************************************************\
Application specific init and in/output routines
\********************************************************************/
/*---- User init function ------------------------------------------*/
void user_init(unsigned char init)
{
//  float xdata temperature;
  unsigned char xdata i, pca_add=0;
  unsigned int xdata crate_add=0, board_address=0;
  if(init) {
     crate_add = cur_sub_addr();
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

  sys_info.group_addr  = 500;

  user_data.IntTemp = 0;
  user_data.Temp58 = 0;
  user_data.Temp33 = 0;
  user_data.error = 0;
  user_data.SerialN = 0x0;
  user_data.control = 0;
  user_data.status = 0;
  user_data.spare1=0;
  user_data.spare2=0;
  rCTL = 0;
  rCSR = 0;
  LTC2620_Flag = 0;
  EEP_CTR_Flag = 0;

  //
  // Initial setting for communication and overall ports (if needed).
  //-----------------------------------------------------------------------------
  SFRPAGE  = CONFIG_PAGE;
  // P0MDOUT already contains Tx in push-pull
  P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull
  // Disable UART1
  XBR2 &= ~0x04;
  //
  // uC Miscellaneous ADCs (V/I Monitoring)
  //-----------------------------------------------------------------------------
  adc_internal_init();

  // Address decoding
//  P3MDOUT  = 0x00; // Crate address in OD
//  P3 = 0xFF;

  //
  // SST Temperatures
  //-----------------------------------------------------------------------------
#ifdef _ADT7486A_
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x01;        // SST_DRV PP
  SFRPAGE  = CPT1_PAGE;
  CPT1CN  |= 0x80;        // Enable the Comparator 1
  CPT1MD   = 0x03;        //Comparator1 Mode Selection
  //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

  ADT7486A_Init(SST_LINE1);
#endif

//---------------------------------------------------------------
//
// SPI bus
  SFRPAGE  = CONFIG_PAGE;
  P2 = 2;                 // Enable READ on MISO
  P2MDOUT |= 0x18;        // SPI_MOSI, SPI_SCK  PP
  SPI_SCK  = 0;
  SPI_MOSI = 0;

//---------------------------------------------------------------
//
// EEPROM access
#ifdef _ExtEEPROM_
  SFRPAGE  = CONFIG_PAGE;
  P2MDOUT |= 0x01;        // RAM_WPn PP
  P2MDOUT |= 0x04;        // RAM_HOLD PP
  RAM_HLDn = 1;           // 
  P1MDOUT |= 0x04;        // RAM_CSn PP

  ExtEEPROM_Init();

  // Read only the serial number from the protected page
  ExtEEPROM_Read(SERIALN_ADD,(unsigned char*)&eepage.SerialN, SERIALN_LENGTH);
  user_data.SerialN = eepage.SerialN;

  // Read all other settings from page 0(non-protected)
  ExtEEPROM_Read(PageAddr[0],(unsigned char*)&eepage, PAGE_SIZE);
  DISABLE_INTERRUPTS;

#endif

//---------------------------------------------------------------
//
//Humidity sensor initialization
#ifdef _HUMSEN_
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x20;        // R/HClock, R/HData (OD)
  // Initializing the SHTxx communication
  HumiSensor_Init(humsense);
  //temporary humidity sensor
  user_data.SHTtemp = 0;
  user_data.SHThumid = 0;
  user_data.rSHTemp = 0;
  user_data.rSHhumid = 0;
#endif

//---------------------------------------------------------------
//
// Configure and read the address
// C C C C C C 1 0 1 is the MSCB Addr[8..0], 9 bits
// Modifying what the board reads from the Pins
  SFRPAGE  = CONFIG_PAGE;
  P3MDOUT |= 0xFF; // PushPull for the address
  pca_add= P3;
  board_address= (((~pca_add)<<1) & 0x01F8)| 0x0005;
  sys_info.node_addr   = board_address;

#ifdef _ADT7486A_
//---------------------------------------------------------------
//
// SST Temperatures, setting temperature offsets
/*
  ADT7486A_Cmd( ADT7486A_ADDR0, SetExt1Offset
             , ((int)eepage.sstOffset[0]>>8)
             ,  (int)eepage.sstOffset[0]
             , SST_LINE1
             , &temperature); //NW

  ADT7486A_Cmd( ADT7486A_ADDR0, SetExt2Offset
             , ((int)eepage.sstOffset[0]>>8)
             ,  (int)eepage.sstOffset[0]
             , SST_LINE1
             , &temperature); //NW
*/
#endif

//---------------------------------------------------------------
//
// Dacs LTC2620 with mirror 
  for(i=0;i<16;i++) {
    user_data.rdac[i] = eepage.rdac[i];
    ltc2620mirror[i] = eepage.rdac[i];
  }

//---------------------------------------------------------------
//
// Main switch PP
  SFRPAGE  = CONFIG_PAGE;
  VREG_5 = VREG_3 = VREG_1 = OFF;
  VCC_EN = OFF;
  P1MDOUT |= 0x08;       // VCC_EN PP
  P1MDOUT &= ~0x40;      // VCC_Flag OD
  P2MDOUT |= 0xE0;       // +1.8, +3.3, +5 Enable PP
  switchonoff(OFF);

#ifdef _LTC2620_
//---------------------------------------------------------------
//
// LED DAC (2 Dacs chip=1 & chip=2)
  LTC2620_Init(1);
  LTC2620_Init(2);
#endif

  // Watchdog debug
  user_data.spare1 = 0;

//---------------------------------------------------------------
//
// Delay setting
  SFRPAGE  = CONFIG_PAGE;
  P0MDOUT |= 0xC0;       // Delay PP
  DELAY_0 = DELAY_1 = 0;
} // End of Init()


/*---- User write function -----------------------------------------*/
#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
  // LED Amplitude DAC
  if((index >= IDXDAC) && (index < IDXDAC+16))
    LTC2620_Flag = SET;

  // Control
  if(index==IDXCTL)
    rCTL = user_data.control;

  // Delay
  if (index==IDXDELAY) {
    DELAY_0 =  user_data.spare & 0x1;
    DELAY_1 = (user_data.spare & 0x2 >> 1);
  }  

  // EEPROM command
  if (index == IDXEEP_CTL)
    EEP_CTR_Flag = SET;
}

/*---- User read function ------------------------------------------*/
unsigned char user_read(unsigned char index)
{
  if (index);
  return 0;
}

/*---- User function called via CMD_USER command -------------------*/
unsigned char user_func(unsigned char *data_in, unsigned char *data_out)
{
  /* echo input data */
  data_out[0] = data_in[0];
  data_out[1] = data_in[1];
  return 2;
}

//-----------------------------------------------------------------------------
//
// Main user loop
void user_loop(void) {

  float xdata volt, temperature;
  unsigned int xdata rvolt, i;
  unsigned char xdata eeprom_channel, ltc2620_idx, ltc2620_chip, ltc2620_chan;
  float* xdata pfData;
  unsigned long xdata mask;
  unsigned int xdata *rpfData;
 
  //-----------------------------------------------------------------------------
  //
  // Watchdog
    DISABLE_INTERRUPTS;
    user_data.spare1++;
    ENABLE_INTERRUPTS;

  //-----------------------------------------------------------------------------
  // Power Up based on CTL bit
  if (CPup) {
    rCSR = user_data.status;
    rESR = 0x0000;   // Reset error status at each Power UP
    // Clear Status
    SsS = OFF;
    DISABLE_INTERRUPTS;
    user_data.status  = rCSR;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
    // Power up Card
    switchonoff(ON);
    delay_ms(1000);
    // Force Check on Voltage during loop
    bCPupdoitNOW = ON;
    // Wait for Check before setting SPup
    // Reset Action
    CPup = CLEAR;  // rCTL not updated yet, publish state after U/I check
  } // Power Up

  //-----------------------------------------------------------------------------
  // Set Manual Shutdown based on Index
  if (CmSd) {
    rCSR = user_data.status;
    SmSd = ON;             // Set Manual Shutdown
    switchonoff(OFF);
    SPup = OFF;            // Clear Qpump and card status
    CmSd = CLEAR;          // Reset action
    publishCtlCsr();       // Publish Registers state
  } // Manual Shutdown

  //-----------------------------------------------------------------------------
  //
  // LED amplitude DAC
#ifdef _LTC2620_
  // Get current status of the Card, don't update DAC in case of no Power, keep flag up
  rCSR = user_data.status;    
  if(SPup && LTC2620_Flag) {
    for(ltc2620_idx=0; ltc2620_idx < 16; ltc2620_idx++) {
      if(bDacdoitNOW || (ltc2620mirror[ltc2620_idx]!= user_data.rdac[ltc2620_idx])) {
        ltc2620_chip = (ltc2620_idx > 7) ? 2 : 1;
        ltc2620_chan = (ltc2620_chip == 2) ? ltc2620_idx - 8: ltc2620_idx;
        LTC2620_Cmd(WriteTo_Update, ltc2620_chip, ltc2620_chan, user_data.rdac[ltc2620_idx]);
        ltc2620mirror[ltc2620_idx] = user_data.rdac[ltc2620_idx];
      }
    }
    LTC2620_Flag = CLEAR;
  }
#endif

#ifdef _HUMSEN_
  //-----------------------------------------------------------------------------
  //
  // Measuring the humidity and temperature every 5 sec
  if ((uptime() - humidTime) > 2) {
    status = HumidSensor_Cmd (&rSHThumi1
                             ,&rSHTtemp1
                             ,&humidity
                             ,&htemperature
                             ,&FCSorig1
                             ,&FCSdevi1
                             ,humsense);
    if (status == DONE){	 
      DISABLE_INTERRUPTS;
      user_data.rSHTemp  = rSHTtemp1;
      user_data.rSHhumid = rSHThumi1;
      user_data.SHThumid = humidity;
      user_data.SHTtemp  = htemperature;
      ENABLE_INTERRUPTS;
    }
    humidTime = uptime();
  }
#endif 

#ifdef _ADT7486A_
  //-----------------------------------------------------------------------------
  // Local board temperature reading Internal SST and 2 Ext SST on board
  if(!ADT7486A_Cmd(ADT7486A_ADDR0, GetIntTemp, SST_LINE1, &temperature)) {
    RdssT = CLEAR;
    if ((temperature < eepage.luCTlimit) || (temperature > eepage.uuCTlimit)) pcbssTT = ON;
    PublishVariable(&(user_data.IntTemp), temperature, pcbssTT);
  } else publishErr(RdssT);

  if(!ADT7486A_Cmd(ADT7486A_ADDR0, GetExt1Temp, SST_LINE1, &temperature)) {
    RdssT = CLEAR;
    if ((temperature < eepage.lSSTlimit) || (temperature > eepage.uSSTlimit)) pcbssTT = ON;
    PublishVariable(&(user_data.Temp58), temperature, pcbssTT);
  } else publishErr(RdssT);

  if(!ADT7486A_Cmd(ADT7486A_ADDR0, GetExt2Temp, SST_LINE1, &temperature)) {
    RdssT = CLEAR;
    if ((temperature < eepage.lSSTlimit) || (temperature > eepage.uSSTlimit)) pcbssTT = ON;
    PublishVariable(&(user_data.Temp33), temperature, pcbssTT);
  } else publishErr(RdssT);
#endif

  //-----------------------------------------------------------------------------
  //
  // read Vdd switch Current Fault
  V6Fault = (V6ddFlag == 1) ? 0 : 1;   // +6Vdd Fault flag

  //-----------------------------------------------------------------------------
  //
  // Periodic check and After power up
  // uCtemperature, Voltage and Current readout
  if ( bCPupdoitNOW || ((uptime() - currentTime) > 1)) {
    //-----------------------------------------------------------------------------
    // uC Temperature reading/monitoring based on time
    // Read uC temperature, set error if temp out of range
    volt = read_voltage(TCHANNEL, &rvolt, IntGAIN1 );
    /* convert to deg. C */
    temperature = 1000 * (volt - 0.776) / 2.86;   // Needs calibration
    /* strip to 0.1 digits */
    temperature = ((int) (temperature * 10 + 0.5)) / 10.0;
    if ((temperature < eepage.luCTlimit) || (temperature > eepage.uuCTlimit)) uCT = ON; // out of range
    PublishVariable(&(user_data.uCTemp), temperature, uCT);

    //-----------------------------------------------------------------------------
    // Internal ADCs monitoring Voltages and Currents based on time
    // All U/I set error if value out of range
    pfData  = &(user_data.iadc);
    rpfData = &(user_data.riadc);
    for (channel=0 ; channel<INTERNAL_N_CHN ; channel++) {
      volt = read_voltage(channel, &rvolt, IntGAIN1);
      DISABLE_INTERRUPTS;
      pfData[channel] = volt;
      rpfData[channel] = rvolt;
      ENABLE_INTERRUPTS;
      mask = (1<<channel); /// Error mask
      if ((volt < eepage.lVIlimit[channel]) || (volt > eepage.uVIlimit[channel]))  rESR |= mask;
      else rESR &= ~mask;   // in range
    }  // U/I loop

    // U/I error not yet published ...
    // update time for next loop
    currentTime = uptime();
  }  // Voltage, Current & Temperature Check 

  //-----------------------------------------------------------------------------
  // Finally take action based on ERROR register
  if (rESR & (VOLTAGE_MASK | UCTEMPERATURE_MASK | BTEMPERATURE_MASK | MAIN_CURRENT_MASK)) {
    SPup = OFF;
    switchonoff(OFF);
    SsS = ON;
  } else if (bCPupdoitNOW) {
    bCPupdoitNOW = OFF;   // Reset flag coming from PowerUp sequence
    SsS = SmSd = OFF;
    SPup = bDacdoitNOW = ON; // Set status and enable DACs writing 
  }

  // Publish Control, Error and Status for all the bdoitNOW actions.
  publishAll();

#ifdef _ExtEEPROM_
  //-----------------------------------------------------------------------------
  //
  // Checking the eeprom control flag
  if (EEP_CTR_Flag) {
    //Checking for the special instruction
    if (user_data.eeCtrSet & EEP_CTRL_KEY) {
      // Valid index range
      if( (int)(user_data.eeCtrSet & 0x000000ff) >= EEP_RW_IDX) {
        // Float area from EEP_RW_IDX, count in Float size, No upper limit specified!
        eep_address = (float*)&eepage + (user_data.eeCtrSet & 0x000000ff);
        //Checking for the write request
        if (user_data.eeCtrSet & EEP_CTRL_WRITE){
          *eep_address = user_data.eepValue;  // Copy user_data into eepage structure
          //Checking for the read request
        } else if (user_data.eeCtrSet & EEP_CTRL_READ) {
          DISABLE_INTERRUPTS;
          user_data.eepValue = *eep_address;  // Publish eep value in user_data
          ENABLE_INTERRUPTS;
        } else {
          // Tell the user that inappropriate task has been requested
          DISABLE_INTERRUPTS;
          user_data.eeCtrSet = EEP_CTRL_INVAL_REQ;
          ENABLE_INTERRUPTS;
        }
      } else {
        DISABLE_INTERRUPTS;
        user_data.eeCtrSet = EEP_CTRL_OFF_RANGE;
        ENABLE_INTERRUPTS;
      }
    } else {
      // Tell the user that invalid key has been provided
      DISABLE_INTERRUPTS;
      user_data.eeCtrSet = EEP_CTRL_INVAL_KEY;
      ENABLE_INTERRUPTS;
    }
    EEP_CTR_Flag = CLEAR;
  }  // if eep

  //-----------------------------------------------------------------------------
  //
  //Writing to the EEPROM
  if (CeeS) {
    // Check if we are here for the first time
    if (!eeprom_flag) {  // first in
      rCSR = user_data.status;

      // Update eepage with the current user_data variables
      for(i=0;i<16;i++)  eepage.rdac[i] = user_data.rdac[i];

      //Temporary store the first address of page
      eeptemp_addr = PageAddr[(unsigned char)(user_data.eepage & 0x07)];
      //Temporary store the first address of data which has to be written
      eeptemp_source = (unsigned char xdata *) &eepage;
    }

    // EEPROM clear request
    if (CeeClr)  eep_request = WRITE_EEPROM;   //---PAA--- WAS CLEAR BUT WILL ERASE EEPROM!
    else         eep_request = WRITE_EEPROM;

    eeprom_channel = ExtEEPROM_Write_Clear (eeptemp_addr
                                         , &eeptemp_source
                                         , PAGE_SIZE
                                         , eep_request
                                         , &eeprom_flag);

    if (eeprom_channel == DONE) {
      SeeS = DONE;
      eeprom_flag = CLEAR;
      CeeS = CLEAR;
      //Set the active page
      user_data.eepage |= ((user_data.eepage & 0x07) << 5);
    } else  SeeS = FAILED;

    publishCtlCsr();      // Publish Registers state
  } // eep Save

  //-----------------------------------------------------------------------------
  //
  //Reading from the EEPROM
  if (CeeR) {
    rCSR = user_data.status;
    // Read the active page
    channel = ExtEEPROM_Read (PageAddr[(unsigned char)(user_data.eepage & 0x07)], (unsigned char*)&eepage, PAGE_SIZE);
    if (channel == DONE) {
      SeeR = DONE;
 
      // Publish user_data new settings
      DISABLE_INTERRUPTS;
      for(i=0;i<16;i++)  user_data.rdac[i] = eepage.rdac[i];
      ENABLE_INTERRUPTS;

      bDacdoitNOW = ON;        // Force a DAC Setting on next loop
    } else  SeeR = FAILED;

    CeeR = CLEAR;       // Reset action
    publishCtlCsr();    // Publish Registers state
  }  // eep Restore
#endif   // EEPROM
}
