/********************************************************************\
 Name:         temp36.c
 Created by:   Noel Wu                May/07/2008


 Contents:     Application specific (user) part of
 Midas Slow Control Bus protocol for temp36 board
 
 $Id$
 \********************************************************************/

 #include <stdio.h>
 #include <math.h>
 #include "mscbemb.h"
 #include "temp36.h"

 #ifdef _ADT7486A_
 #include "Devices/ADT7486A_tsensor.h"
 #endif

 #ifdef _ExtEEPROM_
 #include "Devices/ExtEEPROM.h"
 #endif

 #ifdef _HUMSEN_
 #include "Devices/Humidity_sensor.h"
 #endif
   
 #ifdef _LTC2600_
 #include "Devices/LTC2600_dac.h"
 #endif
 
 #ifdef _ADC_INTERNAL_
 #include "Devices/adc_internal.h"
 #endif

  // Declare globally the number of sub-addresses to framework
 unsigned char idata _n_sub_addr = 1;
 
 // Global declarations
 //-----------------------------------------------------------------------------
 char code  node_name[] = "temp36";
 char idata svn_rev_code[] = "$Rev$";
 
 // Local flag
 bit EEP_CTR_Flag;
 bit LTC2600_Flag;
 bit REF_Flag;

 //EEPROM variables
 int xdata * eep_address;
 static unsigned char tcounter;
 static unsigned char xdata eeprom_flag=CLEAR; 
 unsigned char xdata eeprom_wstatus, eeprom_rstatus;
 unsigned int  xdata eeptemp_addr;
 unsigned char xdata * eeptemp_source;
 unsigned char xdata eep_request;

 //Humidity variables
 unsigned char xdata status;
 float xdata humidity, htemperature;
 unsigned int xdata  rSHTtemp1, rSHThumi1, rSHTtemp2, rSHThumi2;
 unsigned char xdata FCSorig1, FCSdevi1, FCSorig2, FCSdevi2;

 //Temperature variables
 unsigned char xdata cavg=0;
 unsigned char idata channel, avgcount, numavg=0;
 float xdata average;
 float xdata temperature;

#ifdef _LTC2600_
 //DAC variables
 unsigned char xdata DACIndex;
 unsigned char xdata LTC2600_LOAD[] = {LTC2600_LOAD_A,LTC2600_LOAD_B
                          ,LTC2600_LOAD_C,LTC2600_LOAD_D
                          ,LTC2600_LOAD_E,LTC2600_LOAD_F
                          ,LTC2600_LOAD_G,LTC2600_LOAD_H};
#endif
                          
 // Get sysinfo if necessary
 extern SYS_INFO sys_info;          // For address setting


 // User Data structure declaration
 //-----------------------------------------------------------------------------
 MSCB_INFO_VAR code vars[] = {
   4, UNIT_BYTE,            0, 0,           0, "SerialN",    &user_data.SerialN,       //0
   2, UNIT_BYTE,            0, 0,           0, "Error",      &user_data.error,         //1
   1, UNIT_BYTE,            0, 0,           0, "Control",    &user_data.control,       //2
   1, UNIT_BYTE,            0, 0,           0, "Status",     &user_data.status,        //3
   1, UNIT_BYTE,            0, 0,           0, "EEPage",     &user_data.eepage,        //4
   2, UNIT_BYTE,            0, 0,           0, "Navge",        &user_data.navge,       //5     
   1, UNIT_BYTE,            0, 0,           0, "TE01-08",      &user_data.terror[0],   //6 
   1, UNIT_BYTE,            0, 0,           0, "TE09-16",      &user_data.terror[1],   //7 
   1, UNIT_BYTE,            0, 0,           0, "TE17-24",      &user_data.terror[2],   //8 
   1, UNIT_BYTE,            0, 0,           0, "TE25-32",      &user_data.terror[3],   //9 
   1, UNIT_BYTE,            0, 0,           0, "TE33-36",      &user_data.terror[4],   //10 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp01",       &user_data.Temp[0],     //11
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp02",       &user_data.Temp[1],     //12
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp03",       &user_data.Temp[2],     //13
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp04",       &user_data.Temp[3],     //14
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp05",       &user_data.Temp[4],     //15
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp06",       &user_data.Temp[5],     //16
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp07",       &user_data.Temp[6],     //17
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp08",       &user_data.Temp[7],     //18
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp09",       &user_data.Temp[8],     //19
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp10",       &user_data.Temp[9],     //20
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp11",       &user_data.Temp[10],    //21
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp12",       &user_data.Temp[11],    //22
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp13",       &user_data.Temp[12],    //23
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp14",       &user_data.Temp[13],    //24
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp15",       &user_data.Temp[14],    //25
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp16",       &user_data.Temp[15],    //26
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp17",       &user_data.Temp[16],    //27
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp18",       &user_data.Temp[17],    //28
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp19",       &user_data.Temp[18],    //29
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp20",       &user_data.Temp[19],    //30
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp21",       &user_data.Temp[20],    //31
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp22",       &user_data.Temp[21],    //32
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp23",       &user_data.Temp[22],    //33
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp24",       &user_data.Temp[23],    //34
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp25",       &user_data.Temp[24],    //35
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp26",       &user_data.Temp[25],    //36
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp27",       &user_data.Temp[26],    //37
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp28",       &user_data.Temp[27],    //38
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp29",       &user_data.Temp[28],    //39
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp30",       &user_data.Temp[29],    //40
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp31",       &user_data.Temp[30],    //41
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp32",       &user_data.Temp[31],    //42
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp33",       &user_data.Temp[32],    //43
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp34",       &user_data.Temp[33],    //44
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp35",       &user_data.Temp[34],    //45
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Temp36",       &user_data.Temp[35],    //46

   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "REF",          &user_data.ref   ,      //47

   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg01",       &user_data.AT[0],       //48
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg02",       &user_data.AT[1],       //49
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg03",       &user_data.AT[2],       //50
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg04",       &user_data.AT[3],       //51
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg05",       &user_data.AT[4],       //52 
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg06",       &user_data.AT[5],       //53
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg07",       &user_data.AT[6],       //54
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg08",       &user_data.AT[7],       //55
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg09",       &user_data.AT[8],       //56
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg10",       &user_data.AT[9],       //57
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg11",       &user_data.AT[10],      //58
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg12",       &user_data.AT[11],      //59
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg13",       &user_data.AT[12],      //60
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg14",       &user_data.AT[13],      //61
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg15",       &user_data.AT[14],      //62
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg16",       &user_data.AT[15],      //63
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg17",       &user_data.AT[16],      //64
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg18",       &user_data.AT[17],      //65
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Tavg19",       &user_data.AT[18],      //66
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg20",       &user_data.AT[19],      //67
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg21",       &user_data.AT[20],      //68
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg22",       &user_data.AT[21],      //69
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg23",       &user_data.AT[22],      //70
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg24",       &user_data.AT[23],      //71
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg25",       &user_data.AT[24],      //72
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg26",       &user_data.AT[25],      //73
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg27",       &user_data.AT[26],      //74
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg28",       &user_data.AT[27],      //75
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg29",       &user_data.AT[28],      //76
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg30",       &user_data.AT[29],      //77
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg31",       &user_data.AT[30],      //78
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg32",       &user_data.AT[31],      //79
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg33",       &user_data.AT[32],      //80
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg34",       &user_data.AT[33],      //81
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg35",       &user_data.AT[34],      //82
   4, UNIT_CELSIUS,         0, 0, MSCBF_HIDDEN|MSCBF_FLOAT, "Tavg36",       &user_data.AT[35],      //83

   4, UNIT_BYTE,            0, 0,MSCBF_HIDDEN,"eepValue",      &user_data.eepValue,    //84
   4, UNIT_BYTE,            0, 0,MSCBF_HIDDEN,"eeCtrSet",      &user_data.eeCtrSet,    //85

   4, UNIT_CELSIUS, 0, 0,MSCBF_FLOAT, "SHTtemp",  &user_data.SHTtemp1,    //86
   4, 10,           0, 0,MSCBF_FLOAT, "SHThumi",  &user_data.SHThumi1,    //87   
   4, UNIT_CELSIUS, 0, 0,MSCBF_FLOAT, "SHTtemp2", &user_data.SHTtemp2,    //88
   4, 10,           0, 0,MSCBF_FLOAT, "SHThumi2", &user_data.SHThumi2,    //89  

   2, UNIT_BYTE,            0, 0,MSCBF_HIDDEN, "DAC0",         &user_data.DAC[0],      //90
   2, UNIT_BYTE,            0, 0,MSCBF_HIDDEN, "DAC1",         &user_data.DAC[1],      //91
   2, UNIT_BYTE,            0, 0,MSCBF_HIDDEN, "DAC2",         &user_data.DAC[2],      //92
   2, UNIT_BYTE,            0, 0,MSCBF_HIDDEN, "DAC3",         &user_data.DAC[3],      //93
   2, UNIT_BYTE,            0, 0,MSCBF_HIDDEN, "DAC4",         &user_data.DAC[4],      //94
   2, UNIT_BYTE,            0, 0,MSCBF_HIDDEN, "DAC5",         &user_data.DAC[5],      //95
   2, UNIT_BYTE,            0, 0,MSCBF_HIDDEN, "DAC6",         &user_data.DAC[6],      //96
   2, UNIT_BYTE,            0, 0,MSCBF_HIDDEN, "DAC7",         &user_data.DAC[7],      //97 
  
  0
 };
 
 MSCB_INFO_VAR *variables = vars;   // Structure mapping

 
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

// Offset calibration
//-----------------------------------------------------------------------------
void autocalibration(float reference)
{
  float buffer;
  int offset, i;
  for(i=0;i<18; i++){
    //offset channel 2
     buffer=reference-user_data.AT[2*i];
     //Every increment of 16 corresponds to a quarter degree change
     buffer*=4;
     offset=(int)(buffer*16);
    eepage.ext2offset[i]=offset;  
    //offset channel 1
     buffer=reference-user_data.AT[2*i+1];
     //Every increment of 16 corresponds to a quarter degree change
     buffer*=4;
     offset=(int)(buffer*16);   
    eepage.ext1offset[i]=offset;
  }
  return;
}

// converts channel index to eepage structure offset address
//-----------------------------------------------------------------------------
int eepageAddrConvert(unsigned int index)
{
  int add;
  //if index is even
  if(!(index%2))
    add = index/2 + 18 + EEP_RW_IDX; // offset from the eepage struct
    //if index is odd
  else
    add = index/2 + EEP_RW_IDX;  // offset from the eepage struct
  return add;
}

/*---- User init function ------------------------------------------*/
void user_init(unsigned char init)
{
   unsigned char idata i;

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

  if(init) {}

 // Initial setting for communication and overall ports 
 //-----------------------------------------------------------------------------
 // External EEPROM chip
#ifdef _ExtEEPROM_
   SFRPAGE  = CONFIG_PAGE;
   P2MDOUT |= 0x31; // Setting the RAM_CSn. SPI_MOSI, SPI_SCK to push pull
   P2MDOUT &= 0xFB; // Setting the RAM_WPn to open drain
   ExtEEPROM_Init();

   //Get serial number from write protected area 
   ExtEEPROM_Read  (WP_START_ADDR, (unsigned char xdata *)&eepage ,PAGE_SIZE);
   DISABLE_INTERRUPTS;
   user_data.SerialN = eepage.SerialN;
   ENABLE_INTERRUPTS;

  //Refer to the first Page of the non_protected area
   ExtEEPROM_Read  (PageAddr[0] ,(unsigned char xdata *)&eepage ,PAGE_SIZE);
   DISABLE_INTERRUPTS;
   user_data.navge = eepage.navge;
   ENABLE_INTERRUPTS;

#endif

//
// ADT7486A Init
#ifdef _ADT7486A_
   SFRPAGE  = CONFIG_PAGE;
   P0MDOUT |= 0x18; // Setting the SST_DRV (SST) to push pull
   SFRPAGE  = CPT1_PAGE;
   CPT1CN  |= 0x80; // Enable the Comparator 1
   CPT1MD   = 0x03; //Comparator1 Mode Selection
   SFRPAGE  = CPT0_PAGE;
   CPT0CN  |= 0x80; // Enable the Comparator 0
   CPT0MD   = 0x03; //Comparator0 Mode Selection
   //Use default, adequate TYP (CP1 Response Time, no edge triggered interrupt)   
   ADT7486A_Init(SST_LINE1); //Temperature measurements related initialization
   ADT7486A_Init(SST_LINE2); 

// Setting the offset for the temperature sensors
   for (channel=0;channel < 9; channel++){
    // Set the offset for the temp01,03...17 sensors 
    ADT7486A_Cmd(ADT7486A_addrArray[channel]
            , SetExt2Offset
            , (eepage.ext2offset[channel]>>8)
            , eepage.ext2offset[channel]
            , SST_LINE1
            , &temperature);
      delay_us(100); //wait for ADT7486A
   }
   delay_us(300);
   for (channel=9;channel < 18; channel++){
    //Set the offset for the temp19,21...35 sensors
     ADT7486A_Cmd(ADT7486A_addrArray[channel-9]
               , SetExt2Offset
            , (eepage.ext2offset[channel]>>8)
            , eepage.ext2offset[channel]
            , SST_LINE2
            , &temperature);      
      delay_us(100); //wait for ADT7486A
   }                                                                   
   delay_us(300);
   for (channel=0;channel < 9; channel++){
    //Set the offset for the temp02,04...18 sensors
      ADT7486A_Cmd(ADT7486A_addrArray[channel]
               , SetExt1Offset
            , (eepage.ext1offset[channel]>>8)
            , eepage.ext1offset[channel]
            , SST_LINE1
            , &temperature);
      delay_us(100); //wait for ADT7486A
   }
   delay_us(300);
   for (channel=9;channel < 18; channel++){
    //Set the offset for the temp20,22...36 sensors
      ADT7486A_Cmd(ADT7486A_addrArray[channel-9]
               , SetExt1Offset
            , (eepage.ext1offset[channel]>>8)
            , eepage.ext1offset[channel]
            , SST_LINE2
            , &temperature);
    delay_us(100); //wait for ADT7486A
   }
   delay_us(300);
#endif

// User variables initialization
   DISABLE_INTERRUPTS
   user_data.terror[0]=0;
   user_data.terror[1]=0;
   user_data.terror[2]=0;
   user_data.terror[3]=0;
   user_data.terror[4]=0;
   user_data.status=0;
   rCTL = user_data.control=0x4;

  //temporary humidity sensor
  user_data.SHTtemp1 = 0;
  user_data.SHThumi1 = 0;
  user_data.SHTtemp2 = 0;
  user_data.SHThumi2 = 0;

   for (i=0; i<36;i ++)
      user_data.Temp[i] = 0.0;
  user_data.eepage=0;
  user_data.navge=20;
  for(i=0; i<36; i++)
  {
    user_data.AT[i]=0;
  }
  
  user_data.ref=0;

  ENABLE_INTERRUPTS
  // General Variables initialization
  EEP_CTR_Flag = 0;
  rCSR=0;

// Humidity sensor initialization
#ifdef _HUMSEN_
  SFRPAGE  = CONFIG_PAGE;
   P1MDOUT |= 0x07;
  SHT_VDD1=1;
  SHT_GND1=0;
  // Initializing the SHTxx communication
   HumiSensor_Init(humsen1);
#ifdef MORETHANONEHUM
   P3MDOUT |= 0x07;
  SHT_VDD2=1;
  SHT_GND2=0;
  HumiSensor_Init(humsen2);
#endif   
#endif

// DAC initialization
#ifdef _LTC2600_
   SFRPAGE  = CONFIG_PAGE;
   P0MDOUT |= 0x80; // Setting the SUM_DAC_CSn to push pull
   P2MDOUT |= 0x30; // Setting the SPI_MOSI and SPI_SCK to push pull
   LTC2600_Init();
#endif
}

/*---- User write function -----------------------------------------*/
void user_write(unsigned char index) reentrant
{
  if (index == IDXCTL) rCTL = user_data.control;

  if (index == IDXEEP_CTL) EEP_CTR_Flag = SET;

#ifdef _LTC2600_
   if ((index >= First_DACIndex) && (index <= Last_DACIndex)) {
      DACIndex = (index - First_DACIndex);
       // Update Bias Dac voltages as requested by bit5 of control register
      LTC2600_Flag = SET;
   }
#endif

  if(index==IDXNAVG) {
    cavg = 0;
    numavg = 0;
  }   
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


/*---- User loop function ------------------------------------------*/
void user_loop(void)
{

#ifdef _ADT7486A_
  // Read the external temperature from each chip that is connected to SST_LINE1
  // Corresponds to Temp01,03,05...17
   for (channel=0;channel < 9; channel++){
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel]
              , GetExt2Temp
              , SST_LINE1 
              , &temperature)){
         //Clear the bit if there is no error 
         DISABLE_INTERRUPTS;
         user_data.Temp[channel*2] = temperature;
      if(channel<4)
            user_data.terror[0]   &= Terrorclear[2*channel];
      else if(channel<8)
        user_data.terror[1]   &= Terrorclear[2*(channel-4)];
      else 
        user_data.terror[2]   &= Terrorclear[2*(channel-8)];
         ENABLE_INTERRUPTS;
      } else {
      //Set the bit if there is an error
         DISABLE_INTERRUPTS;
      if(channel<4)
            user_data.terror[0]   |= Terrorset[2*channel];
      else if(channel<8)
        user_data.terror[1]   |= Terrorset[2*(channel-4)];
      else 
        user_data.terror[2]   |= Terrorset[2*(channel-8)];
         ENABLE_INTERRUPTS;
      } 
   }

  yield();

  // Read the external temperature from each chip that is connected to SST_LINE2
  // Corresponds to Temp19,21...35
  for (channel=9;channel < 18; channel++){
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel-9]
              , GetExt2Temp
              , SST_LINE2
              , &temperature)){
         
         DISABLE_INTERRUPTS;
         user_data.Temp[channel*2] = temperature;
      if(channel<12)
            user_data.terror[2]   &= Terrorclear[2*(channel-8)];
      else if(channel<16)
        user_data.terror[3]   &= Terrorclear[2*(channel-12)];
      else 
        user_data.terror[4]   &= Terrorclear[2*(channel-16)];
         ENABLE_INTERRUPTS;
      }
      else{
         DISABLE_INTERRUPTS;
      if(channel<12)
            user_data.terror[2]   |= Terrorset[2*(channel-8)];
      else if(channel<16)
        user_data.terror[3]   |= Terrorset[2*(channel-12)];
      else 
        user_data.terror[4]   |= Terrorset[2*(channel-16)];
         ENABLE_INTERRUPTS;
      }
   }
  

  yield();

   // Read the second external temperature from each chip that is connected to SST_LINE1
   // Corresponds to Temp02,04...18  
   for (channel=0;channel < 9; channel++){
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel]
              , GetExt1Temp
              , SST_LINE1
              , &temperature)){
         DISABLE_INTERRUPTS;
         user_data.Temp[(channel*2)+1] = temperature;
      if(channel<4)
            user_data.terror[0]   &= Terrorclear[2*channel+1];
      else if(channel<8)
        user_data.terror[1]   &= Terrorclear[2*(channel-4)+1];
      else 
        user_data.terror[2]   &= Terrorclear[2*(channel-8)+1];
         ENABLE_INTERRUPTS;
      }
      else{
         DISABLE_INTERRUPTS;
      if(channel<4)
            user_data.terror[0]   |= Terrorset[2*channel+1];
      else if(channel<8)
        user_data.terror[1]   |= Terrorset[2*(channel-4)+1];
      else 
        user_data.terror[2]   |= Terrorset[2*(channel-8)+1];
         ENABLE_INTERRUPTS;
      }
   }


  yield();

  // Read the second external temperature from each chip that is connected to SST_LINE2
  // Corresponds to Temp20,22,...36
  for (channel=9;channel < 18; channel++){
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel-9]
              , GetExt1Temp
              , SST_LINE2
              , &temperature)){
         DISABLE_INTERRUPTS;
         user_data.Temp[(channel*2)+1] = temperature;
      if(channel<12)
            user_data.terror[2]   &= Terrorclear[2*(channel-8)+1];
      else if(channel<16)
        user_data.terror[3]   &= Terrorclear[2*(channel-12)+1];
      else 
        user_data.terror[4]   &= Terrorclear[2*(channel-16)+1];
         ENABLE_INTERRUPTS;
      }
      else{
         DISABLE_INTERRUPTS;
      if(channel<12)
            user_data.terror[2]   |= Terrorset[2*(channel-8)+1];
      else if(channel<16)
        user_data.terror[3]   |= Terrorset[2*(channel-12)+1];
      else 
        user_data.terror[4]   |= Terrorset[2*(channel-16)+1];
         ENABLE_INTERRUPTS;
      }
   }


  yield();

   // Calculating the rolling average
   if((user_data.navge <= TAVGMAX) && (user_data.navge != 0)) {
      if(cavg == user_data.navge)
         cavg=1;
      else {
         cavg++;
         if(numavg != user_data.navge) numavg=cavg;
      }

      for(channel=0; channel<36; channel++) {
         Taverage[channel][(cavg-1)] = user_data.Temp[channel];
         average=0;
    
         for(avgcount=0; avgcount<numavg; avgcount++)
            average+=Taverage[channel][avgcount];
         average/=numavg;
         DISABLE_INTERRUPTS;
         user_data.AT[channel]=average;
         ENABLE_INTERRUPTS;
      } 
   }
#endif

#ifdef _ExtEEPROM_
  //-----------------------------------------------------------------------------
  // Checking the eeprom control flag
  if (EEP_CTR_Flag) {
    //Checking for the special instruction
    if (user_data.eeCtrSet & EEP_CTRL_KEY) {
      // Valid index range
      if( (int)(user_data.eeCtrSet & 0x000000ff) < TEMPOFF_LAST_INDX) {
        // Float area from EEP_RW_IDX, count in Float size, No upper limit specified!
        eep_address = (int xdata *)&eepage + eepageAddrConvert((int)(user_data.eeCtrSet & 0x000000ff));
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
      eeptemp_source = (unsigned char xdata *)&eepage;
    }

    // EEPROM Write/Clear request
    if (CeeClr) eep_request = CLEAR_EEPROM;
    else        eep_request = WRITE_EEPROM;

    status = ExtEEPROM_Write_Clear (eeptemp_addr
                                        , &eeptemp_source
                                        , PAGE_SIZE
                                        , eep_request
                                        , &eeprom_flag);

    if (status == DONE) {
      SeeS = DONE;
      eeprom_flag = CLEAR;
      //Set the active page
      user_data.eepage |= ((user_data.eepage & 0x07) << 5);
    } else {
      SeeS = FAILED;
    }

    CeeS = CLEAR;
    // Publish Registers state
    publishCtlCsr();
  }

  //-----------------------------------------------------------------------------
  // EEPROM Restore procedure based on CTL bit
  if (CeeR) {
    rCSR = user_data.status;

    //NW read to eepage(active page) instead of eepage2
    channel = ExtEEPROM_Read (PageAddr[(unsigned char)(user_data.eepage & 0x07)]
                                     , (unsigned char xdata *)&eepage, PAGE_SIZE);

    if (channel == DONE) SeeR = DONE;
    else  SeeR = FAILED;

    CeeR = CLEAR;
    publishCtlCsr();   // Publish Registers state
  }
#endif

//-----------------------------------------------------------------------------
// Humidity sensors
#ifdef _HUMSEN_
  //Measuring the humidity and temperature
   if(CHum){
      status = HumidSensor_Cmd (&rSHThumi1
                               ,&rSHTtemp1
                               ,&humidity
                               ,&htemperature
                               ,&FCSorig1
                               ,&FCSdevi1
                               ,humsen1);
      if (status == DONE) {   
        DISABLE_INTERRUPTS;
        user_data.SHThumi1 = humidity;
        user_data.SHTtemp1 = htemperature;
        ENABLE_INTERRUPTS;
      }
  
#ifdef MORETHANONEHUM
      status = HumidSensor_Cmd (&rSHThumi2
                               ,&rSHTtemp2
                               ,&humidity
                               ,&htemperature
                               ,&FCSorig2
                               ,&FCSdevi2
                               ,humsen2);  
      if (status == DONE) {
        DISABLE_INTERRUPTS;  
        user_data.SHThumi2 = humidity;
        user_data.SHTtemp2 = htemperature;
        ENABLE_INTERRUPTS;
      }
#endif // MORETHANONE
   }

#endif // HUMIDITY

#ifdef _LTC2600_
  //DAC
   if(LTC2600_FLAG) {
      LTC2600_Cmd(WriteTo_Update,LTC2600_LOAD[DACIndex], user_data.DAC[DACIndex]);
      LTC2600_FLAG = CLEAR;
   }
#endif
  
  if(Cref) {
    autocalibration(user_data.ref);
    Sref = 1;
    Cref = 0;
    DISABLE_INTERRUPTS;
    user_data.status=rCSR;
    user_data.control=rCTL;
    ENABLE_INTERRUPTS; 
  }

  led_blink(0, 1, 150);

}