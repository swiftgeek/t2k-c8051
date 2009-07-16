/********************************************************************\
  Name:         Time.c
  Created by:   Pierre-Andre Amaudruz
  Contents:     Epoch time convertion to string UTC, based on the asctime.c
  $Id$ 
\********************************************************************/
#include <stdio.h>          // needed for sprintf
#include "Time.h"

char code month_name[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};  
unsigned int xdata month_days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
// unsigned long xdata tzoffset;          /* timezone offset in seconds */
struct TM xdata tm;

//-----------------------------------
void ascTime(char * str, unsigned long int ltime) {
  char xdata mydate[12], mydate1[10];

  secs_to_tm(ltime, &tm);
// sprintf(mydate, "%.4d-%.2d-%.2d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
  sprintf(mydate, "%.4d-%3s-%.2d", tm.tm_year+1900, month_name[tm.tm_mon], tm.tm_mday);
  sprintf(mydate1, " %.2d:%.2d:%.2d", tm.tm_hour, tm.tm_min, tm.tm_sec);
  sprintf(str, "%s%s", mydate, mydate1);
}

//---------------------------------------
void secs_to_tm (unsigned long int t, struct TM *tm)
{
        unsigned long int xdata days, rem;
        unsigned int xdata y, mon;
        days = t / SECS_PER_DAY;
        rem = t - (days * SECS_PER_DAY);
        tm->tm_hour = rem / SECS_PER_HOUR;
        rem -= tm->tm_hour*SECS_PER_HOUR;
        tm->tm_min = rem / SECS_PER_MIN;
        rem -= tm->tm_min*SECS_PER_MIN;
        tm->tm_sec = rem;
        tm->tm_wday = (days + START_WDAY) % 7;
        y = BASE_YEAR;
        while (days >= 365) {
                if ((y % 4) == 0) {
                        if (days == 365)
                                break;
                        // leap year
                        days--;
                }
                days -= 365;
                y++;
        }
        tm->tm_year = y - 1900;
        tm->tm_yday = days;

        // Set correct # of days for feburary 
        month_days[1] = ((y % 4) == 0) ? 29 : 28;

        for (mon = 0; mon < 12; mon++) {
                if (days < month_days[mon]) {
                        tm->tm_mon = mon;
                        break;
                }
                days -= month_days[mon];
        }
        if (mon == 12)  {
                // PANIC
                days = days + 0;
//                printf ("secs_to_tm:remaining days=%d\n", days);
        }

        tm->tm_mday = days + 1;
        tm->tm_isdst = 0;
        // tm->tm_gmtoff = 0;
}
