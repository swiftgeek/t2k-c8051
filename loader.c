/********************************************************************\
 Name:         loader.c
 Created by:   Bahman Sotoodian                Mar/11/2008
 
 
 Contents:     Application specific (user) part of
 					Midas Slow Control Bus protocol
 					for External EEPROM of FEB64 board

 Memory usage:  
 define(_SPI_PROTOCOL_) 
 
 
 $Id$
 \********************************************************************/
 
 #include <stdio.h>
 #include <math.h>
 #include "mscbemb.h"
 #include "loader.h"
 #include "Devices/AT25160A.h"

#ifdef _PCA9539_
 #include "Devices/PCA9539_io.h"
#endif
 
 //
 // Global declarations
 //-----------------------------------------------------------------------------
 char code  node_name[] = "loader";
 
 //
 // Declare globally the number of sub-addresses to framework
 unsigned char idata _n_sub_addr = 1;


 // User Data structure declaration
 //-----------------------------------------------------------------------------
 MSCB_INFO_VAR code vars[] = {
   1, UNIT_BYTE,            0, 0,           0, "Control",    &user_data.control,      // 0
   1, UNIT_BYTE,            0, 0,           0, "EEPage",     &user_data.eepage,       // 1
	1, UNIT_BYTE,            0, 0,           0, "Status",     &user_data.status,       // 0   
   0
 };

 MSCB_INFO_VAR *variables = vars;   // Structure mapping
 // Get sysinfo if necessary
 extern SYS_INFO sys_info;          // For address setting
 

 
 
 /********************************************************************\
 Application specific init and in/output routines
 \********************************************************************/
 
 /*---- User init function ------------------------------------------*/
 void user_init(unsigned char init)
 {
 	char xdata add;

 	if (init)
	{
	 	// Initialize control and status
   	user_data.control = 0;
		user_data.status = 0;
	}
 	add = cur_sub_addr();

 //
 // Initial setting for communication and overall ports (if needed).
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   // P0MDOUT contains Tx in push-pull
   P0MDOUT |= 0x20;   // add RS485_ENABLE in push-pull
 
 //
 // SPI Setting
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   P3MDOUT |= 0x80; // Setting the RAM_CSn to push pull
   P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
	P2MDOUT &= 0xFE; // Setting the RAM_WPn to open drain
	
#ifdef _PCA9539_
   PCA9539_Init(); //PCA General I/O (Bias Enables and Backplane Addr) initialization
	PCA9539_WriteByte(BACKPLANE_INPUT_ENABLE);
	
	// Physical backplane address retrieval
	//-----------------------------------------------------------------------------
  	PCA9539_Read(BACKPLANE_READ, &add, 1);

	sys_info.node_addr = add;

#endif
	
	if(ExtEEPROM_Init(NUMEBER_PAGES,PAGE_SIZE))
	{
	  user_data.status = (1<<7);
	}

 }

/*---- User write function -----------------------------------------*/
 #pragma NOAREGS

void user_write(unsigned char index) reentrant
{
	if(index);
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
void user_loop(void) 
{
		signed char check=0;

		check=ExtEEPROM_WriteProtect ((unsigned char*)&eepage, PAGE_SIZE);
//		check=ExtEEPROM_Read  (WP_START_ADDR,(unsigned char*)&eepage2, PAGE_SIZE);
		check=ExtEEPROM_Write_Clear (0x0000,(unsigned char*)&eepage, 
		PAGE_SIZE,WRITE);
//		check=ExtEEPROM_Read  (0x0000,(unsigned char*)&test2, PAGE_SIZE);
									
	yield();
} 
