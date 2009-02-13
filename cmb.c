/********************************************************************\
Name:         cmb.c
Created by:   Bahman Sotoodian                
Created on:   April/9/2008 

Contents:     Application specific (user) part of
Midas Slow Control for CMB board.      
JTAG:
0 0 0  0 (no power)
0 2 0 18 (with power/4V)
define(CMB) define(_ADT7486A_) define(_ExtEEPROM_) 
Program Size: data=169.0 xdata=459 code=14211

$Id$
\********************************************************************/

#include <stdio.h>
#include <math.h>
#include "mscbemb.h"
#include "cmb.h"

#include "Devices/adc_internal.h"

#include "Protocols/CMB_SPI_handler.h"

#ifdef   _ADT7486A_
#include "Devices/ADT7486A_tsensor.h"
#endif

#ifdef _ExtEEPROM_
#include "Devices/ExtEEPROM.h"
#endif

//-----------------------------------------------------------------------------
// Global declarations
char code  node_name[] = "cmb";
char idata svn_rev_code[] = "$Rev$";

//-----------------------------------------------------------------------------
sbit CFG_RECOVER = P2 ^ 0;
sbit CMB_CSn     = P2 ^ 1;
sbit CLK_SEL     = P0 ^ 7;
sbit V4_ENn      = P2 ^ 3;
sbit V4_OCn      = P2 ^ 4;

//-----------------------------------------------------------------------------
// Declare globally the number of sub-addresses to framework
unsigned char idata _n_sub_addr = 1;

//
unsigned char xdata channel;
unsigned long xdata tempTime=0, sstTime=0;
unsigned char xdata eeprom_channel, channel;
unsigned int xdata crate_add=0, pca_add=0;

//-----------------------------------------------------------------------------
// Global bit register
unsigned char bdata bChange;
// Local flag
sbit bCPupdoitNOW   = bChange ^ 0;
sbit EEP_CTR_Flag   = bChange ^ 1;

//-----------------------------------------------------------------------------
// User Data structure declaration
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
    errbit = SET;
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
void CMB_SPI_WriteByte(unsigned char cmbdata) {
    CMB_CSn = 0;
    CMBSPI_WriteByte(0x01);
    CMBSPI_WriteByte(cmbdata);
    CMB_CSn = 1;
}

//-----------------------------------------------------------------------------
//
unsigned char CMB_SPI_ReadByte(void) {
unsigned char cmbdata;
    CMB_CSn = 0;
    CMBSPI_WriteByte(0x02);
    cmbdata = CMBSPI_ReadByteRising();
    CMB_CSn = 1;
    return cmbdata;
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
// Switch ON/OFF the 4V main power of the card
void switchonoff(unsigned char command)
{
  if(command==ON)
  {
    // Turn ON card
    SFRPAGE  = CONFIG_PAGE;
    P2MDOUT &= ~0x08;     // V4_ENn(P2.3) OD
    V4_ENn = 0;           // ON

    rESR = 0x0000;   // Reset error status at each Power UP
    rCSR = user_data.status;
    SPup = ON;  // Set Status
    SsS = OFF;  // Clear System Shutdown
    SmSd = OFF; // Clear Manual Shutdown

    // Publish Status and Error
    DISABLE_INTERRUPTS;
    user_data.status  = rCSR;
    user_data.error  = rESR;
    ENABLE_INTERRUPTS;

     // Setup CMB SPI ports   
    P2MDOUT |= 0x20;  // CMB_SPI_SCK  (P2.5) PP
    P2MDOUT |= 0x40;  // CMB_SPI_MOSI (P2.6) PP
    P2MDOUT &= ~0x80; // CMB_SPI_MISO (P2.7) OD
    P2MDOUT |= 0x02; //  CMB_CS       (P2.1) PP
    CMB_CSn = 1;

    CMBSPI_Init();

    delay_ms(1000);
    CMB_SPI_WriteByte(~pca_add);
    delay_us(100);
    user_data.eepage = CMB_SPI_ReadByte();
    delay_us(100);
    CMB_SPI_WriteByte(~pca_add);
    delay_us(100);
    user_data.spare = CMB_SPI_ReadByte();

 } else if(command==OFF) {
    // Switch all the ports to open drain except for...
    SFRPAGE  = CONFIG_PAGE;
    V4_ENn = 1;
  }
}

//-----------------------------------------------------------------------------
// Read internal ADC channel (8 ADC, 1 Temperature)
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
  char xdata i;
  unsigned int xdata board_address=0;

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
  user_data.eepage = 0;

  // Update local registers
  rCTL = user_data.control;
  rCSR = user_data.status;

  //
  // Group setting
  sys_info.group_addr  = 400;

  //-----------------------------------------------------------------------------
  // Initial setting for communication and overall ports (if needed).
  SFRPAGE  = CONFIG_PAGE;
  P0MDOUT |= 0x20;   // add RS485_ENABLE(P0.5) in push-pull

  //-----------------------------------------------------------------------------
  // uC Miscellaneous ADCs (V/I Monitoring)
  SFRPAGE  = CONFIG_PAGE;
  //  P3MDOUT |= 0x1C; //Setting the Regulators control pins to push pull (3 Vreg)
  adc_internal_init();

#ifdef _ADT7486A_
  //-----------------------------------------------------------------------------
  // SST Temperatures
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x01; // Setting the SST_DRV(P1.0) (SST) to push pull
  SFRPAGE  = CPT1_PAGE;
  CPT1CN  |= 0x80; // Enable the Comparator 1
  CPT1MD   = 0x03; //Comparator1 Mode Selection
  //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

  ADT7486A_Init(SST_LINE1);
#endif

  //
  //-----------------------------------------------------------------------------
  // Address retrieval
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
  //-----------------------------------------------------------------------------
  // EEPROM memory Initialization/Retrieval
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

  //-----------------------------------------------------------------------------
  // Clock Selection P0.7 default to External Clock (LVDS)
  SFRPAGE  = CONFIG_PAGE;
  P0MDOUT &= ~0x80;  // CLK_SEL (P0.7) OD
  CLK_SEL = SXclk = ON;

  //-----------------------------------------------------------------------------
  // Over Current Error bit
  SFRPAGE = CONFIG_PAGE;
  P2MDOUT &= ~0x10;  // V4_OC(P2.4) OD for read


  // Configure Recovery
  P2MDOUT |= 0x01;  // CFG_RECOVER(P2.0) PP for write

  //-----------------------------------------------------------------------------
  // Power up the card for now
  switchonoff(OFF);

  SmSd = OFF;  // Set Manual Shutdown
  SPup = OFF;
  // Reset Action
  CmSd = CLEAR;  // rCTL not yet published
  Cdeb1 = CLEAR;
  
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

  //
  //-- EE Page function
  if (index == IDXEEP_CTL) EEP_CTR_Flag = 1;
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
  float* xdata eep_address;
  unsigned int xdata eeptemp_addr;
  //NW make sure eeptemp_source is stored in xdata
  unsigned char* xdata eeptemp_source;
  unsigned char xdata eep_request;
  static  unsigned char xdata eeprom_flag = CLEAR;
  unsigned int *xdata rpfData;
  unsigned int xdata i, rvolt;

  //-----------------------------------------------------------------------------
  // Power Up based on CTL bit
  if (CPup) {
    switchonoff(ON);
    delay_ms(100);
    // Force Check on Voltage
    bCPupdoitNOW = ON;
    // Reset Action
    CPup = CLEAR;  // rCTL not updated yet, publish state after U/I check
    // Reset All error
    rESR = 0x0000;

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
    publishCtlCsr();
  } // Manual Shutdown

  //-----------------------------------------------------------------------------
  // Switch RJ45/Internal Clock source
  if (CXclk) {
    rCSR = user_data.status;
    if (SXclk) CLK_SEL = SXclk = 0;
    else       CLK_SEL = SXclk = 1;
    CXclk = 0;  // Reset command
    publishCtlCsr();  // Publish Clock selection
  } // Switch Clock

  //-----------------------------------------------------------------------------
  // Config Pulse
  if (Ccfg) {
    for (i=0;i<3;i++) {
     CFG_RECOVER = 1;
     delay_us(1);
     CFG_RECOVER = 0;
     delay_us(1);
    }
    delay_us(9);
    Ccfg = 0; 
  } // Configure_recovery

  //-----------------------------------------------------------------------------
  // Debug1 Write/Read Address
  if (Cdeb1) {

    // Write Crate address to the CMB register
    CMB_SPI_WriteByte(~pca_add);

    delay_us(105);

    // Read Crate address from the CMB Register
    user_data.spare = CMB_SPI_ReadByte();
    Cdeb1 = 0; 
  } // 

#ifdef _ExtEEPROM_
  //-----------------------------------------------------------------------------
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
    EEP_CTR_Flag = CLEAR;
  }  // if eep

  //-----------------------------------------------------------------------------
  // EEPROM Save procedure based on CTL bit
  if (CeeS) {
    //Check if we are here for the first time
    if (!eeprom_flag) {  // first in
      rCSR = user_data.status;

      //Temporary store the first address of page
      eeptemp_addr = PageAddr[(unsigned char)(user_data.eepage & 0x07)];
      //Temporary store the first address of data which has to be written
      eeptemp_source = (unsigned char*)&eepage;
    }
    //EPROM clear request
    if (CeeClr) {
      eep_request = CLEAR_EEPROM;
    } else {
      eep_request = WRITE_EEPROM;
    }

    eeprom_channel = ExtEEPROM_Write_Clear (eeptemp_addr
      , &(eeptemp_source)
      , PAGE_SIZE
      , eep_request
      , &eeprom_flag);

    if (eeprom_channel == DONE) {
      SeeS = DONE;
      eeprom_flag = CLEAR;
      CeeS = CLEAR;
      //Set the active page
      user_data.eepage |= ((user_data.eepage & 0x07) << 5);
    } else {
      SeeS = FAILED;
    }

    // Publish Registers state
    publishCtlCsr();
  }

  //-----------------------------------------------------------------------------
  // EEPROM Restore procedure based on CTL bit
  if (CeeR) {
    rCSR = user_data.status;

    //NW read to eepage(active page) instead of eepage2
    channel = ExtEEPROM_Read (PageAddr[(unsigned char)(user_data.eepage & 0x07)]
    , (unsigned char*)&eepage, PAGE_SIZE);

    if (channel == DONE){
      CeeR = CLEAR;
      SeeR = DONE;
    } else
      SeeR = FAILED;

    // Publish Registers state
    publishCtlCsr();
  }
#endif

  //-----------------------------------------------------------------------------
  // Internal ADCs reading Voltages and Currents based on time
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
    if ((temperature < eepage.luCTlimit) || (temperature > eepage.uuCTlimit)) uCT = ON; // out of range
    PublishVariable(&(user_data.uCTemp), temperature, uCT); 

    // Update time
    tempTime = uptime();
  }

  yield();

  //-----------------------------------------------------------------------------
  // SST temperature reading
#ifdef _ADT7486A_
  if (bCPupdoitNOW || ((uptime() - sstTime) > SST_TIME)) {
    // Vreg1 Temperature form the SST device location
    if(!ADT7486A_Cmd(ADT7486A_address, GetIntTemp, SST_LINE1, &temperature)) {
      if ((temperature < eepage.lSSTlimit) || (temperature > eepage.uSSTlimit)) Vreg1ssTT = ON;
      PublishVariable(&user_data.Vrg1Temp, temperature, Vreg1ssTT); 
    } else publishErr(RdssT);

    //
    // Vreg2 Reading and Monitoring of Temperature from the remote diode of the SST device
    if(!ADT7486A_Cmd(ADT7486A_address, GetExt1Temp, SST_LINE1, &temperature)) {
      if ((temperature < eepage.lSSTlimit) || (temperature > eepage.uSSTlimit)) Vreg2ssTT = ON;
      PublishVariable(&user_data.Vrg2Temp, temperature, Vreg2ssTT); 
    } else publishErr(RdssT);

    //
    // FGPA Reading and Monitoring of Temperature from the internal diode of the FPGA
    if(!ADT7486A_Cmd(ADT7486A_address, GetExt2Temp, SST_LINE1, &temperature)) {
      if ((temperature < eepage.luCTlimit) || (temperature > eepage.uuCTlimit)) FPGAssTT = ON;
      PublishVariable(&user_data.FPGATemp, temperature, FPGAssTT); 
    } else publishErr(RdssT);

    // Update time
    sstTime = uptime();
  }
#endif

  //-----------------------------------------------------------------------------
  // Main Current and Voltage Monitoring
  pfData = &(user_data.pDI4Mon);
  for (channel=0; channel<INTERNAL_N_CHN ; channel++) {
    if ((pfData[channel] < eepage.lVIlimit[channel])
     || (pfData[channel] > eepage.uVIlimit[channel])) {
        rESR |= (1<<channel); // out of range
    }
  }

  DISABLE_INTERRUPTS;
  user_data.error   = rESR;
  ENABLE_INTERRUPTS;

  //
  // Read Watchdog state
//  Swdog = WATCHDOG;  // Watchdog state

  //
  // Read Over current switch
  V4_OC = (V4_OCn == 0) ? 1 : 0;

  //
  // Take action based on ERROR
  // Currently only the Temperature are considered
  if (SPup && (rESR & ( UFTEMPERATURE_MASK | BTEMPERATURE_MASK))) {
    switchonoff(OFF);
    SPup = OFF;
    SsS = ON;
  } else if (bCPupdoitNOW) {
    bCPupdoitNOW = OFF;   // Reset flag coming from PowerUp sequence
    SsS = SmSd = OFF;
    SPup = ON;
  }

  //
  // Publish Control, Error and Status.
  publishCtlCsr();
} 
