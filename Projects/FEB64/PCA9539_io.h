/********************************************************************\

  Name:         PCA9539_io.h
  Created by:   Bahman Sotoodian 								Feb/11/2008


  Contents:     PCA9539 I/O register user interface

  $Id: PCA9539_io.h 58 2008-02-28 22:40:33Z fgddaq $

\********************************************************************/
//  need to have FEB64 defined

#ifndef  _PCA9539_IO_H
#define  _PCA9539_IO_H


#define PCA9539_READ            1
#define PCA9539_WRITE           0
#define ADDR_PCA9539            0x74
#define InputPort0				  0x00
#define InputPort1				  0x01
#define OutputPort0				  0x02
#define OutputPort1				  0x03
#define ConfigPort0				  0x06
#define ConfigPort1				  0x07	
#define Input						  0xFF
#define Output						  0x00
#define Bias_OUTPUT_ENABLE      ADDR_PCA9539, ConfigPort0, Output, 0, PCA9539_WRITE
#define BackPlane_INPUT_ENABLE  ADDR_PCA9539, ConfigPort1, Input,  0, PCA9539_WRITE
#define Bias_ALL_HIGH           ADDR_PCA9539, OutputPort0, 0xFF,   0, PCA9539_WRITE
#define Bias_ALL_LOW            ADDR_PCA9539, OutputPort0, 0x00,   0, PCA9539_WRITE
#define Bias_Read			        ADDR_PCA9539, InputPort0 , 0x00  	 
#define BackPlane_Read			  ADDR_PCA9539, InputPort1 , 0x00  	 

//void User_PCA9539(void);
void PCA9539_Init(void);
void PCA9539_Cmd(unsigned char addr, unsigned char cmd, unsigned char datByte,unsigned char *varToBeWritten, bit flag);

#endif