/********************************************************************\
Name:         loader.c
Created by:   Bahman Sotoodian                Mar/11/2008


Contents:     Application specific (user) part of
Midas Slow Control Bus protocol
for External EEPROM of FEB64 board

Memory usage:  

JTAG Chain: 
FEB64 : 1 0 5 0
LPB   : 0 0 0 0 

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
int timeout = 0;

#ifdef L_FEB64
// Vreg Enable port assignment Rev 1 ONLY
sbit REG_EN  = P3 ^ 2;
// RESET output pin for PCA9539 (PIO) and LTC1665 (DACx8)
sbit RESETN  = P1 ^ 3;

#elif defined(L_TEMP36)

#elif defined(L_CMB)

#elif defined(L_LPB)
#endif

// User Data structure declaration
//-----------------------------------------------------------------------------
MSCB_INFO_VAR code vars[] = {
  4, UNIT_BYTE,            0, 0,           0, "SerialN",    &user_data.serialN,  	  // 0
  1, UNIT_BYTE,            0, 0,           0, "Control",    &user_data.control,      // 1
  1, UNIT_BYTE,            0, 0,           0, "EEPage",     &user_data.eepage,       // 2
  1, UNIT_BYTE,            0, 0,           0, "Status",     &user_data.status,       // 3 
  2, UNIT_BYTE,            0, 0,           0, "StrcSze",    &user_data.structsze,    // 4
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
  char xdata pca_add=0;
  unsigned int xdata crate_add=0, board_address=0;

  if (init) {}

  // Initialize control and status
  user_data.control   = 0;
  user_data.status    = 0;
  user_data.structsze = 0;
  EEPROM_FLAG=0;

//-----------------------------------------------------------------------------
//
#ifdef L_TEMP36
  sprintf(sys_info.node_name,"TEMP36");
  SFRPAGE  = CONFIG_PAGE;
  P2MDOUT |= 0x31; // Setting the RAM_CSn. SPI_MOSI, SPI_SCK to push pull
  P2MDOUT &= 0xFB; // Setting the RAM_WPn to open drain

//-----------------------------------------------------------------------------
//
#elif defined(L_FEB64)
  // P3.7:RAMCSn   .6:CSn6      .5:CSn4     .4:SPARE5  | .3:SPARE4  .2:REG_EN   .1:CSn3    .0:CSn2 
  // P2.7:SPARE1   .6:CSn7      .5:CSn6     .4:SPIMOSI | .3:SPISCK  .2:RAMHLDn  .1:SPIMISO .0:RAMWPn 
  // P1.7:ASUMSync .6:ASUMTestn .5:ASUMPWDn .4:ASUMCSn | .3:ResetN  .2:SPARE2   .1:SPARE3  .0:SST_DRV 
  // P0.7:CSn1     .6:CSn0      .5:485TXEN  .4:QPUMPCLK| .3:SMBCLK  .2:SMBDAT   .1:Rx      .0:Tx 
  sprintf(sys_info.node_name,"FEB64");
  P3MDOUT |= 0x80; // RAM_CSn in PP, REG_EN in OD
  P2MDOUT |= 0x18; // SPI_MOSI, SPI_SCK in PP
  P2MDOUT &= 0xFE; // RAM_WPn in OD
  P1MDOUT |= 0x08; // ResetN in PP
  P0MDOUT |= 0x20; // RS485 in PP

// Get ALL Vreg on
//  while((REG_EN == 0) && (timeout < 30)) {
//    delay_ms(100);
//    timeout++;
//  }
// Reset PCA9539 (PIO)
//  RESETN = 0;
// RESETN = 1;
  delay_us(100);

//
// Get Card address through SMB PCA port
//-----------------------------------------------------------------------------
#ifdef _PCA9539_
  SFRPAGE = CONFIG_PAGE;

  PCA9539_Init(); //PCA General I/O (Bias Enables and Backplane Addr) initialization
  delay_us(10);

  //Write to the PCA register for setting 1.x to input
  PCA9539_WriteByte(BACKPLANE_INPUT_ENABLE);
  delay_us(10);

  PCA9539_Read(BACKPLANE_READ, &pca_add, 1);
  delay_us(10);
  //C C C C C C 0 B B is the MSCB Addr[8..0], 9 bits
  //Modifying what the board reads from the PCA
  //Externally, the crate address are reversed

  crate_add= ((~pca_add)<<1)  & 0x01F8;
  //Internally, the board address are not reversed
  board_address = crate_add | ((pca_add) & 0x0003);
  if (board_address == 504) board_address = 0;
  sys_info.node_addr   = board_address;
#endif

//-----------------------------------------------------------------------------
//
#elif defined(L_LPB)
  // P3.7:A7       .6:A6        .5:A5       .4:A4      | .3:A3      .2:A2       .1:A1      .0:A0 
  // P2.7:+1.8En   .6:+3.3En    .5:+5En     .4:SPIMOSI | .3:SPISCK  .2:RAMHLDn  .1:SPIMISO .0:RAMWP
  // P1.7:NC       .6:+6ddFlag  .5:R/HClock .4:R/HData | .3:+6ddEN  .2:RAMCS    .1:D2ASync .0:SST_DRV 
  // P0.7:NC       .6:NC        .5:485TXEN  .4:NC      | .3:NC      .2:NC       .1:Rx      .0:Tx 
  sprintf(sys_info.node_name,"LPB");
  P3MDOUT  = 0x00; // Crate address in OD
  P2MDOUT |= 0x18; // SPI_MOSI, SPI_SCK in PP
  P2MDOUT &= 0xFE; // RAM_WPn in OD
  P1MDOUT |= 0x04; // RAM_CSn in PP
  P0MDOUT |= 0x20; // RS485_ENABLE in PP

  // Read Board address from P3 port directly
  // Configure and read the address
  // C C C C C C 1 0 1 is the MSCB Addr[8..0], 9 bits
  // Modifying what the board reads from the Pins
  pca_add= P3;
  board_address= (((~pca_add)<<1) & 0x01F8)| 0x0005;
  sys_info.node_addr   = board_address;

//-----------------------------------------------------------------------------
//
#else defined(L_CMB)
  sprintf(sys_info.node_name,"CMB");
  P3MDOUT |= 0x80; // RAM_CSn in PP
  P2MDOUT |= 0x18; // SPI_MOSI, SPI_SCK in PP
  P2MDOUT &= 0xFE; // RAM_WPn in OD

#endif	


//-----------------------------------------------------------------------------
//
// Initialize EEPAGE access (SPI)
  if(ExtEEPROM_Init(PAGE_SIZE)) {
    DISABLE_INTERRUPTS;
    user_data.status = 0xFF;
    ENABLE_INTERRUPTS;
  }

  //
  // Read ALL eepage fomr Protected page
  ExtEEPROM_Read(WP_START_ADDR, (unsigned char*)&eepage, PAGE_SIZE);
  DISABLE_INTERRUPTS;
  user_data.serialN = eepage.SerialN;
  user_data.structsze = eepage.structsze;
  ENABLE_INTERRUPTS;

  //
  // Check Structure size and publish it (above)
  if (eepage.structsze != PAGE_SIZE) {
    DISABLE_INTERRUPTS;
    user_data.status = 0xFF;
    ENABLE_INTERRUPTS;
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

  // Wait for the EEPROM_FLAG to be set
  if(EEPROM_FLAG) {
    // Update S/N from mscb to eepage
    eepage.SerialN = user_data.serialN;
    eepage.structsze = PAGE_SIZE;

    // Write to the Protected Memory
    FAILED = ExtEEPROM_WriteProtect((unsigned char*)&eepage, PAGE_SIZE);
    if(!FAILED) {
      DISABLE_INTERRUPTS;
      user_data.status = 1;
      ENABLE_INTERRUPTS;
    }

    // Check S/N readback
    FAILED = ExtEEPROM_Read(WP_START_ADDR,(unsigned char*)&eepage2, PAGE_SIZE);
    if(!FAILED && (user_data.serialN == eepage2.SerialN))	{
      DISABLE_INTERRUPTS;
      user_data.status |= (1<<1);
      ENABLE_INTERRUPTS;
    }

    // Write to Page 0 as well
    FAILED = ExtEEPROM_Write_Clear(page_addr[0],(unsigned char*)&eepage, PAGE_SIZE, WRITE);
    if(!FAILED) {
      DISABLE_INTERRUPTS;
      user_data.status |= (1<<2);
      ENABLE_INTERRUPTS;
    }

    // Check S/N readback
    FAILED = ExtEEPROM_Read(page_addr[0],(unsigned char*)&eepage2, PAGE_SIZE);

    if(!FAILED && (user_data.serialN == eepage2.SerialN)) {
      DISABLE_INTERRUPTS;
      user_data.status |= (1<<3);
      ENABLE_INTERRUPTS;
    }

    // Check for error
    if(user_data.status == 0xF) {
      DISABLE_INTERRUPTS;
      user_data.control = 0;
      user_data.structsze = eepage2.structsze;
      EEPROM_FLAG=0;
      ENABLE_INTERRUPTS;
    }
    // if successful should read 0xF (00001111) in status
  }
  delay_ms(1);
}

