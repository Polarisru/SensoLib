#include "mpu6050.h"
#include "i2c.h"

uint8_t MPU6050_Buffer[16];
_MPU6050_SENSOR MPU6050_Sensor;

//------------------------------------------------------------------------------
/// @brief Read a single bit from an 8-bit device register
/// @param regAddr Register regAddr to read from
/// @param bitNum Bit position to read (0-7)
/// @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
//------------------------------------------------------------------------------
void MPU6050_ReadBit(uint8_t regAddr, uint8_t bitNum, uint8_t *data)
{
  uint8_t tmp, err;
  
  err = I2C_ReadData(MPU6050_Sensor.Port, MPU6050_I2C_ADDR, regAddr, (char*)&tmp, 1);
  if (err==I2C_ERR_OK)
    *data = tmp & (1 << bitNum);
  else
    *data = 0;
}

//------------------------------------------------------------------------------
/// @brief Read multiple bits from an 8-bit device register.
/// @param regAddr Register regAddr to read from
/// @param bitStart First bit position to read (0-7)
/// @param length Number of bits to read (not more than 8)
/// @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
/// @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in readTimeout)
//------------------------------------------------------------------------------
void MPU6050_ReadBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data)
{
  // 01101001 read byte
  // 76543210 bit numbers
  //    xxx   args: bitStart=4, length=3
  //    010   masked
  //   -> 010 shifted
  uint8_t tmp, err;
  uint8_t mask;
  
  err = I2C_ReadData(MPU6050_Sensor.Port, MPU6050_I2C_ADDR, regAddr, (char*)&tmp, 1);
  if (err==I2C_ERR_OK)
  {
    mask = ((1 << length) - 1) << (bitStart - length + 1);
    tmp &= mask;
    tmp >>= (bitStart - length + 1);
    *data = tmp;
  } else
  {
    *data = 0;
  }
}

//------------------------------------------------------------------------------
/// @brief write a single bit in an 8-bit device register.
/// @param slaveAddr I2C slave device address
/// @param regAddr Register regAddr to write to
/// @param bitNum Bit position to write (0-7)
/// @param value New bit value to write
//------------------------------------------------------------------------------
void MPU6050_WriteBit(uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
  uint8_t tmp, err;
  
  err = I2C_ReadData(MPU6050_Sensor.Port, MPU6050_I2C_ADDR, regAddr, (char*)&tmp, 1);
  if (err==I2C_ERR_OK)
  {
    tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    MPU6050_Buffer[0] = regAddr;
    MPU6050_Buffer[1] = tmp;
    I2C_WriteData(MPU6050_Sensor.Port, MPU6050_I2C_ADDR, (char*)MPU6050_Buffer, 2);
  }
}

//------------------------------------------------------------------------------
/// @brief Write multiple bits in an 8-bit device register.
/// @param slaveAddr I2C slave device address
/// @param regAddr Register regAddr to write to
/// @param bitStart First bit position to write (0-7)
/// @param length Number of bits to write (not more than 8)
/// @param data Right-aligned value to write
//------------------------------------------------------------------------------
void MPU6050_WriteBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
  //      010 value to write
  // 76543210 bit numbers
  //    xxx   args: bitStart=4, length=3
  // 00011100 mask byte
  // 10101111 original value (sample)
  // 10100011 original & ~mask
  // 10101011 masked | value
  uint8_t tmp, err;
  uint8_t mask;
  
  err = I2C_ReadData(MPU6050_Sensor.Port, MPU6050_I2C_ADDR, regAddr, (char*)&tmp, 1);
  if (err==I2C_ERR_OK)
  {
    mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    tmp &= ~(mask); // zero all important bits in existing byte
    tmp |= data; // combine data with existing byte
    I2C_WriteData(MPU6050_Sensor.Port, MPU6050_I2C_ADDR, (char*)MPU6050_Buffer, 2);
  }
}

//------------------------------------------------------------------------------
/// @brief Get Device ID.
/// @brief This register is used to verify the identity of the device (0b110100).
/// @return Device ID (should be 0x68, 104 dec, 150 oct)
//------------------------------------------------------------------------------
uint8_t MPU6050_GetDeviceID(void)
{
  uint8_t tmp;
  
  MPU6050_ReadBits(MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, &tmp);
  return tmp;
}

//------------------------------------------------------------------------------
/// @brief Set clock source setting.
/// An internal 8MHz oscillator, gyroscope based clock, or external sources can
/// be selected as the MPU-60X0 clock source. When the internal 8 MHz oscillator
/// or an external source is chosen as the clock source, the MPU-60X0 can operate
/// in low power modes with the gyroscopes disabled.
///
/// Upon power up, the MPU-60X0 clock source defaults to the internal oscillator.
/// However, it is highly recommended that the device be configured to use one of
/// the gyroscopes (or an external clock source) as the clock reference for
/// improved stability. The clock source can be selected according to the following table:
///
/// <pre>
/// CLK_SEL | Clock Source
/// --------+--------------------------------------
/// 0       | Internal oscillator
/// 1       | PLL with X Gyro reference
/// 2       | PLL with Y Gyro reference
/// 3       | PLL with Z Gyro reference
/// 4       | PLL with external 32.768kHz reference
/// 5       | PLL with external 19.2MHz reference
/// 6       | Reserved
/// 7       | Stops the clock and keeps the timing generator in reset
/// </pre>
//------------------------------------------------------------------------------
void MPU6050_SetClockSource(uint8_t source)
{
  MPU6050_WriteBits(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

//------------------------------------------------------------------------------
/// @brief Set full-scale gyroscope range.
/// @param range New full-scale gyroscope range value
//------------------------------------------------------------------------------
void MPU6050_SetFullScaleGyroRange(uint8_t range)
{
  MPU6050_WriteBits(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
  switch (range)
  {
    case MPU6050_GYRO_FS_250:
      MPU6050_Sensor.GyroScale = 250;
      break;
    case MPU6050_GYRO_FS_500:
      MPU6050_Sensor.GyroScale = 500;
      break;
    case MPU6050_GYRO_FS_1000:
      MPU6050_Sensor.GyroScale = 1000;
      break;
    case MPU6050_GYRO_FS_2000:
    default:
      MPU6050_Sensor.GyroScale = 2000;
      break;
  }
}

// GYRO_CONFIG register

//------------------------------------------------------------------------------
/// @brief Get full-scale gyroscope range.
/// The FS_SEL parameter allows setting the full-scale range of the gyro sensors,
/// as described in the table below.
///
/// <pre>
/// 0 = +/- 250 degrees/sec
/// 1 = +/- 500 degrees/sec
/// 2 = +/- 1000 degrees/sec
/// 3 = +/- 2000 degrees/sec
/// </pre>
///
/// @return Current full-scale gyroscope range setting
//------------------------------------------------------------------------------
uint8_t MPU6050_GetFullScaleGyroRange(void)
{
  uint8_t tmp;
  
  MPU6050_ReadBits(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, &tmp);
  return tmp;
}

//------------------------------------------------------------------------------
/// @brief Get full-scale accelerometer range.
/// The FS_SEL parameter allows setting the full-scale range of the accelerometer
/// sensors, as described in the table below.
///
/// <pre>
/// 0 = +/- 2g
/// 1 = +/- 4g
/// 2 = +/- 8g
/// 3 = +/- 16g
/// </pre>
///
/// @return Current full-scale accelerometer range setting
//------------------------------------------------------------------------------
uint8_t MPU6050_GetFullScaleAccelRange(void)
{
  uint8_t tmp;
  
  MPU6050_ReadBits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, &tmp);
  return tmp;
}

//------------------------------------------------------------------------------
/// @brief Set full-scale accelerometer range.
/// @param range New full-scale accelerometer range setting
//------------------------------------------------------------------------------
void MPU6050_SetFullScaleAccelRange(uint8_t range)
{
  MPU6050_WriteBits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
  switch (range)
  {
    case MPU6050_ACCEL_FS_2:
      MPU6050_Sensor.AccelScale = 2;
      break;
    case MPU6050_ACCEL_FS_4:
      MPU6050_Sensor.AccelScale = 4;
    case MPU6050_ACCEL_FS_8:
      MPU6050_Sensor.AccelScale = 8;
    case MPU6050_ACCEL_FS_16:
    default:
      MPU6050_Sensor.AccelScale = 16;
      break;
  }
}

//------------------------------------------------------------------------------
/// @brief Get sleep mode status.
/// Setting the SLEEP bit in the register puts the device into very low power
/// sleep mode. In this mode, only the serial interface and internal registers
/// remain active, allowing for a very low standby current. Clearing this bit
/// puts the device back into normal mode. To save power, the individual standby
/// selections for each of the gyros should be used if any gyro axis is not used
/// by the application.
/// @return Current sleep mode enabled status
//------------------------------------------------------------------------------
TBool MPU6050_GetSleepModeStatus()
{
  uint8_t tmp;
  
  MPU6050_ReadBit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, &tmp);
  return tmp == 0x00 ? false : true;
}

//------------------------------------------------------------------------------
/// @brief Set sleep mode status.
/// @param enabled New sleep mode enabled status
//------------------------------------------------------------------------------
void MPU6050_SetSleepModeStatus(FunctionalState newState)
{
  MPU6050_WriteBit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, newState);
}

//------------------------------------------------------------------------------
/// @brief Get raw 6-axis motion sensor readings (accel/gyro).
/// Retrieves all currently available motion sensor values.
/// @param AccelGyro 16-bit signed integer array of length 6
/// @see MPU6050_RA_ACCEL_XOUT_H
//------------------------------------------------------------------------------
uint8_t MPU6050_GetRawAccelGyro(int16_t* AccelGyro)
{
  uint8_t tmpBuffer[14];
  uint8_t err;
    
  err = I2C_ReadData(MPU6050_Sensor.Port, MPU6050_I2C_ADDR, MPU6050_RA_ACCEL_XOUT_H, (char*)tmpBuffer, 14);
  if (err==I2C_ERR_OK)
  {
    // get acceleration
    for (int i = 0; i < 3; i++)
      AccelGyro[i] = ((int16_t) ((uint16_t) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
    // get angular rate
    for (int i = 4; i < 7; i++)
      AccelGyro[i - 1] = ((int16_t) ((uint16_t) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
  }
  return err;
}

//------------------------------------------------------------------------------
/// @brief initialize digital accelerometer MPU6050
//------------------------------------------------------------------------------
void MPU6050_Init(uint8_t port)
{ 
  if (port>=I2C_NUM_LAST)
    return;
 
  MPU6050_Sensor.Port = port;  
  MPU6050_Sensor.Error = 0;

  MPU6050_SetClockSource(MPU6050_CLOCK_PLL_XGYRO);
  MPU6050_SetFullScaleGyroRange(MPU6050_GYRO_FS_250);
  MPU6050_SetFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  MPU6050_SetSleepModeStatus(DISABLE);
}

//------------------------------------------------------------------------------
// MPU6050_Read - read data from magnetometer
//------------------------------------------------------------------------------
void MPU6050_Read(void)
{
  uint8_t err;
  uint8_t i;
  int16_t AccelGyro[6];
  
  err = MPU6050_GetRawAccelGyro(AccelGyro);
  if (err==I2C_ERR_OK)
  {
    for (i = 0; i < 3; i++)
    {
      MPU6050_Sensor.Accel[i] = (float)AccelGyro[i]*MPU6050_Sensor.AccelScale/32768;
      MPU6050_Sensor.Gyro[i] = (float)AccelGyro[i+3]*MPU6050_Sensor.GyroScale/32768;
    }
  } else
  {
  }
}
