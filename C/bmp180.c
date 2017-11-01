#include "bmp180.h"
#include "i2c.h"
#include "rtos.h"

_BMP180_SENSOR BMP180_Sensor;
_BMP180_CALIBR BMP180_Calibr;

//------------------------------------------------------------------------------
// BMP180_Init - initialize pressure sensor
//------------------------------------------------------------------------------
void BMP180_Init(uint8_t Port)
{
  uint8_t data[24];
  int err, i;
  
  if (Port>=I2C_NUM_LAST)
    return;
 
  BMP180_Sensor.Port = Port;  
  BMP180_Sensor.Error = 0;
  
  err = I2C_ReadData(BMP180_Sensor.Port, BMP180_I2C_ADDR, BMP180_REG_EEPROM, (char*)data, BMP180_EEPROM_DATA_LEN);
	
  if (err==I2C_ERR_OK)
	{
    i = 0;
    BMP180_Calibr.AC1 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.AC2 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.AC3 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.AC4 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.AC5 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.AC6 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.B1 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.B2 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.MB = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.MC = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
    BMP180_Calibr.MD = (int16_t)(data[i] << 8 | data[i + 1]);
  } else
  {
    BMP180_Sensor.Error = 1;
  }
}

//------------------------------------------------------------------------------
// BMP180_Read - read data (temperature and pressure) from sensor
//------------------------------------------------------------------------------
void BMP180_Read(void)
{
  uint8_t data[16];
  uint32_t UT;
  long UP, X1, X2, X3, B3, B6, p;
  unsigned long B4, B7;
  uint8_t err;
  
	//Start temperature measurement
  data[0] = BMP180_REG_CONTROL;
  data[1] = BMP180_CMD_TEMPERATURE;
  I2C_WriteData(BMP180_Sensor.Port, BMP180_I2C_ADDR, (char*)data, 2);
	//Wait to measurement
	//os_dly_wait(BMP180_DELAY_TEMPERATURE);
  //osDelay(BMP180_DELAY_TEMPERATURE);
  vTaskDelay(BMP180_DELAY_TEMPERATURE);
	//Read uncompensated temperature value
  err = I2C_ReadData(BMP180_Sensor.Port, BMP180_I2C_ADDR, BMP180_REG_RESULT, (char*)data, 2);
  if (err==I2C_ERR_OK)
  {  
    UT = data[0] << 8 | data[1];
    // Calculate true temperature
    X1 = (UT - BMP180_Calibr.AC6) * BMP180_Calibr.AC5 * BMP180_1_32768;
    X2 = BMP180_Calibr.MC * 2048 / (X1 + BMP180_Calibr.MD);
    BMP180_Calibr.B5 = X1 + X2;	
    // Get temperature in degrees
    BMP180_Sensor.Temperature = (BMP180_Calibr.B5 + 8) / ((float)160);
  } else
  {
    return;
  }
	
	//Start pressure measurement
  data[0] = BMP180_REG_CONTROL;
  data[1] = BMP180_CMD_PRESSURE_0;
  I2C_WriteData(BMP180_Sensor.Port, BMP180_I2C_ADDR, (char*)data, 2);
	//Wait to measurement
	//os_dly_wait(BMP180_DELAY_PRESSURE_0);
  //osDelay(BMP180_DELAY_PRESSURE_0);
  vTaskDelay(BMP180_DELAY_PRESSURE_0);
	//Read uncompensated pressure value
	err = I2C_ReadData(BMP180_Sensor.Port, BMP180_I2C_ADDR, BMP180_REG_RESULT, (char*)data, 3);	
  if (err==I2C_ERR_OK)
  {  
    // Get uncompensated pressure
    UP = (data[0] << 16 | data[1] << 8 | data[2]) >> (8 - OSS);
    // Calculate true pressure
    B6 = BMP180_Calibr.B5 - 4000;
    X1 = (BMP180_Calibr.B2 * (B6 * B6 * BMP180_1_4096)) * BMP180_1_2048;
    X2 = BMP180_Calibr.AC2 * B6 * BMP180_1_2048;
    X3 = X1 + X2;
    B3 = (((BMP180_Calibr.AC1 * 4 + X3) << OSS) + 2) * 0.25;
    X1 = BMP180_Calibr.AC3 * B6 * BMP180_1_8192;
    X2 = (BMP180_Calibr.B1 * (B6 * B6 * BMP180_1_4096)) * BMP180_1_65536;
    X3 = ((X1 + X2) + 2) * 0.25;
    B4 = BMP180_Calibr.AC4 * (uint32_t)(X3 + 32768) * BMP180_1_32768;
    B7 = ((uint32_t)UP - B3) * (50000 >> OSS);
    if (B7 < 0x80000000) 
      p = (B7 * 2) / B4;
    else
      p = (B7 / B4) * 2;
    X1 = ((float)p * BMP180_1_256) * ((float)p * BMP180_1_256);
    X1 = (X1 * 3038) * BMP180_1_65536;
    X2 = (-7357 * p) * BMP180_1_65536;
    p = p + (X1 + X2 + 3791) * BMP180_1_16;
    
    // Save pressure
    BMP180_Sensor.Pressure = p;	
  } else
  {
  }
}
