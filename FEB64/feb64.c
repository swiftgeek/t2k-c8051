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
 define(FEB64) define(_SPI_PROTOCOL_) define(_LTC1665_) define(_SMB_PROTOCOL_) define (_LTC1669_)  define(_
-LTC2600_) define(_SST_PROTOCOL_) define(_ADT7486A_)
 Program Size: data=139.6 xdata=274 code=13497
 
 CODE: 16KB(0x4000), paging in 512B(0x200) 
 CODE(?PR?UPGRADE?MSCBMAIN (0xF600)) 
 
 
 $Id$
 \********************************************************************/
 
 #include <stdio.h>
 #include <math.h>
 #include "mscbemb.h"
 #include "feb64.h"
 
 #include "pca_internal.h"
 #include "adc_internal.h"
 
 #ifdef _PCA9539_
 #include "PCA9539_io.h"
 #endif
 
 #ifdef _LTC1669_
 #include "LTC1669_dac.h"
 #endif
 
 #ifdef _LTC1665_
 #include "LTC1665_dac.h"
 #endif
 
 #ifdef _LTC2600_
 #include "LTC2600_dac.h"
 #endif
 
 #ifdef  _ADT7486A_
 #include "ADT7486A_tsensor.h"
 #endif

 //
 // Global declarations
 //-----------------------------------------------------------------------------
 char code  node_name[] = "FEB64";
 char idata svn_rev_code[] = "$Rev$";
 // declare globally the number of sub-addresses to framework
 unsigned char idata _n_sub_addr = 1;
 unsigned long xdata currentTime=0, sstTime=0;
 unsigned char xdata channel, chipAdd, chipChan;
 unsigned char xdata BiasIndex, AsumIndex;
 unsigned char bdata bChange;
 sbit bdoitNOW = bChange ^ 0;

 // User Data structure declaration
 //-----------------------------------------------------------------------------
 MSCB_INFO_VAR code vars[] = {
   4, UNIT_BYTE,            0, 0,           0, "Error",      &user_data.error,        // 0
   1, UNIT_BYTE,            0, 0,           0, "Control",    &user_data.control,      // 1
   1, UNIT_BYTE,            0, 0,           0, "Status",     &user_data.status,       // 2
   1, UNIT_BYTE,            0, 0,           0, "EEPage",     &user_data.eepage,       // 3
   1, UNIT_BYTE,            0, 0,           0, "swBias",     &user_data.swBias,       // 4
   2, UNIT_BYTE,            0, 0,           0, "rQpump",     &user_data.rQpump,       // 5
  
   2, UNIT_BYTE,            0, 0,           0, "rAsum0",     &user_data.rAsum[0],     // 6
   2, UNIT_BYTE,            0, 0,           0, "rAsum1",     &user_data.rAsum[1],     // 7
   2, UNIT_BYTE,            0, 0,           0, "rAsum2",     &user_data.rAsum[2],     // 8
   2, UNIT_BYTE,            0, 0,           0, "rAsum3",     &user_data.rAsum[3],     // 9
   2, UNIT_BYTE,            0, 0,           0, "rAsum4",     &user_data.rAsum[4],     // 10
   2, UNIT_BYTE,            0, 0,           0, "rAsum5",     &user_data.rAsum[5],     // 11
   2, UNIT_BYTE,            0, 0,           0, "rAsum6",     &user_data.rAsum[6],     // 12
   2, UNIT_BYTE,            0, 0,           0, "rAsum7",     &user_data.rAsum[7],     // 13
  
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBias",      &user_data.VBias,        // 14
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "IBias",      &user_data.IBias,        // 15
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "pDVMon",     &user_data.pDVMon,       // 16
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "pAVMon",     &user_data.pAVMon,       // 17
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "nAVMon",     &user_data.nAVMon,       // 18
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "nAIMon",     &user_data.nAIMon,       // 19
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "pAIMon",     &user_data.pAIMon,       // 20
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "pDIMon",     &user_data.pDIMon,       // 21
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "uCTemp",     &user_data.uCTemp,       // 22
 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp0",      &user_data.Temp[0],      // 23
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp1",      &user_data.Temp[1],      // 24
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp2",      &user_data.Temp[2],      // 25
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp3",      &user_data.Temp[3],      // 26
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp4",      &user_data.Temp[4],      // 27
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp5",      &user_data.Temp[5],      // 28
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp6",      &user_data.Temp[6],      // 29
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp7",      &user_data.Temp[7],      // 30
 
   1,UNIT_BYTE,             0, 0,           0, "rBias00",    &user_data.rBias[0],     // 31
   1,UNIT_BYTE,             0, 0,           0, "rBias01",    &user_data.rBias[1],     // 32
   1,UNIT_BYTE,             0, 0,           0, "rBias02",    &user_data.rBias[2],     // 33
   1,UNIT_BYTE,             0, 0,           0, "rBias03",    &user_data.rBias[3],     // 34
   1,UNIT_BYTE,             0, 0,           0, "rBias04",    &user_data.rBias[4],     // 35
   1,UNIT_BYTE,             0, 0,           0, "rBias05",    &user_data.rBias[5],     // 36
   1,UNIT_BYTE,             0, 0,           0, "rBias06",    &user_data.rBias[6],     // 37
   1,UNIT_BYTE,             0, 0,           0, "rBias07",    &user_data.rBias[7],     // 38
   1,UNIT_BYTE,             0, 0,           0, "rBias08",    &user_data.rBias[8],     // 39
   1,UNIT_BYTE,             0, 0,           0, "rBias09",    &user_data.rBias[9],     // 40
   1,UNIT_BYTE,             0, 0,           0, "rBias10",    &user_data.rBias[10],    // 41
   1,UNIT_BYTE,             0, 0,           0, "rBias11",    &user_data.rBias[11],    // 42
   1,UNIT_BYTE,             0, 0,           0, "rBias12",    &user_data.rBias[12],    // 43
   1,UNIT_BYTE,             0, 0,           0, "rBias13",    &user_data.rBias[13],    // 44
   1,UNIT_BYTE,             0, 0,           0, "rBias14",    &user_data.rBias[14],    // 45
   1,UNIT_BYTE,             0, 0,           0, "rBias15",    &user_data.rBias[15],    // 46
   1,UNIT_BYTE,             0, 0,           0, "rBias16",    &user_data.rBias[16],    // 47
   1,UNIT_BYTE,             0, 0,           0, "rBias17",    &user_data.rBias[17],    // 48
   1,UNIT_BYTE,             0, 0,           0, "rBias18",    &user_data.rBias[18],    // 49
   1,UNIT_BYTE,             0, 0,           0, "rBias19",    &user_data.rBias[19],    // 50
   1,UNIT_BYTE,             0, 0,           0, "rBias20",    &user_data.rBias[20],    // 51
   1,UNIT_BYTE,             0, 0,           0, "rBias21",    &user_data.rBias[21],    // 52
   1,UNIT_BYTE,             0, 0,           0, "rBias22",    &user_data.rBias[22],    // 53
   1,UNIT_BYTE,             0, 0,           0, "rBias23",    &user_data.rBias[23],    // 54
   1,UNIT_BYTE,             0, 0,           0, "rBias24",    &user_data.rBias[24],    // 55
   1,UNIT_BYTE,             0, 0,           0, "rBias25",    &user_data.rBias[25],    // 56
   1,UNIT_BYTE,             0, 0,           0, "rBias26",    &user_data.rBias[26],    // 57
   1,UNIT_BYTE,             0, 0,           0, "rBias27",    &user_data.rBias[27],    // 58
   1,UNIT_BYTE,             0, 0,           0, "rBias28",    &user_data.rBias[28],    // 59
   1,UNIT_BYTE,             0, 0,           0, "rBias29",    &user_data.rBias[29],    // 60
   1,UNIT_BYTE,             0, 0,           0, "rBias30",    &user_data.rBias[30],    // 61
   1,UNIT_BYTE,             0, 0,           0, "rBias31",    &user_data.rBias[31],    // 62
   1,UNIT_BYTE,             0, 0,           0, "rBias32",    &user_data.rBias[32],    // 63
   1,UNIT_BYTE,             0, 0,           0, "rBias33",    &user_data.rBias[33],    // 64
   1,UNIT_BYTE,             0, 0,           0, "rBias34",    &user_data.rBias[34],    // 65
   1,UNIT_BYTE,             0, 0,           0, "rBias35",    &user_data.rBias[35],    // 66
   1,UNIT_BYTE,             0, 0,           0, "rBias36",    &user_data.rBias[36],    // 67
   1,UNIT_BYTE,             0, 0,           0, "rBias37",    &user_data.rBias[37],    // 68
   1,UNIT_BYTE,             0, 0,           0, "rBias38",    &user_data.rBias[38],    // 69
   1,UNIT_BYTE,             0, 0,           0, "rBias39",    &user_data.rBias[39],    // 70
   1,UNIT_BYTE,             0, 0,           0, "rBias40",    &user_data.rBias[40],    // 71
   1,UNIT_BYTE,             0, 0,           0, "rBias41",    &user_data.rBias[41],    // 72
   1,UNIT_BYTE,             0, 0,           0, "rBias42",    &user_data.rBias[42],    // 73
   1,UNIT_BYTE,             0, 0,           0, "rBias43",    &user_data.rBias[43],    // 74
   1,UNIT_BYTE,             0, 0,           0, "rBias44",    &user_data.rBias[44],    // 75
   1,UNIT_BYTE,             0, 0,           0, "rBias45",    &user_data.rBias[45],    // 76
   1,UNIT_BYTE,             0, 0,           0, "rBias46",    &user_data.rBias[46],    // 77
   1,UNIT_BYTE,             0, 0,           0, "rBias47",    &user_data.rBias[47],    // 78
   1,UNIT_BYTE,             0, 0,           0, "rBias48",    &user_data.rBias[48],    // 79
   1,UNIT_BYTE,             0, 0,           0, "rBias49",    &user_data.rBias[49],    // 80
   1,UNIT_BYTE,             0, 0,           0, "rBias50",    &user_data.rBias[50],    // 81
   1,UNIT_BYTE,             0, 0,           0, "rBias51",    &user_data.rBias[51],    // 82
   1,UNIT_BYTE,             0, 0,           0, "rBias52",    &user_data.rBias[52],    // 83
   1,UNIT_BYTE,             0, 0,           0, "rBias53",    &user_data.rBias[53],    // 84
   1,UNIT_BYTE,             0, 0,           0, "rBias54",    &user_data.rBias[54],    // 85
   1,UNIT_BYTE,             0, 0,           0, "rBias55",    &user_data.rBias[55],    // 86
   1,UNIT_BYTE,             0, 0,           0, "rBias56",    &user_data.rBias[56],    // 87
   1,UNIT_BYTE,             0, 0,           0, "rBias57",    &user_data.rBias[57],    // 88
   1,UNIT_BYTE,             0, 0,           0, "rBias58",    &user_data.rBias[58],    // 89
   1,UNIT_BYTE,             0, 0,           0, "rBias59",    &user_data.rBias[59],    // 90
   1,UNIT_BYTE,             0, 0,           0, "rBias60",    &user_data.rBias[60],    // 91
   1,UNIT_BYTE,             0, 0,           0, "rBias61",    &user_data.rBias[61],    // 92
   1,UNIT_BYTE,             0, 0,           0, "rBias62",    &user_data.rBias[62],    // 93
   1,UNIT_BYTE,             0, 0,           0, "rBias63",    &user_data.rBias[63],    // 94
 
   /*
   4, UNIT_BYTE,      0, 0,     0,  "rVBias",   &user_data.rVBias,   // 78
   4, UNIT_BYTE,      0, 0,     0,  "rIBias",  &user_data.rIBias,    // 79
   4, UNIT_BYTE,      0, 0,     0,  "rpAV",    &user_data.rpAV,    // 80
   4, UNIT_BYTE,      0, 0,     0,  "rpAI",    &user_data.rpAI,    // 81
   4, UNIT_BYTE,      0, 0,     0,  "rpDV",    &user_data.rpDV,    // 82
   4, UNIT_BYTE,      0, 0,     0,  "rpDI",    &user_data.rpDI,    // 83
   4, UNIT_BYTE,      0, 0,     0,  "rnAV",    &user_data.rnAV,    // 84
   4, UNIT_BYTE,      0, 0,     0,  "rnAI",    &user_data.rnAI,    // 85
 
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon0",   &user_data.VBMon[0], // 94
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon1",   &user_data.VBMon[1], // 95
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon2",   &user_data.VBMon[2], // 96
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon3",   &user_data.VBMon[3], // 97
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon4",   &user_data.VBMon[4], // 98
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon5",   &user_data.VBMon[5], // 99
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon6",   &user_data.VBMon[6], // 100
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VBMon7",   &user_data.VBMon[7], // 101
 
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBMon0",   &user_data.IBMon[0], // 102
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBMon1",   &user_data.IBMon[1], // 103
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBMon2",   &user_data.IBMon[2], // 104
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBMon3",   &user_data.IBMon[3], // 105
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBMon4",   &user_data.IBMon[4], // 106
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBMon5",   &user_data.IBMon[5], // 107
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBMon6",   &user_data.IBMon[6], // 108
   4, UNIT_AMPERE, PRFX_MILLI, 0, MSCBF_FLOAT, "IBMon7",   &user_data.IBMon[7], // 109
 
   4, UNIT_BYTE,      0, 0,     0,  "rVBMon0",  &user_data.rVBMon[0], // 110
   4, UNIT_BYTE,      0, 0,     0,  "rVBMon1",  &user_data.rVBMon[1], // 111
   4, UNIT_BYTE,      0, 0,     0,  "rVBMon2",  &user_data.rVBMon[2], // 112
   4, UNIT_BYTE,      0, 0,     0,  "rVBMon3",  &user_data.rVBMon[3], // 113
   4, UNIT_BYTE,      0, 0,     0,  "rVBMon4",  &user_data.rVBMon[4], // 114
   4, UNIT_BYTE,      0, 0,     0,  "rVBMon5",  &user_data.rVBMon[5], // 115
   4, UNIT_BYTE,      0, 0,     0,  "rVBMon6",  &user_data.rVBMon[6], // 116
   4, UNIT_BYTE,      0, 0,     0,  "rVBMon7",  &user_data.rVBMon[7], // 117
 
   4, UNIT_BYTE,      0, 0,     0,  "rIBMon0",  &user_data.rIBMon[0], // 118
   4, UNIT_BYTE,      0, 0,     0,  "rIBMon1",  &user_data.rIBMon[1], // 119
   4, UNIT_BYTE,      0, 0,     0,  "rIBMon2",  &user_data.rIBMon[2], // 120
   4, UNIT_BYTE,      0, 0,     0,  "rIBMon3",  &user_data.rIBMon[3], // 121
   4, UNIT_BYTE,      0, 0,     0,  "rIBMon4",  &user_data.rIBMon[4], // 122
   4, UNIT_BYTE,      0, 0,     0,  "rIBMon5",  &user_data.rIBMon[5], // 123
   4, UNIT_BYTE,      0, 0,     0,  "rIBMon6",  &user_data.rIBMon[6], // 124
   4, UNIT_BYTE,      0, 0,     0,  "rIBMon7",  &user_data.rIBMon[7], // 125
 
   */
   0
 };
 MSCB_INFO_VAR *variables = vars;   // Structure mapping
 //MSCB_INFO_VAR *xdataStruct = xStruct;
 // Get sysinfo if necessary
 extern SYS_INFO sys_info;
 
 unsigned char ADT7486A_addrArray[] = {ADT7486A_ADDR_ARRAY0,ADT7486A_ADDR_ARRAY1,ADT7486A_ADDR_ARRAY2,ADT7486A_ADDR_ARRAY3};
 
 
 /********************************************************************\
 Application specific init and in/output routines
 \********************************************************************/
 
 /*---- User init function ------------------------------------------*/
 void user_init(unsigned char init)
 {
   char xdata i, add;
 
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
 
   add = cur_sub_addr();
 
 // Initialize control
   user_data.control = 0;
 
 //
 // default settings, set only when uC-EEPROM is being erased and written
 //-----------------------------------------------------------------------------
   if(init)
   {
     user_data.control    = 0x00;    //Turn off the charge pump
     user_data.status     = 0x80;    // Shutdown state
     user_data.swBias     = 0x00;    //Turn off all the switches
     user_data.rQpump     = 0x00;    //Set to the lowest scale
     for(i=0;i<8;i++)
       user_data.rAsum[i] = 0x80;    //BS not sure to what value initialize it
     for(i=0;i<64;i++)
       user_data.rBias[i] = 0xFF;   //Set the DAC to lowest value
     sys_info.group_addr  = 400;
     sys_info.node_addr   = 0xFF00;
   }
 
 //
 // Initial setting for communication and overall ports (if needed).
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   // P0MDOUT contains Tx in push-pull
   P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull
 
 //
 // uC Charge Pump frequency setup
 //-----------------------------------------------------------------------------
   pca_operation(Q_PUMP_INIT); //Charge Pump initialization (crossbar settings)
   pca_operation(Q_PUMP_OFF);   //Initially turn it off
 
 //
 // uC Miscellaneous ADCs (V/I Monitoring)
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   P3MDOUT |= 0x1C; //Setting the Regulators control pins to push pull (3 Vreg)
   adc_internal_init();
  
 //
 // SST Temperatures
 //-----------------------------------------------------------------------------
 #ifdef _ADT7486A_
   SFRPAGE  = CONFIG_PAGE;
   P1MDOUT |= 0x01; // Setting the SST_DRV (SST) to push pull
   SFRPAGE  = CPT1_PAGE;
   CPT1CN  |= 0x80; // Enable the Comparator 1
   CPT1MD   = 0x02; //Comparator1 Mode Selection
   //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)
 
   ADT7486A_Init();
 #endif
 
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
 // SMB Charge Pump DAC
 //-----------------------------------------------------------------------------
 #ifdef _LTC1669_
   LTC1669_Init(); //I2C DAC initialization (D_CH_PUMP)
 #endif
 
 //
 // SMB Bias Voltage switches
 //-----------------------------------------------------------------------------
 #ifdef _PCA9539_
   PCA9539_Init(); //PCA General I/O (Bais Enables and Backplane Addr) initialization
 #endif
 
 //
 // Physical address retrieval
 //-----------------------------------------------------------------------------
 
 //
 // EEPROM memory Initialization/Retrieval
 //-----------------------------------------------------------------------------
 
 //
 // Final steps 
 //-----------------------------------------------------------------------------
   rESR = 0x00000000;
   rEER = 0xC2;     // Page 3, Active page 2
	rCTL = 0;
	rCSR = 0x00;    //
	user_data.control    = 0x80;    // Manual Shutdown
	user_data.error      = rESR;    // Default Error register
	user_data.eepage     = rEER;    // Default EE page
	user_data.status     = rCSR;    // Shutdown state
	user_data.swBias     = 0x00;    //Turn off all the switches
	user_data.rQpump     = 0x00;    //Set to the lowest scale
	for(i=0;i<8;i++)
	  user_data.rAsum[i] = i;       //BS not sure to what value initialize it
	for(i=0;i<64;i++) 
	user_data.rBias[i] = i;         //Set the DAC to lowest value

   EN_pD5V  = ON;
   EN_pA5V  = OFF;
   EN_nA5V  = OFF;

 }
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
       PCA9539_Cmd(ADDR_PCA9539, ConfigPort0, user_data.swBias, 0, PCA9539_WRITE);
     } // !Shutdown
#endif
   }
//
// -- ASUM Threshold DACs
   if ((index >= FIRST_ASUM) && (index < LAST_ASUM)) {
     AsumIndex = (index - FIRST_ASUM);
#ifdef _LTC2600_
   // Update Bias Dac voltages as requested by bit5 of control register
     LTC2600_Cmd(LTC2600_LOAD_H, user_data.rAsum[AsumIndex]);
#endif
   }
//
// -- Index Bias Dac
   if ((index >= FIRST_BIAS) && (index < LAST_BIAS)) {
     BiasIndex = (index - FIRST_BIAS);
     chipChan = (BiasIndex / 8) + 1;
     chipAdd  = (BiasIndex % 8) + 1;
#ifdef _LTC1665_
     LTC1665_Cmd(chipAdd,user_data.rBias[BiasIndex] ,chipChan);
#endif
  }
//
// --- Index Qpump DAC
 #ifdef _LTC1669_
   if(SqPump) {
     LTC1669_Cmd1(ADDR_LTC1669, user_data.rQpump, LTC1669_WRITE);
   }
 #endif
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
  float xdata volt, temperature, *uiMonitoring;
  unsigned long xdata mask;

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
      EN_pD5V = ON;
      EN_pA5V = ON;
      EN_nA5V = ON;
      delay_ms(10);
	   // Force Check on Voltage 
      bdoitNOW = ON;
      // Wait for Check before setting SPup
    } // !Shutdown

    // Reset Action        
    CPup = 0;  // rCTL not updated yet
    // Publish state after V/U check

  } // Power Up 

  if ( bdoitNOW || (uptime() - currentTime) > 1) {
    //-----------------------------------------------------------------------------
    //
    // Internal ADCs monitoring Voltages and Currents based on time
    //
    // Time to do V/I Reg reading
    uiMonitoring = &(user_data.VBias);
    rCSR = user_data.status;
    rESR = user_data.error;
    for (channel=0, mask=0 ; channel<INTERNAL_N_CHN ; channel++, mask++) {
      volt = read_voltage(channel);
      volt = volt * coeff[channel] + offset[channel];
      DISABLE_INTERRUPTS;
      uiMonitoring [channel] = volt;
      ENABLE_INTERRUPTS;
      mask = (1<<channel);  // Should be 4 bytes for ESR 
 		if ((channel > 1) && !SqPump) {  // Skip vQ, I
		   if ((volt >= eepage.lVIlimit[channel])
		     && (volt <= eepage.uVIlimit[channel])) {
		       rESR &= ~mask; // in range
		   }
		   else {
		     rESR |= mask; // out of range
		   }      
		}
    }
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
    user_data.error   = rESR;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;

    //-----------------------------------------------------------------------------
    //
    // uC Temperature reading/monitoring based on time
    rCSR = user_data.status;
    rESR = user_data.error;
    // Read uC temperature
    volt = read_voltage(TCHANNEL);
    /* convert to deg. C */
    temperature = 1000 * (volt - 0.776) / 2.86;   // Needs calibration
    /* strip to 0.1 digits */
    temperature = ((int) (temperature * 10 + 0.5)) / 10.0;
    DISABLE_INTERRUPTS;
    user_data.uCTemp = (float) temperature;
    ENABLE_INTERRUPTS;
    if ((volt >= eepage.luCTlimit) && (volt <= eepage.uuCTlimit)) {
      uCT = OFF; // in range
    } else {
      uCT = ON; // out of range
    }      
    // Set Shutdown if Error in System Register 
    if ((rESR & SHUTDOWN_MASK) && !SmSd) {
      pca_operation(Q_PUMP_OFF);  // Toggle state
      SqPump   = OFF;
      SsS = ON;
      SPup = OFF;  
    } else {
      SsS = OFF; // Remove System Shutdown
    }
    // Publish Error state
    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.error   = rESR;
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
      EN_pD5V  = ON;  // For debugging
      EN_pA5V  = OFF;
      EN_nA5V  = OFF;
      SqPump   = OFF;
      SPup     = OFF;
    } // Manula Shutdown
    // Reset Action        
    CmSd = 0;  // rCTL not yet published

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
    CqPump = 0;

    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  } // Control Pump


  //-----------------------------------------------------------------------------
  // Temperature reading/monitoring based on time
#ifdef _ADT7486A_
  if (uptime() - sstTime > 10) {
    for (channel=0;channel < ADT7486A_NUM; channel++) {
      ADT7486A_Cmd(ADT7486A_addrArray[channel]  , GetIntTemp
        , &user_data.Temp[channel*2]);
      ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt2Temp
        , &user_data.Temp[channel*2+1]);
    }
    sstTime = uptime();
  }
#endif

  //-----------------------------------------------------------------------------
  // EEPROM Save procedure based on CTL bit
  if (CeeS) {
    rCSR = user_data.status;
    //    channel = eeprom_write((user_data.eepage & 0xC0) >> 6);
    if (channel) {
      SeeS = DONE;
    } else {
      SeeS = FAILED;
    }

    // Reset Action        
    CeeS = 0;  // rCTL not yet published
    // Publish Qpump state
    // Publish Registers state
    DISABLE_INTERRUPTS;
    user_data.control = rCTL;
    user_data.status  = rCSR;
    ENABLE_INTERRUPTS;
  }

  //-----------------------------------------------------------------------------
  // EEPROM Restore procedure based on CTL bit
  if (CeeR) {
    rCSR = user_data.status;
    //    channel = eeprom_read((user_data.eepage & 0xC0) >> 6);
    if (channel)
      SeeR = DONE;
    else
      SeeR = FAILED;
    // Reset Action
    CeeS = 0;  // rCTL not yet published
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
