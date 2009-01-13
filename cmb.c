/********************************************************************\
Name:         cmb.c
Created by:   Bahman Sotoodian                
Created on:   April/9/2008 

Contents:     Application specific (user) part of
Midas Slow Control for CMB board.      

define(_SST_PROTOCOL_) 

$Id$
\********************************************************************/

#include <stdio.h>
#include <math.h>
#include "mscbemb.h"
#include "cmb.h"

#include "Devices/adc_internal.h"

#ifdef   _ADT7486A_
#include "Devices/ADT7486A_tsensor.h"
#endif

#ifdef _ExtEEPROM_
#include "Devices/ExtEEPROM.h"
#endif
//
// Global declarations
//-----------------------------------------------------------------------------
char code  node_name[] = "cmb";
char idata svn_rev_code[] = "$Rev$";

//
// Port declaration
sbit SC_DEBUG0   = P2 ^ 5;
sbit SC_DEBUG1   = P2 ^ 6;
sbit SC_DEBUG2   = P2 ^ 7;
sbit CFG_RECOVER = P2 ^ 0;
sbit WATCHDOG    = P2 ^ 1;
sbit CLK_SEL     = P0 ^ 7;
sbit V4_ENn      = P2 ^ 3;
sbit V4_OCn      = P2 ^ 4;

//
// Declare globally the number of sub-addresses to framework
unsigned char idata _n_sub_addr = 1;

//
unsigned char xdata channel;
unsigned long xdata tempTime=0, sstTime=0;

// Global bit register
unsigned char bdata bChange;
// Local flag
sbit bCPupdoitNOW   = bChange ^ 0;

// User Data structure declaration
//-----------------------------------------------------------------------------
MSCB_INFO_VAR code vars[] = {
  4, UNIT_BYTE,     0, 0,           0, "SerialN",    &user_data.SerialN,    // 0
  2, UNIT_BYTE,     0, 0,           0, "Error",      &user_data.error,      // 1
  1, UNIT_BYTE,     0, 0,           0, "Control",    &user_data.control,    // 2
  1, UNIT_BYTE,     0, 0,           0, "Status",     &user_data.status,     // 3
  1, UNIT_BYTE,     0, 0,           0, "EEPage",     &user_data.eepage,     // 4
  1, UNIT_BYTE,     0, 0,           0, "Spare",      &user_data.spare,      // 5

  4, UNIT_AMPERE,   0, 0, MSCBF_FLOAT, "pDI4Mon",    &user_data.pDI4Mon,    // 6
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "pDV4Mon",    &user_data.pDV4Mon,    // 7
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "pAV33Mon",   &user_data.pAV33Mon,   // 8
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "pAV25Mon",   &user_data.pAV25Mon,   // 9
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "pDV15Mon",   &user_data.pDV15Mon,   // 10
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "pDV18Mon",   &user_data.pDV18Mon,   // 11
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "pDV25Mon",   &user_data.pDV25Mon,   // 12
  4, UNIT_VOLT,     0, 0, MSCBF_FLOAT, "pDV33Mon",   &user_data.pDV33Mon,   // 13

  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "uCTemp",     &user_data.uCTemp,     // 14
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "FpgaTemp",   &user_data.FPGATemp,   // 15
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "Vrg1Temp",   &user_data.Vrg1Temp,   // 16
  4, UNIT_CELSIUS,  0, 0, MSCBF_FLOAT, "Vrg2Temp",   &user_data.Vrg2Temp,   // 17 

  2, UNIT_BYTE,     0, 0,   0, "rDI4Mon",            &user_data.rpDI4Mon,   // 18
  2, UNIT_BYTE,     0, 0,   0, "rDV4Mon",            &user_data.rpDV4Mon,   // 19
  2, UNIT_BYTE,     0, 0,   0, "rAV33Mon",           &user_data.rpAV33Mon,  // 20
  2, UNIT_BYTE,     0, 0,   0, "rA25VMon",           &user_data.rA25VMon,   // 21
  2, UNIT_BYTE,     0, 0,   0, "rDV15Mon",           &user_data.rpDV15Mon,  // 22
  2, UNIT_BYTE,     0, 0,   0, "rDV18Mon",           &user_data.rpDV18Mon,  // 23 
  2, UNIT_BYTE,     0, 0,   0, "rDV25Mon",           &user_data.rpDV25Mon,  // 24
  2, UNIT_BYTE,     0, 0,   0, "rDV33Mon",           &user_data.rpDV33Mon,  // 25 

  4, UNIT_BYTE,     0, 0,MSCBF_FLOAT|MSCBF_HIDDEN,  "eepValue", &user_data.eepValue,  // 26
  4, UNIT_BYTE,     0, 0,MSCBF_HIDDEN,              "eeCtrSet", &user_data.eeCtrSet,  // 27
  0
};

MSCB_INFO_VAR *variables = vars;   // Structure mapping
// Get sysinfo if necessary
extern SYS_INFO sys_info;          // For address setting

//
// Switch ON/OFF the 4V main power of the card
//-----------------------------------------------------------------------------
void switchonoff(unsigned char command)
{
  if(command==ON)
  {
    // Turn ON card
    SFRPAGE  = CONFIG_PAGE;
    P2MDOUT &= ~0x08;     // V4_ENn(P2.3) OD
    V4_ENn = 0;   // ON

    rESR = 0x0000;   // Reset error status at each Power UP
    rCSR = user_data.status;
    SPup = ON;  // Set Status
    SsS = OFF;  // Clear Status

    // Publish Status and Error
    DISABLE_INTERRUPTS;
    user_data.status  = rCSR;
    user_data.error  = rESR;
    ENABLE_INTERRUPTS;

  } else if(command==OFF) {
    // Switch all the ports to open drain except for...
    SFRPAGE  = CONFIG_PAGE;
    V4_ENn = 1;
  }
}

//
// Read internal ADC channel (8 ADC, 1 Temperature)
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
  *rvalue =  rawsum / 10;
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
  char xdata i, pca_add=0;
  unsigned int xdata crate_add=0, board_address=0;

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

  board_address = cur_sub_addr();

  if (init){
    user_data.FPGATemp = 0;
    user_data.Vrg1Temp = 0;
    user_data.Vrg2Temp = 0;
    user_data.error = 0;
    user_data.SerialN = 0x0;
    sys_info.node_addr = cur_sub_addr();
  }

  user_data.control = 0;
  user_data.status = 0;
  user_data.error = 0;
  user_data.spare = 0;

  //
  // Group setting
  sys_info.group_addr  = 400;

  //
  // Initial setting for communication and overall ports (if needed).
  //-----------------------------------------------------------------------------
  SFRPAGE  = CONFIG_PAGE;
  P0MDOUT |= 0x20;   // add RS485_ENABLE(P0.5) in push-pull

  //
  // uC Miscellaneous ADCs (V/I Monitoring)
  //-----------------------------------------------------------------------------
  SFRPAGE  = CONFIG_PAGE;
  //  P3MDOUT |= 0x1C; //Setting the Regulators control pins to push pull (3 Vreg)
  adc_internal_init();

#ifdef _ADT7486A_
  //
  // SST Temperatures
  //-----------------------------------------------------------------------------
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x01; // Setting the SST_DRV(P1.0) (SST) to push pull
  SFRPAGE  = CPT1_PAGE;
  CPT1CN  |= 0x80; // Enable the Comparator 1
  CPT1MD   = 0x03; //Comparator1 Mode Selection
  //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

  ADT7486A_Init(SST_LINE1);
#endif

  //
  // Address retrieval
  //-----------------------------------------------------------------------------
  // Configure and read the address
  // C C C C C C 1 0 0 is the MSCB Addr[8..0], 9 bits
  // Modifying what the board reads from the Pins 
  SFRPAGE = CONFIG_PAGE;
  // Change p3 to digital input
  P3MDOUT = 0x00;
  P3=0xFF;
  // Read crate address
  pca_add= P3;
  crate_add= ((~pca_add)<<3)  & 0x01F8;
  board_address=(crate_add &  0x01FC) | 0x0004;
  sys_info.node_addr   = board_address; 

#ifdef _ExtEEPROM_
  //
  // EEPROM memory Initialization/Retrieval
  //-----------------------------------------------------------------------------
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x80;  // Setting the RAM_CSn(P1.7) to push pull
  P1MDOUT |= 0x06;  // Setting the SPI_MOSI(P1.1) and SPI_SCK(P1.2) to push pull
  P1MDOUT &= ~0x20; // Setting the RAM_WPn(P1.5) to OD
  ExtEEPROM_Init();

  // Read only the serial number from the protected page
  ExtEEPROM_Read(SERIALN_ADD,(unsigned char*)&eepage.SerialN, SERIALN_LENGTH);
  user_data.SerialN = eepage.SerialN;
  //
  // Read all other settings from page 0(non-protected)
  ExtEEPROM_Read(PageAddr[0],(unsigned char*)&eepage, PAGE_SIZE);
  DISABLE_INTERRUPTS;
  ENABLE_INTERRUPTS;
#endif

  //
  // Clock Selection P0.7
  //-----------------------------------------------------------------------------
  SFRPAGE  = CONFIG_PAGE;
  P0MDOUT &= ~0x80;  // CLK_SEL (P0.7) OD
  CLK_SEL = 1;

  //
  // Over Current Error 
  //-----------------------------------------------------------------------------
  SFRPAGE = CONFIG_PAGE;
  P2MDOUT &= ~0x10;  // V4_OC(P2.4) OD for read

  // Power up the card for now
  //-----------------------------------------------------------------------------
  switchonoff(OFF);
  rCTL = user_data.control;
  rCSR = user_data.status;
  SmSd = OFF;  // Set Manual Shutdown
  SPup = OFF;
  // Reset Action
  CmSd = CLEAR;  // rCTL not yet published
  
  // Publish Registers state
  DISABLE_INTERRUPTS;
  user_data.control = rCTL;
  user_data.status  = rCSR;
  ENABLE_INTERRUPTS;
}

/*---- User write function -----------------------------------------*/
#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
  rCSR = user_data.status;
  if (index == IDXCTL) {
    rCTL = user_data.control;
  } // IDXCTL
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

  float xdata volt, temperature, *pfData;
  unsigned int *xdata rpfData;
  unsigned int xdata rvolt;

  //-----------------------------------------------------------------------------
  // Power Up based on CTL bit
  if (CPup) {
    switchonoff(ON);

    delay_ms(100);

    // Force Check on Voltage
    bCPupdoitNOW = ON;

    // Reset Action
    CPup = CLEAR;  // rCTL not updated yet, publish state after U/I check
  } // Power Up

  //-----------------------------------------------------------------------------
  // Set Manual Shutdown based on Index
  if (CmSd) {
    rCSR = user_data.status;
    SmSd = ON;  // Set Manual Shutdown
    switchonoff(OFF);
    SPup = OFF;

    // Reset Action
    CmSd = CLEAR;  // rCTL not yet published

    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  } // Manual Shutdown

  //-----------------------------------------------------------------------------
  // Switch RJ45/Internal Clock source
  if (CXclk) {
    rCSR = user_data.status;
    if (SXclk) {
      CLK_SEL = 0;
      SXclk = 0;
    } else {
      CLK_SEL = 1;
      SXclk = 1;
    }
    CXclk = 0;

    // Publish Closk selection
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  } // Switch Clock

  //
  //-----------------------------------------------------------------------------
  // Internal ADCs monitoring Voltages and Currents based on time
  if (bCPupdoitNOW || ((uptime() - tempTime) > TEMP_TIME)) {
    pfData = &(user_data.pDI4Mon);
    rpfData = &(user_data.rpDI4Mon);
    for (channel=0; channel<INTERNAL_N_CHN ; channel++) {
      volt = read_voltage(channel,&rvolt, IGAIN1);
      DISABLE_INTERRUPTS;
      pfData[channel] = volt;
      rpfData[channel]= rvolt;
      ENABLE_INTERRUPTS; 
    }

    //
    // Read uC temperature    
    volt = read_voltage(TCHANNEL,&rvolt, IGAIN1);
    /* convert to deg. C */
    temperature = 1000 * (volt - 0.776) / 2.86;   // Needs calibration
    /* strip to 0.1 digits */
    temperature = ((int) (temperature * 10 + 0.5)) / 10.0;
    DISABLE_INTERRUPTS;
    user_data.uCTemp = (float) temperature;
    ENABLE_INTERRUPTS; 
    if ((temperature >= eepage.luCTlimit) && (temperature <= eepage.uuCTlimit)) {
      uCT = OFF; // in range
    } else {
      uCT = ON; // out of range
    }

    // Update time
    tempTime = uptime();
  }

  yield();
  //
  //-----------------------------------------------------------------------------
  // Internal temperature reading
#ifdef _ADT7486A_
  if (bCPupdoitNOW || ((uptime() - sstTime) > SST_TIME)) {
    // Vreg1 Temperature form the SST device location
    if(!ADT7486A_Cmd(ADT7486A_address, GetIntTemp, SST_LINE1, &temperature)) {
      if ((temperature >= eepage.lSSTlimit) && (temperature <= eepage.uSSTlimit)) {
        Vreg1ssTT = OFF; // in range
      } else {
        Vreg1ssTT = ON; // out of range
      }
      DISABLE_INTERRUPTS;
      user_data.Vrg1Temp = temperature;
      user_data.error   = rESR;
      ENABLE_INTERRUPTS;
    } else {
      DISABLE_INTERRUPTS;
      Vreg1ssTT = SET;
      user_data.error   = rESR;
      ENABLE_INTERRUPTS;
    }

    //
    // Vreg2 Temperature from the remote diode of the SST device
    if(!ADT7486A_Cmd(ADT7486A_address, GetExt1Temp, SST_LINE1, &temperature)) {
      if ((temperature >= eepage.lSSTlimit) && (temperature <= eepage.uSSTlimit)) {
        Vreg2ssTT = OFF; // in range
      } else {
        Vreg2ssTT = ON; // out of range
      }
      DISABLE_INTERRUPTS;
      user_data.Vrg2Temp = temperature;
      user_data.error   = rESR;
      ENABLE_INTERRUPTS;
    } else {
      DISABLE_INTERRUPTS;
      Vreg2ssTT = SET;
      user_data.error   = rESR;
      ENABLE_INTERRUPTS;
    }

    //
    // FGPA Temperature from the internal diode of the FPGA
    if(!ADT7486A_Cmd(ADT7486A_address, GetExt2Temp, SST_LINE1, &temperature)) {
      if ((temperature >= eepage.luCTlimit) && (temperature <= eepage.uuCTlimit)) {
        FPGAssTT = OFF; // in range
      } else {
        FPGAssTT = ON; // out of range
      }
      DISABLE_INTERRUPTS;
      user_data.FPGATemp = temperature;
      user_data.error   = rESR;
      ENABLE_INTERRUPTS;
    } else {
      DISABLE_INTERRUPTS;
      FPGAssTT = SET;
      user_data.error   = rESR;
      ENABLE_INTERRUPTS;
    }
    // Update time
    tempTime = uptime();
  }

#endif
  // Take action based on ERROR
  if (rESR & ( UFTEMPERATURE_MASK | BTEMPERATURE_MASK)) {
//   switchonoff(OFF);
//   SsS = ON;
  } else if (bCPupdoitNOW) {
    bCPupdoitNOW = OFF;   // Reset flag coming from PowerUp sequence
    SsS = SmSd = OFF;
    SPup = ON;
  }

  //
  // Read Over current switch
  V4_OC = (V4_OCn == 0) ? 1 : 0;

  //
  // Publish Control, Error and Status.
  DISABLE_INTERRUPTS;
  user_data.control = rCTL;
  user_data.error   = rESR;
  user_data.status  = rCSR;
  ENABLE_INTERRUPTS;
} 
