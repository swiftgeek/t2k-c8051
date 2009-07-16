/********************************************************************\

  Name:      generic-p0d.c

  Contents:  Application specific (user) part of Midas Slow Control
             Bus protocol for the P0D Power supplies control/monitor
 CODE(?PR?UPGRADE?MSCBMAIN (0x6800))             
 Program Size: data=185.1 xdata=122 code=11262

 $Id$

\********************************************************************/

#include <stdio.h>          // needed for sprintf
#include "mscbemb.h"
#include "Devices/Time.h"
#include "Protocols/SmaRTC.h"

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

// Global declarations
char code node_name[] = "P0DPS";
char idata svn_rev_code[] = "$Rev$";

#define ALL_RESET 0xF

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct {
   unsigned char error;
   unsigned char control;
   unsigned char status;
   unsigned char active;
   unsigned char v5mon;
   unsigned char v15mon;
   unsigned long mytime;
  char           date[24];
} xdata user_data;

MSCB_INFO_VAR code vars[] = {

   1, UNIT_BYTE,            0, 0,            0, "Error",   &user_data.error,     // 0
   1, UNIT_BYTE,            0, 0,            0, "Control", &user_data.control,   // 1
   1, UNIT_BYTE,            0, 0,            0, "Status",  &user_data.status,    // 2
   1, UNIT_BYTE,            0, 0,            0, "Active",  &user_data.active,    // 3
   1, UNIT_BYTE,            0, 0,            0, "V5_Mon",  &user_data.v5mon,     // 4
   1, UNIT_BYTE,            0, 0,            0, "V15_Mon", &user_data.v15mon,    // 5
   4, UNIT_BYTE,            0, 0, MSCBF_HIDDEN, "BinDate", &user_data.mytime,    // 6
  24, UNIT_STRING,          0, 0, MSCBF_HIDDEN, "Date",    &user_data.date[0],   // 7
  0
};

MSCB_INFO_VAR *variables = vars;
/* P0D pin listing                  On GPIB */
sbit RESET_1  = P1 ^ 0;          // out Pin 17
sbit RESET_2  = P1 ^ 1;          // out Pin 5
sbit RESET_3  = P1 ^ 2;          // out Pin 6
sbit RESET_4  = P1 ^ 3;          // out Pin 7

sbit V5MON_1  = P1 ^ 4;          // in Pin 8
sbit V5MON_2  = P1 ^ 5;          // in Pin 9
sbit V5MON_3  = P1 ^ 6;          // in Pin 10
sbit V5MON_4  = P1 ^ 7;          // in Pin 11

sbit V15MON_1 = P2 ^ 0;          // in Pin 1
sbit V15MON_2 = P2 ^ 1;          // in Pin 2
sbit V15MON_3 = P2 ^ 2;          // in Pin 3
sbit V15MON_4 = P2 ^ 3;          // in Pin 4

sbit ACTIVE_1 = P2 ^ 4;          // in Pin 13
sbit ACTIVE_2 = P2 ^ 5;          // in Pin 14
sbit ACTIVE_3 = P2 ^ 6;          // in Pin 15
sbit ACTIVE_4 = P2 ^ 7;          // in Pin 16

sbit DATA_OEn  = P0 ^ 3;
sbit CNTRL_OEn = P0 ^ 0;

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

unsigned char user_func(unsigned char *data_in, unsigned char *data_out);
unsigned char user_read(unsigned char index);
void user_write(unsigned char index) reentrant;
void PulseReset(unsigned char ch);


void PulseReset(unsigned char ch)
{
  if (ch == ALL_RESET) {
    P1 |= 0x0F;
    delay_us(100);
    P1 &= ~0x0F;
  } else {
    P1 |= (1<<ch);
    delay_us(100);
    P1 &= ~(1<<ch);
  }

}

/*---- User init function ------------------------------------------*/
extern SYS_INFO sys_info;

void user_init (unsigned char init)
{
  char xdata i;

  /* Format the SVN and store this code SVN revision into the system */
  for (i = 0; i < 4; i++) {
    if (svn_rev_code[6 + i] < 48) {
      svn_rev_code[6 + i] = '0';
    }
  }
  sys_info.svn_revision = (svn_rev_code[6] - '0') * 1000 +
    (svn_rev_code[7] - '0') * 100 +
    (svn_rev_code[8] - '0') * 10 + (svn_rev_code[9] - '0');

  if (init); 
  
// Group setting
  sys_info.group_addr  = 600;


// Set Output Port in PP
 P1MDOUT |= 0x0F;
 RESET_1 = RESET_2 = RESET_3 = RESET_4 = 0;

// Set Input V5Monx port in OD
 P1MDOUT &= ~0xF0;
 V5MON_1 = V5MON_2 = V5MON_3 = V5MON_4 = 0;

// Set Input V15Monx port in OD
 P2MDOUT &= ~0x0F;
 V15MON_1 = V15MON_2 = V15MON_3 = V15MON_4 = 0;

// Set Input Activex port in OD
 P2MDOUT &= ~0xF0;
 ACTIVE_1 = ACTIVE_2 = ACTIVE_3 = ACTIVE_4 = 0;

// Enable Buffers
 P0MDOUT &= ~0x09;    // Set OD for Enable
 DATA_OEn  = CNTRL_OEn = 0;			// Enable buffers

// Initialize registers
 user_data.error = 0;
 user_data.control = 0;
 user_data.status = 0;
 user_data.active = 0;
 user_data.v5mon = 0;
 user_data.v15mon = 0;

// Init internal RTClock 
 SmaRTCInit();

   if (init) {
  }
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void user_write(unsigned char index) reentrant  // index corresponds to MSCB_INFO_VAR code vars[]
{
   if (index == 1) {
      if (user_data.control >= 0x0F) {
        PulseReset(ALL_RESET);
      } else {
        if (user_data.control & 0x1) PulseReset(1);
        if (user_data.control & 0x2) PulseReset(2);
        if (user_data.control & 0x4) PulseReset(3);
        if (user_data.control & 0x8) PulseReset(4);
      }
   }
   if (index == 6)
      SmaRTCSetTime(user_data.mytime);
}

/*---- User read function ------------------------------------------*/
unsigned char user_read(unsigned char index)
{
   if (index);
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
  unsigned char v5mon, v15mon, active;
  unsigned long int ltime;
  char xdata mydate[12];

// Read Monitors
  v5mon = ((P1 & 0xF0) >> 4);
  v15mon = (P2 & 0x0F);
  active = ((P2 & 0xF0) >> 4);

// Publish results
  DISABLE_INTERRUPTS;
  user_data.v5mon = v5mon;
  user_data.v15mon = v15mon;
  user_data.active = active;
  ENABLE_INTERRUPTS;

// Get RTClock  
  ltime = SmaRTCRead();
  user_data.mytime = ltime;
  ascTime(&mydate[0], ltime);
  sprintf(user_data.date, "%s", mydate);

// Slowdown a bit
  delay_ms(250);

  // Show some action
  led_blink(1, 1, 250);
}

