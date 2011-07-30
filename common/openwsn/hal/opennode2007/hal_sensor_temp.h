#ifndef _HAL_SENSOR_TEMP_
#define _HAL_SENSOR_TEMP_

#define  DQ_PIN  30    //P0.30 
#define  DQ_PORT 0

typedef struct{
	uint8 id;
}TiTempSensorAdapter;

void delay(int time);

uint8 ResetDS(void);


//读一字节子程序

unsigned char read_byte(void);
 

//Write a byte

void write_byte(unsigned char val);

void ReadTemps(unsigned char *val);

#endif