/**********************************************************************************\
  Name:			SHT7x_protocol.c
  Author:		MST
  Modified by: Bahman Sotoodian 	
  Created: 		March 28, 2008
  Description:	Humidity and Temperature measurements
  Copyrigth:   (c) Sensirion AG	
  $Id$
\**********************************************************************************/


//-----------------------------------------------------------------------------------
//Include files
//-----------------------------------------------------------------------------------
#include "../mscbemb.h"
#include "SHT7x_protocol.h"


/* CRC-8 Table for Polynomial x^8 + x^5 + x^4 (used for FCS in SHT7x Protocol) */
unsigned char code SHT7x_FCS_data[] = {
	  0,  49,  98,  83, 196, 245, 166, 151, 
	185, 136, 219, 234, 125,  76,  31,  46, 
	 67, 114,  33,  16, 135, 182, 229, 212, 
	250, 203, 152, 169,  62,  15,  92, 109, 
	134, 183, 228, 213,  66, 115,  32,  17, 
	 63,  14,  93, 108, 251, 202, 153, 168, 
	197, 244, 167, 150,   1,  48,  99,  82, 
	124,  77,  30,  47, 184, 137, 218, 235, 
	 61,  12,  95, 110, 249, 200, 155, 170, 
	132, 181, 230, 215,  64, 113,  34,  19, 
	126,  79,  28,  45, 186, 139, 216, 233, 
	199, 246, 165, 148,   3,  50,  97,  80, 
	187, 138, 217, 232, 127,  78,  29,  44, 
	  2,  51,  96,  81, 198, 247, 164, 149, 
	248, 201, 154, 171,  60,  13,  94, 111, 
	 65, 112,  35,  18, 133, 180, 231, 214, 
	122,  75,  24,  41, 190, 143, 220, 237, 
	195, 242, 161, 144,   7,  54, 101,  84, 
	 57,   8,  91, 106, 253, 204, 159, 174, 
	128, 177, 226, 211,  68, 117,  38,  23, 
	252, 205, 158, 175,  56,   9,  90, 107, 
	 69, 116,  39,  22, 129, 176, 227, 210, 
	191, 142, 221, 236, 123,  74,  25,  40, 
	  6,  55, 100,  85, 194, 243, 160, 145, 
	 71, 118,  37,  20, 131, 178, 225, 208, 
	254, 207, 156, 173,  58,  11,  88, 105, 
	  4,  53, 102,  87, 192, 241, 162, 147, 
	189, 140, 223, 238, 121,  72,  27,  42, 
	193, 240, 163, 146,   5,  52, 103,  86, 
	120,  73,  26,  43, 188, 141, 222, 239, 
	130, 179, 224, 209,  70, 119,  36,  21, 
	 59,  10,  89, 104, 255, 206, 157, 172
};

//
//----------------------------------------------------------------------------------
// Single calculation for 8-bit cyclic redundancy checksum
// Reentrant function 
unsigned char SHT7x_FCS_Step(unsigned int msg, unsigned char fcs) reentrant {
   return SHT7x_FCS_data[(msg ^ fcs)];
}


//Initializing the SHT7x protocol communication
void SHT7x_Init(int humsen){
	
	SHT7x_ConnectionReset(humsen);
}

//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA: 																     |_______|
//			   _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
//---------------------------------------------------------------------------------
void SHT7x_ConnectionReset(int humsen){
	unsigned char i;
	if(humsen==1)
	{
		//Initial state
		SHT7x_DATA1=1; 
		SHT7x_SCK1=0;	  		
	}
#ifdef MORETHANONEHUM
	else if(humsen==2)
	{
		SHT7x_DATA2=1;
		SHT7x_SCK2=0;
	}
#endif
	for(i=0;i<9;i++){ 	//9 SCK cycles
		  SHT7x_ClockOnce(humsen);
		}

	SHT7x_TransStart(humsen); //transmission start
}        
 
//---------------------------------------------------------------------------------
// Generates a transmission start
// 	  _____         _______
// DATA: 	 |_______|
// 			___     ___
// SCK : __|   |___|   |______	
//---------------------------------------------------------------------------------
void SHT7x_TransStart(int humsen){
	if(humsen==1)
	{
		//Initial state
		SHT7x_DATA1=1; 
		delay_us(SHT7x_DELAY);
		SHT7x_SCK1=1;
		delay_us(SHT7x_DELAY);
		SHT7x_DATA1=0;
		delay_us(SHT7x_DELAY);
		SHT7x_SCK1=0;
		delay_us(SHT7x_DELAY);		//low pulse, but in the sample code, it used 3 _nop_();
		SHT7x_SCK1=1;
		delay_us(SHT7x_DELAY);
		SHT7x_DATA1=1;
		delay_us(SHT7x_DELAY);
		SHT7x_SCK1=0;
	}
#ifdef MORETHANONEHUM
	else if(humsen==2)
	{
		//Initial state
		SHT7x_DATA2=1; 
		delay_us(SHT7x_DELAY);
		SHT7x_SCK2=1;
		delay_us(SHT7x_DELAY);
		SHT7x_DATA2=0;
		delay_us(SHT7x_DELAY);
		SHT7x_SCK2=0;
		delay_us(SHT7x_DELAY);		//low pulse, but in the sample code, it used 3 _nop_();
		SHT7x_SCK2=1;
		delay_us(SHT7x_DELAY);
		SHT7x_DATA2=1;
		delay_us(SHT7x_DELAY);
		SHT7x_SCK2=0;
	}
#endif
}

void SHT7x_ClockOnce(int humsen){
	if(humsen==1)
	{
		delay_us(SHT7x_DELAY);
		SHT7x_SCK1 = 1;
		delay_us(SHT7x_DELAY);
		SHT7x_SCK1 = 0;
	}
#ifdef MORETHANONEHUM
	else if(humsen==2)
	{
		delay_us(SHT7x_DELAY);
		SHT7x_SCK2 = 1;
		delay_us(SHT7x_DELAY);
		SHT7x_SCK2 = 0;
	}
#endif
}

// Writes a byte on the DATA line and checks the acknowledge
unsigned char SHT7x_WriteByte(unsigned char dataToSend, int humsen){

  signed char xdata i;
  unsigned char xdata status;
  if(humsen==1)
  {  
  		for(i = 7; i >= 0; i--) {
	  		SHT7x_DATA1 = (dataToSend >> i) & 0x01;
			SHT7x_ClockOnce(humsen);	
		}

		SHT7x_DATA1 = 1;							//Release SHT7x_DATA line
		delay_us(SHT7x_DELAY);
		SHT7x_SCK1 = 1;								//The 9th clock cycle for ACK/NACK
		delay_us(SHT7x_DELAY);											   
		status = SHT7x_DATA1;						//Check ACK(SHT7x_DATA will be pulled down by SHT7x device)
		SHT7x_SCK1=0;
		delay_us(SHT7x_DELAY);
	}
#ifdef MORETHANONEHUM
	else if(humsen==2)
	{
  		for(i = 7; i >= 0; i--) {
	  		SHT7x_DATA2 = (dataToSend >> i) & 0x01;
			SHT7x_ClockOnce(humsen);	
		}
		SHT7x_DATA2 = 1;							//Release SHT7x_DATA line
		delay_us(SHT7x_DELAY);
		SHT7x_SCK2 = 1;								//The 9th clock cycle for ACK/NACK
		delay_us(SHT7x_DELAY);											   
		status = SHT7x_DATA2;						//Check ACK(SHT7x_DATA will be pulled down by SHT7x device)
		SHT7x_SCK2=0;
		delay_us(SHT7x_DELAY);
	}	
#endif
	if (status == NACK)						//1 in case of No acknowledge
		 return ERROR; 						
	return DONE;
}


// Reads a byte form the SHT7x_DATA line and gives an acknowledge in case of "ack=1"
unsigned char SHT7x_ReadByte(unsigned char read_flag, int humsen){

	signed char i;
	unsigned char dataReceived=0, din=0;
	if(humsen==1)
	{
		SHT7x_DATA1=1; //release SHT7x_DATA line
		//NW Read when the clock line is high 
		for (i = 7; i >= 0; i--) { 
	 		SHT7x_ClockOnce2(humsen);
			din = SHT7x_DATA1;
    		dataReceived |= (din << i);	
		}

		SHT7x_DATA1=read_flag; 		//Sending ACK/NACK. In case of ACK pull the SHT7x_DATA line low
		SHT7x_ClockOnce(humsen);			//The 9th clock cycle
		SHT7x_DATA1=1;		  			//Release the SHT7x_DATA line
	}
#ifdef MORETHANONEHUM
	else if(humsen==2)
	{
		SHT7x_DATA2=1; //release SHT7x_DATA line
		//NW Read when the clock line is high 
		for (i = 7; i >= 0; i--) { 
	 		SHT7x_ClockOnce2(humsen);
			din = SHT7x_DATA2;
    		dataReceived |= (din << i);	
		}
		SHT7x_DATA2=read_flag; 		//Sending ACK/NACK. In case of ACK pull the SHT7x_DATA line low
		SHT7x_ClockOnce(humsen);			//The 9th clock cycle
		SHT7x_DATA2=1;		  			//Release the SHT7x_DATA line
	}
#endif
	return dataReceived;
}
void SHT7x_ClockOnce2(int humsen){

	if(humsen==1)
	{
		delay_us(SHT7x_DELAY);
		SHT7x_SCK1 = 0;
		delay_us(SHT7x_DELAY);
		SHT7x_SCK1 = 1;
	}
#ifdef MORETHANONEHUM
	else if(humsen==2)
	{
		delay_us(SHT7x_DELAY);
		SHT7x_SCK2 = 0;
		delay_us(SHT7x_DELAY);
		SHT7x_SCK2 = 1;
	}
#endif-+
}
