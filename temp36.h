/********************************************************************\

  Name:         temp36.h
  Created by:   Noel Wu				

  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for temp 36 board

\********************************************************************/

unsigned char ADT7486A_addrArray[] = {0x48, 0x4A};
float xdata temperature1, temperature2;
struct
{
  	unsigned char error[5];
	unsigned long SerialN;
	unsigned char control;       
	unsigned char status;	
  	float         Temp[36];    // ADT7486A external2 temperature [degree celsius]
}xdata user_data;

/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
void user_init(unsigned char init);
void user_loop(void);
void user_write(unsigned char index) reentrant;
unsigned char user_read(unsigned char index);
unsigned char user_func(unsigned char *data_in, unsigned char *data_out);
