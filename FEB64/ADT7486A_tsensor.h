/**********************************************************************************\
  Name:         ADT7486A_tsensor.h
  Created by:   Brian Lee						     May/11/2007
  Modified by:  Bahman Sotoodian					  March/03/2008


  Contents:     Temperature sensor array (ADT7486A) handler for feb64 board

  $Id: ADT7486A_tsensor.h 32 2008-02-12 01:11:22Z midas $
\**********************************************************************************/

#ifndef  _ADT7486A_TSENSOR_H
#define  _ADT7486A_TSENSOR_H

// --------------------------------------------------------
// ADT7486A related definitions and function prototypes
// --------------------------------------------------------

// Command Map (ADT7486A) - Including WL and RL
// Format is as follows:
// (1)CommandName (2)writeLenngth, (3)readLength, (4)command, (5)data(if applicable, defined by user), 

#define Ping          0x00, 0x00, 0x00, 0x00, 0x00
#define GetIntTemp    0x01, 0x02, 0x00, 0x00, 0x00
#define GetExt1Temp   0x01, 0x02, 0x01, 0x00, 0x00
#define GetExt2Temp   0x01, 0x02, 0x02, 0x00, 0x00
#define GetAllTemps   0x01, 0x06, 0x00, 0x00, 0x00
#define SetExt1Offset 0x03, 0x00, 0xE0
#define GetExt1Offset 0x01, 0x02, 0xE0, 0x00, 0x00
#define SetExt2Offset 0x03, 0x00, 0xE1
#define GetExt2Offset 0x01, 0x02, 0xE1, 0x00, 0x00
#define ResetDevice   0x01, 0x00, 0xF6, 0x00, 0x00
#define GetDIB_8bytes 0x01, 0x10, 0xF7, 0x00, 0x00

#define DONT_READ_DATA  0xff			//When we have a Ping or SetOffset command, we do not read any data
#define READ_DATA     	0x00			//When we have a GetTemp or GetOffset command, we want to read the data 
#define AVG_COUNT   	   2 			   //The number of times that we want to acquire temperature
#define ADT7486A_CONVERSION_TIME 1  //Delay in us
												

#define SET_OFFSET_WL 	0x03 			//The WriteLength of SetOffset command
#define PING_WL			0x00			//The WriteLength of Ping command
#define GET_CMD_WL		0x01			//The WriteLength of all Get commands
#define GET_CMD_RL		0x02			//The ReadLength  of all Get commands
#define RESET_CMD			0xF6 			//The command part of ResetDevice 

// Defining the type of error that we have encounted during the 
// temperature reading  
#define FCS_WERROR					-1			//FCS mismatch after writing to SST device
#define FCS_RERROR					-2			//FCS mismatch after reading the temperature 
#define ClIENT_ABORT					-3			//When client wants to abort the communication
#define PING_OFFSET_RESET_CMD  	 1			//When we have a Ping or Offset command and we have not read the temperature
#define INVALID_TEMP					-5			//When the temperature reading is invalid,
#define OUT_OF_RANGE	   			-6			//When the temperature reading is out of defined range


#define ADT7486A_NUM 4					//Number of ADT7486A devies on feb64 board				  
#define N_TEMP_TRIALS	20				//Maximum number of times that we want to try reading good temperature 
												//before telling the user that temperature reading had failed
#define CONVERSION_FACTOR1 0.9294	//The first conversion factor for correcting temp readings
#define CONVERSION_FACTOR2 0.8569   //The second conversion factor for correcting temp readings 

void ADT7486A_Init(void);
signed char ADT7486A_Cmd(unsigned char addr, unsigned char writeLength
                       , unsigned char readLength, unsigned char command
							  , unsigned char datMSB, unsigned char datLSB,float *varToBeSent);
signed char ADT7486A_Read(unsigned char writeFCS_Originator, unsigned char cmdFlag,float *TempReceived);

#endif
