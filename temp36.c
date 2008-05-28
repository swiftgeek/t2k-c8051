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

  // Declare globally the number of sub-addresses to framework
 unsigned char idata _n_sub_addr = 1;
 
 // Global declarations
 //-----------------------------------------------------------------------------
 char code  node_name[] = "temp36";
 char idata svn_rev_code[] = "$Rev$";
 
 // Local flag
 bit EEP_CTR_FLAG;

 
 // Get sysinfo if necessary
 extern SYS_INFO sys_info;          // For address setting


 // User Data structure declaration
 //-----------------------------------------------------------------------------
 MSCB_INFO_VAR code vars[] = {
   4, UNIT_BYTE,            0, 0,           0, "SerialN",      &user_data.SerialN,         // 0
   1, UNIT_BYTE,            0, 0,           0, "Control",      &user_data.control,         // 1
   1, UNIT_BYTE,            0, 0,           0, "Status",       &user_data.status,          // 2
	1, UNIT_BYTE,            0, 0,           0, "EEPage",       &user_data.eepage,          // 3	
   1, UNIT_BYTE,            0, 0,           0, "TE01-08",      &user_data.terror[0],       // 4
   1, UNIT_BYTE,            0, 0,           0, "TE09-16",      &user_data.terror[1],       // 5
   1, UNIT_BYTE,            0, 0,           0, "TE17-24",      &user_data.terror[2],       // 6
   1, UNIT_BYTE,            0, 0,           0, "TE25-32",      &user_data.terror[3],       // 7
   1, UNIT_BYTE,            0, 0,           0, "TE33-36",      &user_data.terror[4],       // 8
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp01",       &user_data.Temp[0],         // 9
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp02",       &user_data.Temp[1],        // 10
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp03",       &user_data.Temp[2],        // 11
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp04",       &user_data.Temp[3],        // 12
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp05",       &user_data.Temp[4],        // 13
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp06",       &user_data.Temp[5],        // 14
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp07",       &user_data.Temp[6],        // 15
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp08",       &user_data.Temp[7],        // 16
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp09",       &user_data.Temp[8],        // 17
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp10",       &user_data.Temp[9],        // 18
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp11",       &user_data.Temp[10],       // 19
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp12",       &user_data.Temp[11],       // 20
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp13",       &user_data.Temp[12],       // 21
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp14",       &user_data.Temp[13],       // 22
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp15",       &user_data.Temp[14],       // 23
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp16",       &user_data.Temp[15],       // 24
	4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp17",       &user_data.Temp[16],       // 25
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp18",       &user_data.Temp[17],       // 26
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp19",       &user_data.Temp[18],       // 27
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp20",       &user_data.Temp[19],       // 28
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp21",       &user_data.Temp[20],       // 29
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp22",       &user_data.Temp[21],       // 30
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp23",       &user_data.Temp[22],       // 31
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp24",       &user_data.Temp[23],       // 32
	4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp25",       &user_data.Temp[24],       // 33
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp26",       &user_data.Temp[25],       // 34
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp27",       &user_data.Temp[26],       // 35
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp28",       &user_data.Temp[27],       // 36
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp29",       &user_data.Temp[28],       // 37
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp30",       &user_data.Temp[29],       // 38
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp31",       &user_data.Temp[30],       // 39
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp32",       &user_data.Temp[31],       // 40
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp33",       &user_data.Temp[32],       // 41
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp34",       &user_data.Temp[33],       // 42
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp35",       &user_data.Temp[34],       // 43
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp36",       &user_data.Temp[35],       // 44

	4, UNIT_BYTE,            0, 0, MSCBF_FLOAT,"eepValue",      &user_data.eepValue,       // 45
   4, UNIT_BYTE,            0, 0,           0,"eeCtrSet",      &user_data.eeCtrSet, 	   // 46

	2, UNIT_BYTE,            0, 0,           0, "ext01",        &user_data.ext1offset[0],  // 47
	2, UNIT_BYTE,            0, 0,           0, "ext02",        &user_data.ext1offset[1],  // 48
	2, UNIT_BYTE,            0, 0,           0, "ext03",        &user_data.ext1offset[2],  // 49  
	2, UNIT_BYTE,            0, 0,           0, "ext04",        &user_data.ext1offset[3],  // 50
	2, UNIT_BYTE,            0, 0,           0, "ext05",        &user_data.ext1offset[4],  // 51
	2, UNIT_BYTE,            0, 0,           0, "ext06",        &user_data.ext1offset[5],  // 52
	2, UNIT_BYTE,            0, 0,           0, "ext07",        &user_data.ext1offset[6],  // 53
	2, UNIT_BYTE,            0, 0,           0, "ext08",        &user_data.ext1offset[7],  // 54
	2, UNIT_BYTE,            0, 0,           0, "ext09",        &user_data.ext1offset[8],  // 55
	2, UNIT_BYTE,            0, 0,           0, "ext10",        &user_data.ext1offset[9],  // 56
	2, UNIT_BYTE,            0, 0,           0, "ext11",        &user_data.ext1offset[10], // 57
	2, UNIT_BYTE,            0, 0,           0, "ext12",        &user_data.ext1offset[11], // 58
	2, UNIT_BYTE,            0, 0,           0, "ext13",        &user_data.ext1offset[12], // 59
	2, UNIT_BYTE,            0, 0,           0, "ext14",        &user_data.ext1offset[13], // 60
	2, UNIT_BYTE,            0, 0,           0, "ext15",        &user_data.ext1offset[14], // 61
	2, UNIT_BYTE,            0, 0,           0, "ext16",        &user_data.ext1offset[15], // 62
	2, UNIT_BYTE,            0, 0,           0, "ext17",        &user_data.ext1offset[16], // 63
	2, UNIT_BYTE,            0, 0,           0, "ext18",        &user_data.ext1offset[17], // 64
	
	2, UNIT_BYTE,            0, 0,           0, "ext19",        &user_data.ext2offset[0],  // 65
	2, UNIT_BYTE,            0, 0,           0, "ext20",        &user_data.ext2offset[1],  // 66
	2, UNIT_BYTE,            0, 0,           0, "ext21",        &user_data.ext2offset[2],  // 67
	2, UNIT_BYTE,            0, 0,           0, "ext22",        &user_data.ext2offset[3],  // 58
	2, UNIT_BYTE,            0, 0,           0, "ext23",        &user_data.ext2offset[4],  // 69
	2, UNIT_BYTE,            0, 0,           0, "ext24",        &user_data.ext2offset[5],  // 70
	2, UNIT_BYTE,            0, 0,           0, "ext25",        &user_data.ext2offset[6],  // 71
	2, UNIT_BYTE,            0, 0,           0, "ext26",        &user_data.ext2offset[7],  // 72
	2, UNIT_BYTE,            0, 0,           0, "ext27",        &user_data.ext2offset[8],  // 73
	2, UNIT_BYTE,            0, 0,           0, "ext28",        &user_data.ext2offset[9],  // 74
	2, UNIT_BYTE,            0, 0,           0, "ext29",        &user_data.ext2offset[10], // 75
	2, UNIT_BYTE,            0, 0,           0, "ext30",        &user_data.ext2offset[11], // 76
	2, UNIT_BYTE,            0, 0,           0, "ext31",        &user_data.ext2offset[12], // 77
	2, UNIT_BYTE,            0, 0,           0, "ext32",        &user_data.ext2offset[13], // 78
	2, UNIT_BYTE,            0, 0,           0, "ext33",        &user_data.ext2offset[14], // 79
	2, UNIT_BYTE,            0, 0,           0, "ext34",        &user_data.ext2offset[15], // 80
	2, UNIT_BYTE,            0, 0,           0, "ext35",        &user_data.ext2offset[16], // 81
	2, UNIT_BYTE,            0, 0,           0, "ext36",        &user_data.ext2offset[17],	// 82
	0
 };
 
 MSCB_INFO_VAR *variables = vars;   // Structure mapping


void user_write(unsigned char index) reentrant;

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
   unsigned char idata i, channel;

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

 // Initial setting for communication and overall ports 
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

	//Setting the offset for the temperature sensors
   for (channel=0;channel < 9; channel++)
	{
		//Set the offset for the temp01,03...17 sensors 
      ADT7486A_Cmd(ADT7486A_addrArray[channel], SetExt2Offset, (Text2offset[channel]>>8), Text2offset[channel], SST_LINE1, &temperature);
		delay_us(100); //wait for ADT7486A
	}
	delay_us(100);
   for (channel=9;channel < 18; channel++)
	{
		//Set the offset for the temp19,21...35 sensors
	   ADT7486A_Cmd(ADT7486A_addrArray[channel-9], SetExt2Offset, (Text2offset[channel]>>8), Text2offset[channel], SST_LINE2, &temperature);      
		delay_us(100); //wait for ADT7486A
	}
	delay_us(100);
   for (channel=0;channel < 9; channel++)
	{
		//Set the offset for the temp02,04...18 sensors
      ADT7486A_Cmd(ADT7486A_addrArray[channel], SetExt1Offset, (Text1offset[channel]>>8), Text1offset[channel], SST_LINE1, &temperature);
		delay_us(100); //wait for ADT7486A
	}
	delay_us(100);
   for (channel=9;channel < 18; channel++)
	{
		//Set the offset for the temp20,22...36 sensors
      ADT7486A_Cmd(ADT7486A_addrArray[channel-9], SetExt1Offset, (Text1offset[channel]>>8), Text1offset[channel], SST_LINE2, &temperature);
		delay_us(100); //wait for ADT7486A
	}
#endif

   //External EEPROM chip
#ifdef _ExtEEPROM_
   SFRPAGE  = CONFIG_PAGE;
   P2MDOUT |= 0x31; // Setting the RAM_CSn. SPI_MOSI, SPI_SCK to push pull
   P2MDOUT &= 0xFB; // Setting the RAM_WPn to open drain
   ExtEEPROM_Init();
#endif


   //User variables initialization
   DISABLE_INTERRUPTS
   user_data.terror[0]=0;
   user_data.terror[1]=0;
   user_data.terror[2]=0;
   user_data.terror[3]=0;
   user_data.terror[4]=0;
	user_data.control=0;
	user_data.status=0;
   for (i=0; i<36;i ++)
      user_data.Temp[i] = 0.0;
   for (i=0; i<18;i ++)
	{
      user_data.ext1offset[i] = 0.0;
		user_data.ext2offset[i] = 0.0;
	}
	user_data.eepage=0;
   ENABLE_INTERRUPTS
   //General Variables initialization
	EEP_CTR_FLAG=0;
	rCTL=0;
	rCSR=0;
}

/*---- User write function -----------------------------------------*/

void user_write(unsigned char index) reentrant
{
	rCSR=user_data.status;
	if(index==IDXCTL)
		rCTL=user_data.control;
   if (index == IDXEEP_CTL)
      EEP_CTR_FLAG = SET;
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
   unsigned char idata channel;
	int* xdata eep_address;
	static unsigned char eeprom_flag=CLEAR; 
   unsigned char xdata eeprom_wstatus, eeprom_rstatus;
   unsigned int  xdata eeptemp_addr;
   static unsigned char* xdata eeptemp_source;
   unsigned char xdata eep_request;

#ifdef _ADT7486A_
	//Read the external temperature from each chip that is connected to SST_LINE1
	//Corresponds to Temp01,03,05...17
   for (channel=0;channel < 9; channel++)
	{
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt2Temp, SST_LINE1, &temperature))
      {
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
      }
      else
      {
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
	//Read the external temperature from each chip that is connected to SST_LINE2
   //Corresponds to Temp19,21...35
	for (channel=9;channel < 18; channel++)
	{
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel-9], GetExt2Temp, SST_LINE2, &temperature))
      {
         
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
      else
      {
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
	//Read the second external temperature from each chip that is connected to SST_LINE1
   //Corresponds to Temp02,04...18  
   for (channel=0;channel < 9; channel++)
	{
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel], GetExt1Temp, SST_LINE1, &temperature))
      {
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
      else
      {
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
	//Read the second external temperature from each chip that is connected to SST_LINE2
   //Corresponds to Temp20,22,...36
	for (channel=9;channel < 18; channel++)
	{
      if(!ADT7486A_Cmd(ADT7486A_addrArray[channel-9], GetExt1Temp, SST_LINE2, &temperature))
      {
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
      else
      {
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
#endif
#ifdef _ExtEEPROM_
   if (EEP_CTR_FLAG)
   {
      //Checking for the special instruction
      if (user_data.eeCtrSet & EEP_CTRL_KEY){
	   	eep_address = (int*)(&eepage) + (int)(user_data.eeCtrSet & 0x000000ff);
   	   //Checking for the write request
			if (user_data.eeCtrSet & EEP_CTRL_WRITE){
      		if ((user_data.eeCtrSet & 0x000000ff) < ((SERIALN_ADD - WP_START_ADDR)/2))
					*eep_address = user_data.eepValue;
				else
				{
           	   DISABLE_INTERRUPTS;
					// Tell the user that the index is over the limit
          	   user_data.eepValue = EEP_CTRL_INVAL_OVERLIMIT;
          	   ENABLE_INTERRUPTS;
				}	
        	  //Checking for the read request
        	} else if (user_data.eeCtrSet & EEP_CTRL_READ){
					DISABLE_INTERRUPTS;
           	 	user_data.eepValue = *eep_address;
					ENABLE_INTERRUPTS;
        	} else {

          	   // Tell the user that inappropriate task has been requested
           	   DISABLE_INTERRUPTS;
          	   user_data.eepValue = EEP_CTRL_INVAL_REQ;
          	   ENABLE_INTERRUPTS;
         }
	
     	} else {

        	// Tell the user that invalid key has been provided
         DISABLE_INTERRUPTS;
         user_data.eepValue = EEP_CTRL_INVAL_KEY;
         ENABLE_INTERRUPTS;
      }

      EEP_CTR_FLAG = CLEAR;
   }
	//Writing to the EEPROM
   if (CeeS) 
	{
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
	//Reading from the EEPROM
   if (CeeR) {
      rCSR = user_data.status;
      eeprom_rstatus = ExtEEPROM_Read  (eeptemp_addr,
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
		//Display the values read from the EEPROM
		if(SeeR)
		{
			for(channel=0; channel<18; channel++)
			{
		   	user_data.ext1offset[channel]=eepage2.ext1offset[channel];
				user_data.ext2offset[channel]=eepage2.ext2offset[channel];	
			}
		}
		user_data.SerialN=eepage2.SerialN;
      ENABLE_INTERRUPTS;
   }
#endif

 	led_blink(1, 1, 250);
}