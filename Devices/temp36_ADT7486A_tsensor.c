/**********************************************************************************\
Name: 				ADT7486A_tsensor.c
Created by: 	Brian Lee 							 May/11/2007
Modified By:	Bahman Sotoodian					 March/03/2008

Contents: 		Temperature sensor array (ADT7486A) handler

$Id: ADT7486A_tsensor.c 130 2008-03-19 21:27:42Z bryerton $
\**********************************************************************************/

#ifndef _SST_PROTOCOL_
#define _SST_PROTOCOL_
#endif

// --------------------------------------------------------
//	Include files
// --------------------------------------------------------
#include "../mscbemb.h"
#include "../Protocols/temp36_SST_handler.h"
#include "temp36_ADT7486A_tsensor.h"

//
//----------------------------------------------------------------------------------
/**
Initialize the SST communication protocol
*/
void ADT7486A_Init(int SST_LINE) {	
	SST_Init(SST_LINE);
}

//
//----------------------------------------------------------------------------------
/** 
Sends commands to the device. Not all the functions are currently implemented
Ths is the cmd list...
* <center> test purpose.
*  \image html temp.jpg
* </center> 

@attention	Conversion time 12,38,38ms round-robin for all 3 channels of the device.
						Internal averaging, doesn't require one at this level. If anyway desided 
			>100ms< needs to be added if polling on single channel!
@param addr 				Address of the client which Originator wants to talk to
@param command				Command (see ADT7486A_tsensor.h file or ADT7486A manual)
@param writeLength			Length of the data Originator wants to send 
@param readLength 		Length of the data Originator wants to receive
@param datMSB 			Most Significant byte of the temperature offset value
			   (for SetExt1OffSet() and SetExt2OffSet() commands)
@param datLSB 			Least Significant byte of the temperature offset value
@param *fdata 					Temperature 
@return SUCCESS, status from ADT7486A_Read()
*/	
signed char ADT7486A_Cmd(unsigned char addr, unsigned char command
											 , unsigned char writeLength, unsigned char readLength 
											 , unsigned char datMSB, unsigned char datLSB
											 , float *fdata, int SST_LINE) {
	signed char k, status, ntry;							
	unsigned char xdata writeFCS_Org; 			 // Originator's side write FCS 
	float xdata tempsum = 0.0f; 			 // Read sum for average	

	//Calculate originator's side write FCS
	writeFCS_Org = FCS_Step(command
		, FCS_Step(readLength
		, FCS_Step(writeLength
		, FCS_Step(addr
		, 0x00)))); 

	// setOffSet commands, add the 2 bytes of data
	if(writeLength == SET_OFFSET_WL) {
		writeFCS_Org = FCS_Step(writeFCS_Org
			, FCS_Step(datMSB
			, FCS_Step(datLSB
			, 0x00)));
	}

	k 	 = AVG_COUNT;
	ntry = N_TEMP_TRIALS; 
 
	//Run averaging and max tries
	while(k && ntry) {

		//Start the message
		SST_DrvLow(SST_LINE); 						 // Sending two zero (the address timing negotiation bits)
		SST_DrvLow(SST_LINE);
		SST_WriteByte(addr,SST_LINE);		 // Target address
		SST_DrvLow(SST_LINE); 			 // Send the message timing negotiation bit
		SST_WriteByte(writeLength,SST_LINE);  // WriteLength
		SST_WriteByte(readLength,SST_LINE);	 // ReadLength
		if(writeLength == PING_WL) {
			// PING
			return ADT7486A_Read(writeFCS_Org, DONT_READ_DATA, fdata, SST_LINE);
		}

		// Send CMD
		SST_WriteByte(command,SST_LINE); 	//Optional : sending the commands

		// RESET
		if(command == RESET_CMD) {
			return ADT7486A_Read(writeFCS_Org, DONT_READ_DATA, fdata,SST_LINE);
		}
		// SET
		if(writeLength == SET_OFFSET_WL) {
			//Send the data in little endian format
			SST_WriteByte(datLSB,SST_LINE);
			SST_WriteByte(datMSB,SST_LINE);
			return ADT7486A_Read(writeFCS_Org, DONT_READ_DATA, fdata,SST_LINE);			
		}
		// GET 
		if((writeLength == GET_CMD_WL) && (readLength == GET_CMD_RL)) {
			if(k) {
				status = ADT7486A_Read(writeFCS_Org, READ_DATA, fdata,SST_LINE);
				if (status == SUCCESS) {
					tempsum += *fdata;
					k--;
				}
			} 
		if (k == 0) { 	
				*fdata = tempsum / AVG_COUNT;
		return SUCCESS;
			} 	
		}

		//Clear for the next msg and delay for conversion to finish
		if (AVG_COUNT > 1) {
		SST_Clear(SST_LINE);
			delay_ms(ADT7486A_CONVERSION_TIME);
	}
	ntry--;
	} // out of while (ntry done with error) 

	return status;	
}

//
//----------------------------------------------------------------------------------
/** 
Reads 2 bytes of info which are Internal Temp, External Temp1,External Temp2,
GetExt1Offset and GetExt2Offset.
@param writeFCS_Originator	Originator's writeFCS 
@param cmdFlag				Flag used to distinguish if the command is a Ping/Reset
											or not.
@param	*temperature 									Single temperature read
@return SUCCESS, INVALID_TEMP, FCS_WERROR, CLIENT_ABORT, OUT_OF_RANGE 								 							
*/
signed char ADT7486A_Read(unsigned char writeFCS_Originator
												, unsigned char cmdFlag
						, float *temperature, int SST_LINE) {	
	//Declare local variables
	unsigned char LSB_Received; 	
	unsigned char MSB_Received; 		
	signed	short xdata itemperature;
	unsigned char writeFCS_Client = 0x00;	 // client write FCS
	unsigned char readFCS_Client	= 0x00;  // client read FCS	(rm var once debugged)

	// Get the client FCS to compare against the originator
	writeFCS_Client = SST_ReadByte(SST_LINE);

	//WriteFCS check (needs to match to continue)
	if(writeFCS_Originator != writeFCS_Client)
		return FCS_WERROR;

	// Client is requesting a message abort
	if(writeFCS_Originator == ~writeFCS_Client)
		return CLIENT_ABORT; 

	// No data to read	
	if (cmdFlag == DONT_READ_DATA) 
		return PING_OFFSET_RESET_CMD;

	// Data to read
	if(cmdFlag == READ_DATA) 
	{
		// Read data
		LSB_Received 	= SST_ReadByte(SST_LINE); 	//get LSB
		MSB_Received 	= SST_ReadByte(SST_LINE); 	//get MSB
		readFCS_Client	= SST_ReadByte(SST_LINE); 	//get FCS

		// Client FC check
		if(FCS_Step(MSB_Received, FCS_Step(LSB_Received, 0x00)) != readFCS_Client) 
			return FCS_RERROR;		 // No match 

		// Temperature Conversion +/- 512 degC on 16bits
		// if negative, then do two's complement
		if((MSB_Received & 0x80) == 0x80) {
			MSB_Received = ~MSB_Received;
			LSB_Received = ~LSB_Received + 0x01;
		}

		// In 1/64th degree Celsius
		itemperature	= (MSB_Received << 8) + LSB_Received; 	
		*temperature	= ((float) itemperature) / 64;
//		*temperature *= CONVERSION_FACTOR1; 	// -PAA- Should be ONE
		*temperature -= CONVERSION_FACTOR2;

		// Check whether temperature is in range
		if ((*temperature > 100.) || (*temperature < -10.)) 
			return OUT_OF_RANGE; 	
		else if (*temperature == -CONVERSION_FACTOR2) 
			return INVALID_TEMP;
	}

	// Valid temperature
	return SUCCESS;
}
