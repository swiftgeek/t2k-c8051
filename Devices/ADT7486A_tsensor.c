/**********************************************************************************\
  Name:         ADT7486A_tsensor.c
  Created by:   Brian Lee						     May/11/2007
  Modified By:  Bahman Sotoodian					  March/03/2008

  Contents:     Temperature sensor array (ADT7486A) handler

  Version:		Rev 1.2

  $Id$
\**********************************************************************************/

// --------------------------------------------------------
//  Include files
// --------------------------------------------------------
#include "../mscbemb.h"
#include "../Protocols/SST_handler.h"
#include "ADT7486A_tsensor.h"

void ADT7486A_Init(void)
/**********************************************************************************\
  Routine: ADT7486A_Init
  Purpose: ADT7486A temperature sensor array initialization
  Input:void
  Function value:void
\**********************************************************************************/
{	
	SST_Init();
}

signed char ADT7486A_Cmd(unsigned char addr, unsigned char writeLength, unsigned char readLength, 
				unsigned char command, unsigned char datMSB, unsigned char datLSB, float *varToBeSent)
/**********************************************************************************\

  Routine: ADT7486A_Cmd 

  Purpose: To send commands to the clients

  Input:
   unsigned char addr			Address of the client which Originator wants to talk to	
	unsigned char writeLength	Length of the data Originator wants to send	
	unsigned char readLength	Length of the data Originator wants to receive
	unsigned char command		Command (see ADT7486A_tsensor.h file or ADT7486A manual)
	unsigned char datMSB	 	   Most Significant byte of the temperature offset value
								      (for SetExt1OffSet() and SetExt2OffSet() commands)
	unsigned char datLSB		   Least Significant byte of the temperature offset value
								      (for SetExt1OffSet() and SetExt2OffSet() commands)

  Function value:					Signed char. It returns 0 when we have a valid temperature reading
    

\**********************************************************************************/
{
	//Declare the local variables 
	unsigned char xdata TempCounter = 0;	  //To keep track of number of times, we have gone through the loop
	float xdata tempDataBuffer = 0x00;		  //Temporary store the valid and invalid temperature readings	
	unsigned char xdata writeFCS_Org = 0x00; //Originator's side write FCS	
   signed char xdata k = 0;					  //"k" represents number of times that we want to read a valid temperature 	
	float xdata dataBuffer = 0.0;				  //We store the valid temperature readings in dataBuffer
	extern unsigned char xdata FCS_Mismatch; //Global variable which indicate number of times that we have FCS mismatch 
	signed char Read_Status = 0;				  //The Read_Status will reflect the status of read operation

	//Calculate originator's side write FCS
	writeFCS_Org = FCS_Step(command, FCS_Step(readLength, FCS_Step(writeLength, FCS_Step(addr, 0x00))));	
	
	//If the command is setOffSet commands, then there are 2 more bytes of data to be added on
	if(writeLength == SET_OFFSET_WL)
	{
		writeFCS_Org = FCS_Step(datMSB, FCS_Step(datLSB, writeFCS_Org));
	}
	
   //Run averaging starting
	//If we do not obtain appropriate temperature reading after N_TEMP_TRIALS attempts
	//We have to terminate the loop.
	while((k != AVG_COUNT) && (TempCounter <N_TEMP_TRIALS))
	{
		TempCounter++;
		
		//Start the message
		SST_DrvLow();						//Sending two zero (the address timing negotiation bits)
		SST_DrvLow();
		SST_WriteByte(addr); 			//Target address
		SST_DrvLow();						//Send the message timing negotiation bit
		SST_WriteByte(writeLength);   //WriteLength
		SST_WriteByte(readLength);    //ReadLength
		if(writeLength != PING_WL)    //When we do not have the Ping command
		{
			SST_WriteByte(command); 	//Optional : sending the commands
		}
		else 									//When we have the Ping command
		{			
			return ADT7486A_Read(writeFCS_Org, DONT_READ_DATA,& tempDataBuffer);
			
		}
		if(writeLength == SET_OFFSET_WL) //If we have the SetOffset command
		{
			//Send the data in little endian format, LSB before MSB
			SST_WriteByte(datLSB);
			SST_WriteByte(datMSB);
			return ADT7486A_Read(writeFCS_Org, DONT_READ_DATA, & tempDataBuffer);
			
		}
		
		//If the received ReadLength and WriteLength matched with the Get commands, we do the readings
		if((writeLength == GET_CMD_WL) && (readLength == GET_CMD_RL)) 
		{
			if(k != AVG_COUNT)
			{
				//Get the temperature and the status of read operation
				Read_Status= ADT7486A_Read(writeFCS_Org, READ_DATA, &tempDataBuffer);
				if (!Read_Status) 	//Read_Status would return 0, when we have a valid temperature
				{				
					dataBuffer += tempDataBuffer;
					k++;
				}
				else
				{
					FCS_Mismatch++;			//Number of times that we have mismatch
            }
			}
			if(k == AVG_COUNT)
			{		
					*varToBeSent = dataBuffer/k;				
			}		
		}
		else if(command == RESET_CMD)	   //if reset command
		{
			return ADT7486A_Read(writeFCS_Org, DONT_READ_DATA, &tempDataBuffer);
		}

		//Clear for the next msg and delay for conversion to finish
		SST_Clear();
      delay_us(ADT7486A_CONVERSION_TIME);
	}
	
	//If we do not have any valid temperature, we would return the Read_Status
	if (TempCounter == N_TEMP_TRIALS)
		{
	      return Read_Status;
      }

	//Reset averaging buffer variable
	dataBuffer = 0.0;
	
	//Reset the counting variable used for averaging and break the loop
	k = 0;
	
	return 0;
}

signed char ADT7486A_Read(unsigned char writeFCS_Originator, unsigned char cmdFlag,float *TempReceived)
/**********************************************************************************\

  Routine: ADT7486A_Read

  Purpose:  Read only needs to be working for reading 2 bytes of info
		   	Which are Internal Temp, External Temp1,External Temp2, GetExt1Offset and GetExt2Offset
		   	Others should be verified on oscilloscope since mscb returns a data at maximum
		   	4bytes, (allTemp and GetDIB commands take 6 bytes and 16 bytes, which is impossible anyway)

  Input:
   unsigned char writeFCS_Originator	Originator's writeFCS 
	unsigned char cmdFlag				   Flag used to distinguish if the command is a Ping/Reset
													or not.
	float *TempReceived 					   The variable that we want to store the avraged temperatures reading

  Function value:
    signed char								The ReadStatus which would indicate whether the temperature 
											   	is valid or not

\**********************************************************************************/
{	
	//Declare local variables
	unsigned char xdata LSB_Received = 0x00; 		//Max 2bytes of info from 5 different possible commands
	unsigned char xdata MSB_Received = 0x00; 		
	unsigned char xdata writeFCS_Client = 0x00;	//The client write FCS
	unsigned char xdata readFCS_Client = 0x00;   //The client read FCS	
	float xdata convertedTemp = 0.0;

	writeFCS_Client = SST_ReadByte(); // Get Write FCS from Client when we have finished 
												 // the write operation

	
	//WriteFCS check
	if(writeFCS_Originator != writeFCS_Client)
	{
		return FCS_WERROR;  //If the FCS Writes don't match up, then return FCS_wError
	}

	// Client is requesting a message abort
	if(writeFCS_Originator == ~ writeFCS_Client)
	{
		return ClIENT_ABORT; 
	}
	
	//If it's not the Ping Command SetOffset or Reset command
	if(cmdFlag == READ_DATA) 
	{
		//Take the data in to a variable called datareceived
		LSB_Received 	= SST_ReadByte();   //get LSB
		MSB_Received 	= SST_ReadByte();   //get MSB
		readFCS_Client = SST_ReadByte();   //get Read FCS from Client

      //************************************************************************************
		//Data is thoroughly received, now analyze the received data
		//ReadFCS check
		//************************************************************************************	
	
		if(FCS_Step(MSB_Received, FCS_Step(LSB_Received, 0x00)) != readFCS_Client) 
		{
			return FCS_RERROR; //If the read FCS's don't match up, then return FCS_rError
		}		
	
		//************************************************************************************
		//Temperature Conversion:
		//Convert the Temperature according to the structure of bits defined by
		//ADT7486A manual
		//Convert 2bytes of temperature from 
		//GetExt1OffSet/GetExt2OffSet/GetIntTemp()/GetExt1Temp()/GetExt2Temp() commands
		//Mask out the first bit (sign bit)
		//and if the sign bit is 1, then do two's complement
		//************************************************************************************
		
		if((MSB_Received & 0x80) == 0x80) 
		{
			MSB_Received = ~MSB_Received;
			LSB_Received = ~LSB_Received + 0x01;
		}
		
		//Calculate the converted Temperature
		convertedTemp = ((float) ((MSB_Received << 8) *0.015625)) 
		              + ((float) (LSB_Received *0.015625));
    	convertedTemp *= CONVERSION_FACTOR1;
    	
		convertedTemp -= CONVERSION_FACTOR2;
    	
		//To check whether the temperature is in range or not
		if (convertedTemp > 100. || convertedTemp < -10.) 
			return OUT_OF_RANGE; 	
	   else if (convertedTemp == -CONVERSION_FACTOR2) 
			return INVALID_TEMP;
		}

	else //If it is the Ping,SetOffset or Reset command
		{
			return PING_OFFSET_RESET_CMD;
		}
	//Return the converted temperature value
	*TempReceived = convertedTemp; 	
	//Indicate a valid temperature
	return 0;
}
