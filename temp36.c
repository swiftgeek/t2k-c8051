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
 #include "Devices/temp36_ADT7486A_tsensor.h"
 #endif

 #ifdef _ExtEEPROM_
 #include "Devices/ExtEEPROM.h"
 #endif

  // Declare globally the number of sub-addresses to framework
 unsigned char idata _n_sub_addr = 1;
 unsigned char bdata bChange;
 // Global declarations
 //-----------------------------------------------------------------------------
 char code  node_name[] = "temp36";
 char idata svn_rev_code[] = "$Rev$";
 // Local flag
 sbit AT7486A_FLAG = bChange ^ 0;
 sbit EEP_W_FLAG   = bChange ^ 1;
 sbit EEP_R_FLAG   = bChange ^ 2;

 // Get sysinfo if necessary
 extern SYS_INFO sys_info;          // For address setting


 // User Data structure declaration
 //-----------------------------------------------------------------------------
 MSCB_INFO_VAR code vars[] = {
   4, UNIT_BYTE,            0, 0,           0, "SerialN",       &user_data.SerialN,      // 0
   1, UNIT_BYTE,            0, 0,           0, "Control",       &user_data.control,      // 2
   1, UNIT_BYTE,            0, 0,           0, "Status",        &user_data.status,       // 3
	1, UNIT_BYTE,            0, 0,           0, "EEPage",        &user_data.eepage,       // 3
   4, UNIT_BYTE,            0, 0,           0, "TError",        &user_data.error,        // 1
   4, UNIT_BYTE,            0, 0,           0, "TError2",       &user_data.error2,       // 1
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp0-0",       &user_data.Temp[0],      // 4
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp0-1",       &user_data.Temp[1],      // 5
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp1-0",       &user_data.Temp[2],      // 6
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp1-1",       &user_data.Temp[3],      // 7
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp2-0",       &user_data.Temp[4],      // 8
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp2-1",       &user_data.Temp[5],      // 9
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp3-0",       &user_data.Temp[6],      // 10
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp3-1",       &user_data.Temp[7],      // 11
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp4-0",       &user_data.Temp[8],      // 12
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp4-1",       &user_data.Temp[9],      // 13
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp5-0",       &user_data.Temp[10],     // 14
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp5-1",       &user_data.Temp[11],     // 15
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp6-0",       &user_data.Temp[12],     // 16
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp6-1",       &user_data.Temp[13],     // 17
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp7-0",       &user_data.Temp[14],     // 18
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp7-1",       &user_data.Temp[15],     // 19
	4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp8-0",       &user_data.Temp[16],     // 20
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp8-1",       &user_data.Temp[17],     // 21
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp9-0",       &user_data.Temp[18],     // 22
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp9-1",       &user_data.Temp[19],     // 23
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp11-0",      &user_data.Temp[20],     // 24
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp11-1",      &user_data.Temp[21],     // 25
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp12-0",      &user_data.Temp[22],     // 26
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp12-1",      &user_data.Temp[23],     // 27
	4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp13-0",      &user_data.Temp[24],     // 28
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp13-1",      &user_data.Temp[25],     // 29
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp14-0",      &user_data.Temp[26],     // 30
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp14-1",      &user_data.Temp[27],     // 31
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp15-0",      &user_data.Temp[28],     // 32
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp15-1",      &user_data.Temp[29],     // 33
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp16-0",      &user_data.Temp[30],     // 34
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp16-1",      &user_data.Temp[31],     // 35
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp17-0",      &user_data.Temp[32],     // 36
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp17-1",      &user_data.Temp[33],     // 37
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp18-0",      &user_data.Temp[34],     // 38
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp18-1",      &user_data.Temp[35],     // 39
 };
 
 MSCB_INFO_VAR *variables = vars;   // Structure mapping


void user_write(unsigned char index) reentrant;

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

 // Initial setting for communication and overall ports (if needed).
 //-----------------------------------------------------------------------------
  //ADT7486A Init
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
  //State initialization
  DISABLE_INTERRUPTS
  user_data.error=0;
  user_data.error2=0;
  for (i=0; i<36;i ++)
      user_data.Temp[i] = 0.0;
  ENABLE_INTERRUPTS
#endif

  //External EEPROM chip
#ifdef _ExtEEPROM_
  SFRPAGE  = CONFIG_PAGE;
  P2MDOUT |= 0x31; // Setting the RAM_CSn. SPI_MOSI, SPI_SCK to push pull
  P2MDOUT &= 0xFB; // Setting the RAM_WPn to open drain
  ExtEEPROM_Init();
#endif
  rCTL=0;
  rCSR=0;
}

/*---- User write function -----------------------------------------*/

void user_write(unsigned char index) reentrant
{
	rCSR=user_data.status;
	if(index==IDXCTL)
		rCTL=user_data.control;
    
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
   unsigned char idata channel, eeprom_wstatus, eeprom_rstatus;
	unsigned int xdata eeptemp_addr;
	unsigned char xdata *eeptemp_source, eep_request;
	static unsigned char eeprom_flag=CLEAR; 
#ifdef _ADT7486A_
	//Read the first external temperature from each chip that is connected to SST_LINE1
   for (channel=0;channel < 9; channel++)
	{
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt1Temp, &temperature, SST_LINE1))
      {
         
         DISABLE_INTERRUPTS;
         user_data.Temp[channel*2] = temperature;
         user_data.error   |= (CLEAR<<channel);
         ENABLE_INTERRUPTS;
      }
      else
      {
         DISABLE_INTERRUPTS;
         user_data.error   |= (SET<<channel);;
         ENABLE_INTERRUPTS;
      }
   }
	//Read the first external temperature from each chip that is connected to SST_LINE2
   for (channel=9;channel < 18; channel++)
	{
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel-9], GetExt1Temp, &temperature, SST_LINE2))
      {
         
         DISABLE_INTERRUPTS;
         user_data.Temp[channel*2] = temperature;
         user_data.error   |= (CLEAR<<channel);
         ENABLE_INTERRUPTS;
      }
      else
      {
         DISABLE_INTERRUPTS;
         user_data.error   |= (SET<<channel);;
         ENABLE_INTERRUPTS;
      }
   }
	//Read the second external temperature from each chip that is connected to SST_LINE1
   for (channel=0;channel < 9; channel++)
	{
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt2Temp, &temperature,SST_LINE1))
      {
         DISABLE_INTERRUPTS;
         user_data.Temp[(channel*2)+1] = temperature;
         user_data.error2   |= (CLEAR<<channel);
         ENABLE_INTERRUPTS;
      }
      else
      {
         DISABLE_INTERRUPTS;
         user_data.error2   |= (SET<<channel);
         ENABLE_INTERRUPTS;
      }
   }
	//Read the second external temperature from each chip that is connected to SST_LINE2
   for (channel=9;channel < 18; channel++)
	{
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel-9], GetExt2Temp, &temperature,SST_LINE2))
      {
         DISABLE_INTERRUPTS;
         user_data.Temp[(channel*2)+1] = temperature;
         user_data.error2   |= (CLEAR<<channel);
         ENABLE_INTERRUPTS;
      }
      else
      {
         DISABLE_INTERRUPTS;
         user_data.error2   |= (SET<<channel);
         ENABLE_INTERRUPTS;
      }
   }
#endif
#ifdef _ExtEEPROM_
   if (CeeS) 
	{
       //Check if we are here for the first time
      if (!eeprom_flag){
         rCSR = user_data.status;
         //Temporary store the first address of page
         eeptemp_addr = PageAddr[(unsigned char)(user_data.eepage & 0x07)];
         //Temporary store the first address of data which has to be written
         eeptemp_source = (unsigned char*)&eepage;
      }
      //EPROM clear request
      if (CeeClr)
         eep_request = CLEAR_EEPROM;
      else 
         eep_request = WRITE_EEPROM;
      
      eeprom_wstatus = ExtEEPROM_Write_Clear (eeptemp_addr,&(eeptemp_source),
      PAGE_SIZE,eep_request,&eeprom_flag);

      if (eeprom_wstatus == DONE) {
         SeeS = DONE;
         eeprom_flag = CLEAR;
         CeeS = CLEAR;
         //Set the active page
         user_data.eepage |= ((user_data.eepage & 0x07) <<5);
      } 
		else 
         SeeS = FAILED;
 
      // Publish Registers state
      DISABLE_INTERRUPTS;
      user_data.control = rCTL;
      user_data.status  = rCSR;
      ENABLE_INTERRUPTS;
   }
   if (CeeR) {
      rCSR = user_data.status;
      eeprom_rstatus = ExtEEPROM_Read  (PageAddr[(unsigned char)(user_data.eepage & 0x07)],
      (unsigned char*)&eepage2, PAGE_SIZE);
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
 	led_blink(1, 1, 250);
}