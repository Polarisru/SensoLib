#include "ds3231.h"
#include "i2c.h"
#include "timedate.h"
#include "utils.h"

uint8_t DS3231_Data[16];

_DS3231_SENSOR DS3231_Sensor;

//------------------------------------------------------------------------------
// DS3231_GetDateTime - get current date/time
//------------------------------------------------------------------------------
void DS3231_GetDateTime(TTimeDate *DateTime)
{
  int err;
  
  err = I2C_ReadData(DS3231_Sensor.Port, DS3231_I2C_ADDR, DS3231_REG_SECONDS, (char*)DS3231_Data, 7);
  if (err==I2C_ERR_OK)
  {  
    DateTime->Second = UTILS_Bcd2Hex(DS3231_Data[0]);
    DateTime->Minute = UTILS_Bcd2Hex(DS3231_Data[1]);
    DateTime->Hour = UTILS_Bcd2Hex(DS3231_Data[2]);
    DateTime->Day = UTILS_Bcd2Hex(DS3231_Data[4]);
    DateTime->Month = UTILS_Bcd2Hex(DS3231_Data[5]);
    DateTime->Year = UTILS_Bcd2Hex(DS3231_Data[6]);
  }
}

//------------------------------------------------------------------------------
// DS3231_SetTime - set current time
//------------------------------------------------------------------------------
void DS3231_SetTime(TTimeDate *DateTime)
{
  DS3231_Data[0] = DS3231_REG_SECONDS;
  DS3231_Data[1] = UTILS_Hex2Bcd(DateTime->Second);
  DS3231_Data[2] = UTILS_Hex2Bcd(DateTime->Minute);
  DS3231_Data[3] = UTILS_Hex2Bcd(DateTime->Hour);
  I2C_WriteData(DS3231_Sensor.Port, DS3231_I2C_ADDR, (char*)DS3231_Data, 4);
}

//------------------------------------------------------------------------------
// DS3231_SetDate - set current date
//------------------------------------------------------------------------------
void DS3231_SetDate(TTimeDate *DateTime)
{
  DS3231_Data[0] = DS3231_REG_DATE;
  DS3231_Data[1] = UTILS_Hex2Bcd(DateTime->Day);
  DS3231_Data[2] = UTILS_Hex2Bcd(DateTime->Month);
  DS3231_Data[3] = UTILS_Hex2Bcd(DateTime->Year);
  I2C_WriteData(DS3231_Sensor.Port, DS3231_I2C_ADDR, (char*)DS3231_Data, 4);
}

//------------------------------------------------------------------------------
// DS3231_GetDayOfWeek - get day of week
//------------------------------------------------------------------------------
int DS3231_GetDayOfWeek(void)
{
  int err;
  
  err = I2C_ReadData(DS3231_Sensor.Port, DS3231_I2C_ADDR, DS3231_REG_DAY, (char*)DS3231_Data, 1);
  if (err==I2C_ERR_OK)
  {
    return DS3231_Data[0];
  }
  
  return 0;
}

//------------------------------------------------------------------------------
// DS3231_SetDayOfWeek - set day of week
//------------------------------------------------------------------------------
void DS3231_SetDayOfWeek(int DayOfWeek)
{
  DS3231_Data[0] = DS3231_REG_DAY;
  DS3231_Data[1] = DayOfWeek;
  I2C_WriteData(DS3231_Sensor.Port, DS3231_I2C_ADDR, (char*)DS3231_Data, 2);
}

//------------------------------------------------------------------------------
// DS3231_Init - initialize real-time clock
//------------------------------------------------------------------------------
void DS3231_Init(int Port)
{
  if (Port>=I2C_NUM_LAST)
    return;

  DS3231_Sensor.Port = Port;
}
