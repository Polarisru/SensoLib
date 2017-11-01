/*----------------------------------------------------------------------------
 *      Erweka - UniMain
 *----------------------------------------------------------------------------
 *      Name:    tmp102.c
 *      Purpose: TMP102 temperature sensor module
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 Erweka - All rights reserved.
 *---------------------------------------------------------------------------*/

/**
 @file   tmp102.c
 @brief  TMP102 temperature sensor module
 @date  14.10.2015
 @addtogroup Hardware
 @{
 @defgroup TMP102 TMP102
 @brief TMP102 sensor driver
 @{
*/

#include "i2c.h"
#include "tmp102.h"

_TMP102_SENSOR TMP102_Sensor;

//------------------------------------------------------------------------------
/// @brief      Calculate temperature from raw data
/// @param [in] value raw value from sensor
/// @return     Temperature in degrees
//------------------------------------------------------------------------------
double TMP102_CalcTemp(uint16_t value)
{
	double temp;
	
  if ((value & 0x0800)==0x00)
	{	
    temp = value*0.0625;
	} else 
	{
		value ^= 0x0FFF;
		value &= 0x07FF;
		temp = value*0.0625;
		temp *= -1.00;    
	}

	return temp;
}

//------------------------------------------------------------------------------
/// @brief  Main temperature request task (for RTOS)
//------------------------------------------------------------------------------
void TMP102_Task(void)
{	
	static int err;
	uint8_t buf[20];
	uint16_t rawtemp;
	static double tmp = 0;
  static int counter = 0;
  static int errors = 0;
	
  err = I2C_ReadData(TMP102_Sensor.Port, TMP102_I2C_ADDRESS, TMP102_CMD_READ, (char*)buf, 2);

  //err = TMP102_Read(buf);
  if (err==I2C_ERR_OK) 
  {
    errors = 0;
    rawtemp = buf[0] << 8;
    rawtemp |= buf[1];
    rawtemp = rawtemp >> 4;
    tmp += TMP102_CalcTemp(rawtemp);
    TMP102_Sensor.Error = 0;
    counter++;
    if (counter>=TMP102_COUNTER)
    {
      TMP102_Sensor.Temperature = tmp/TMP102_COUNTER;
      tmp = 0;
      counter = 0;
    }  
  } else 
  {
    errors++;
    if (errors>=TMP102_ERRORS_NUMBER)
    {
      TMP102_Sensor.Temperature = -99.9; 
      TMP102_Sensor.Error = 1;
    }
  }
}

//------------------------------------------------------------------------------
/// @brief        Get current temperature from sensor
/// @param [out]  temp 
/// @return       Error code
//------------------------------------------------------------------------------
int TMP102_GetTemp(float *temp)
{
  if (TMP102_Sensor.Error)
    return 1;
  
  *temp = TMP102_Sensor.Temperature;
  
  return 0;
}

//------------------------------------------------------------------------------
/// @brief  TMP102 sensor initialization
//------------------------------------------------------------------------------
void TMP102_Init(int Port)
{
  if (Port>=I2C_NUM_LAST)
    return;
  
  TMP102_Sensor.Temperature = 0;
  TMP102_Sensor.Error = 0;
  TMP102_Sensor.Port = Port;
}

/// @}
/// @}
