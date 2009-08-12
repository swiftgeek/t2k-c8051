     /**********************************************************************************\
  Name:			  Humidity_sensor.c
  Author:   	Bahman Sotoodian 	
  Created: 		March 28, 2008
  Description:	Humidity and Temperature measurements

  $Id$
\**********************************************************************************/


//---------------------------------------------------------------------------------
//Include files
//---------------------------------------------------------------------------------
#include "../mscbemb.h"
#include "../Protocols/SHT7x_protocol.h"
#include "Humidity_sensor.h"

//Initializing the SHT7x protocol communication 
void HumiSensor_Init(int humsen){
	
	SHT7x_Init(humsen);
}


//----------------------------------------------------------------------------------
// 1. connection reset
// 2. measure humidity(12 bit) and temperature(14 bit)
// 3. calculate humidity [%RH] and temperature [C]
// 4. calculate dew point [C]
//----------------------------------------------------------------------------------------
unsigned char HumidSensor_Cmd (unsigned int *rhumidity,unsigned int *rtemperature, 
float *humidity, float *temperature,unsigned char *OrigFCS, unsigned char *DeviceFCS, int humsen){
	
	float         xdata temp_humi, temp_tempe;
	unsigned char xdata TempStatus,HumiStatus;
	unsigned int  xdata rtemp,rhumi;
	unsigned char xdata temp_FCSOrig,temp_FCSDevice;
	
	TempStatus = SHT7x_Measure(&rtemp, TEMP, &temp_FCSOrig, &temp_FCSDevice, humsen); //measure temperature
	HumiStatus = SHT7x_Measure(&rhumi, HUMI, &temp_FCSOrig, &temp_FCSDevice, humsen); //measure humidity
	
	*DeviceFCS = temp_FCSDevice;
	*OrigFCS = temp_FCSOrig;
	*rtemperature = rtemp;
	*rhumidity = rhumi;

  temp_humi  = (float) rhumi;	
  temp_tempe = (float) rtemp;
  SHT7x_Correction(&temp_humi,&temp_tempe); 			//calculate humidity, temperature
  *humidity = temp_humi;
  *temperature = temp_tempe;								

	if((TempStatus == ERROR) || (HumiStatus == ERROR)){ 
		SHT7x_ConnectionReset(humsen); //in case of an error: connection reset
		// delay_ms(800);
		return ERROR;
	} 

	//wait approx. 0.8s to avoid heating up SHTxx
  //delay_ms(800);
	return DONE;
}

//----------------------------------------------------------------------------------------
// Makes a measurement (Humidity/Temperature) with checksum
//----------------------------------------------------------------------------------------
signed char SHT7x_Measure(unsigned int *DataToSend
                        , unsigned char flag
                        , unsigned char *FCSoriginator
                        , unsigned char *FCSclient
                        , int humsen)
{
	unsigned char xdata check_flag, status = DONE;
	unsigned char xdata FCSdevice, FCSorig, MSBdata, LSBdata;
	unsigned int  xdata measurements=0;
	unsigned long xdata SHT_time =0, temp_check=0;

	
	SHT7x_TransStart(humsen);						 //transmission start
	
	switch(flag) { 								 //send command to sensor
		case TEMP: 
			status = SHT7x_WriteByte(MEASURE_TEMP, humsen); 
			check_flag = MEASURE_TEMP;
			break;
		
		case HUMI : 
			status = SHT7x_WriteByte(MEASURE_HUMI, humsen); //if device does not ack, returns error
			check_flag = MEASURE_HUMI;
			break;
		
		default : 
			break;
	}
	
	SHT_time = uptime();

	do {
		if(humsen==1) {
			if(SHT7x_DATA1 == 0) {
				temp_check = 1; 
				break; //wait until sensor has finished the measurement
		 	}
		}
#ifdef MORETHANONEHUM
		else if(humsen==2) {
			if(SHT7x_DATA2 == 0) {
				temp_check = 1; 
				break; //wait until sensor has finished the measurement
		 	}			
		}
#endif
	}while ((uptime() - SHT_time) < SHT_TIME);
	

	if(temp_check == 0){ 
		SHT7x_ConnectionReset(humsen);
		return ERROR; 			  // timeout is reached
	}

	//14bit temperature measurements
	//12bit humidity measurements
	MSBdata  	= SHT7x_ReadByte(ACK, humsen);		  //read the first byte (MSB)
	LSBdata  	= SHT7x_ReadByte(ACK, humsen);      //read the second byte (LSB)
	FCSdevice = SHT7x_ReadByte(ACK, humsen);      //read the last byte (checksum)
	FCSdevice = ReverseByte (FCSdevice);
	
	//Calculate originator's side write FCS
  FCSorig = SHT7x_FCS_Step(LSBdata
          , SHT7x_FCS_Step(MSBdata
          , SHT7x_FCS_Step(check_flag
          , 0x00)));	

	*FCSclient = FCSdevice;
	*FCSoriginator = FCSorig;

	// Checking the checksum
	if (FCSdevice != FCSorig) {
		SHT7x_ConnectionReset(humsen);
		return ERROR;
  }

	measurements = (MSBdata	<<8) | (LSBdata);

	if(flag == TEMP)
		measurements &= 0x3fff;
	else
		measurements &= 0x0fff;
	
	*DataToSend =  measurements;
	return status;     
}

//-------------------------------------------------------------
//
//
void SHT7x_Correction(float *p_humidity ,float *p_temperature)
{ 
	// Assuming VDD to be 3.5V but the actual voltage is 3.3V
	*p_temperature = (*p_temperature * D2) + D1 ; 	
	*p_humidity = (*p_humidity) * (*p_humidity * C3) + *p_humidity * C2 + C1; 	
	
	*p_humidity=(*p_temperature - 25)*(T1+ *p_humidity * T2)+ *p_humidity; //calc. Temperature compensated humidity [%RH]

	if(*p_humidity > 100)
		*p_humidity = 100; 		    // Cut if the value is outside of
	
	if(*p_humidity <0.1)
		*p_humidity = 0.1;        // the physical possible range

}


/*
//-------------------------------------------------------------
//
//
unsigned char SHT7x_StatusRead(unsigned char *StatusRead, int humsen)
{
	unsigned char status=1;

	SHT7x_TransStart(humsen);
	status = SHT7x_WriteByte(STATUS_REG_R, humsen);
  delay_ms(20);
	*StatusRead = SHT7x_ReadByte(NACK, humsen);
	return status;

}


//-------------------------------------------------------------
//
//
unsigned char SHT7x_StatusWrite(unsigned char StatusWrite, int humsen){
	unsigned char status;
	
	SHT7x_TransStart(humsen);
	status = SHT7x_WriteByte(STATUS_REG_W, humsen);
	status = SHT7x_WriteByte(StatusWrite, humsen);
	
	return status;
}
*/	
//-------------------------------------------------------------
//
//
unsigned char ReverseByte (unsigned char dataBeReversed){

	unsigned char i,j,temp = 0;
	
	for ( i=0x80,j=7;i>=0x10;i/=2,j-=2 )
			temp |= ((dataBeReversed & i) >> j);		
	
	for ( i=0x08,j=1;i>=0x01;i/=2,j+=2 )
			temp |= ((dataBeReversed & i) << j);
	return temp;
}

 





