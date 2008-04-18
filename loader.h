/********************************************************************\

  Name:         loader.h
  Created by:   Bahman Sotoodian				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for External EEPROM of FEB64 test board

  $Id:$

\********************************************************************/
#ifndef  _LOADER_H_
#define  _LOADER_H_



#define PAGE_SIZE			0xC8
#define NUMEBER_PAGES	0x06
#define CLEAR				0x01
#define WRITE				0x00

//Defining the page mapping

unsigned int xdata page_addr[]={0x600,0x0,0x100,0x200,0x300,0x400,0x500};

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
	unsigned char control;       
	unsigned char eepage;
	unsigned char status;	     
};

struct user_data_type xdata user_data;

struct EEPAGE {
float lVIlimit[8]; // vQ iQ +6Vd +6Va -6Va -6Ia +6Ia +6Id 
float uVIlimit[8];   
float luCTlimit;
float uuCTlimit;
float lSSTlimit;
float uSSTlimit;
float lVQlimit;
float uVQlimit;
float lIQlimit;
float uIQlimit;
float lVBiaslimit;
float uVBiaslimit;
float lIBiaslimit;
float uIBiaslimit;
};

//   vQ   iQ  +6Vd  +6Va  -6Va  -6Ia  +6Ia  +6Id 
// uC Temperature
// SST Temperature
//   vQ    iQ  +6Vd  +6Va -6Va  -6Ia   +6Ia  +6Id 
struct EEPAGE xdata eepage = {
     30.0, 0.0, 5.5, 5.5, -6.5, -20.0, 5.0,   5.0
    ,73.0, 0.1, 6.5, 6.5, -5.5, -1.0, 100.0, 200.0
	 ,23., 45.
    ,20., 30.
};

struct EEPAGE xdata eepage2 ;

struct TEST{
unsigned char Var1;
unsigned char Var2;
};

struct TEST xdata test1 = {
0x33,0x22
};

struct TEST xdata test2;
/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
void user_init(unsigned char init);
void user_loop(void);
void user_write(unsigned char index) reentrant;
unsigned char user_read(unsigned char index);
unsigned char user_func(unsigned char *data_in, unsigned char *data_out);

#endif
