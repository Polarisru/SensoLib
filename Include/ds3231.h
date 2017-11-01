#ifndef _DS3231_H
#define _DS3231_H

#include "defines.h"
#include "Platform.h"
#include "timedate.h"

#define DS3231_I2C_ADDR       (0xD0)

#define DS3231_REG_SECONDS    0x00
#define DS3231_REG_MINUTES    0x01
#define DS3231_REG_HOURS      0x02
#define DS3231_REG_DAY        0x03
#define DS3231_REG_DATE       0x04                             
#define DS3231_REG_MONTH      0x05                           
#define DS3231_REG_YEAR       0x06                   
#define DS3231_REG_ALR1_SEC   0x07       
#define DS3231_REG_ALR1_MIN   0x08
#define DS3231_REG_ALR1_HOUR  0x09           
#define DS3231_REG_ALR1_DATE  0x0A 
#define DS3231_REG_ALR2_MIN   0x0B   
#define DS3231_REG_ALR2_HOUR  0x0C
#define DS3231_REG_ALR2_DATE  0x0D
#define DS3231_REG_CONTROL    0x0E
#define DS3231_REG_STATUS     0x0F
#define DS3231_REG_AGEOFFS    0x10
#define DS3231_REG_TEMP_MSB   0x11
#define DS3231_REG_TEMP_LSB   0x12 

#define DS3231_SQW_RATE_1		  0
#define DS3231_SQW_RATE_1K		1
#define DS3231_SQW_RATE_4K		2
#define DS3231_SQW_RATE_8K		3

#define DS3231_SQW_RATE_BIT   3
#define DS3231_SQW_BIT		    6

typedef struct {
  int       Port;                 // I2C port number
	uint8_t			ID[4];								// Sensor HW ID
	uint8_t			Error;								// if Sensor read or write fails
} _DS3231_SENSOR __attribute__ ((aligned (4))); 

extern void DS3231_GetDateTime(TTimeDate *DateTime);
extern void DS3231_SetTime(TTimeDate *DateTime);
extern void DS3231_SetDate(TTimeDate *DateTime);
extern int DS3231_GetDayOfWeek(void);
extern void DS3231_SetDayOfWeek(int DayOfWeek);
extern void DS3231_Init(int Port);

#endif
