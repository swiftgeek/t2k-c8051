/********************************************************************\
Name:         feb64.c
Created by:   Bahman Sotoodian                Feb/11/2008
Modified  :   Noel Wu                         .. Aug 2008

Contents:     Application specific (user) part of
Midas Slow Control Bus protocol for FEB64 board

SMB      _PCA9539_      : Bias switches and Backplane readback
SST      _ADT7486A_     : SST Temperature
SMB      _LTC1669_      : Q pump DAC
SPI      _LTC1665_      : SiPm_DACs
SPI      _LTC2600_      : Asum DAC

define(FEB64)     define(_LTC1665_)   define (_LTC1669_) 
define(_LTC2600_) define(_ADT7486A_)  define(_PCA9539_)
define(_LTC2495_) define(_ExtEEPROM_) define(FEB64REV1) 

Memory usage:
Program Size: data=139.6 xdata=274 code=13497
Program Size: data=150.2 xdata=711 code=15134
Program Size: data=160.0 xdata=523 code=14686
Program Size: data=166.5 xdata=1309 code=22083 - Aug 20/2008
Program Size: data=167.4 xdata=1483 code=23701 - Sep 06/2008
Program Size: data=58.4 xdata=1630 code=27180 - Jan 29/2009 Large model
CODE(?PR?UPGRADE?MSCBMAIN (0xD000)) 

// P3.7:RAMCSn   .6:CSn5      .5:CSn4     .4:SPARE5  | .3:SPARE4   .2:REG_EN    .1:CSn3     .0:CSn2 
// P2.7:SPARE1   .6:CSn7      .5:CSn6     .4:SPIMOSI | .3:SPISCK   .2:RAMHLDn   .1:SPIMISO  .0:RAMWPn 
// P1.7:ASUMSync .6:ASUMTestn .5:ASUMPWDn .4:ASUMCSn | .3:SST_DRV2 .2:PIORESETN .1:SST_DRV1 .0:DACRESETN 
// P0.7:CSn1     .6:CSn0      .5:485TXEN  .4:QPUMPCLK| .3:SMBCLK   .2:SMBDAT    .1:Rx       .0:Tx 

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
//-----------------------------------------------------------------------------
// Global declarations
char code  node_name[] = "FEB64";
char idata svn_rev_code[] = "$Rev$";

//
// Declare globally the number of sub-addresses to framework
unsigned char idata _n_sub_addr = 1;

// local variables
int xdata k, calState=0, calCount=0;
float xdata calNumber = 10.0;
char xdata calQpumpSave=0, calSwSave=0;
unsigned long xdata currentTime=0, sstTime=0,sstExtTime=0, calTime=0;
unsigned char xdata eeprom_channel,channel, chipAdd, chipChan;
unsigned char xdata BiasIndex, AsumIndex;
unsigned char xdata SST_LINE1=1, SST_LINE2=1;
unsigned long xdata loopcnt = 0;

// Global bit register
unsigned char bdata bChange;

// Local flag
sbit bCPupdoitNOW   = bChange ^ 0;
sbit bDacdoitNOW    = bChange ^ 1;

//Defining the user_write() flag actions
sbit PCA_Flag       = bChange ^ 2;
sbit LTC2600_Flag   = bChange ^ 3;
sbit LTC1665_Flag   = bChange ^ 4;
sbit LTC1669_Flag   = bChange ^ 5;
sbit EEP_CTR_Flag   = bChange ^ 6;
sbit CAsum_Flag     = bChange ^ 7;

//
// RESET output pin for PCA9539 (PIO) and LTC1665 (DACx8)
sbit DACRESETN      = P1 ^ 3;
sbit PCARESETN      = P1 ^ 1;
//
// ASUM port
sbit ASUM_SYNC      = P1 ^ 7;
sbit ASUM_TESTN     = P1 ^ 6;
sbit ASUM_PWDN      = P1 ^ 5;
//
// Vreg Enable port assignment
sbit REG_EN         = P3 ^ 2;

//-----------------------------------------------------------------------------
// User Data structure declaration
MSCB_INFO_VAR code vars[] = {
  4, UNIT_BYTE,            0, 0,           0, "SerialN",    &user_data.SerialN,      //0
  2, UNIT_BYTE,            0, 0,           0, "Error",      &user_data.error,        //1
  1, UNIT_BYTE,            0, 0,           0, "Control",    &user_data.control,      //2
  1, UNIT_BYTE,            0, 0,           0, "Status",     &user_data.status,       //3
  1, UNIT_BYTE,            0, 0,           0, "EEPage",     &user_data.eepage,       //4
  1, UNIT_BYTE,            0, 0,           0, "swBias",     &user_data.swBias,       //5
  2, UNIT_BYTE,            0, 0,           0, "rQpump",     &user_data.rQpump,       //6

  4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBias",      &user_data.VBias,        //7
  4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBias",      &user_data.IBias,        //8
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
  4, UNIT_BYTE,            0, 0,MSCBF_HIDDEN,              "watchdog", &user_data.watchdog,  // 143
  4, UNIT_BYTE,            0, 0,MSCBF_HIDDEN,              "smbdebug", &user_data.debugsmb,  // 144

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
// Update from eepage the local ADC2 Table
void updateAdc2Table(void)
{
  int i, j;
  for(i=0;i<16;i++) {
    if (adc2mscb_table[i].current) {
      j = adc2mscb_table[i].mscbIdx;
      adc2mscb_table[i].Offst = (signed int) eepage.iBiasOffset[j];
    } else {
      j = adc2mscb_table[i].mscbIdx;
      adc2mscb_table[i].Offst = (signed int) eepage.vBiasOffset[j];
    }
  }
}

//-----------------------------------------------------------------------------
// Watchdog counter function
void watchdog(short int state)
{
  loopcnt++;
  DISABLE_INTERRUPTS;
  user_data.watchdog = ((loopcnt<<16) | (state & 0xFFFF));
  ENABLE_INTERRUPTS;
}

//-----------------------------------------------------------------------------
//
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
  voltage = (float)  voltage / 1024.0 * INT_VREF;  // conversion
  if ( channel != TCHANNEL)
    voltage = voltage * coeff + offset;

  return voltage;
}

//-----------------------------------------------------------------------------
//
void switchonoff(unsigned char command)
{
  char i;
  char xdata swConversion;
  if(command==ON)
  {
    //-----------------------------------------------------------------------------
    // Enable the voltage regulator to power the card
    SFRPAGE  = CONFIG_PAGE;
    P3MDOUT |= 0x04;  // Set Vreg enable to PP
    REG_EN = ON;

    //-----------------------------------------------------------------------------
    // Setup the ASUM ports (P1.7, 6,5)
    P1MDOUT |= 0xE0;  // Sync, Test, PowerDwnN Push-pull

    // Pulse the serializer (ASUM)
    ASUM_PWDN  = 1;    // Power UP
    delay_ms(1000);
    ASUM_SYNC = 1;     // Issue sync words.
    delay_us(100);     // Wait sometime.
    ASUM_TESTN = 1;    // Choose Non test mode. (TEST=neg true)
    ASUM_SYNC = 0;     // Negate the sync process.

    //-----------------------------------------------------------------------------
    // uC Charge Pump frequency setup
    pca_operation(Q_PUMP_INIT);  // Charge Pump initialization (crossbar settings)
    //Sept 09 default Qpump ON at Power up
    pca_operation(Q_PUMP_ON);

#ifdef _LTC1665_
    //-----------------------------------------------------------------------------
    // SPI Dac (Bias voltages, 64 channels)
    SFRPAGE  = CONFIG_PAGE;
    P2MDOUT |= 0x18; // Set SPI_MOSI and SPI_SCK in PP
    P0MDOUT |= 0xC0; // Set BIAS_DAC_CSn1 and BIAS_DAC_CSn2 in PP
    P3MDOUT |= 0x63; // Set BIAS_DAC_CSn3,4,5,6 in PP
    P2MDOUT |= 0x60; // Set BIAS_DAC_CSn7,8 in PP
    P1MDOUT |= 0x0A; // Set SST_DRV2, SST_DRV1 in PP
    DACRESETN  = 1;  // in OD for now
    LTC1665_Init();
#endif

#ifdef _LTC2600_
    //-----------------------------------------------------------------------------
    // SPI ASUM DAC (8 channels)
    SFRPAGE  = CONFIG_PAGE;
    P2MDOUT |= 0x18;  // Set SPI_MOSI and SPI_SCK in PP
    P1MDOUT |= 0x10;  // Set SUM_DAC_CSn in PP
    LTC2600_Init();
#endif

#ifdef _ExtEEPROM_
    //-----------------------------------------------------------------------------
    //SPI ExtEEPROM
    SFRPAGE  = CONFIG_PAGE;
    P2MDOUT |= 0x18; // Set SPI_MOSI and SPI_SCK in PP
    P3MDOUT |= 0x80; // Set RAM_CSn in PP
    P2MDOUT &= ~0x01; // Set RAM_WPn in OD
    ExtEEPROM_Init();
#endif

#ifdef _LTC1669_
    //-----------------------------------------------------------------------------
    // SMB Charge Pump DAC
    LTC1669_Init(); //I2C DAC initialization
#endif

#ifdef _LTC2497_
    LTC2497_Init();
#endif

#ifdef _LTC2495_
    LTC2495_Init();
#endif

#ifdef _PCA9539_
    //-----------------------------------------------------------------------------
    // SMB Bias Voltage switches
    SFRPAGE = CONFIG_PAGE;
    P1MDOUT |= 0x08;
    PCA9539_Init(); //PCA General I/O (Bias Enables and Backplane Addr) initialization
    delay_us(10);
    PCA9539_WriteByte(BIAS_OUTPUT_ENABLE);
    delay_us(10);
    swConversion = user_data.swBias;
    PCA9539_Conversion(&swConversion);
    PCA9539_WriteByte(BIAS_WRITE, ~swConversion);
    delay_us(10);
    PCA9539_WriteByte(BACKPLANE_INPUT_ENABLE);
    delay_us(10);
#endif

    // Activate charge pump dac settings
    LTC1669_Flag = ON;

    // Activate 64 bias dac settings
    // user_data contains correct values, force update 
    // by setting mirror to 0xff 
    for(i=0;i<64;i++) ltc1665mirror[i] = 0xFF;

    // Needs power for DAC update, by now it should be ON
    // Force DAC updates
    LTC1665_Flag = ON;
  }
  else if(command==OFF) {
    pca_operation(Q_PUMP_OFF);   // Initially turn it off

    // Switch all the ports to open drain except for...
    SFRPAGE  = CONFIG_PAGE;

    //Ram_CSn maintained in PP set it to High
    // P3.7:RAMCSn   .6:CSn5      .5:CSn4     .4:SPARE5  | .3:SPARE4   .2:REG_EN    .1:CSn3     .0:CSn2 
    P3MDOUT = 0x80;
    P3 &= 0x80;

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Needs to stay on for JTAG debug access!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    REG_EN  = OFF;   // Shutdown All Regulators 

    //SPI communication for EEPROM is maintained
    // P2.7:SPARE1   .6:CSn7      .5:CSn6     .4:SPIMOSI | .3:SPISCK   .2:RAMHLDn   .1:SPIMISO  .0:RAMWPn 
    P2MDOUT &= 0x1F;
    P2 &= 0x0F;

    //SST and PCARESETN is maintained ==1 and PP
    // P1.7:ASUMSync .6:ASUMTestn .5:ASUMPWDn .4:ASUMCSn | .3:SST_DRV2 .2:PIORESETN .1:SST_DRV1 .0:DACRESETN 
    P1MDOUT  |= 0x0F;  // in PP
    DACRESETN = 1;
    PCARESETN = 1;
    P1 &= ~0x0A;  // SST1, SST2 set Low

    // mscb communication
    // P0.7:CSn1     .6:CSn0      .5:485TXEN  .4:QPUMPCLK| .3:SMBCLK   .2:SMBDAT    .1:Rx       .0:Tx 
    P0MDOUT &= 0x20;
    P0 &= ~0xC0;  // Set CS1,0 to Low
  }
}

/********************************************************************\
Application specific init and in/output routines
\********************************************************************/

/*---- User init function ------------------------------------------*/
void user_init(unsigned char init)
{
  char xdata i, pca_add=0;
  float xdata temperature;
  unsigned int xdata crate_add=0, board_address=0;
  unsigned char xdata swConversion=0;
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

  //-----------------------------------------------------------------------------
  // default settings, set only when uC-EEPROM is being erased and written
  if(init) {
    user_data.control    = 0x00;    // Turn off the charge pump
    user_data.status     = 0x80;    // Shutdown state
    user_data.swBias     = 0x00;    // Turn off all the switches
    user_data.rQpump     = 0x00;    // Set to the lowest scale
    for(i=0;i<8;i++) {
      user_data.rAsum[i] = 0x80;
      user_data.Temp[i]  = 0.0;
    }
    for(i=0;i<64;i++) {
      user_data.rBias[i] = 0xFF;    // Set the DAC to lowest value
      ltc1665mirror[i]      = 0xFF;
    }
  }
  sys_info.group_addr  = 100;

  //-----------------------------------------------------------------------------
  // Local Flags
  bChange = 0;

  //-----------------------------------------------------------------------------
  // Initial setting for communication and overall ports (if needed).
  SFRPAGE  = CONFIG_PAGE;
  P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull

  //-----------------------------------------------------------------------------
  // uC Miscellaneous ADCs (V/I Monitoring)
  adc_internal_init();

  //-----------------------------------------------------------------------------
  //SPI ExtEEPROM
#ifdef _ExtEEPROM_
  SFRPAGE  = CONFIG_PAGE;
  P2MDOUT |= 0x18;  // Set SPI_MOSI and SPI_SCK in PP
  P2MDOUT &= ~0x01; // Set RAM_WPn in OD
  P3MDOUT |= 0x80;  // Set RAM_CSn in PP
  ExtEEPROM_Init();
#endif

  //-----------------------------------------------------------------------------
  // Reset LTC1665 (DACx8), Reset PCA9539 (PIO)
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x5;   // PIO/DACRESETN in PP
  DACRESETN = 1;    // No reset 
  PCARESETN = 1;    // No reset

  //-----------------------------------------------------------------------------
  // Initialize PCA for address reading and switch settings
#ifdef _PCA9539_
  SFRPAGE = CONFIG_PAGE;
//  P1MDOUT |= 0x08;
  PCA9539_Init(); //PCA General I/O (Bias Enables and Backplane Addr) initialization
  delay_us(10);
  //Write to the PCA register for setting 0.x to output
  PCA9539_WriteByte(BIAS_OUTPUT_ENABLE);
  delay_us(10);
  //Set the port pins to high
  PCA9539_WriteByte(BIAS_ENABLE);
  delay_us(10);
  //Write to the PCA register for setting 1.x to input
  PCA9539_WriteByte(BACKPLANE_INPUT_ENABLE);
  delay_us(10);
  
 //-----------------------------------------------------------------------------
  // Physical backplane address retrieval
  PCA9539_Read(BACKPLANE_READ, &pca_add, 1);
  //C C C C C C 0 B B is the MSCB Addr[8..0], 9 bits
  //Modifying what the board reads from the PCA
  //Externally, the crate address are reversed
  crate_add= ((~pca_add)<<1)  & 0x01F8;
  //Internally, the board address are not reversed
  board_address = crate_add | ((pca_add) & 0x0003);
  sys_info.node_addr   = board_address;
#endif

  //-----------------------------------------------------------------------------
  // EEPROM memory Initialization/Retrieval
#ifdef _ExtEEPROM_
  //Read only the serial number from the protected page
  // (without the iBiasOffset)Protected page starts from 0x600, serial number is located in 0x690
  // (with the iBiasOffset) Protected page starts from 0x600, serial number is located in 0x6B0
  ExtEEPROM_Read(SERIALN_ADD,(unsigned char*)&eepage.SerialN, SERIALN_LENGTH);
  user_data.SerialN = eepage.SerialN;
  //Read all other settings from page 0(non-protected)
  ExtEEPROM_Read(PageAddr[0],(unsigned char*)&eepage, PAGE_SIZE);
  DISABLE_INTERRUPTS;
  for(i=0;i<8;i++)
    user_data.rAsum[i] = eepage.rasum[i];
  for(i=0;i<64;i++) {
    user_data.rBias[i] = eepage.rbias[i];
    ltc1665mirror[i] = eepage.rbias[i];
  }
  user_data.rQpump = (unsigned int) (eepage.rqpump & 0x3FF);
  user_data.swBias = eepage.SWbias;
  ENABLE_INTERRUPTS;

  // Update adc2mscb_table[adcChannel].Offst values for current ONLY
  updateAdc2Table();
#endif

  //-----------------------------------------------------------------------------
  // SST Temperatures, setting temperature offsets
#ifdef _ADT7486A_
  SFRPAGE  = CONFIG_PAGE;
  P1MDOUT |= 0x0A; // Set SST_DRV1/2 in PP
  // External 4 SST devices
  SFRPAGE  = CPT1_PAGE;
  CPT1CN  |= 0x80; // Enable the Comparator 1
  CPT1MD   = 0x03; //Comparator1 Mode Selection
  // Local 1 SST device
  SFRPAGE  = CPT0_PAGE;
  CPT0CN  |= 0x80; // Enable the Comparator 0
  CPT0MD   = 0x03; //Comparator0 Mode Selection
 //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)

  ADT7486A_Init(SST_LINE1); //  external devices
  ADT7486A_Init(SST_LINE2); //  local devices

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

  // Missing offset loading for local SST devices.
#endif

  //-----------------------------------------------------------------------------
  // Turn off the card before the timer expires
  switchonoff(OFF);

  //-----------------------------------------------------------------------------
  // Final steps
  rESR = 0x0000;              // No error!
  rEER = 0x00;                    // Active page 0, To be written or read page 0
  rCTL = 0;                       // Reset control
  rCSR = 0x00;                    // Initial CSR
  user_data.error      = rESR;    // Default Error register //NW
  user_data.eepage     = rEER;    // Default EE page
  user_data.status     = rCSR;    // Shutdown state
  user_data.control    = 0x80;    // Manual Shutdown
  user_data.watchdog   = 0;       // Watchdog counter for debugging purpose
  for(i=0;i<8;i++){
    user_data.VBMon[i] = 0;
    user_data.IBMon[i] = 0;
    user_data.rVBMon[i] = 0;
    user_data.rIBMon[i] = 0;
    user_data.Temp[i]  = 0.0;
    user_data.eepValue = 0.0;
    user_data.eeCtrSet = 0;
  }

  // Debug pin on SPARE1
  P2MDOUT |= 0x80;  P2 |= 0x80;  // Timing port in PP = 0
}

//
// Final state of all input pins and their associated registers
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
  if (index == IDXCTL) {
    rCTL = user_data.control;
    if (Ccal) calState = 10;
  } // IDXCTL

  //
  // -- Index Bias Dac
  if (index == IDXBSWITCH) {
#ifdef _PCA9539_
    if (!SsS) PCA_Flag = 1;  // !Shutdown
#endif
  }

  //
  //-- EE Page function
  if (index == IDXEEP_CTL) EEP_CTR_Flag = 1;

  // -- ASUM Threshold DACs
  if ((index >= FIRST_ASUM) && (index < LAST_ASUM)) {
    AsumIndex = (index - FIRST_ASUM);
#ifdef _LTC2600_
    // Update Bias Dac voltages as requested by bit5 of control register
    if (!SsS) LTC2600_Flag = 1;  // !Shutdown
#endif
  }
  //
  // -- Index Bias Dac
  if ((index >= FIRST_BIAS) && (index < LAST_BIAS)) {
#ifdef _LTC1665_
    if (!SsS) LTC1665_Flag = 1;  // Shutdown
#endif
  }
  //
  // --- Index Qpump DAC
#ifdef _LTC1669_  //SqPump && 
  if((index == IDXQVOLT) && !SsS) LTC1669_Flag = 1;   // Qpump on and !Shutdown
#endif

  //
  // --- ASum Hidden Register
  if (index == IDXASUM_CTL) {
    //-PAA- At boot the Sync is high for 1sec while power is up
    // to shutdown sync w 142 0x4
    // to restart       w 142 0x5 (Sync + pwd)
    //                  w 142 0x4 (sync = 0)
    ASUM_SYNC   =  user_data.asumCtl & 0x1;
    ASUM_TESTN  = (user_data.asumCtl & 0x2) >> 1;
    ASUM_PWDN   = (user_data.asumCtl & 0x4) >> 1;
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
  float xdata volt, temperature, adc_value, ftemp;
  //NW make sure pointers are stored in xdata
  float* xdata pfData;
  float* xdata eep_address;
  unsigned long xdata mask;
  signed long xdata result;
  unsigned int xdata eeptemp_addr, *rpfData, rvolt, i;
  //NW make sure eeptemp_source is stored in xdata
  unsigned char* xdata eeptemp_source;
  unsigned char xdata swConversion;
  unsigned char xdata eep_request;
  static  unsigned char xdata eeprom_flag = CLEAR;
  static xdata char adcChannel = 0; // -PAA was 16 ... special start value
  //ltc1665
  char xdata ltc1665_index, ltc1665_chipChan, ltc1665_chipAdd;

  timing=1;

  //External ADC for REV0 cards
  //-----------------------------------------------------------------------------
#ifdef _LTC2497_
  LTC2497_StartConversion(ADDR_LTC2497, adcChannel);
  // Make sure the result is valid!
  if(!LTC2497_ReadConversion(ADDR_LTC2497, adcChannel, &result)) {
    result = -CONVER_FAC1;
  }
  adc_value = ((float)((result) + CONVER_FAC1) * (float)(EXT_VREF / CONVER_FAC2));
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

  //-----------------------------------------------------------------------------
  // External ADC for REV1 cards
  // Increment channel per user_loop
#ifdef _LTC2495_

  LTC2495_StartConversion(ADDR_LTC2495, adcChannel, adc2mscb_table[adcChannel].gain);

  if(!LTC2495_ReadConversion(ADDR_LTC2495, adcChannel, &result, adc2mscb_table[adcChannel].gain)) {
    result = -CONVER_FAC1;
  }

  adc_value = ((float)((result - adc2mscb_table[adcChannel].Offst) + CONVER_FAC1) * (float)(EXT_VREF / CONVER_FAC2));
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
//-PAA
  adcChannel = user_data.eepage;
//  adcChannel++;                 // increment channel
  adcChannel %= N_RB_CHANNEL;   // modulus 16

#endif

  //-----------------------------------------------------------------------------
  // Checking switches flag for toggling the switches
#ifdef _PCA9539_
  if(PCA_Flag) {
    swConversion = user_data.swBias;
    PCA9539_Conversion(&swConversion);
    PCA9539_WriteByte(BIAS_WRITE, ~swConversion);
    PCA_Flag = CLEAR;
  }
#endif

  //-----------------------------------------------------------------------------
  // Checking ASUM flag for updating the asum values
#ifdef _LTC2600_
  if(LTC2600_Flag) {
    LTC2600_Cmd(WriteTo_Update,LTC2600_LOAD[AsumIndex], user_data.rAsum[AsumIndex]);
    LTC2600_Flag = CLEAR;
  }
#endif

  //-----------------------------------------------------------------------------
  // Checking DAC charge pump flag for adjusting the charge pump voltage
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
    rESR = 0x0000;   // Reset error status at each Power UP
    // Clear Status
    SsS = OFF;
    DISABLE_INTERRUPTS;
    user_data.status  = rCSR;
    user_data.error   = rESR;
    ENABLE_INTERRUPTS;
    // Power up Card
    switchonoff(ON);
    delay_ms(100);
    // Force Check on Voltage during loop
    bCPupdoitNOW = ON;
    // Wait for Check before setting SPup
    // Reset Action
    CPup = CLEAR;  // rCTL not updated yet, publish state after U/I check
  } // Power Up

#ifdef _ADT7486A_
  //-----------------------------------------------------------------------------
  // Temperature reading/monitoring based on time for internal temperature
  // Board Temperature
  if ( bCPupdoitNOW || ((uptime() - sstTime) > SST_TIME)) {

    for (channel=0;channel < NCHANNEL_ADT7486A; channel++) {
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetIntTemp, SST_LINE1, &temperature)) {
        RdssT = CLEAR;
        if ((temperature >= eepage.luCTlimit) && (temperature <= eepage.uuCTlimit)) {
          IntssTT = OFF; // in range
        } else {
          IntssTT = ON; // out of range
        }
        DISABLE_INTERRUPTS;
        user_data.ssTemp[channel] = temperature;
        ENABLE_INTERRUPTS;
      } else {
        DISABLE_INTERRUPTS;
        RdssT = ON;     // Error Active
        ENABLE_INTERRUPTS;
      }
    }  // For loop
    sstTime = uptime();
  }  // Internal temperature

  //-----------------------------------------------------------------------------
  // Temperature reading/monitoring based on time for external temperature
  // FGD Temperature
  if (bCPupdoitNOW || ((uptime() - sstExtTime) > SST_TIME)) {
    for (channel=0;channel < NCHANNEL_ADT7486A; channel++){
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt1Temp, SST_LINE1, &temperature)){
        RdssT = CLEAR;
        if ((temperature < eepage.lSSTlimit) || (temperature > eepage.uSSTlimit)) ExtssTT = ON;
        PublishVariable(&(user_data.Temp[(channel*2)]), temperature, ExtssTT);
      } else publishErr(RdssT);
    } // For loop 1

    for (channel=0;channel < NCHANNEL_ADT7486A; channel++) {
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt2Temp, SST_LINE1, &temperature)) {
        RdssT = CLEAR;
        if ((temperature < eepage.lSSTlimit) || (temperature > eepage.uSSTlimit)) ExtssTT = ON;
        PublishVariable(&(user_data.Temp[(channel*2)+1]), temperature, ExtssTT);
      } else publishErr(RdssT);
    }  // For loop 2

    // ext Temp error not yet published...

    sstExtTime = uptime();
  } // External Temperature
#endif

  //-----------------------------------------------------------------------------
  // Periodic check and After power up
  // uCtemperature, Voltage and Current readout
  if ( bCPupdoitNOW || ((uptime() - currentTime) > 1)) {
    //-----------------------------------------------------------------------------
    // uC Temperature reading/monitoring based on time
    // Read uC temperature, set error if temp out of range
    volt = read_voltage(TCHANNEL, &rvolt, 0, 0, IntGAIN1 );
    /* convert to deg. C */
    temperature = 1000 * (volt - 0.776) / 2.86;   // Needs calibration
    /* strip to 0.1 digits */
    temperature = ((int) (temperature * 10 + 0.5)) / 10.0;
    if ((temperature < eepage.luCTlimit) || (temperature > eepage.uuCTlimit)) uCT = ON; // out of range
    PublishVariable(&(user_data.uCTemp), temperature, ExtssTT);

    //-----------------------------------------------------------------------------
    // Internal ADCs monitoring Voltages and Currents based on time
    // All U/I except Qpump, set error if value out of range
    pfData  = &(user_data.VBias);
    rpfData = &(user_data.rVBias);
    // Skip Current limit check if Ccurrent has been set (rCTL:0x4)
    for (channel=0 ; channel<INTERNAL_N_CHN ; channel++) {
      volt = read_voltage(channel, &rvolt, iadc_table[channel].coeff
                                         , iadc_table[channel].offset
                                         , iadc_table[channel].gain );
      DISABLE_INTERRUPTS;
      pfData[channel] = volt;
      rpfData[channel] = rvolt;
      ENABLE_INTERRUPTS;
      mask = (1<<channel); /// Error mask

      // Check for Qpump Voltage ON (> 40V), set error if value out of range
      if (channel == 0)
        if ((volt < eepage.lVIlimit[channel])) SqPump = OFF;
        else SqPump = ON;

      // Skip the first two channels(charge pump)
      if ((channel > 1)) {  // Skip vQ, I
        if ((volt < eepage.lVIlimit[channel]) || (volt > eepage.uVIlimit[channel])) rESR |= mask; // out of range
      } // skip v/iQ channel
    }  // for loop

    // U/I error not yet published ...

    // update time for next loop
    currentTime = uptime();
  }  // Voltage, Current & Temperature test

  //-----------------------------------------------------------------------------
  // Finally take action based on ERROR register
  if (rESR & (VOLTAGE_MASK | UCTEMPERATURE_MASK | BTEMPERATURE_MASK | FGDTEMPERATURE_MASK)) {
    SPup = SqPump = OFF;
    switchonoff(OFF);
    SsS = ON;
  } else if (bCPupdoitNOW) {
    bCPupdoitNOW = OFF;   // Reset flag coming from PowerUp sequence
    SsS = SmSd = OFF;
    SPup = bDacdoitNOW = ON; // Set status and enable DACs writing 
  }

  // Publish Control, Error and Status for all the bdoitNOW actions.
  publishAll();

  //-----------------------------------------------------------------------------
  // Checking APD flag for updating the APD dacs
#ifdef _LTC1665_
  // Get current status of the Card, don't update DAC in case of no Power, keep flag up
  rCSR = user_data.status;    
  if(SPup && LTC1665_Flag) {
    for(ltc1665_index=0; ltc1665_index<64; ltc1665_index++) {
      if(bDacdoitNOW || (ltc1665mirror[ltc1665_index]!= user_data.rBias[ltc1665_index])) {
        ltc1665_chipChan = (ltc1665_index / 8) + 1;
        ltc1665_chipAdd  = (ltc1665_index % 8) + 1;
        LTC1665_Cmd(ltc1665_chipAdd,user_data.rBias[ltc1665_index] , ltc1665_chipChan);
        ltc1665mirror[ltc1665_index] = user_data.rBias[ltc1665_index];
      }
    }
    //LTC1665_Cmd(chipAdd,user_data.rBias[BiasIndex] ,chipChan);
    LTC1665_Flag = CLEAR;
    // Reset the force DAC setting when coming from CPup 
    bDacdoitNOW = OFF;
  }
#endif

  //-----------------------------------------------------------------------------
  // Set Manual Shutdown based on Index
  if (CmSd) {
    rCSR = user_data.status;
    SmSd = ON;  // Set Manual Shutdown
    pca_operation(Q_PUMP_OFF);
    switchonoff(OFF);
    SPup = SqPump = OFF;   // Clear Qpump and card status
    CmSd = CLEAR;          // Reset action
    publishCtlCsr();       // Publish Registers state
  } // Manual Shutdown

  //-----------------------------------------------------------------------------
  // Toggle  Asum counter ON/OFF based on Index
  if (CAsum) {
    if (ASUM_TESTN) ASUM_TESTN = OFF;
    else            ASUM_TESTN = ON;
    SAsum = ASUM_TESTN;   // Set status based on port pin state
    CAsum = CLEAR;        // Reset Action
    publishCtlCsr();      // Publish Registers state
  } // Asum

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
  // EEPROM Save procedure based on CTL bit
#ifdef _ExtEEPROM_
  if (CeeS) {
    // Check if we are here for the first time
    if (!eeprom_flag) {  // first in
      rCSR = user_data.status;

      // Update eepage with the current user_data variables
      for(i=0;i<8;i++)  eepage.rasum[i] = user_data.rAsum[i];
      for(i=0;i<64;i++) eepage.rbias[i] = user_data.rBias[i];
      eepage.rqpump = ((unsigned int) user_data.rQpump & 0x3FF);
      eepage.SWbias = user_data.swBias;

      //Temporary store the first address of page
      eeptemp_addr = PageAddr[(unsigned char)(user_data.eepage & 0x07)];
      //Temporary store the first address of data which has to be written
      eeptemp_source = (unsigned char*)&eepage;
    }

    // EEPROM clear request
    if (CeeClr)  eep_request = WRITE_EEPROM;   //---PAA--- WAS CLEAR BUT WILL ERASE EEPROM!
    else         eep_request = WRITE_EEPROM;

    eeprom_channel = ExtEEPROM_Write_Clear (eeptemp_addr, &(eeptemp_source)
                     , PAGE_SIZE, eep_request, &eeprom_flag);

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
  // EEPROM Restore procedure based on CTL bit
  if (CeeR) {
    rCSR = user_data.status;
    // Read the active page
    channel = ExtEEPROM_Read (PageAddr[(unsigned char)(user_data.eepage & 0x07)], (unsigned char*)&eepage, PAGE_SIZE);
    if (channel == DONE) {
      SeeR = DONE;
 
      // Publish user_data new settings
      DISABLE_INTERRUPTS;
      for(i=0;i<8;i++)  user_data.rAsum[i] = eepage.rasum[i];
      for(i=0;i<64;i++) user_data.rBias[i] = ltc1665mirror[i] = eepage.rbias[i];
      user_data.rQpump = (unsigned int) (eepage.rqpump & 0x3FF);
      user_data.swBias = eepage.SWbias;
      ENABLE_INTERRUPTS;

      bDacdoitNOW = ON;        // Force a DAC Setting on next loop

      // Update adc2mscb_table[adcChannel].Offst values for current and offset
      updateAdc2Table();
    } else  SeeR = FAILED;

    CeeR = CLEAR;       // Reset action
    publishCtlCsr();    // Publish Registers state
  }  // eep Restore
#endif

  //-----------------------------------------------------------------------------
  // Auto calibration for the iBiasOffset
  if (Ccal) {
    switch (calState) {
    case 10:
      // Check Card ON Qpump ON to start with:
      rCSR = user_data.status;
      if (SPup && SqPump && (user_data.swBias == 0xff)) {
        Scal = ON;
        // Publish Registers state
        DISABLE_INTERRUPTS;
        user_data.status  = rCSR;
        ENABLE_INTERRUPTS;
        calState = 15;
      } else { 
        SeeS = CLEAR;
        Ccal = CLEAR;
        Scal = OFF;
        calState = 0;   // unknown state
        // Publish Registers state
        publishCtlCsr();
      }
      break;
    case 15:
      // Remember previous rQpump & Switch and set Cal value
      calQpumpSave = user_data.rQpump;
      calSwSave    = user_data.swBias;
#ifdef _LTC1669_
      LTC1669_SetDAC(ADDR_LTC1669, LTC1669_INT_BG_REF, eepage.calQpump);
#endif
      // Reset eePage Offset array
      for (k=0;k<8;k++) eepage.iBiasOffset[k] = 0.0;
      calState = 30;
      // Reset calTime to current time
      calTime = uptime();
      break;
    case 30:
      // Wait 10 seconds
      if ((uptime() - calTime) > 10) {
        calCount = 0;
        calState = 40;
      }
      break;
    case 40:
      // Read Offset N times
      if (calCount < calNumber) {
        if (adcChannel == 0) {
          calCount++;
          for (k=0;k<8;k++) eepage.iBiasOffset[k] += (float) user_data.rIBMon[k];
        }
      } else calState = 50;
      break;
    case 50:
      // Compute average
      for (k=0;k<8;k++) {
        ftemp = eepage.iBiasOffset[k] / calNumber;
        eepage.iBiasOffset[k] = ftemp;
      }
      // Update ADC Table
      updateAdc2Table();
      calState = 60;
      break;
    case 60:
      // Restore Qpump value
#ifdef _LTC1669_
      LTC1669_SetDAC(ADDR_LTC1669, LTC1669_INT_BG_REF, calQpumpSave);
#endif
      // Clear the Calibration command
      Ccal = CLEAR;
      calState = 0;   // unknown state
      Scal = OFF;
      // eepage written to EEprom
      CeeS = ON;
      // Publish Registers state
      publishCtlCsr();
      break;
    default:
      break;
    }
    DISABLE_INTERRUPTS;
    user_data.debugsmb = calState;
    ENABLE_INTERRUPTS;
  }  // Calibration

  //-----------------------------------------------------------------------------
  // General loop delay
  DISABLE_INTERRUPTS;
  user_data.debugsmb = smbdebug;
  ENABLE_INTERRUPTS;
  timing=0;
  delay_ms(10);
}  // End of User loop
