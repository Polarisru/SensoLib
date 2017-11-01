#ifndef _DS1307_H
#define _DS1307_H

#include "defines.h"
#include "Platform.h"
#include "timedate.h"

#define DS1307_I2C_ADDR       (0xD0)

#define DS1307_REG_SECONDS    0x00
#define DS1307_REG_MINUTES    0x01
#define DS1307_REG_HOURS      0x02
#define DS1307_REG_DAY        0x03
#define DS1307_REG_DATE       0x04                             
#define DS1307_REG_MONTH      0x05                           
#define DS1307_REG_YEAR       0x06                   
#define DS1307_REG_CONTROL    0x07  

#define DS1307_SRAM_START     0x08
#define DS1307_SRAM_SIZE      56

#define DS1307_SQW_RATE_1		  0
#define DS1307_SQW_RATE_4K		1
#define DS1307_SQW_RATE_8K		2
#define DS1307_SQW_RATE_32K		3

#define DS1307_SQW_RATE_BIT   0
#define DS1307_SQW_BIT		    4

typedef struct {
  int       Port;                 // I2C port number
	TByte			ID[4];								// Sensor HW ID
	TByte			Error;								// if Sensor read or write fails
} _DS1307_SENSOR __attribute__ ((aligned (4))); 

extern void DS1307_GetDateTime(TTimeDate *DateTime);
extern void DS1307_SetTime(TTimeDate *DateTime);
extern void DS1307_SetDate(TTimeDate *DateTime);
extern int DS1307_GetDayOfWeek(void);
extern void DS1307_SetDayOfWeek(int DayOfWeek);
extern void DS1307_SwitchOutput(int On, int Freq);
extern int DS1307_ReadData(int Addr, char *Buff, int Len);
extern int DS1307_WriteData(int Addr, char *Buff, int Len);
extern void DS1307_Init(int Port);
	 
#endif
