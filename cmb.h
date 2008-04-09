/********************************************************************\
 Name:         cmb.h
 Created by:   Bahman Sotoodian                
 Created on:   April/9/2008 
 
 Contents:     Application specific (user) part of
 					Midas Slow Control for CMB board. 					 
 
 $Id:$
 \********************************************************************/
#ifndef _CMB_H_
#define _CMB_H_

// ADT7486A temperature addresses
#define ADT7486A_address 0x48	

// Global definition
#define CLEAR 0
#define SET 1

// ESR Error Register
unsigned long bdata rESR;
sbit IntssTT   = rESR ^ 0; 
sbit Ext1ssTT  = rESR ^ 1;	
sbit Ext2ssTT  = rESR ^ 2;	

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/
struct user_data_type {
unsigned char error;
float	pIs4V;
float IsSC;       
float A33VMon;    
float A25VMon;    
float p15VMon;    
float	p18VMon;    
float p25VMon;    
float p33VMon;    
float uCTemp;     

float ssTemp[3];  

unsigned int rpIs4V;
unsigned int rIsSC;       
unsigned int rA33VMon;    
unsigned int rA25VMon;    
unsigned int rp15VMon;    
unsigned int rp18VMon;    
unsigned int rp25VMon;    
unsigned int rp33VMon;    

};
struct user_data_type xdata user_data;

/********************************************************************\
  Application specific init and inout/output routines
\********************************************************************/
void user_init(unsigned char init);
void user_loop(void);
void user_write(unsigned char index) reentrant;
unsigned char user_read(unsigned char index);
unsigned char user_func(unsigned char *data_in, unsigned char *data_out);

#endif