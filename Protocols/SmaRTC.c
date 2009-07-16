/********************************************************************\
  Name:         SmaRTC.c
  Created by:   Pierre-Andre Amaudruz
  Contents:     Real Time clock low level access specifically for the F410.
  See gpib410.c or generic-p0d.c code for usage.
  Mainly requires in init() :
  SmaRTCInit();
in loop()  :
     ltime = SmaRTCRead();  // RTC read
     user_data.mytime = ltime; // Epoch time 
     SmaRTCString(&mydate[0], ltime);
     sprintf(user_data.date, "%s", mydate);



  $Id$
\********************************************************************/
//
// 'F41x smaRTClock read out test
//
#include <c8051F410.h>
#include "stdio.h"
#include "SmaRTC.h"
uchar data rtc_myTime[RTC_TimeSize];

//-----------------------------------
/*
void main( void )
{
  int i;
  PCA0MD &= ~0x40; // disable WDT temporarily

  OSCICN = 0x87; // SYSCLK = 24.5MHz

  SmaRTCInit();

  SmaRTCSetTime();

  for (i=0;i<10;i++) {
  SmaRTCRead();
  }
}
*/


//-----------------------------------
void SmaRTCInit(void) {
  RTC0KEY = 0xA5; // Unlock the smaRTClock interface
  RTC0KEY = 0xF1;
// start RTC crystal OSC
  RTC_WRITE( RTC0XCN, 0x40 ); // auto-gain, Xtal
  RTC_WAIT_WRITE( RTC0CN, 0x80 ); // enable the smaRTClock
  RTC_WAIT();
  do { // wait for stabilized
    RTC_READ( RTC0XCN );
    RTC_WAIT();
  } while ( !(RTC0DAT & 0x10) ); // check CLKVLD
}

//-----------------------------------
void SmaRTCSetTime(unsigned long int mytime) {
  int i;

  for (i=2;i<6;i++) {
    rtc_myTime[i] = (uchar) (mytime >> (8*(i-2)) & 0xFF);    
  }
// set RTC time
  rtc_WriteTimeReg( CAPTURE0 ); // set CAPTRUE register
  RTC_WAIT_WRITE( RTC0CN, 0x82 ) // enable, set time
  RTC_WAIT(); // wait for set time complete
  do {
    RTC_READ( RTC0CN );
    RTC_WAIT();
  } while ( RTC0DAT & 0x02 ); // check RTC0SET
  RTC_WRITE( RTC0CN, 0x90 ); // run RTC
}

//-----------------------------------
unsigned long int SmaRTCRead(void) {
  unsigned long int ltime;
  int i;
// read RTC time
  RTC_WAIT_WRITE( RTC0CN, 0x91 ) // enable, capture
  RTC_WAIT(); // wait for set time complete
  do {
    RTC_READ( RTC0CN );
    RTC_WAIT();
  } while ( RTC0DAT & 0x01 ); // check RTC0CAP
  rtc_ReadTimeReg( CAPTURE0 ); // read out CAPTURE register
  ltime = 0;
  for (i=2;i<6;i++) {
   ltime |= ((unsigned long int) rtc_myTime[i]) << (8*(i-2));
  }
  return ltime;
}

//-----------------------------------
// Write rtc_Time[] to the smaRTClock CAPTURE/ALARM register
//
void rtc_WriteTimeReg( uchar addr ) {
  uchar idx;

  RTC_WAIT_WRITE( addr, rtc_myTime[ 0 ] );
  for ( idx = 1; idx < RTC_TimeSize; idx++ ) {
  RTC_WAIT();
  RTC0DAT = rtc_myTime[ idx ];
  }
}

//------------------------------------
// Read data from the smaRTClock CAPTURE/ALARM register
// and store it to rtc_Time[]
//
void rtc_ReadTimeReg( uchar addr ) {
uchar idx;

  RTC_WAIT(); // set the first address with auto-read
  RTC0ADR = addr | RTC_BUSY | RTC_AUTORD | RTC_SHORT;
  for ( idx = 0; idx < RTC_TimeSize; idx++ ) {
  RTC_WAIT();
  rtc_myTime[ idx ] = RTC0DAT;
  }
}
