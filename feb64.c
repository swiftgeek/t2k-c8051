/********************************************************************\
 Name:         feb64.c
 Created by:   Bahman Sotoodian                Feb/11/2008


 Contents:     Application specific (user) part of
 Midas Slow Control Bus protocol
 for FEB64 board

        SMB      _PCA9539_      : Bias switches and Backplane readback
        SST      _ADT7486A_     : SST Temperature
        SMB      _LTC1669_      : Q pump DAC
        SPI      _LTC1665_      : SiPm_DACs
        SPI      _LTC2600_      : Asum DAC
 
 Memory usage:
 define(FEB64) define(_SPI_PROTOCOL_) define(_LTC1665_)
 define(_SMB_PROTOCOL_) define (_LTC1669_)  define(_LTC2600_)
 define(_SST_PROTOCOL_) define(_ADT7486A_) 

 Program Size: data=139.6 xdata=274 code=13497
 Program Size: data=150.2 xdata=711 code=15134
 Program Size: data=160.0 xdata=523 code=14686


 CODE: 16KB(0x4000), paging in 512B(0x200)
 CODE(?PR?UPGRADE?MSCBMAIN (0xF600))


 $Id$
 \********************************************************************/

 #include <stdio.h>
 #include <math.h>
 #include "mscbemb.h"
 #include "feb64.h"

 #include "Devices/pca_internal.h"
 #include "Devices/adc_internal.h"

 #ifdef  _ADT7486A_
 #include "Devices/ADT7486A_tsensor.h"
 #endif

 #ifdef _PCA9539_
 #include "Devices/PCA9539_io.h"
 #endif

 #ifdef _LTC1669_
 #include "Devices/LTC1669_dac.h"
 #endif

 #ifdef _LTC1665_
 #include "Devices/LTC1665_dac.h"
 #endif

 #ifdef _LTC2600_
 #include "Devices/LTC2600_dac.h"
 #endif

 #ifdef _LTC2497_
 #include "Devices/LTC2497_adc.h"
 #endif

 #ifdef _LTC2495_
 #include "Devices/LTC2495_adc.h"
 #endif

 #ifdef _ExtEEPROM_
 #include "Devices/ExtEEPROM.h"
 #endif
 //
 // Global declarations
 //-----------------------------------------------------------------------------
 char code  node_name[] = "FEB64";
 char idata svn_rev_code[] = "$Rev$";

 //
 // Declare globally the number of sub-addresses to framework
 unsigned char idata _n_sub_addr = 1;

// local variables
 unsigned long xdata currentTime=0, sstTime=0,sstExtTime=0;
 unsigned char xdata eeprom_channel,channel, chipAdd, chipChan;
 unsigned char xdata BiasIndex, AsumIndex;
 unsigned char xdata SST_LINE1=1;
 // Global bit register
 unsigned char bdata bChange;
// Local flag
 sbit bdoitNOW       = bChange ^ 0;
//Defining the user_write() flag actions
 sbit PCA_Flag       = bChange ^ 1;
 sbit LTC2600_Flag   = bChange ^ 2;
 sbit LTC1665_Flag   = bChange ^ 3;
 sbit LTC1669_Flag   = bChange ^ 4;
 sbit EEP_CTR_FLAG   = bChange ^ 5;

 sbit ASUM_SYNC      = P1 ^7;
 sbit ASUM_TESTN     = P1 ^6;
 sbit ASUM_PWDN      = P1 ^5;

 // User Data structure declaration
 //-----------------------------------------------------------------------------
 MSCB_INFO_VAR code vars[] = {
   4, UNIT_BYTE,            0, 0,           0, "SerialN",    &user_data.SerialN,      //0  
   2, UNIT_BYTE,            0, 0,           0, "Error",      &user_data.error,        //1  
   1, UNIT_BYTE,            0, 0,           0, "Control",    &user_data.control,      //2  
   1, UNIT_BYTE,            0, 0,           0, "Status",     &user_data.status,       //3  
   1, UNIT_BYTE,            0, 0,           0, "EEPage",     &user_data.eepage,       //4  
   1, UNIT_BYTE,            0, 0,           0, "swBias",     &user_data.swBias,       //5  
   2, UNIT_BYTE,            0, 0,           0, "rQpump",     &user_data.rQpump,       //6  

   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBias",      &user_data.VBias,        //7  
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBias",      &user_data.IBias,        //8  
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "pDVMon",     &user_data.pDVMon,       //9  
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "pAVMon",     &user_data.pAVMon,       //10 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "nAVMon",     &user_data.nAVMon,       //11 
   4, UNIT_AMPERE,          0, 0, MSCBF_FLOAT, "nAIMon",     &user_data.nAIMon,       //12 
   4, UNIT_AMPERE,          0, 0, MSCBF_FLOAT, "pAIMon",     &user_data.pAIMon,       //13 
   4, UNIT_AMPERE,          0, 0, MSCBF_FLOAT, "pDIMon",     &user_data.pDIMon,       //14 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "uCTemp",     &user_data.uCTemp,       //15 

   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp0",      &user_data.Temp[0],      //16 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp1",      &user_data.Temp[1],      //17 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp2",      &user_data.Temp[2],      //18 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp3",      &user_data.Temp[3],      //19 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp4",      &user_data.Temp[4],      //20 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp5",      &user_data.Temp[5],      //21 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp6",      &user_data.Temp[6],      //22 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp7",      &user_data.Temp[7],      //23 

   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon0",     &user_data.VBMon[0],     //24 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon1",     &user_data.VBMon[1],     //25 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon2",     &user_data.VBMon[2],     //26 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon3",     &user_data.VBMon[3],     //27 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon4",     &user_data.VBMon[4],     //28 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon5",     &user_data.VBMon[5],     //29 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon6",     &user_data.VBMon[6],     //30 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon7",     &user_data.VBMon[7],     //31 

   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBMon0",     &user_data.IBMon[0],     //32 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBMon1",     &user_data.IBMon[1],     //33 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBMon2",     &user_data.IBMon[2],     //34 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBMon3",     &user_data.IBMon[3],     //35 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBMon4",     &user_data.IBMon[4],     //36 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBMon5",     &user_data.IBMon[5],     //37 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBMon6",     &user_data.IBMon[6],     //38 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBMon7",     &user_data.IBMon[7],     //39 

   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "ssTemp0",    &user_data.ssTemp[0],    //40 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "ssTemp1",    &user_data.ssTemp[1],    //41 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "ssTemp2",    &user_data.ssTemp[2],    //42 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "ssTemp3",    &user_data.ssTemp[3],    //43 

   2, UNIT_BYTE,            0, 0,           0, "rAsum0",     &user_data.rAsum[0],     //44 
   2, UNIT_BYTE,            0, 0,           0, "rAsum1",     &user_data.rAsum[1],     //45 
   2, UNIT_BYTE,            0, 0,           0, "rAsum2",     &user_data.rAsum[2],     //46 
   2, UNIT_BYTE,            0, 0,           0, "rAsum3",     &user_data.rAsum[3],     //47 
   2, UNIT_BYTE,            0, 0,           0, "rAsum4",     &user_data.rAsum[4],     //48 
   2, UNIT_BYTE,            0, 0,           0, "rAsum5",     &user_data.rAsum[5],     //49 
   2, UNIT_BYTE,            0, 0,           0, "rAsum6",     &user_data.rAsum[6],     //50 
   2, UNIT_BYTE,            0, 0,           0, "rAsum7",     &user_data.rAsum[7],     //51 

   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias00",    &user_data.rBias[0],     //52 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias01",    &user_data.rBias[1],     //53 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias02",    &user_data.rBias[2],     //54 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias03",    &user_data.rBias[3],     //55 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias04",    &user_data.rBias[4],     //56 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias05",    &user_data.rBias[5],     //57 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias06",    &user_data.rBias[6],     //58 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias07",    &user_data.rBias[7],     //59 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias08",    &user_data.rBias[8],     //60 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias09",    &user_data.rBias[9],     //61 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias10",    &user_data.rBias[10],    //62 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias11",    &user_data.rBias[11],    //63 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias12",    &user_data.rBias[12],    //64 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias13",    &user_data.rBias[13],    //65 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias14",    &user_data.rBias[14],    //66 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias15",    &user_data.rBias[15],    //67 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias16",    &user_data.rBias[16],    //68 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias17",    &user_data.rBias[17],    //69 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias18",    &user_data.rBias[18],    //70 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias19",    &user_data.rBias[19],    //71 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias20",    &user_data.rBias[20],    //72 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias21",    &user_data.rBias[21],    //73 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias22",    &user_data.rBias[22],    //74 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias23",    &user_data.rBias[23],    //75 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias24",    &user_data.rBias[24],    //76 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias25",    &user_data.rBias[25],    //77 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias26",    &user_data.rBias[26],    //78 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias27",    &user_data.rBias[27],    //79 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias28",    &user_data.rBias[28],    //80 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias29",    &user_data.rBias[29],    //81 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias30",    &user_data.rBias[30],    //82 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias31",    &user_data.rBias[31],    //83 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias32",    &user_data.rBias[32],    //84 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias33",    &user_data.rBias[33],    //85 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias34",    &user_data.rBias[34],    //86 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias35",    &user_data.rBias[35],    //87 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias36",    &user_data.rBias[36],    //88 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias37",    &user_data.rBias[37],    //89 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias38",    &user_data.rBias[38],    //90 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias39",    &user_data.rBias[39],    //91 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias40",    &user_data.rBias[40],    //92 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias41",    &user_data.rBias[41],    //93 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias42",    &user_data.rBias[42],    //94 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias43",    &user_data.rBias[43],    //95 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias44",    &user_data.rBias[44],    //96 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias45",    &user_data.rBias[45],    //97 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias46",    &user_data.rBias[46],    //98 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias47",    &user_data.rBias[47],    //99 
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias48",    &user_data.rBias[48],    //100
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias49",    &user_data.rBias[49],    //101
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias50",    &user_data.rBias[50],    //102
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias51",    &user_data.rBias[51],    //103
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias52",    &user_data.rBias[52],    //104
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias53",    &user_data.rBias[53],    //105
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias54",    &user_data.rBias[54],    //106
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias55",    &user_data.rBias[55],    //107
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias56",    &user_data.rBias[56],    //108
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias57",    &user_data.rBias[57],    //109
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias58",    &user_data.rBias[58],    //110
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias59",    &user_data.rBias[59],    //111
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias60",    &user_data.rBias[60],    //112
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias61",    &user_data.rBias[61],    //113
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias62",    &user_data.rBias[62],    //114
   1,UNIT_BYTE,             0, 0,MSCBF_HIDDEN, "rBias63",    &user_data.rBias[63],    //115

   2, UNIT_BYTE,            0, 0,           0, "rVBias",     &user_data.rVBias,       //116
   2, UNIT_BYTE,            0, 0,           0, "rIBias",     &user_data.rIBias,       //117
   2, UNIT_BYTE,            0, 0,           0, "rpDV",       &user_data.rpDV,         //118
   2, UNIT_BYTE,            0, 0,           0, "rpAV",       &user_data.rpAV,         //119
   2, UNIT_BYTE,            0, 0,           0, "rnAV",       &user_data.rnAV,         //120
   2, UNIT_BYTE,            0, 0,           0, "rnAI",       &user_data.rnAI,         //121
   2, UNIT_BYTE,            0, 0,           0, "rpAI",       &user_data.rpAI,         //122
   2, UNIT_BYTE,            0, 0,           0, "rpDI",       &user_data.rpDI,         //123

   4, UNIT_BYTE,            0, 0,           0,  "rVBMon0",   &user_data.rVBMon[0],    //124
   4, UNIT_BYTE,            0, 0,           0,  "rVBMon1",   &user_data.rVBMon[1],    //125
   4, UNIT_BYTE,            0, 0,           0,  "rVBMon2",   &user_data.rVBMon[2],    //126
   4, UNIT_BYTE,            0, 0,           0,  "rVBMon3",   &user_data.rVBMon[3],    //127
   4, UNIT_BYTE,            0, 0,           0,  "rVBMon4",   &user_data.rVBMon[4],    //128
   4, UNIT_BYTE,            0, 0,           0,  "rVBMon5",   &user_data.rVBMon[5],    //129
   4, UNIT_BYTE,            0, 0,           0,  "rVBMon6",   &user_data.rVBMon[6],    //130
   4, UNIT_BYTE,            0, 0,           0,  "rVBMon7",   &user_data.rVBMon[7],    //131
   4, UNIT_BYTE,            0, 0,           0,  "rIBMon0",   &user_data.rIBMon[0],    //132
   4, UNIT_BYTE,            0, 0,           0,  "rIBMon1",   &user_data.rIBMon[1],    //133
   4, UNIT_BYTE,            0, 0,           0,  "rIBMon2",   &user_data.rIBMon[2],    //134
   4, UNIT_BYTE,            0, 0,           0,  "rIBMon3",   &user_data.rIBMon[3],    //135
   4, UNIT_BYTE,            0, 0,           0,  "rIBMon4",   &user_data.rIBMon[4],    //136
   4, UNIT_BYTE,            0, 0,           0,  "rIBMon5",   &user_data.rIBMon[5],    //137
   4, UNIT_BYTE,            0, 0,           0,  "rIBMon6",   &user_data.rIBMon[6],    //138
   4, UNIT_BYTE,            0, 0,           0,  "rIBMon7",   &user_data.rIBMon[7],    //139

   4, UNIT_BYTE,            0, 0,MSCBF_FLOAT|MSCBF_HIDDEN,  "eepValue", &user_data.eepValue,  // 140
   4, UNIT_BYTE,            0, 0,MSCBF_HIDDEN,              "eeCtrSet", &user_data.eeCtrSet,  // 141
   4, UNIT_BYTE,            0, 0,MSCBF_HIDDEN,              "asumctl", &user_data.asumCtl,    // 142

	0
 };
 MSCB_INFO_VAR *variables = vars;   // Structure mapping

 // Get sysinfo if necessary
 extern SYS_INFO sys_info;          // For address setting

#ifdef _ADT7486A_
 unsigned char ADT7486A_addrArray[] = {ADT7486A_ADDR1
                                     , ADT7486A_ADDR0
                                     , ADT7486A_ADDR3
                                     , ADT7486A_ADDR2};
#endif

#ifdef _LTC2600_
 unsigned char xdata LTC2600_LOAD[] = {LTC2600_LOAD_A,LTC2600_LOAD_B,
                                       LTC2600_LOAD_C,LTC2600_LOAD_D,
                                       LTC2600_LOAD_E,LTC2600_LOAD_F,
                                       LTC2600_LOAD_G,LTC2600_LOAD_H};
#endif

 //
 // 
 //-----------------------------------------------------------------------------
float read_voltage(unsigned char channel,unsigned int *rvalue,  float coeff, float offset, unsigned char gain)
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
	  voltage = voltage * coeff + offset;

  return voltage;
}
//
//
//-----------------------------------------------------------------------------
void switchonoff(unsigned char command)
{
    if(command==ON)
    {
        //
        // Initial setting for communication and overall ports (if needed).
        //-----------------------------------------------------------------------------
        SFRPAGE  = CONFIG_PAGE;
        // P0MDOUT contains Tx in push-pull
        P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull
        //
        // Setup the ASUM ports (P1.7,6,5)
        //-----------------------------------------------------------------------------
        P1MDOUT |= 0xE0;  // Sync, TestN, PowerDwnN 
        delay_us(1000);
        ASUM_SYNC  = 0;
        ASUM_TESTN = 0;
        ASUM_PWDN  = 1;

        //
        // uC Miscellaneous ADCs (V/I Monitoring)
        //-----------------------------------------------------------------------------
        SFRPAGE  = CONFIG_PAGE;
        // P3.2 OpenDrain for the 3 Vreg enable
        // P3.3,4 Spares on Push-pull
        // P3MDOUT |= 0x18;
        P3MDOUT &= 0xFB;  // Set Vreg enable to Open drain
        adc_internal_init();
        //
        // SPI Dac (Bias voltages, 64 channels)
        //-----------------------------------------------------------------------------
        #ifdef _LTC1665_
        SFRPAGE  = CONFIG_PAGE;
        P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
        P0MDOUT |= 0xC0; // Setting the BIAS_DAC_CSn1 and BIAS_DAC_CSn2 to push pull
        P3MDOUT |= 0x63; // Setting the BIAS_DAC_CSn3,4,5,6 to push pull
        P0MDOUT |= 0x60; // Setting the BIAS_DAC_CSn7,8 to push pull
        LTC1665_Init();
        #endif
        //
        // SPI ASUM DAC (8 channels)
        //-----------------------------------------------------------------------------
        #ifdef _LTC2600_
        SFRPAGE  = CONFIG_PAGE;
        P1MDOUT |= 0x10; // Setting the SUM_DAC_CSn to push pull
        P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
        LTC2600_Init();
        #endif
        //
        //SPI ExtEEPROM
        //-----------------------------------------------------------------------------
        #ifdef _ExtEEPROM_
        SFRPAGE  = CONFIG_PAGE;
        P3MDOUT |= 0x80; // Setting the RAM_CSn to push pull
        P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
        P2MDOUT &= 0xFE; // Setting the RAM_WPn to open drain
        ExtEEPROM_Init();
        #endif
        //
        // SMB Charge Pump DAC
        //-----------------------------------------------------------------------------
        #ifdef _LTC1669_
        LTC1669_Init(); //I2C DAC initialization (D_CH_PUMP)
        #endif

        #ifdef _LTC2497_
        LTC2497_Init();
        #endif

        #ifdef _LTC2495_
        LTC2495_Init();
        #endif
        //
        // SMB Bias Voltage switches
        //-----------------------------------------------------------------------------
        #ifdef _PCA9539_
		  SFRPAGE = CONFIG_PAGE;
		  P1MDOUT |= 0x08;

        PCA9539_Init(); //PCA General I/O (Bias Enables and Backplane Addr) initialization

        PCA9539_WriteByte(BIAS_OUTPUT_ENABLE);
        PCA9539_WriteByte(BACKPLANE_INPUT_ENABLE);
        #endif
        //
        // SST Temperatures
        //-----------------------------------------------------------------------------_
        #ifdef ADT7486A     
        SFRPAGE  = CONFIG_PAGE;
        P1MDOUT |= 0x01; // Setting the SST_DRV (SST) to push pull
        #endif   

        REG_EN=ON;
    }
    else if(command==OFF)
    {       
        SFRPAGE  = CONFIG_PAGE;
        //Switch all the ports to open drain except for mscb communication 
        P0MDOUT &= 0x23;
        P0 &= 0x23;
        P1MDOUT = 0;
        P1=0;
        P2MDOUT=0;
        P2=0;
        P3MDOUT=0;
        P3=0;
    }
}

 /********************************************************************\
 Application specific init and in/output routines
 \********************************************************************/

 /*---- User init function ------------------------------------------*/
 void user_init(unsigned char init)
 {
   char xdata i, pca_add=0;
	unsigned int xdata crate_add=0, board_address=0;
	float xdata temperature;
 // Inititialize local variables

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


 //
 // default settings, set only when uC-EEPROM is being erased and written
 //-----------------------------------------------------------------------------
   if(init)
   {
     user_data.control    = 0x00;    // Turn off the charge pump
     user_data.status     = 0x80;    // Shutdown state
     user_data.swBias     = 0x00;    // Turn off all the switches
     user_data.rQpump     = 0x00;    // Set to the lowest scale
     for(i=0;i<8;i++){
        user_data.rAsum[i] = 0x80;
        user_data.Temp[i]  = 0.0;
     }
     for(i=0;i<64;i++)
	  {
        user_data.rBias[i] = 0xFF;    // Set the DAC to lowest value
	   	 //NW mirror of the rBias values
		  ltc1665mirror[i]      = 0xFF;
	  }
     sys_info.group_addr  = 400;
   }

 //
 // Initial setting for communication and overall ports (if needed).
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   // P0MDOUT contains Tx in push-pull
   P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull

 //
 // Setup the ASUM ports (P1.7,6,5)
 //-----------------------------------------------------------------------------
  P1MDOUT |= 0xE0;  // Sync, TestN, PowerDwnN 
  delay_us(1000);
  ASUM_SYNC  = 0;
  ASUM_TESTN = 0;
  ASUM_PWDN  = 1;
 //
 // uC Charge Pump frequency setup
 //-----------------------------------------------------------------------------
   pca_operation(Q_PUMP_INIT); //Charge Pump initialization (crossbar settings)
   pca_operation(Q_PUMP_OFF);   //Initially turn it off

 //
 // uC Miscellaneous ADCs (V/I Monitoring)
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   // P3.2 OpenDrain for the 3 Vreg enable
   // P3.3,4 Spares on Push-pull
//   P3MDOUT |= 0x18;
   P3MDOUT &= 0xFB;  // Set Vreg enable to Open drain
   adc_internal_init();


 //
 // SPI Dac (Bias voltages, 64 channels)
 //-----------------------------------------------------------------------------
 #ifdef _LTC1665_
   SFRPAGE  = CONFIG_PAGE;
   P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
   P0MDOUT |= 0xC0; // Setting the BIAS_DAC_CSn1 and BIAS_DAC_CSn2 to push pull
   P3MDOUT |= 0x63; // Setting the BIAS_DAC_CSn3,4,5,6 to push pull
   P0MDOUT |= 0x60; // Setting the BIAS_DAC_CSn7,8 to push pull

   LTC1665_Init();
 #endif

 //
 // SPI ASUM DAC (8 channels)
 //-----------------------------------------------------------------------------
 #ifdef _LTC2600_
   SFRPAGE  = CONFIG_PAGE;
   P1MDOUT |= 0x10; // Setting the SUM_DAC_CSn to push pull
   P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
   LTC2600_Init();
 #endif

 //
 //SPI ExtEEPROM
 //-----------------------------------------------------------------------------
 #ifdef _ExtEEPROM_
  SFRPAGE  = CONFIG_PAGE;
   P3MDOUT |= 0x80; // Setting the RAM_CSn to push pull
   P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
  P2MDOUT &= 0xFE; // Setting the RAM_WPn to open drain
  ExtEEPROM_Init();
 #endif
 //
 // SMB Charge Pump DAC
 //-----------------------------------------------------------------------------
 #ifdef _LTC1669_
   LTC1669_Init(); //I2C DAC initialization (D_CH_PUMP)
 #endif

 #ifdef _LTC2497_
   LTC2497_Init();
  // Start Conversion on Channel 0
 #endif

 #ifdef _LTC2495_
   LTC2495_Init();
  // Start Conversion on Channel 0
 #endif
 //
 // SMB Bias Voltage switches
 //-----------------------------------------------------------------------------
 #ifdef _PCA9539_
  PCA9539_Init(); //PCA General I/O (Bias Enables and Backplane Addr) initialization

  PCA9539_WriteByte(BIAS_OUTPUT_ENABLE);
  PCA9539_WriteByte(BACKPLANE_INPUT_ENABLE);

 //
 // Physical backplane address retrieval
 //-----------------------------------------------------------------------------
  PCA9539_Read(BACKPLANE_READ, &pca_add, 1);
  //C C C C C C 0 B B is the MSCB Addr[8..0], 9 bits
  //Modifying what the board reads from the PCA
  //Externally, the crate address are reversed 
  crate_add= ((~pca_add)<<1)  & 0x01F8;
  //Internally, the board address are not reversed
  board_address=crate_add | ((pca_add) & 0x0003);
  sys_info.node_addr   = board_address; 
 #endif

 //
 // EEPROM memory Initialization/Retrieval
 //-----------------------------------------------------------------------------
#ifdef _ExtEEPROM_
  //Read only the serial number from the protected page
  ExtEEPROM_Read(SERIALN_ADD,(unsigned char*)&eepage.SerialN, SERIALN_LENGTH);
  user_data.SerialN = eepage.SerialN;
  //Read all other settings from page 0(non-protected)
  ExtEEPROM_Read(PageAddr[0],(unsigned char*)&eepage, PAGE_SIZE);
  DISABLE_INTERRUPTS;
  for(i=0;i<8;i++)
  		user_data.rAsum[i] = eepage.rasum[i];
  for(i=0;i<64;i++)
  {
  		user_data.rBias[i] = eepage.rbias[i];
		//NW mirror of the rBias values
		ltc1665mirror[i] = eepage.rbias[i];
  }
  user_data.rQpump = eepage.rqpump;
  user_data.swBias = eepage.SWbias;
  //Activate switch settings
  PCA_Flag=SET;
  //Activate charge pump dac settings
  LTC1669_Flag=SET;
  //Activate 64 bias dac settings
  LTC1665_Flag=SET;
  ENABLE_INTERRUPTS;
#endif
 //
 // EEPROM serial number Retrieval
 //-----------------------------------------------------------------------------
// #ifdef _ExtEEPROM_
//  ExtEEPROM_Read(SERIALN_ADD,(unsigned char*)&user_data.SerialN, SERIALN_LENGTH);
// #endif

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

   ADT7486A_Init(SST_LINE1); //NW modified ADT7486A to support more than one line
 
   for (channel=0;channel < NCHANNEL_ADT7486A; channel++){
      ADT7486A_Cmd(ADT7486A_addrArray[channel]
		           , SetExt1Offset
					  , ((int)eepage.ext1offset[channel]>>8)
					  , (int)eepage.ext1offset[channel]
					  , SST_LINE1
					  , &temperature); //NW
		delay_us(100);

   }
   for (channel=0;channel < NCHANNEL_ADT7486A; channel++){
      ADT7486A_Cmd(ADT7486A_addrArray[channel]
		           , SetExt2Offset
					  , ((int)eepage.ext2offset[channel]>>8)
					  , (int)eepage.ext2offset[channel]
					  , SST_LINE1
					  , &temperature); //NW
		delay_us(100);
   }
 #endif
 //
 // Final steps
 //-----------------------------------------------------------------------------
  rESR = 0x0000;              // No error!
  rEER = 0x00;                    // Active page 0, To be written or read page 0
  rCTL = 0;                       // Reset control
  rCSR = 0x00;                    // Initial CSR
  user_data.error      = rESR;    // Default Error register //NW
  user_data.eepage     = rEER;    // Default EE page
  user_data.status     = rCSR;    // Shutdown state
  user_data.control    = 0x80;    // Manual Shutdown
  for(i=0;i<8;i++){
     user_data.VBMon[i] = 0;
     user_data.IBMon[i] = 0;
     user_data.rVBMon[i] = 0;
     user_data.rIBMon[i] = 0;
     user_data.Temp[i]  = 0.0;
     user_data.eepValue = 0.0;
     user_data.eeCtrSet = 0;
  }
#ifdef FEB64REV0
  EN_pD5V  = ON;                  // Needed for debug
  EN_pA5V  = ON;                  //-PAA- needed for ASUM test
  EN_nA5V  = ON;                  //-PAA- needed for ASUM test
#elif defined(FEB64REV1)
  REG_EN = ON;       //to be determined...
#endif
}

//
// Final state of all input pins and their associated registers
//

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/*---- User write function -----------------------------------------*/
 #pragma NOAREGS

void user_write(unsigned char index) reentrant
{

// In case of you changing common control bit (ex. EXT/IN switch bit)
// the change should be distributed to other channels.
  rCSR = user_data.status;
   if (index == IDXCTL)
    rCTL = user_data.control;
//
// -- Index Bias Dac
   if (index == IDXBSWITCH) {
    #ifdef _PCA9539_
         if (!SsS) {
           PCA_Flag = 1;
           } // !Shutdown
    #endif
   }

//
//-- Index Threashold update
  if (index == IDXEEP_CTL)
    EEP_CTR_FLAG = 1;


// -- ASUM Threshold DACs
   if ((index >= FIRST_ASUM) && (index < LAST_ASUM)) {
      AsumIndex = (index - FIRST_ASUM);
    #ifdef _LTC2600_
       // Update Bias Dac voltages as requested by bit5 of control register
         LTC2600_Flag = 1;
    #endif
   }
//
// -- Index Bias Dac
   if ((index >= FIRST_BIAS) && (index < LAST_BIAS)) {
    #ifdef _LTC1665_
         LTC1665_Flag = 1;
    #endif
    }
//
// --- Index Qpump DAC
 #ifdef _LTC1669_
   if(SqPump) {
     LTC1669_Flag = 1;
   }
 #endif

//
// --- ASum Hidden Register
   if (index == IDXASUM_CTL) {
   //-PAA- At boot the Sync is high for 1sec while power is up
   // to shutdown sync w 143 0x4
   // to restart       w 143 0x5 (Sync + pwd)
   //                  w 143 0x4 (sync = 0)
	ASUM_SYNC  =  user_data.asumCtl & 0x1;
	ASUM_TESTN = (user_data.asumCtl & 0x2) >> 1;
	ASUM_PWDN  = (user_data.asumCtl & 0x4) >> 1;
   }

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
 //-----------------------------------------------------------------------------
 //-----------------------------------------------------------------------------
 /*
 Main loop should include:

 - V/I Reg reading/Monitoring on Time base
 - Read all the V/I Reg (Internal ADCs)
 - Check V/I against nominal values
 - Action: Shutdown card

 - uCTemperature reading/monitoring
 - Read internal uC Temperature
 - Check against Max uC value
 - Action: Shutdown card

 - Temperature reading/monitoring time based sync on Timer1 and MSCB req
 - Read all 12 SST temperature
 - Check against nominal values

 - I Bias reading
 - Read all 8 Bias current + Global bias current
 > Shutdown offending Vreg, report in status

 - I Bias monitoring
 - Check for sum consistency and agains nominal value
 - Switch off offending channel, report in status
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void user_loop(void) {
  float xdata volt, temperature, adc_value;
  //NW make sure pointers are stored in xdata
  float* xdata pfData;
  float* xdata eep_address;
  unsigned long xdata mask;
  signed long xdata result;
  unsigned int xdata eeptemp_addr,*rpfData,rvolt;
  //NW make sure eeptemp_source is stored in xdata
  unsigned char* xdata eeptemp_source;
  unsigned char xdata swConversion;
  unsigned char xdata eep_request;
  static  unsigned char xdata eeprom_flag =CLEAR;
  static xdata char adcChannel = 0; // -PAA was 16 ... special start value
  //ltc1665
  char xdata ltc1665_index, ltc1665_chipChan, ltc1665_chipAdd; 
 
  char chartemporary;
  int inttemporary;   
  chartemporary=0x89;
  inttemporary= chartemporary & 0x1F8;
  //
  //-----------------------------------------------------------------------------
  
  #ifdef _LTC2497_
      LTC2497_StartConversion(ADDR_LTC2497, adcChannel);
		// Make sure the result is valid!
      if(!LTC2497_ReadConversion(ADDR_LTC2497, adcChannel, &result)) {
      	result = -CONVER_FAC1;
      }
      adc_value = ((float)((result) 
                    + CONVER_FAC1) * (float)(EXT_VREF / CONVER_FAC2));
      adc_value = (adc_value * adc2mscb_table[adcChannel].Coef + adc2mscb_table[adcChannel].Offst);


      DISABLE_INTERRUPTS;
      if(adc2mscb_table[adcChannel].current) {
        user_data.IBMon[adc2mscb_table[adcChannel].mscbIdx] = adc_value;
        user_data.rIBMon[adc2mscb_table[adcChannel].mscbIdx] = result;
      } else {
        user_data.VBMon[adc2mscb_table[adcChannel].mscbIdx] = adc_value;
        user_data.rVBMon[adc2mscb_table[adcChannel].mscbIdx] = result;
      }
      ENABLE_INTERRUPTS;
     
      adcChannel++;
      adcChannel %= N_RB_CHANNEL;
	 
  #endif
  
  #ifdef _LTC2495_
      
      LTC2495_StartConversion(ADDR_LTC2495, adcChannel, adc2mscb_table[adcChannel].gain);

      if(!LTC2495_ReadConversion(ADDR_LTC2495, adcChannel, &result, adc2mscb_table[adcChannel].gain)) {
   	    result = -CONVER_FAC1;
      }

      adc_value = ((float)((result - adc2mscb_table[adcChannel].Offst) 
                    + CONVER_FAC1) * (float)(EXT_VREF / CONVER_FAC2));
      adc_value = (adc_value * adc2mscb_table[adcChannel].Coef) ;
      //dividing by the gain
      adc_value /= pow(2, (adc2mscb_table[adcChannel].gain + adc2mscb_table[adcChannel].current));
      DISABLE_INTERRUPTS;
      if(adc2mscb_table[adcChannel].current) {
        user_data.IBMon[adc2mscb_table[adcChannel].mscbIdx] = adc_value;
        user_data.rIBMon[adc2mscb_table[adcChannel].mscbIdx] = result;
      } else {
        user_data.VBMon[adc2mscb_table[adcChannel].mscbIdx] = adc_value;
        user_data.rVBMon[adc2mscb_table[adcChannel].mscbIdx] = result;
      }                                 
      ENABLE_INTERRUPTS;
    

      adcChannel++;                 // increment channel
      adcChannel %= N_RB_CHANNEL;   // modulus 16

  #endif

  //
  //-----------------------------------------------------------------------------
  //Checking the eeprom control flag
  if (EEP_CTR_FLAG)
  {
      //Checking for the special instruction
      if (user_data.eeCtrSet & EEP_CTRL_KEY)
		{
	   	eep_address = (float*)&eepage + (user_data.eeCtrSet & 0x000000ff);
   	   //Checking for the write request
		if (user_data.eeCtrSet & EEP_CTRL_WRITE){
			//NW added (SERIALN_ADD - WP_START_ADDR) is divided by 4 because the pointer
			// moves by 4 bytes for every increment
      	if ((user_data.eeCtrSet & 0x000000ff) <= ((SERIALN_ADD - WP_START_ADDR)/4))
				*eep_address = user_data.eepValue;
        	  //Checking for the read request
      } 
		else if (user_data.eeCtrSet & EEP_CTRL_READ)
		{
			DISABLE_INTERRUPTS;
         user_data.eepValue = *eep_address;
			ENABLE_INTERRUPTS;
      } 
		else 
		{
         // Tell the user that inappropriate task has been requested
         DISABLE_INTERRUPTS;
         user_data.eepValue = EEP_CTRL_INVAL_REQ;
         ENABLE_INTERRUPTS;
      }
	
  } 
  else 
  {
      // Tell the user that invalid key has been provided
      DISABLE_INTERRUPTS;
      user_data.eepValue = EEP_CTRL_INVAL_KEY;
      ENABLE_INTERRUPTS;
  }

      EEP_CTR_FLAG = CLEAR;
  }

  //
  //-----------------------------------------------------------------------------
  //Checking the flags
  #ifdef _PCA9539_
    if(PCA_Flag){
      swConversion = user_data.swBias;
      PCA9539_Conversion(&swConversion);
      PCA9539_WriteByte(BIAS_WRITE, ~swConversion);
       PCA_Flag = CLEAR;
    }
  #endif

  #ifdef _LTC2600_
    if(LTC2600_Flag) {
        LTC2600_Cmd(WriteTo_Update,LTC2600_LOAD[AsumIndex], user_data.rAsum[AsumIndex]);
        LTC2600_Flag = CLEAR;
    }
  #endif

  #ifdef _LTC1665_
     if(LTC1665_Flag) {
	  	  for(ltc1665_index=0; ltc1665_index<64; ltc1665_index++){
		     if(ltc1665mirror[ltc1665_index] != user_data.rBias[ltc1665_index]){
     			  ltc1665_chipChan = (ltc1665_index / 8) + 1;
     			  ltc1665_chipAdd  = (ltc1665_index % 8) + 1;
			     LTC1665_Cmd(ltc1665_chipAdd,user_data.rBias[ltc1665_index] , ltc1665_chipChan);
				  ltc1665mirror[ltc1665_index]=user_data.rBias[ltc1665_index];
			  }
		      		
		  }	
        //LTC1665_Cmd(chipAdd,user_data.rBias[BiasIndex] ,chipChan);
        LTC1665_Flag = CLEAR;
      }
  #endif

  #ifdef _LTC1669_
    if(LTC1669_Flag) {
        LTC1669_SetDAC(ADDR_LTC1669, LTC1669_INT_BG_REF, user_data.rQpump);
        LTC1669_Flag = CLEAR;
      }
  #endif

  //-----------------------------------------------------------------------------
  // Power Up based on CTL bit
  if (CPup) {
    rCSR = user_data.status;
    if (!SsS  || SsS || SmSd) { // Shutdown or NOT
    // Publish Registers
    SsS = OFF;
    DISABLE_INTERRUPTS;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
    // Power up Card
      // Vreg ON
#ifdef FEB64REV0
      EN_pD5V = ON;
      EN_pA5V = ON;
      EN_nA5V = ON;
#elif defined(FEB64REV1)
      //switchonoff(ON);
		REG_EN = ON;
#endif
      delay_ms(10);
     // Force Check on Voltage
      bdoitNOW = ON;
      // Wait for Check before setting SPup
    } // !Shutdown

    // Reset Action
    CPup = CLEAR;  // rCTL not updated yet
    // Publish state after V/U check

  } // Power Up

  if ( bdoitNOW || (uptime() - currentTime) > 1) {
    //-----------------------------------------------------------------------------
    //
    // Internal ADCs monitoring Voltages and Currents based on time
    //
    // Time to do V/I Reg reading
    pfData = &(user_data.VBias);
    rpfData = &(user_data.rVBias);
    rCSR = user_data.status;
    rESR = user_data.error; //NW
    for (channel=0, mask=0 ; channel<INTERNAL_N_CHN ; channel++, mask++) {
      volt = read_voltage(channel
                          ,&rvolt
                          ,iadc_table[channel].coeff
                          ,iadc_table[channel].offset
                          ,iadc_table[channel].gain );
      DISABLE_INTERRUPTS;
      pfData[channel] = volt;
      rpfData[channel] = rvolt;
      ENABLE_INTERRUPTS;
      mask = (1<<channel);  // Should be 4 bytes for ESR
   /*  if ((channel > 1) && !SqPump) {  // Skip vQ, I
       if ((volt >= eepage.lVIlimit[channel])
         && (volt <= eepage.uVIlimit[channel])) {
           rESR &= ~mask; // in range
       }
       else {
         rESR |= mask; // out of range
       }
    }
   */ }
    if (bdoitNOW) {
     if (rESR & SHUTDOWN_MASK) {
        pca_operation(Q_PUMP_OFF);
        SqPump   = OFF;
        SsS = ON;
        SPup = OFF;
      } else {
        SsS = OFF; // Remove System Shutdown
      SmSd = OFF;
      SPup = ON;
      }
   }

    bdoitNOW = OFF;   // Reset flag coming from PowerUp sequence

    // Publish Registers
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.error   = rESR; //NW
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;

    //-----------------------------------------------------------------------------
    //
    // uC Temperature reading/monitoring based on time
    rCSR = user_data.status;
    rESR = user_data.error; //NW
    // Read uC temperature
    volt = read_voltage(TCHANNEL,&rvolt, 0, 0, IGAIN1 );
    /* convert to deg. C */
    temperature = 1000 * (volt - 0.776) / 2.86;   // Needs calibration
    /* strip to 0.1 digits */
    temperature = ((int) (temperature * 10 + 0.5)) / 10.0;
    DISABLE_INTERRUPTS;
    user_data.uCTemp = (float) temperature;
    ENABLE_INTERRUPTS;
	 //NW temperature instead of volt?
    if ((volt >= eepage.luCTlimit) && (volt <= eepage.uuCTlimit)) {
      uCT = OFF; // in range
    } else {
      uCT = ON; // out of range
    }
    // Set Shutdown if Error in System Register
    if ((rESR & SHUTDOWN_MASK) && !SmSd) {
      pca_operation(Q_PUMP_OFF);  // Toggle state
      SqPump   = OFF;
      SsS  = ON;
      SPup = OFF;
    } else {
      SsS  = OFF; // Remove System Shutdown
    }
    // Publish Error state
    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.error   = rESR; //NW
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;

    // update time for next loop
    currentTime = uptime();
  }

  //-----------------------------------------------------------------------------
  // Set Manual Shutdown based on Index
  if (CmSd) {
    rCSR = user_data.status;
    if (!SsS) {
      SmSd = ON;  // Set Manual Shutdown
      pca_operation(Q_PUMP_OFF);
#ifdef FEB64REV0
      EN_pD5V  = ON;  // For debugging
      EN_pA5V  = OFF;
      EN_nA5V  = OFF;
#elif defined(FEB64REV1)
      //switchonoff(OFF);
		REG_EN = OFF;
#endif
      SqPump   = OFF;
      SPup     = OFF;
    } // Manula Shutdown
    // Reset Action
    CmSd = CLEAR;  // rCTL not yet published

    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  } // !Shutdown

  //-----------------------------------------------------------------------------
  // Toggle Qpump ON/OFF based on Index
  if (CqPump) {
    rCSR = user_data.status;
    if (!SsS) {
      if (SqPump) {
        pca_operation(Q_PUMP_OFF);  // Toggle state
        SqPump   = OFF;
      }
      else {
        pca_operation(Q_PUMP_ON);   // Toggle state
        SqPump   = ON;
      }
    } // !Shutdown

    // Reset Action
    CqPump = CLEAR;

    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  } // Control Pump

#ifdef _ADT7486A_
  //BS     To setting the offset associated with the temperature reading.
  //      in that case, we probably can get rid of the conversion factors
  //      ADT7486A_Cmd(ADT7486A_addrArray[channel], SetExt1Offset, &temperature)
  //      ADT7486A_Cmd(ADT7486A_addrArray[channel], SetExt2Offset, &temperature)

  //-----------------------------------------------------------------------------
  // Temperature reading/monitoring based on time for external temperature
  if (uptime() - sstExtTime > SST_TIME){
    for (channel=0;channel < NCHANNEL_ADT7486A; channel++){
        if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt1Temp, SST_LINE1, &temperature)) //NW
      {
        ExtssTT = CLEAR;
         DISABLE_INTERRUPTS;
         user_data.Temp[channel*2] = temperature;
         user_data.error   = rESR;  //NW
         ENABLE_INTERRUPTS;
      }
      else
      {
        DISABLE_INTERRUPTS;
        ExtssTT = SET;
        user_data.error   = rESR;  //NW
        ENABLE_INTERRUPTS;
      }
     }
    for (channel=0;channel < NCHANNEL_ADT7486A; channel++){
        if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt2Temp, SST_LINE1, &temperature)) //NW
      {
        ExtssTT = CLEAR;
         DISABLE_INTERRUPTS;
         user_data.Temp[(channel*2)+1] = temperature;
         user_data.error   = rESR;  //NW
         ENABLE_INTERRUPTS;
      }
      else
      {
        DISABLE_INTERRUPTS;
        ExtssTT = SET;
        user_data.error   = rESR; //NW
        ENABLE_INTERRUPTS;
      }
     }

     sstExtTime = uptime();
  }


  //-----------------------------------------------------------------------------
  // Temperature reading/monitoring based on time for internal temperature
  if (uptime() - sstTime > SST_TIME) {
    for (channel=0;channel < NCHANNEL_ADT7486A; channel++) {
    if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetIntTemp, SST_LINE1, &temperature)) //NW
    {
       IntssTT = 0;
       DISABLE_INTERRUPTS;
       user_data.ssTemp[channel] = temperature;
       user_data.error   = rESR;  //NW
       ENABLE_INTERRUPTS;
    }
    else
    {
      DISABLE_INTERRUPTS;
      IntssTT = 1;
      user_data.error   = rESR; //NW
      ENABLE_INTERRUPTS;
    }
    }
    sstTime = uptime();
  }

#endif

  //-----------------------------------------------------------------------------
  // EEPROM Save procedure based on CTL bit
  #ifdef _ExtEEPROM_
    if (CeeS) {
       //Check if we are here for the first time
      if (!eeprom_flag){
        rCSR = user_data.status;
      //Temporary store the first address of page
       eeptemp_addr = PageAddr[(unsigned char)(user_data.eepage & 0x07)];
      //Temporary store the first address of data which has to be written
       eeptemp_source = (unsigned char*)&eepage;
      }
     //EPROM clear request
     if (CeeClr){
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
      user_data.eepage |= ((user_data.eepage & 0x07) <<5);
      } else {
        SeeS = FAILED;
       }

      // Publish Qpump state
      // Publish Registers state
      DISABLE_INTERRUPTS;
      user_data.control = rCTL;
      user_data.status  = rCSR;
      ENABLE_INTERRUPTS;
     }
#endif
  //-----------------------------------------------------------------------------
  // EEPROM Restore procedure based on CTL bit
  if (CeeR) {
    rCSR = user_data.status;

#ifdef _ExtEEPROM_
	//NW read to eepage(active page) instead of eepage2
    channel = ExtEEPROM_Read  (PageAddr[(unsigned char)(user_data.eepage & 0x07)],
   (unsigned char*)&eepage, PAGE_SIZE);
#endif

    if (channel == DONE){
      CeeR = CLEAR;
      SeeR = DONE;
    } else
      SeeR = FAILED;

    // Publish Qpump state
    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  }

  //-----------------------------------------------------------------------------
  //
  // General loop delay

  delay_ms(10);

  //
  // General loop delay
} // End of User loop
