/**********************************************************************************\
  Name:         LTC2605CGN.h
  Created by:   Brian Lee						     May/11/2007
  Modified by:  Bahman Sotoodian					  March/03/2008


  Contents:     Temperature sensor array (ADT7486A) handler for feb64 board

  $Id: ADT7486A_tsensor.h 45 2008-03-12 07:19:53Z midas $
\**********************************************************************************/

#ifndef _LTC2605CGN_H_
#define _LTC2605CGN_H_

//Defining eacg DAQ address
#define DAQ_A	 0x00
#define DAQ_B	 0x01
#define DAQ_C	 0x02
#define DAQ_D   0x03
#define DAQ_E	 0x04
#define DAQ_F	 0x05
#define DAQ_G   0x06
#define DAQ_H 	 0x07
#define ALL_DAQ 0x0f


//Defining the commands
#define WREG_n					0x00
#define UPDATE_REG_n 		0x01
#define WREG_n_UPDATE_All	0x02
#define WREG_n_UPDATE_n		0x03
#define POWER_DOWN_n			0x40
#define NO_OPERATION			0x11

void LTC2605_Init(void);
void LTC2605_WriteByte(unsigned char addr, unsigned char selectPort, unsigned int dataByte);

#endif