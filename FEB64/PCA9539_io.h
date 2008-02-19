/********************************************************************\

  Name:         PCA9539_io.h
  Created by:   Bahman Sotoodian 								Feb/11/2008


  Contents:     PCA9539 I/O register user interface

  $Id:$

\********************************************************************/
//  need to have FEB64 defined

#ifndef  _PCA9539_IO_H
#define  _PCA9539_IO_H


#define PCA9539_READ 1
#define PCA9539_WRITE 0
#define ADDR_PCA9539 0x74
#define Bias_ALL_HIGH ADDR_PCA9539, 0x02, 0xFF, PCA9539_WRITE
#define Bias_ALL_LOW ADDR_PCA9539, 0x02, 0x00, PCA9539_WRITE
#define BackPlane_ALL_HIGH ADDR_PCA9539, 0x03, 0xFF, PCA9539_WRITE
#define BackPlane_ALL_LOW ADDR_PCA9539, 0x03, 0x00, PCA9539_WRITE
#define Bias_OUTPUT_ENABLE ADDR_PCA9539, 0x06, 0x00, PCA9539_WRITE
#define BackPlane_OUTPUT_ENABLE ADDR_PCA9539, 0x07, 0x00, PCA9539_WRITE

//void User_PCA9539(void);
void PCA9539_Init(void);
void PCA9539_Cmd(unsigned char addr, unsigned char cmd, unsigned char datByte, bit flag);

#endif