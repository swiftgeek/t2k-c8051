/**********************************************************************************\
  Name:         temp36_ADT7486A_tsensor.h
  Created by:   Brian Lee						     May/11/2007
  Modified by:  Bahman Sotoodian					  March/03/2008
  Modified by:  Noel Wu                        May/12/2008

  Contents:     Temperature sensor array (ADT7486A) handler for feb64 board

  $Id$
\**********************************************************************************/

#ifndef  _ADT7486A_TSENSOR_H
#define  _ADT7486A_TSENSOR_H

// --------------------------------------------------------
// ADT7486A related definitions and function prototypes
// --------------------------------------------------------

// Command Map (ADT7486A) - Including WL and RL
// Format is as follows:
// (1) CommandName 
// (2) writeLength
// (3) readLength
// (4) command
// (5) data (if applicable, defined by user), 
//                    cmd   wLen  rLen  MSB  LSB
#define Ping          0x00, 0x00, 0x00, 0x00, 0x00
#define GetIntTemp    0x00, 0x01, 0x02, 0x00, 0x00
#define GetExt1Temp   0x01, 0x01, 0x02, 0x00, 0x00
#define GetExt2Temp   0x02, 0x01, 0x02, 0x00, 0x00
#define GetAllTemps   0x00, 0x01, 0x06
#define SetExt1Offset 0xE0, 0x03, 0x00, 0x00, 0x00
#define GetExt1Offset 0xE0, 0x01, 0x02, 0x00, 0x00
#define SetExt2Offset 0xE1, 0x03, 0x00, 0x00, 0x00
#define GetExt2Offset 0xE1, 0x01, 0x02, 0x00, 0x00
#define ResetDevice   0xF6, 0x01, 0x00

#define GetDIB_8bytes 0xF7, 0x01, 0x10, 0x00, 0x00
#define DONT_READ_DATA  (unsigned char) 0xff			// For Ping, SetOffset command, we do not read any data
#define READ_DATA     	(unsigned char) 0x00			// For GetTemp, GetmsOffset command, we want to read the data 
#define AVG_COUNT   	  1 			// The number of times that we want to acquire temperature
#define ADT7486A_CONVERSION_TIME  30      // Delay in ms
												

#define SET_OFFSET_WL 	(unsigned char) 0x03 			// The WriteLength of SetOffset command
#define PING_WL			0x00			// The WriteLength of Ping command
#define GET_CMD_WL		0x01			// The WriteLength of all Get commands
#define GET_CMD_RL		0x02			// The ReadLength  of all Get commands
#define RESET_CMD		0xF6 			// The command part of ResetDevice 

// Defining the type of error that we have encounted during the 
// temperature reading  
#define SUCCESS                      0
#define FCS_WERROR					-1			//FCS mismatch after writing to SST device
#define FCS_RERROR					-2			//FCS mismatch after reading the temperature 
#define CLIENT_ABORT				-3			//When client wants to abort the communication
#define PING_OFFSET_RESET_CMD  	     1			//When we have a Ping or Offset command and we have not read the temperature
#define INVALID_TEMP				-5			//When the temperature reading is invalid,
#define OUT_OF_RANGE	   			-6			//When the temperature reading is out of defined range


#define N_TEMP_TRIALS	20			// Max try loop before escaping with status != SUCCESS 
#define CONVERSION_FACTOR1 0.9294f 	// Should be 1.0 (-PAA- to be verified)
#define CONVERSION_FACTOR2 0.8569f  // Only offset is required (in principle)

void ADT7486A_Init(int SST_LINE);
signed char ADT7486A_Cmd(unsigned char addr, unsigned char command
                       , unsigned char writeLength, unsigned char readLength 
					   , unsigned char datMSB, unsigned char datLSB
					   , float *fdata, int SST_LINE);
signed char ADT7486A_Read(unsigned char writeFCS_Originator
                        , unsigned char cmdflag
                        , float *temperature,int SST_LINE);

#endif // _ADT7486A_TSENSOR_H