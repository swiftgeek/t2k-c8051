/********************************************************************\

  Name:      ccbertan410.c

  Contents:  Application specific (user) part of Midas Slow Control
             Bus protocol for Bertan 225 Series 30 KV HV power supply
             Include RTC GMT
Program Size: data=206.1 xdata=143 code=13689 without RTC_410
Program Size: data=221.1 xdata=242 code=15704 
             
  $Id$

\********************************************************************/

#include <c8051F410.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Time.h"
#include "SmaRTC.h"
#include "mscbemb.h"

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

char code node_name[] = "TPC_HV";
char xdata svn_rev_code[] = "$Rev$";

#define MAX_VOLTAGE 28000  // maximum voltage in Volts - this is a hard limit
#define MAX_CURRENT 75     // maximum current im micro Ampere - this is a hard limit
#define MINDEL 200        // minimum delay in ms between ramp steps
#define TPC_R 393.4        // total TPC resistance (MOhms)
#define IOFF -0.1718       // hv supply current offset

#define GPIB_ADDR_0 4      // GPIB address for channel 0 

unsigned char idata chn_bits=0;
/* chn_bits defines... */
#define DEMAND_CHANGED     (1<<0)
#define RAMP_UP            (1<<1)
#define RAMP_DOWN          (1<<2)
#define HV_LIMIT_CHANGED   (1<<3)
#define CUR_LIMIT_CHANGED  (1<<4)

float xdata u_actual;
unsigned long xdata t_ramp;
char xdata str[32], *buf;

#ifdef RTC_410
unsigned long int ltime;
char xdata mydate[24];
#endif

unsigned char xdata tiwp;

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

/* CSR status bits -> user_data.status */
#define STATUS_RAMP_UP     (1<<0)
#define STATUS_RAMP_DOWN   (1<<1)
#define STATUS_VLIMIT      (1<<2)
#define STATUS_ILIMIT      (1<<3)
#define STATUS_LOWCUR      (1<<4)
#define STATUS_HICUR       (1<<5)

struct {
   unsigned char status;
   float u_demand;
   float u_meas;
   float i_meas;
   unsigned int ramp_up;
   unsigned int ramp_down;
   float u_limit;
   float i_limit;
   char  warning[32];
   char date[24];
   unsigned char gpib_adr;
   unsigned long mytime;
} xdata user_data;

MSCB_INFO_VAR code vars[] = {

   1, UNIT_BYTE,            0, 0,           0, "Status",  &user_data.status,      // 0
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "Udemand", &user_data.u_demand,    // 1
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "Umeas",   &user_data.u_meas,      // 2
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "Imeas",   &user_data.i_meas,      // 3
   2, UNIT_VOLT,            0, 0,           0, "RampUp",  &user_data.ramp_up,     // 4
   2, UNIT_VOLT,            0, 0,           0, "RampDown",&user_data.ramp_down,   // 5
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "Ulimit",  &user_data.u_limit,     // 6
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "Ilimit",  &user_data.i_limit,     // 7
  32, UNIT_STRING,          0, 0,           0, "warning", &user_data.warning,     // 8
#ifdef RTC_410
  24, UNIT_STRING,          0, 0,           0,  "GMT Date",&user_data.date[0],    // 9
#endif
   1, UNIT_BYTE,            0, 0, MSCBF_HIDDEN, "GPIB Adr",&user_data.gpib_adr,   // 10
#ifdef RTC_410
   4, UNIT_BYTE,            0, 0, MSCBF_HIDDEN, "GMT Epok",&user_data.mytime,     // 11
#endif
   0
};

MSCB_INFO_VAR *variables = vars;

#ifdef CPU_C8051F410
#define GPIB_DATA P2

/* GPIB control/status bits DB24 */
sbit GPIB_REN  = P1 ^ 0;          // REMOTE ENABLE, 	Pin 9
sbit GPIB_EOI  = P1 ^ 1;          // END-OR-IDENTIFY, 	Pin 10
sbit GPIB_DAV  = P1 ^ 2;          // DATA VALID,		Pin 11
sbit GPIB_NRFD = P1 ^ 3;          // NOT READY FOR DATA,Pin 12
sbit GPIB_NDAC = P1 ^ 4;          // NOT DATA ACCEPTED,	Pin 13
sbit GPIB_IFC  = P1 ^ 5;          // INTERFACE CLEAR,	Pin 14
sbit GPIB_SRQ  = P1 ^ 6;          // SERVICE REQUEST,	Pin 15
sbit GPIB_ATN  = P1 ^ 7;          // ATTENTION,			Pin 16

sbit BUF_CLE   = P0 ^ 0;   // P0.
sbit BUF_DATAE = P0 ^ 3;   // P0.3
#endif

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char index) reentrant;
unsigned char send (unsigned char adr, char *str);
unsigned char send_byte (unsigned char b);
unsigned char enter(unsigned char adr, char *str, unsigned char maxlen);

void set_voltage_limit (float value);
void set_hv(float value) reentrant;
void read_hvi(void);
void ramp_hv(void);
void set_current_limit (float value);

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
    (svn_rev_code[8] - '0') * 10 +
    (svn_rev_code[9] - '0');

//	P0MDIN = 0xFF;						// default 0xFF all digital pins
// 	P1MDIN = 0xFF;
//	P2MDIN = 0xFF;

  P0MDOUT = 0x18;					// Default OD 485TX, TXD
  P1MDOUT = 0x00;					// OD
  P2MDOUT = 0xFF;					// OD

  P0 = 0x03;    // default 0xFF
  P1 = 0xFF;
  P2 = 0xFF;

  /* set initial state of lines */
  GPIB_DATA = 0xFF;
  GPIB_EOI = 1;
  GPIB_DAV = 1;
  GPIB_NRFD = 1;
  GPIB_NDAC = 1;
  GPIB_IFC = 1;
  GPIB_SRQ = 1;
  GPIB_ATN = 1;
  GPIB_REN = 1;

  BUF_CLE = 0;			// Enable buffers
  BUF_DATAE = 0;

  /* initialize GPIB */
  GPIB_IFC = 0;
  delay_ms (1);
  GPIB_IFC = 1;

  GPIB_ATN = 0;
  send_byte (0x14);		// DCL
  GPIB_ATN = 1;
  
  user_data.gpib_adr = GPIB_ADDR_0;
  
  sprintf (str, "OE2");				// prohibit overvoltage setting
  send (user_data.gpib_adr, str);
  sprintf (str, "OC0");				// hv supply will *not* shutdown if current overload
  send (user_data.gpib_adr, str);
  sprintf (str, "SE0");				// enables SRQ in response to overvoltage detection
  send (user_data.gpib_adr, str);
  sprintf (str, "SC0");				// enables SRQ in response to overcurrent detection
  send (user_data.gpib_adr, str);

  user_data.status = 0;
  if (init) {
   user_data.ramp_up = 10;
   user_data.ramp_down = 10;
   user_data.u_limit = MAX_VOLTAGE;
   user_data.i_limit = MAX_CURRENT;
  }
  sprintf (user_data.warning, "current OK");

 // set_voltage_limit(user_data.u_limit);
 // set_current_limit(user_data.i_limit);

 // read_hvi();                                // check to see if hv is already on

  if ( user_data.u_meas < 10 )               // bertan hv must be off
  {
      user_data.u_demand = 0;
      u_actual = 0;
      set_hv(0);
  }
  else                                       // bertan hv must already be on
  {
      user_data.u_demand = user_data.u_meas; // restore to status quo
      u_actual = user_data.u_meas;
  }

#ifdef RTC_410
// Init RTC on 410
   SmaRTCInit();
#endif
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void user_write(unsigned char index) reentrant  // index corresponds to MSCB_INFO_VAR code vars[]
{
   if (index == 1) {
      /* indicate new demand voltage */
      chn_bits |= DEMAND_CHANGED;
   }

   /* check voltage limit */
   if (index == 6) {
      if (user_data.u_limit > MAX_VOLTAGE) {
         user_data.u_limit = MAX_VOLTAGE;
      }
      chn_bits |= HV_LIMIT_CHANGED;
   }

   /* check current limit */
   if (index == 7) {
      if (user_data.i_limit > MAX_CURRENT) {
         user_data.i_limit = MAX_CURRENT;
      }
      chn_bits |= CUR_LIMIT_CHANGED;
   }
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

/*---- Functions for GPIB port -------------------------------------*/
unsigned char send_byte(unsigned char b)
{
   unsigned int i;

//   yield();

   /* wait for NRFD go high */
   for (i = 0; i < 1000; i++)
      if (GPIB_NRFD == 1)
         break;

   if (GPIB_NRFD == 0)
      return 0;

   GPIB_DATA = ~b;              // negate
   delay_us(10);                // setup time, let signals settle
   GPIB_DAV = 0;

   /* wait for NDAC go high */
   for (i = 0; i < 1000; i++) {
      delay_us(10);
      if (GPIB_NDAC == 1)
         break;
   }

   if (GPIB_NDAC == 0) {
      GPIB_DAV = 1;
      GPIB_DATA = 0xFF;
      return 0;                 // timeout
   }

   GPIB_DAV = 1;
   GPIB_DATA = 0xFF;            // prepare for input

   /* wait for NRFD go high */
   for (i = 0; i < 1000; i++)
      if (GPIB_NRFD == 1)
         break;

   if (GPIB_NRFD == 0)
      return 0;

   return 1;
}

/*------------------------------------------------------------------*/
unsigned char send(unsigned char adr, char *str)
{
   int i; //, len;
   char s;

  /*---- address cycle ----*/

   GPIB_ATN = 0;                // assert attention
   send_byte(0x3F);             // unlisten
   send_byte(0x5F);             // untalk
   send_byte(0x20 | adr);       // listen device
   send_byte(0x40 | 21);        // talk 21
   GPIB_ATN = 1;                // remove attention

  /*---- data cycles ----*/
//   len = strlen(str);
   for (i = 0; str[i] > 0; i++) {
      s = send_byte(str[i]);
      if (s == 0) return 0;
   }

   GPIB_EOI = 0;
   send_byte(0x0A);             // NL
   GPIB_EOI = 1;

   return i;
}

/*------------------------------------------------------------------*/
unsigned char enter(unsigned char adr, char *str, unsigned char maxlen)
{
   unsigned long t;
   unsigned char i, flag;
   unsigned int j;

  /*---- address cycle ----*/

   GPIB_ATN = 0;                // assert attention
   send_byte(0x3F);             // unlisten
   send_byte(0x5F);             // untalk
   send_byte(0x20 | 21);        // listen 21
   send_byte(0x40 | adr);       // talk device
   GPIB_ATN = 1;                // remove attention

  /*---- data cycles ----*/

   GPIB_NDAC = 0;               // init NDAC line

   for(i=0;i<maxlen;i++) str[i] = 0;
   
   for (i = 0; i < maxlen; i++) {
      yield();

      GPIB_NRFD = 1;            // signal ready for data

      /* wait 1s for DAV go low */
      t = time();
      do {
         if (GPIB_DAV == 0)
            break;

         yield();

      } while (time() - t < 100);

      if (GPIB_DAV == 1) {
         GPIB_NDAC = 1;
         GPIB_NRFD = 1;
         return 0;           // timeout
      }

//      led_blink(1, 1, 100);     // signal data received

      GPIB_NRFD = 0;            // signal busy

      str[i] = ~GPIB_DATA;      // read negated data

      flag = GPIB_EOI;          // read EOI flag

      GPIB_NDAC = 1;            // signal acknowledge

      /* wait for DAV go high */
      for (j = 0; j < 1000; j++) {
        delay_us(10);
        if (GPIB_DAV == 1)
          break;
      }
      GPIB_NDAC = 0;            // remove acknowledge

      if (flag == 0)            // stop if end of data
         break;
   }

   GPIB_NDAC = 1;               // release handshake lines
   GPIB_NRFD = 1;

   /* stop talker */
   GPIB_ATN = 0;                // assert attention
   send_byte(0x3F);             // unlisten
   send_byte(0x5F);             // untalk
   GPIB_ATN = 1;                // remove attention

   return i;
}

/*------------------------------------------------------------------*/

void set_voltage_limit (float value)
{
  // Set the voltage limit

  sprintf (str, "L%06.3fKG", value/1000);
  send (user_data.gpib_adr, str);
  delay_ms(2);

  return;
}

/*------------------------------------------------------------------*/

void set_current_limit (float value)
{
  // Set the current limit

  sprintf (str, "L%06.3fUG", value);
  send (user_data.gpib_adr, str);
  delay_ms(2);

  return;
}

/*------------------------------------------------------------------*/

void set_hv(float value) reentrant
{
  if (value > user_data.u_limit)   // check for limit
  {
    value = user_data.u_limit;
    user_data.status |= STATUS_VLIMIT;
  } 
  else
  {
    user_data.status &= ~STATUS_VLIMIT;
  }

  sprintf (str, "P%06.3fKG", value/1000);
  send (user_data.gpib_adr, str);
}

/*------------------------------------------------------------------*/

void read_hvi(void)
{
  float hv, current;

  sprintf (str, "T0");
  send (user_data.gpib_adr, str);

  tiwp = enter(user_data.gpib_adr, str, sizeof(str));
/*
  if (tiwp) {
    DISABLE_INTERRUPTS;
    sprintf (user_data.warning, str);
    ENABLE_INTERRUPTS;
  } 
*/

//-PAA For test
//  sprintf(str, "N V12.345K 987.54U");

  hv  =  str[8] - 48;
  hv += (str[7] - 48)*10;
  hv += (str[6] - 48)*100;
  hv += (str[4] - 48)*1000;
  hv += (str[3] - 48)*10000;
  if (hv < 0) hv = 0;
 
  current  = (str[17] - 48)/100;
  current += (str[16] - 48)/10;
  current += (str[14] - 48);
  current += (str[13] - 48)*10;
  current += (str[12] - 48)*100;
  if (current < 0) current = 0;

  DISABLE_INTERRUPTS;
  user_data.u_meas = hv;
  user_data.i_meas = current;
  ENABLE_INTERRUPTS;

  if ( current >= user_data.i_limit )  {
    user_data.status |= STATUS_ILIMIT;
  } else
  {
    user_data.status &= ~STATUS_ILIMIT;
  }

  if ( current > IOFF + 1.1 * hv/TPC_R )
  {
    sprintf (user_data.warning, "Warning! High current - %6.3f", current);
  DISABLE_INTERRUPTS;
    user_data.status |=  STATUS_HICUR;
    user_data.status &= ~STATUS_LOWCUR;
  ENABLE_INTERRUPTS;
  }
  else if ( current < IOFF + 0.9 * 1000 * hv/TPC_R )
  {
    sprintf (user_data.warning, "Warning! Low current - %6.3f", current);
  DISABLE_INTERRUPTS;
    user_data.status |=  STATUS_LOWCUR;
    user_data.status &= ~STATUS_HICUR;
  ENABLE_INTERRUPTS;
  }
  else
  {
    sprintf (user_data.warning, "current OK");
  DISABLE_INTERRUPTS;
    user_data.status &= ~STATUS_HICUR;
    user_data.status &= ~STATUS_LOWCUR;
  ENABLE_INTERRUPTS;
  }
}

/*------------------------------------------------------------------*/

void ramp_hv(void)
{
   int delta;

   /* only process ramping when HV is on and not tripped */
   if (!(user_data.status & STATUS_ILIMIT)) {

      if (chn_bits & DEMAND_CHANGED) {
         /* start ramping */

         if (user_data.u_demand > u_actual &&
             user_data.ramp_up > 0) {
            /* ramp up */
            chn_bits |= RAMP_UP;
            chn_bits &= ~RAMP_DOWN;
            user_data.status |= STATUS_RAMP_UP;
            user_data.status &= ~STATUS_RAMP_DOWN;
            chn_bits &= ~DEMAND_CHANGED;
         }

         if (user_data.u_demand < u_actual &&
             user_data.ramp_down > 0) {
            /* ramp down */
            chn_bits &= ~RAMP_UP;
            chn_bits |= RAMP_DOWN;
            user_data.status &= ~STATUS_RAMP_UP;
            user_data.status |= STATUS_RAMP_DOWN;
            chn_bits &= ~DEMAND_CHANGED;
         }

         /* remember start time */
         t_ramp = time();
      }

      /* ramp up */
      if (chn_bits & RAMP_UP) {
         delta = time() - t_ramp;  // delta is milli-seconds since last ramp step
         if (delta > MINDEL) {
            u_actual += (float) user_data.ramp_up;

            if (u_actual >= user_data.u_demand) {
               /* finish ramping */

               u_actual = user_data.u_demand;
               chn_bits &= ~RAMP_UP;
               user_data.status &= ~STATUS_RAMP_UP;
            }

            set_hv(u_actual);
            t_ramp = time();
         }
      }

      /* ramp down */
      if (chn_bits & RAMP_DOWN) {
         delta = time() - t_ramp;  // delta is milli-seconds since last ramp step
         if (delta > MINDEL) {
            u_actual -= (float) user_data.ramp_down;

            if (u_actual <= user_data.u_demand) {
               /* finish ramping */

               u_actual = user_data.u_demand;
               chn_bits &= ~RAMP_DOWN;
               user_data.status &= ~STATUS_RAMP_DOWN;
            }

            set_hv(u_actual);
            t_ramp = time();
         }
      }
   }
}

/*---- User loop function ------------------------------------------*/

void user_loop(void)
{

  /* set voltage limit if changed */
  if (chn_bits & HV_LIMIT_CHANGED) {
    set_voltage_limit(user_data.u_limit);
    chn_bits &= ~HV_LIMIT_CHANGED;
  }

  /* set current limit if changed */
  if (chn_bits & CUR_LIMIT_CHANGED) {
    set_current_limit(user_data.i_limit);
    chn_bits &= ~CUR_LIMIT_CHANGED;
  }

  // Read Voltage and Current
  read_hvi();

  // Yield to other activities (MSCB)
//  yield();

  // Do ramping if necessary
  ramp_hv();

#ifdef RTC_410
  // RTC stuff
  ltime = SmaRTCRead();
  user_data.mytime = ltime;
  ascTime(mydate, ltime);
  sprintf(user_data.date, "%s", mydate);
#endif

  // Slow it down
  delay_ms (MINDEL);

  // Loop timing
  led_blink(0, 1, 50);
}

