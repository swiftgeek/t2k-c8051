/********************************************************************\

  Name:     PowerSwitch.h
  $Id$

\********************************************************************/

#define N_POWER_CHANNELS 15
#define SST_LINE1         1

//
//--- MSCB structure
struct user_data_type {
// System
  unsigned char error;
  unsigned char control;
  unsigned char status;
  float         value[15];
  float         temperature[6];
  float         localtemp[3];
};

struct user_data_type xdata user_data;
// Global definition
// Global ON / OFF definition
#define ON     1
#define DONE   1
#define SET    1
#define OFF    0
#define CLEAR  0

// Indices for user_write functions
#define IDXCTL            2

// ESR Error Register
unsigned char bdata rESR;
sbit shutdown  = rESR ^ 0;  //0x1
