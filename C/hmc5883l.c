#include <math.h>
#include "hmc5883l.h"
#include "i2c.h"

uint8_t HMC5883L_Buffer[16];
_HMC5883L_SENSOR HMC5883L_Sensor;

//------------------------------------------------------------------------------
/// @brief Get number of samples averaged per measurement.
/// @return Current samples averaged per measurement (0-3 for 1/2/4/8 respectively)
//------------------------------------------------------------------------------
uint8_t HMC5883L_GetSampleAveraging(void) 
{
  uint8_t err;
  
  err = I2C_ReadData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, HMC5883L_RA_CONFIG_A, (char*)HMC5883L_Buffer, 1);
  if (err==I2C_ERR_OK)
  {
    return (HMC5883L_Buffer[0]>>HMC5883L_CRB_AVERAGE_OFFS);
  }
  return HMC5883L_AVERAGING_1;
}
//------------------------------------------------------------------------------
/// @brief Set number of samples averaged per measurement.
/// @param averaging New samples averaged per measurement setting(0-3 for 1/2/4/8 respectively)
//------------------------------------------------------------------------------
void HMC5883L_SetSampleAveraging(uint8_t averaging) 
{
  HMC5883L_Buffer[0] = HMC5883L_RA_CONFIG_A;
  HMC5883L_Buffer[1] = averaging << HMC5883L_CRB_AVERAGE_OFFS;
  I2C_WriteData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, (char*)HMC5883L_Buffer, 2);
}

//------------------------------------------------------------------------------
/// @return Get Current magnetic field gain value
/// @return Current magnetic field gain value, uint8_t
//------------------------------------------------------------------------------
uint8_t HMC5883L_GetGain(void) 
{
  uint8_t err;
  
  err = I2C_ReadData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, HMC5883L_RA_CONFIG_B, (char*)HMC5883L_Buffer, 1);
  if (err==I2C_ERR_OK)
  {
    return (HMC5883L_Buffer[0]>>HMC5883L_CRB_GAIN_OFFS);
  }
  return HMC5883L_GAIN_1370;
}

//------------------------------------------------------------------------------
/// @brief Set magnetic field gain value.
/// @param gain New magnetic field gain value
//------------------------------------------------------------------------------
void HMC5883L_SetGain(uint8_t gain) 
{
  HMC5883L_Buffer[0] = HMC5883L_RA_CONFIG_B;
  HMC5883L_Buffer[1] = gain << HMC5883L_CRB_GAIN_OFFS;
  I2C_WriteData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, (char*)HMC5883L_Buffer, 2);
}

//------------------------------------------------------------------------------
/// @brief Set magnetometer mode
//------------------------------------------------------------------------------
void HMC5883L_SetMode(uint8_t newMode) 
{
  HMC5883L_Buffer[0] = HMC5883L_RA_MODE;
  HMC5883L_Buffer[1] = newMode;
  I2C_WriteData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, (char*)HMC5883L_Buffer, 2);
}

//------------------------------------------------------------------------------
/// @brief Get magnetometer mode
//------------------------------------------------------------------------------
uint8_t HMC5883L_GetMode(void) 
{
  uint8_t err;
  
  err = I2C_ReadData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, HMC5883L_RA_MODE, (char*)HMC5883L_Buffer, 1);
  if (err==I2C_ERR_OK)
  {
    return HMC5883L_Buffer[0];
  }
  return HMC5883L_MODE_CONTINUOUS;
}

//------------------------------------------------------------------------------
/// @brief Get sensor id
//------------------------------------------------------------------------------
TBool HMC5883L_GetId(void) 
{
  uint8_t err;
  
  err = I2C_ReadData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, HMC5883L_RA_ID_A, (char*)HMC5883L_Buffer, 3);
  if (err==I2C_ERR_OK)
  {
    if ((HMC5883L_Buffer[0]==HMC5883L_ID_SIGN_0)&&(HMC5883L_Buffer[1]==HMC5883L_ID_SIGN_1)&&
        (HMC5883L_Buffer[2]==HMC5883L_ID_SIGN_2))
      return true;
  }
  
  return false;
}

//------------------------------------------------------------------------------
/// @brief Init magnetometer module
//------------------------------------------------------------------------------
void HMC5883L_Init(uint8_t Port)
{ 
  if (Port>=I2C_NUM_LAST)
    return;
  
  HMC5883L_Sensor.X = 0;
  HMC5883L_Sensor.Y = 0;
  HMC5883L_Sensor.Z = 0;
  HMC5883L_Sensor.Port = Port;
  HMC5883L_Sensor.Error = 0;
  I2C_Initialize(Port, HMC5883L_I2C_ADDR, HMC5883L_I2C_SPEED);
  
  if (!HMC5883L_GetId())
  {
    //sensor not found
    HMC5883L_Sensor.Error = 1;
    return;
  }
  
  HMC5883L_Buffer[0] = HMC5883L_RA_CONFIG_A;
  HMC5883L_Buffer[1] = (HMC5883L_AVERAGING_8<<5) | (HMC5883L_RATE_1P5<<2);
  I2C_WriteData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, (char*)HMC5883L_Buffer, 2);
  
  HMC5883L_SetGain(HMC5883L_GAIN_1090);
  HMC5883L_SetMode(HMC5883L_MODE_SINGLE);
}

//------------------------------------------------------------------------------
// HMC5883L_Read - read data from magnetometer
//------------------------------------------------------------------------------
void HMC5883L_Read(void)
{
  uint8_t err;
  float heading;
  float declinationAngle;
  
  if (HMC5883L_Sensor.Error)
    return;
  
  err = I2C_ReadData(HMC5883L_Sensor.Port, HMC5883L_I2C_ADDR, HMC5883L_RA_DATAX_H, (char*)HMC5883L_Buffer, 6);
  if (err==I2C_ERR_OK)
	{
    HMC5883L_Sensor.X = ((int16_t)HMC5883L_Buffer[0]<<8) + HMC5883L_Buffer[1];
    HMC5883L_Sensor.Y = ((int16_t)HMC5883L_Buffer[4]<<8) + HMC5883L_Buffer[5];
    HMC5883L_Sensor.Z = ((int16_t)HMC5883L_Buffer[2]<<8) + HMC5883L_Buffer[3];
    
    HMC5883L_Sensor.ScaledX = HMC5883L_Sensor.X/1.0;
    HMC5883L_Sensor.ScaledY = HMC5883L_Sensor.Y/1.0;
    HMC5883L_Sensor.ScaledZ = HMC5883L_Sensor.Z/1.0;
    // Calculate heading when the magnetometer is level, then correct for signs of axis.
    // Atan2() automatically check the correct formula taking care of the quadrant you are in
    heading = atan2(HMC5883L_Sensor.ScaledY, HMC5883L_Sensor.ScaledX);

    // Once you have your heading, you must then add your 'Declination Angle',
    // which is the 'Error' of the magnetic field in your location. Mine is 0.0404 
    // Find yours here: http://www.magnetic-declination.com/
    // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
    declinationAngle = 0.0404;
    heading += declinationAngle;

    // Correct for when signs are reversed.
    if (heading < 0)
      heading += 2*PI;

    // Check for wrap due to addition of declination.
    if (heading > 2*PI)
      heading -= 2*PI;

    // Convert radians to degrees for readability.
    HMC5883L_Sensor.Heading = heading * 180/PI; 
  } else
  {
  }
  HMC5883L_SetMode(HMC5883L_MODE_SINGLE);
}
