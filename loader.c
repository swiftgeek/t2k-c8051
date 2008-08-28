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
 	 4, UNIT_BYTE,            0, 0,           0, "SerialN",    &user_data.serialN,  	  // 0
   1, UNIT_BYTE,            0, 0,           0, "Control",    &user_data.control,      // 1
   1, UNIT_BYTE,            0, 0,           0, "EEPage",     &user_data.eepage,       // 2
   1, UNIT_BYTE,            0, 0,           0, "Status",     &user_data.status,       // 3 
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

  if (init) {}

	// Initialize control and status
  user_data.control = 0;
	user_data.status = 0;
 	add = cur_sub_addr();
	EEPROM_FLAG=0;
 //
 // Initial setting for communication and overall ports (if needed).
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
   // P0MDOUT contains Tx in push-pull
   P0MDOUT |= 0x04;   // add RS485_ENABLE in push-pull

 //
 // SPI Setting
 //-----------------------------------------------------------------------------
   SFRPAGE  = CONFIG_PAGE;
#ifdef L_TEMP36
   P2MDOUT |= 0x31; // Setting the RAM_CSn. SPI_MOSI, SPI_SCK to push pull
   P2MDOUT &= 0xFB; // Setting the RAM_WPn to open drain
#elif defined(L_FEB64)
   P3MDOUT |= 0x80; // Setting the RAM_CSn to push pull
   P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
	 P2MDOUT &= 0xFE; // Setting the RAM_WPn to open drain
   P1MDOUT |= 0x08; // ResetN 
#elif defined(L_CMB)
   P3MDOUT |= 0x80; // Setting the RAM_CSn to push pull
   P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
	 P2MDOUT &= 0xFE; // Setting the RAM_WPn to open drain
#else defined(L_LPB)
   P3MDOUT |= 0x80; // Setting the RAM_CSn to push pull
   P2MDOUT |= 0x18; // Setting the SPI_MOSI and SPI_SCK to push pull
	P2MDOUT &= 0xFE; // Setting the RAM_WPn to open drain
#endif	
#ifdef _PCA9539_
   PCA9539_Init(); //PCA General I/O (Bias Enables and Backplane Addr) initialization
	 PCA9539_WriteByte(BACKPLANE_INPUT_ENABLE);
	
	// Physical backplane address retrieval
	//-----------------------------------------------------------------------------
  PCA9539_Read(BACKPLANE_READ, &add, 1);
	sys_info.node_addr = add;
#endif
	
// Adjust Node name based on the type of Board
#ifdef L_TEMP36
  sprintf(sys_info.node_name,"TEMP36");
#elif defined(L_FEB64)
  sprintf(sys_info.node_name,"FEB64");
#elif defined(L_CMB)
  sprintf(sys_info.node_name,"CMB");
#elif defined(L_LPB)
  sprintf(sys_info.node_name,"LPB");
#endif
   
  // Check if EEPROM size is lartge enough for structure
	if(ExtEEPROM_Init(NUMBER_PAGES, PAGE_SIZE))
	{
	  user_data.status = (1<<7);
	}
 }

/*---- User write function -----------------------------------------*/
 #pragma NOAREGS

void user_write(unsigned char index) reentrant
{
	if(index==IDXCTL) {
		EEPROM_FLAG=1;
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
void user_loop(void) 
{
	unsigned char FAILED=0;

	//Wait for the EEPROM_FLAG to be set
	if(EEPROM_FLAG)
	{
		//Write to the Protected Memory
		eepage.SerialN=user_data.serialN;
		FAILED=ExtEEPROM_WriteProtect ((unsigned char*)&eepage, PAGE_SIZE);
		if(!FAILED)
		{
			DISABLE_INTERRUPTS;
			user_data.status=1;
			ENABLE_INTERRUPTS;
		}
		//For testing purpose..
      FAILED=ExtEEPROM_Read  (WP_START_ADDR,(unsigned char*)&eepage2, PAGE_SIZE);
		if(!FAILED && (user_data.serialN == eepage2.SerialN))
		{
			DISABLE_INTERRUPTS;
			user_data.status |= (1<<1);
			ENABLE_INTERRUPTS;
		}
		//Write to Page 0
	   FAILED =ExtEEPROM_Write_Clear (page_addr[1],(unsigned char*)&eepage, PAGE_SIZE, WRITE);
		if(!FAILED)
		{
			DISABLE_INTERRUPTS;
			user_data.status |= (1<<2);
			ENABLE_INTERRUPTS;
		}
		//For testing purpose...
      FAILED=ExtEEPROM_Read  (page_addr[1],(unsigned char*)&eepage2, PAGE_SIZE);
		if(!FAILED && (user_data.serialN == eepage2.SerialN))
		{
			DISABLE_INTERRUPTS;
			user_data.status |= (1<<3);
			ENABLE_INTERRUPTS;
		}
		if(user_data.status==15)
		{
			DISABLE_INTERRUPTS;
			user_data.control = 0;
			EEPROM_FLAG=0;
			ENABLE_INTERRUPTS;
		}
	}
  delay_ms(1);
} 
