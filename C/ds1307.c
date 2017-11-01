#include <string.h>
#include "ds1307.h"
#include "i2c.h"
#include "utils.h"

TByte DS1307_Data[16];

_DS1307_SENSOR DS1307_Sensor;

//------------------------------------------------------------------------------
// DS1307_GetDateTime - get current date/time
//------------------------------------------------------------------------------
void DS1307_GetDateTime(TTimeDate *DateTime)
{
  int err;
  
  err = I2C_ReadData(DS1307_Sensor.Port, DS1307_I2C_ADDR, DS1307_REG_SECONDS, (char*)DS1307_Data, 7);
  if (err==I2C_ERR_OK)
  {  
    DateTime->Second = UTILS_Bcd2Hex(DS1307_Data[0]);
    DateTime->Minute = UTILS_Bcd2Hex(DS1307_Data[1]);
    DateTime->Hour = UTILS_Bcd2Hex(DS1307_Data[2]);
    DateTime->Day = UTILS_Bcd2Hex(DS1307_Data[4]);
    DateTime->Month = UTILS_Bcd2Hex(DS1307_Data[5]);
    DateTime->Year = UTILS_Bcd2Hex(DS1307_Data[6]);
  }
}

//------------------------------------------------------------------------------
// DS1307_SetTime - set current time
//------------------------------------------------------------------------------
void DS1307_SetTime(TTimeDate *DateTime)
{
  DS1307_Data[0] = DS1307_REG_SECONDS;
  DS1307_Data[1] = UTILS_Hex2Bcd(DateTime->Second);
  DS1307_Data[2] = UTILS_Hex2Bcd(DateTime->Minute);
  DS1307_Data[3] = UTILS_Hex2Bcd(DateTime->Hour);
  I2C_WriteData(DS1307_Sensor.Port, DS1307_I2C_ADDR, (char*)DS1307_Data, 4);
}

//------------------------------------------------------------------------------
// DS1307_SetDate - set current date
//------------------------------------------------------------------------------
void DS1307_SetDate(TTimeDate *DateTime)
{
  DS1307_Data[0] = DS1307_REG_DATE;
  DS1307_Data[1] = UTILS_Hex2Bcd(DateTime->Day);
  DS1307_Data[2] = UTILS_Hex2Bcd(DateTime->Month);
  DS1307_Data[3] = UTILS_Hex2Bcd(DateTime->Year);
  I2C_WriteData(DS1307_Sensor.Port, DS1307_I2C_ADDR, (char*)DS1307_Data, 4);
}

//------------------------------------------------------------------------------
// DS1307_GetDayOfWeek - get day of week
//------------------------------------------------------------------------------
int DS1307_GetDayOfWeek(void)
{
  int err;
  
  err = I2C_ReadData(DS1307_Sensor.Port, DS1307_I2C_ADDR, DS1307_REG_DAY, (char*)DS1307_Data, 1);
  if (err==I2C_ERR_OK)
  {
    return DS1307_Data[0];
  }
  
  return 0;
}

//------------------------------------------------------------------------------
// DS1307_SetDayOfWeek - set day of week
//------------------------------------------------------------------------------
void DS1307_SetDayOfWeek(int DayOfWeek)
{
  DS1307_Data[0] = DS1307_REG_DAY;
  DS1307_Data[1] = DayOfWeek;
  I2C_WriteData(DS1307_Sensor.Port, DS1307_I2C_ADDR, (char*)DS1307_Data, 2);
}

//------------------------------------------------------------------------------
// DS1307_SwitchOutput - switch chip output on/off
//------------------------------------------------------------------------------
void DS1307_SwitchOutput(int On, int Freq)
{
  if (Freq>DS1307_SQW_RATE_32K)
    return;
  
  DS1307_Data[0] = DS1307_REG_CONTROL;
  if (On)
  {
    DS1307_Data[1] = (1<<DS1307_SQW_BIT) | (Freq<<DS1307_SQW_RATE_BIT);
  } else
  {
    DS1307_Data[1] = 0;
  }
  I2C_WriteData(DS1307_Sensor.Port, DS1307_I2C_ADDR, (char*)DS1307_Data, 2);
}

//------------------------------------------------------------------------------
// DS1307_ReadData - read data from chip NV-SRAM
//------------------------------------------------------------------------------
int DS1307_ReadData(int Addr, char *Buff, int Len)
{
  int err;
  
  if (Len>(DS1307_SRAM_SIZE-Addr))
    Len = DS1307_SRAM_SIZE - Addr;
  err = I2C_ReadData(DS1307_Sensor.Port, DS1307_I2C_ADDR, DS1307_SRAM_START + Addr, (char*)Buff, Len);
  if (err==I2C_ERR_OK)
    return 1;
  
  return 0;
}

//------------------------------------------------------------------------------
// DS1307_ReadData - write data to chip NV-SRAM
//------------------------------------------------------------------------------
int DS1307_WriteData(int Addr, char *Buff, int Len)
{
  char tempBuff[DS1307_SRAM_SIZE];
  
  if (Len>(DS1307_SRAM_SIZE-Addr))
    Len = DS1307_SRAM_SIZE - Addr;
  tempBuff[0] = DS1307_SRAM_START + Addr;
  memcpy(&tempBuff[1], Buff, Len);
  I2C_WriteData(DS1307_Sensor.Port, DS1307_I2C_ADDR, tempBuff, Len + 1);
  
  return 1;
}

//------------------------------------------------------------------------------
// DS1307_Init - initialize real-time clock
//------------------------------------------------------------------------------
void DS1307_Init(int Port)
{
  if (Port>=I2C_NUM_LAST)
    return;

  DS1307_Sensor.Port = Port;
}
