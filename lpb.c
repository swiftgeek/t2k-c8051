/********************************************************************\
Name:         lpb.c
Created by:   Noel Wu
Created on:   August/6/2008

Contents:     Application specific (user) part of
              Midas Slow Control for LPB board.

// P3.7:A7       .6:A6        .5:A5       .4:A4      | .3:A3      .2:A2       .1:A1      .0:A0 
// P2.7:+1.8En   .6:+3.3En    .5:+5En     .4:SPIMOSI | .3:SPISCK  .2:RAMHLDn  .1:SPIMISO .0:RAMWP
// P1.7:NC       .6:+6ddFlag  .5:R/HClock .4:R/HData | .3:+6ddEN  .2:RAMCS    .1:D2ASync .0:SST_DRV 
// P0.7:NC       .6:NC        .5:485TXEN  .4:NC      | .3:NC      .2:NC       .1:Rx      .0:Tx 

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

#ifdef   _AD5300_
#include "Devices/AD5300.h"
#endif

#ifdef _ExtEEPROM_
#include "Devices/ExtEEPROM.h"
#endif
//
// Global declarations
//-----------------------------------------------------------------------------
char code  node_name[] = "LPB";

//
// Declare globally the number of sub-addresses to framework
unsigned char idata _n_sub_addr = 1;

// Local flag
bit EEP_CTR_FLAG;
unsigned char channel;
unsigned long xdata currentTime=0;

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
static unsigned char* xdata eeptemp_source;
unsigned char xdata eep_request;

// User Data structure declaration
//-----------------------------------------------------------------------------
MSCB_INFO_VAR code vars[] = {
  4, UNIT_BYTE,      0, 0,           0, "SerialN",               &user_data.SerialN,   // 0
  1, UNIT_BYTE,      0, 0,           0, "Error",                 &user_data.error,     // 1
  1, UNIT_BYTE,      0, 0,           0, "Control",               &user_data.control,   // 2
  1, UNIT_BYTE,      0, 0,           0, "Status",                &user_data.status,    // 3 
  1, UNIT_BYTE,      0, 0,           0, "eepage",                &user_data.eepage,    // 4
  4, UNIT_VOLT,      0, 0, MSCBF_FLOAT, "DtoAshft",              &user_data.iadc[0],   // 5
  4, UNIT_VOLT,      0, 0, MSCBF_FLOAT, "pDVsc",                 &user_data.iadc[1],   // 6
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDIsc",                 &user_data.iadc[2],   // 7
  4, UNIT_VOLT,      0, 0, MSCBF_FLOAT, "pDV6",                  &user_data.iadc[3],   // 8
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDI6",                  &user_data.iadc[4],   // 9
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDI5",                  &user_data.iadc[5],   // 10
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDI33",                 &user_data.iadc[6],   // 11
  4, UNIT_AMPERE,    0, 0, MSCBF_FLOAT, "pDI18",                 &user_data.iadc[7],   // 12
  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "uCTemp",                &user_data.uCTemp,    // 13

  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "IntTemp",               &user_data.IntTemp,   // 14
  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "58Temp",                &user_data._58Temp,   // 15
  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "33Temp",                &user_data._33Temp,   // 16

  4, UNIT_CELSIUS,   0, 0, MSCBF_FLOAT, "SHTtemp",               &user_data.SHTtemp,   // 17
  4, UNIT_PERCENT,   0, 0, MSCBF_FLOAT, "RHumid",                &user_data.SHThumi,   // 18

  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "DtoAshft",              &user_data.riadc[0],  // 19
  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "rpDVsc",                &user_data.riadc[1],  // 20
  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "rpDIsc",                &user_data.riadc[2],  // 21
  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "rpDV6",                 &user_data.riadc[3],  // 22  
  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "rpDI6",                 &user_data.riadc[4],  // 23
  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "rpDI5",                 &user_data.riadc[5],  // 24
  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "rpDI33",                &user_data.riadc[6],  // 25
  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "rpDI18",                &user_data.riadc[7],  // 26
  1, UNIT_BYTE,      0, 0,           0, "rdac",                  &user_data.rdac,      // 27

  1, UNIT_BYTE,      0, 0,           0, "watchdog",              &user_data.spare1,    // 28
  2, UNIT_BYTE,      0, 0,           0, "spare2",                &user_data.spare2,    // 29

  4, UNIT_BYTE,      0, 0, MSCBF_FLOAT, "eepValue",              &user_data.eepValue,  // 30
  4, UNIT_BYTE,      0, 0,           0, "eeCtrSet",              &user_data.eeCtrSet,  // 31
  0
};

MSCB_INFO_VAR *variables = vars;   // Structure mapping
// Get sysinfo if necessary
extern SYS_INFO sys_info;          // For address setting

//
//
//-----------------------------------------------------------------------------
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

/********************************************************************\
Application specific init and in/output routines
\********************************************************************/
/*---- User init function ------------------------------------------*/
void user_init(unsigned char init)
{
  float xdata temperature;
  unsigned char xdata pca_add=0;
  unsigned int xdata crate_add=0, board_address=0;
  if(init) {
     crate_add = cur_sub_addr();
  }
  sys_info.group_addr  = 500;

  user_data.IntTemp = 0;
  user_data._58Temp = 0;
  user_data._33Temp = 0;
  user_data.error = 0;
  user_data.SerialN = 0x0;
  user_data.control = 0;
  user_data.status = 0;
  user_data.rdac = 0;
  user_data.spare1=0;
  user_data.spare2=0;
  rCTL = 0;
  rCSR = 0;
  AD5300_FLAG = 0;
  EEP_CTR_FLAG = 0;

  //
  // Initial setting for communication and overall ports (if needed).
  //-----------------------------------------------------------------------------
  SFRPAGE  = CONFIG_PAGE;
  // P0MDOUT already contains Tx in push-pull
  P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull

  //
  // uC Miscellaneous ADCs (V/I Monitoring)
  //-----------------------------------------------------------------------------
  adc_internal_init();


  //
  // SST Temperatures
  //-----------------------------------------------------------------------------
#ifdef _ADT7486A_
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x01; // Setting the SST_DRV (SST) to push pull
  SFRPAGE  = CPT1_PAGE;
  CPT1CN  |= 0x80; // Enable the Comparator 1
  CPT1MD   = 0x03; //Comparator1 Mode Selection
  //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

  ADT7486A_Init(SST_LINE1);
#endif

//---------------------------------------------------------------
//
// LED DAC
#ifdef _AD5300_
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x02; // Setting the DtoA_SYNC push pull
  P2MDOUT |= 0x18; // Setting SPI_MOSI, SPI_SCK to push pull
  AD5300_Init();
#endif

//---------------------------------------------------------------
//
// EEPROM access
#ifdef _ExtEEPROM_
  SFRPAGE  = CONFIG_PAGE;
  P3MDOUT  = 0x00; // Crate address in OD
  P2MDOUT |= 0x18; // SPI_MOSI, SPI_SCK in PP
  P2MDOUT &= 0xFE; // RAM_WPn in OD
  P1MDOUT |= 0x04; // RAM_CSn in PP
  P0MDOUT |= 0x20; // RS485_ENABLE in PP
  ExtEEPROM_Init();

  //Get serial number from write protected area
  ExtEEPROM_Read  (WP_START_ADDR, (unsigned char*)&eepage, PAGE_SIZE);
  DISABLE_INTERRUPTS;
  user_data.SerialN = eepage.SerialN;
  ENABLE_INTERRUPTS;

  //Refer to the first Page of the non_protected area
  ExtEEPROM_Read  (PageAddr[0], (unsigned char*)&eepage ,PAGE_SIZE);
#endif

//---------------------------------------------------------------
//
//Humidity sensor initialization
#ifdef _HUMSEN_
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x20;  // P1.5:CLK(PP), P1.4:DATA 10K PullUp (OD)
  // Initializing the SHTxx communication
  HumiSensor_Init(humsense);
  //temporary humidity sensor
  user_data.SHTtemp = 0;
  user_data.SHThumi = 0;
#endif

// P3.7:A7       .6:A6        .5:A5       .4:A4      | .3:A3      .2:A2       .1:A1      .0:A0 
// P2.7:+1.8En   .6:+3.3En    .5:+5En     .4:SPIMOSI | .3:SPISCK  .2:RAMHLDn  .1:SPIMISO .0:RAMWP
// P1.7:NC       .6:+6ddFlag  .5:R/HClock .4:R/HData | .3:+6ddEN  .2:RAMCS    .1:D2ASync .0:SST_DRV 
// P0.7:NC       .6:NC        .5:485TXEN  .4:NC      | .3:NC      .2:NC       .1:Rx      .0:Tx 

//---------------------------------------------------------------
//
// Configure and read the address
// C C C C C C 1 0 1 is the MSCB Addr[8..0], 9 bits
// Modifying what the board reads from the Pins
  pca_add= P3;
  board_address= (((~pca_add)<<1) & 0x01F8)| 0x0005;
  sys_info.node_addr   = board_address;

//---------------------------------------------------------------
// SST Temperatures, setting temperature offsets
#ifdef _ADT7486A_
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x01; // Setting the SST_DRV (SST) to push pull
  SFRPAGE  = CPT1_PAGE;
  CPT1CN  |= 0x80; // Enable the Comparator 1
  CPT1MD   = 0x03; //Comparator1 Mode Selection
  //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

  ADT7486A_Init(SST_LINE1); //NW modified ADT7486A to support more than one line

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
#endif

//---------------------------------------------------------------
//
  P1MDOUT |= 0x08;
  VCC_EN = OFF;
  P2MDOUT |= 0xE0;
  VREG_5 = ON;
  VREG_3 = ON;
  VREG_1 = ON;

  // Watchdog
  user_data.spare1 = 0;
}

/*---- User write function -----------------------------------------*/
#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
  // LED Amplitude DAC
  if(index==IDXDAC)
    AD5300_FLAG = SET;

  // Control
  if(index==IDXCTL)
    rCTL = user_data.control;
  
  // EEPROM command
  if (index == IDXEEP_CTL)
    EEP_CTR_FLAG = SET;
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
void user_loop(void) {

  float xdata volt, temperature;
  unsigned int xdata rvolt;

  // Watchdog
    DISABLE_INTERRUPTS;
    user_data.spare1++;
    ENABLE_INTERRUPTS;

  //-----------------------------------------------------------------------------
  //
  // Command Power Up
  if(CPup){
    rCSR = user_data.status;
    VCC_EN = ON;
    SmSd = OFF;
    CPup = OFF;
    SPup = ON;
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  }

  //-----------------------------------------------------------------------------
  //
  // Command Shutdown
  if (CmSd){
    rCSR = user_data.status;
    VCC_EN = OFF;
    CPup = OFF;
    CmSd = OFF;
    SmSd = ON;
    SPup = OFF;
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  }

  //-----------------------------------------------------------------------------
#ifdef _HUMSEN_
  //Measuring the humidity and temperature
  if ((uptime() - currentTime) > 2) {
    status = HumidSensor_Cmd (&rSHThumi1
                             ,&rSHTtemp1
                             ,&humidity
                             ,&htemperature
                             ,&FCSorig1
                             ,&FCSdevi1
                             ,humsense);
    if (status == DONE){	 
      DISABLE_INTERRUPTS;
      user_data.SHThumi = humidity;
      user_data.SHTtemp = htemperature;
      ENABLE_INTERRUPTS;
    }
    currentTime = uptime();
  }
#endif 


  //-----------------------------------------------------------------------------
  // 
  // Internal ADCs monitoring Voltages and Currents based on time
  for (channel=0; channel<INTERNAL_N_CHN ; channel++) {
    volt = read_voltage(channel,&rvolt, IGAIN1);
    DISABLE_INTERRUPTS;
    user_data.iadc[channel] = volt;
    user_data.riadc[channel]= rvolt;
    ENABLE_INTERRUPTS;
  }

  // Read uC temperature
  volt = read_voltage(TCHANNEL, &rvolt, IGAIN1);
  /* convert to deg. C */
  temperature = 1000 * (volt - 0.776) / 2.86;   // Needs calibration
  /* strip to 0.1 digits */
  temperature = ((int) (temperature * 10 + 0.5)) / 10.0;
  DISABLE_INTERRUPTS;
  user_data.uCTemp = (float) temperature;
  ENABLE_INTERRUPTS;

  //-----------------------------------------------------------------------------
  //
  // Internal temperature reading
#ifdef _ADT7486A_
  if(!ADT7486A_Cmd(ADT7486A_ADDR0, GetIntTemp, SST_LINE1, &temperature)){
    IntssTT = CLEAR;
    DISABLE_INTERRUPTS;
    user_data.IntTemp = temperature;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
  } else {
    DISABLE_INTERRUPTS;
    IntssTT = SET;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
  }
  delay_us(500);

  //-----------------------------------------------------------------------------
  //
  // External temperature readings
  if(!ADT7486A_Cmd(ADT7486A_ADDR0, GetExt1Temp, SST_LINE1, &temperature)){
    Ext1ssTT = CLEAR;
    DISABLE_INTERRUPTS;
    user_data._58Temp = temperature;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
  } else {
    DISABLE_INTERRUPTS;
    Ext1ssTT = SET;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
  }

  delay_us(500);

  if(!ADT7486A_Cmd(ADT7486A_ADDR0, GetExt2Temp, SST_LINE1, &temperature)){
    Ext2ssTT = CLEAR;
    DISABLE_INTERRUPTS;
    user_data._33Temp = temperature;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
  } else {
    DISABLE_INTERRUPTS;
    Ext2ssTT = SET;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
  }
#endif

  //
  //-----------------------------------------------------------------------------
#ifdef _ExtEEPROM_

  //
  //-----------------------------------------------------------------------------
  // Checking the eeprom control flag
  if (EEP_CTR_FLAG) {
    //Checking for the special instruction
    if (user_data.eeCtrSet & EEP_CTRL_KEY) {
      // Valid index range
      if( (int)(user_data.eeCtrSet & 0x000000ff) >= EEP_RW_IDX) {
        // Float area from EEP_RW_IDX, count in Float size, No upper limit specified!
        eep_address = (float*)&eepage + (user_data.eeCtrSet & 0x000000ff);
        //Checking for the write request
        if (user_data.eeCtrSet & EEP_CTRL_WRITE){
          *eep_address = user_data.eepValue;
        //Checking for the read request
        } else if (user_data.eeCtrSet & EEP_CTRL_READ) {
          DISABLE_INTERRUPTS;
          user_data.eepValue = *eep_address;
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
    EEP_CTR_FLAG = CLEAR;
  }  // if eep

  //
  //-----------------------------------------------------------------------------
  //Writing to the EEPROM
  if (CeeS){
    //Check if we are here for the first time
    if (!eeprom_flag){
      rCSR = user_data.status;
      //Temporary store the first address of page
      eeptemp_addr = PageAddr[(unsigned char)(user_data.eepage & 0x07)];
      //Temporary store the first address of data which has to be written
      eeptemp_source = (unsigned char *)&eepage;
    }
    //EPROM clear request
    if (CeeClr)
      eep_request = CLEAR_EEPROM;
    else
      eep_request = WRITE_EEPROM;

    eeprom_wstatus = ExtEEPROM_Write_Clear (eeptemp_addr
      , &(eeptemp_source)
      , PAGE_SIZE
      , eep_request       // W / Clear
      , &eeprom_flag);    // Check to see if
    //everything has been written
    if (eeprom_wstatus == DONE) {
      SeeS = DONE;
      eeprom_flag = CLEAR;
      CeeS = CLEAR;
    }
    else
      SeeS = FAILED;

    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  }

  //
  //-----------------------------------------------------------------------------
  //Reading from the EEPROM
  if (CeeR) {
    rCSR = user_data.status;
    eeprom_rstatus = ExtEEPROM_Read  (eeptemp_addr,
      (unsigned char*)&eepage, PAGE_SIZE);
    if (eeprom_rstatus == DONE){
      CeeR = CLEAR;
      SeeR = DONE;
    }
    else
      SeeR = FAILED;

    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  }
#endif

  //
  //-----------------------------------------------------------------------------
  // LED amplitude DAC
#ifdef _AD5300_
  if(AD5300_FLAG) {
    AD5300_Cmd(user_data.rdac);
    AD5300_FLAG=CLEAR;
  }
#endif
}
