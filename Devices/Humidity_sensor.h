/**********************************************************************************\
  Name:			   Humidity_Sensor.h
  Author:   	 Bahman Sotoodian 	
  Created: 		 March 28, 2008
  Description: Humidity and Temperature measurements

  $Id$
\**********************************************************************************/



//Defining the SHT7x instructions 		//adr  command  r/w											
#define STATUS_REG_W 0x06 					  //000	  0011 	 0
#define STATUS_REG_R 0x07 					  //000   0011 	 1
#define MEASURE_TEMP 0x03 					  //000   0001 	 1
#define MEASURE_HUMI 0x05 					  //000   0010 	 1
#define RESET 0x1e 						        //000   1111 	 0

#define TEMP 1
#define HUMI 2
#define ERROR 0
#define DONE  1
#define SHT_TIME  2
//#define SHT_TIME 0x3AF5C //in us

#define C1 -4.0 			// for 12 Bit humidity readings
#define C2 0.0405 		// for 12 Bit humidity readings
#define C3 -0.0000028 // for 12 Bit humidity readings
#define T1 0.01 			// compensation for humidity at high temp
#define T2 0.00008 		// compensation for humidity at high temp
#define D1 -39.66			// for 14 Bit @ 3.5V temp readings
#define D2 0.01			  // for 14 Bit @ 3.5V temp readings


void HumiSensor_Init(int humsen);
signed char SHT7x_Measure(unsigned int *DataToSend,
unsigned char flag, unsigned char *FCSoriginator, unsigned char *FCSclient, int humsen);
unsigned char HumidSensor_Cmd (unsigned int *rhumidity,unsigned int *rtemperature, 
float *humidity, float *temperature,unsigned char *OrigFCS, unsigned char *DeviceFCS, int humsen);
void SHT7x_Correction (float *p_humidity ,float *p_temperature);
unsigned char ReverseStatus (unsigned char value);
unsigned char ReverseByte (unsigned char dataBeReversed);
unsigned char SHT7x_StatusRead(unsigned char *StatusRead, int humsen);
unsigned char SHT7x_StatusWrite(unsigned char StatusWrite, int humsen);



