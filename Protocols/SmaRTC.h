/********************************************************************\
  Name:         SmaRTC.h
  Created by:   Pierre-Andre Amaudruz
  Contents:     Real Time clock low level access specifically for the F410.
  $Id$
\********************************************************************/
#ifndef  _SMARTC_H
#define  _SMARTC_H

typedef unsigned char uchar;

// RTC register address
enum {
CAPTURE0, CAPTURE1, CAPTURE2, CAPTURE3, CAPTURE4, CAPTURE5,
RTC0CN, RTC0XCN, ALARM0, ALARM1, ALARM2, ALARM3, ALARM4, ALARM5,
RAMADDR, RAMDATA };

// bits on RTC0ADR
#define RTC_SHORT 0x10
#define RTC_VREGEN 0x20
#define RTC_AUTORD 0x40
#define RTC_BUSY 0x80

// macro to handle RTC register
#define RTC_WAIT() while ( RTC0ADR & RTC_BUSY )
#define RTC_WRITE( addr, dat ) { RTC0ADR = (addr) | RTC_SHORT; RTC0DAT = dat; }
#define RTC_READ( addr ) { RTC0ADR = (addr) | RTC_SHORT | RTC_BUSY; }
#define RTC_WAIT_WRITE( addr, dat ) { RTC_WAIT(); RTC_WRITE( addr, dat ); }
#define RTC_WAIT_READ( addr, dat ) { RTC_WAIT(); RTC_READ( addr ); RTC_WAIT(); dat = RTC0DAT; }

// captured RTC time
#define RTC_TimeSize 6

// local function prototype
void SmaRTCInit(void);
void rtc_WriteTimeReg( uchar addr );
void rtc_ReadTimeReg( uchar addr );
void SmaRTCSetTime(unsigned long int mytime);
unsigned long int SmaRTCRead(void);

#endif