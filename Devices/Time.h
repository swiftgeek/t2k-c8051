/********************************************************************\
  Name:         Time.h
  Created by:   Pierre-Andre Amaudruz
  Contents:     Epoch time convertion to string, based on the asctime.c
  $Id$
\********************************************************************/
#ifndef  _TIME_H
#define  _TIME_H

struct TM {
  int tm_sec;                   /* Seconds.     [0-60] (1 leap second) */
  int tm_min;                   /* Minutes.     [0-59] */
  int tm_hour;                  /* Hours.       [0-23] */
  int tm_mday;                  /* Day.         [1-31] */
  int tm_mon;                   /* Month.       [0-11] */
  int tm_year;                  /* Year - 1900.  */
  int tm_wday;                  /* Day of week. [0-6] */
  int tm_yday;                  /* Days in year.[0-365] */
  int tm_isdst;                 /* DST.         [-1/0/1]*/
};

#define BASE_YEAR       1970
#define START_WDAY      4       /* constant. don't change */
#define SECS_PER_MIN    (unsigned long int) 60
#define SECS_PER_HOUR   (unsigned long int)(SECS_PER_MIN*60)
#define SECS_PER_DAY    (unsigned long int)(SECS_PER_HOUR*24)

void ascTime(char * str, unsigned long int ltime);
void secs_to_tm (unsigned long int t, struct TM *tm);

#endif