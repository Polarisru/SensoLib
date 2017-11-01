#include "i2c.h"
#include "lm75a.h"

_LM75A_SENSOR LM75A_Sensor;

//------------------------------------------------------------------------------
// LM75A_Init - initialize temperature sensor
//------------------------------------------------------------------------------
void LM75A_Init(int Port)
{
  if (Port>=I2C_NUM_LAST)
    return;
  
  LM75A_Sensor.Port = Port;
  LM75A_Sensor.Error = 0;
}

//------------------------------------------------------------------------------
// LM75A_GetTemp - get temperature from the sensor
//------------------------------------------------------------------------------
float LM75A_GetTemp(void)
{
  uint8_t tBuff[10];
  uint16_t temp;
  TBool sign = false;
  float Result;
	int err;
  static int errors = 0;
      
  err = I2C_ReadData(LM75A_Sensor.Port, LM75_I2C_ADDR, LM75_REG_TEMP, (char*)tBuff, 2);
	
  if (err==I2C_ERR_OK)
	{
    errors = 0;
		temp = (uint16_t)tBuff[0]<<8 | tBuff[1];
		temp >>= 5;
		if (temp&(1<<10))
			sign = true;
		temp &= ~(1<<10);
		
		Result = (float)temp/8;
		
		if (sign)
			Result = -Result;
	} else
	{
    errors++;
		Result = 0;
    LM75A_Sensor.Error = 1;
	}
  
  return Result;
}
