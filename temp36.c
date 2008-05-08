/********************************************************************\
 Name:         temp36.c
 Created by:   Noel Wu                May/07/2008


 Contents:     Application specific (user) part of
 Midas Slow Control Bus protocol for temp36 board

 \********************************************************************/

 #include <stdio.h>
 #include <math.h>
 #include "mscbemb.h"
 #include "temp36.h"
 #include "Devices/ADT7486A_tsensor.h"

  // Declare globally the number of sub-addresses to framework
 unsigned char idata _n_sub_addr = 1;

 // Global declarations
 //-----------------------------------------------------------------------------
 char code  node_name[] = "temp36";
 char idata svn_rev_code[] = "$Rev$";

 // Get sysinfo if necessary
 extern SYS_INFO sys_info;          // For address setting

 //Decaration of variables
 unsigned char bdata rErr;
 sbit EXTtemp=rErr^1;

	

 // User Data structure declaration
 //-----------------------------------------------------------------------------
 MSCB_INFO_VAR code vars[] = {
   4, UNIT_BYTE,            0, 0,           0, "SerialN",    &user_data.SerialN,      // 0
   5, UNIT_BYTE,            0, 0,           0, "Error",      &user_data.error,        // 1
   1, UNIT_BYTE,            0, 0,           0, "Control",    &user_data.control,      // 2
   1, UNIT_BYTE,            0, 0,           0, "Status",     &user_data.status,       // 3
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp0",      &user_data.Temp[0],      // 4
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp1",      &user_data.Temp[1],      // 5
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp2",      &user_data.Temp[2],      // 6
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp3",      &user_data.Temp[3],      // 7
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp4",      &user_data.Temp[4],      // 8
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp5",      &user_data.Temp[5],      // 9
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp6",      &user_data.Temp[6],      // 10
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp7",      &user_data.Temp[7],      // 11
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp8",      &user_data.Temp[8],      // 12
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp9",      &user_data.Temp[9],      // 13
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp10",      &user_data.Temp[10],     // 14
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp11",      &user_data.Temp[11],     // 15
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp12",      &user_data.Temp[12],     // 16
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp13",      &user_data.Temp[13],     // 17
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp14",      &user_data.Temp[14],     // 18
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp15",      &user_data.Temp[15],     // 19
	4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp16",      &user_data.Temp[16],     // 20
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp17",      &user_data.Temp[17],     // 21
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp18",      &user_data.Temp[18],     // 22
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp19",      &user_data.Temp[19],     // 23
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp20",      &user_data.Temp[20],     // 24
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp21",      &user_data.Temp[21],     // 25
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp22",      &user_data.Temp[22],     // 26
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp23",      &user_data.Temp[23],     // 27
	4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp24",      &user_data.Temp[24],     // 28
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp25",      &user_data.Temp[25],     // 29
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp26",      &user_data.Temp[26],     // 30
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp27",      &user_data.Temp[27],     // 31
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp28",      &user_data.Temp[28],     // 32
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp29",      &user_data.Temp[29],     // 33
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp30",      &user_data.Temp[30],     // 34
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp31",      &user_data.Temp[31],     // 35
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp32",      &user_data.Temp[32],     // 36
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp33",      &user_data.Temp[33],     // 37
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp34",      &user_data.Temp[34],     // 38
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp35",      &user_data.Temp[35],     // 39
   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp36",      &user_data.Temp[36],     // 40
 };
 
 MSCB_INFO_VAR *variables = vars;   // Structure mapping


void user_write(unsigned char index) reentrant;

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
  char xdata i;

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
  SFRPAGE  = CONFIG_PAGE;
   // P0MDOUT contains Tx in push-pull
  P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull

	//ADT7486A Init
  P1MDOUT |= 0x01; // Setting the SST_DRV (SST) to push pull
  	// Set SST_REF & SST_IO P2 ^ 6/7 to Open-Drain and Analog Input 
  	// so that it accepts the ~650mV set by voltage divider.
  P2MDOUT |= 0x3F;
  SFRPAGE  = CPT1_PAGE;
  CPT1CN  |= 0x80; // Enable the Comparator 1
  CPT1MD   = 0x02; //Comparator1 Mode Selection
  
  ADT7486A_Init(); //Temperature measurements related initialization

  //error initialization
  rErr = 0x00;
  user_data.error[0] = rErr;  

  // Temperature initialization   
  for (i=0; i<36;i ++)
      user_data.Temp[i] = 0.0;
}

/*---- User write function -----------------------------------------*/

void user_write(unsigned char index) reentrant;

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
  unsigned char i;

  for (i=0;i<18;i++) {
      if (( ADT7486A_Cmd(ADT7486A_addrArray[0], GetExt1Temp, &temperature1)||
  		ADT7486A_Cmd(ADT7486A_addrArray[1], GetExt2Temp, &temperature2)) != SUCCESS)
            EXTtemp = 1;
		DISABLE_INTERRUPTS;
		user_data.Temp[i]=temperature1;
		user_data.Temp[i+1]=temperature2;
		ENABLE_INTERRUPTS;		
  }

 led_blink(1, 1, 250);

}