#ifndef _TIMEDATE_H
#define _TIMEDATE_H

#include <stdint.h>
#include "defines.h"

#define TIMEDATE_DEF_YEAR  (15)  //2015

#define TIMEDATE_FIRSTDAY_2000    6	      // 1.1.2000 was a Saturday (0 = Sunday)

#pragma pack(1)
typedef struct
{
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
} TTimeDate;
#pragma pack()

typedef struct
{
  uint8_t Day;
  uint8_t Month;
  uint8_t Hour;
  uint8_t Ext;
} TDate;

enum TIMEDATE_COMPARE_RESULTS
{
  TIMEDATE_LESS=-1,
  TIMEDATE_EQUAL=0,
  TIMEDATE_MORE=1
};

#define SECOND_WEIGHT	(1.0/(24*60*60L))
#define MINUTE_WEIGHT	(1.0/(24*60))
#define HOUR_WEIGHT		(1.0/24)

// Number of days in a month
extern const uint8_t TIMEDATE_MaxDay[13];
//extern const char TIMEDATE_DayOfWeek[8][4];
//extern const char TIMEDATE_DayWeek[][10];
//extern const char TIMEDATE_MonthName[13][10];

extern const char TIMEDATE_DateFormatEUR[]; 
extern const char TIMEDATE_DateFormatUSA[];
extern const char TIMEDATE_DateFormatCHN[]; 
extern const char TIMEDATE_TimeFormatEUR[]; 
extern const char TIMEDATE_TimeFormatUSA_AM[];
extern const char TIMEDATE_TimeFormatUSA_PM[];
extern const char TIMEDATE_TimeFormatCHN[]; 

// compare date only
extern signed char TIMEDATE_CompareDate(TTimeDate *dt0, TTimeDate *dt1);
// get date of week
extern uint8_t TIMEDATE_GetDayOfWeek(TTimeDate *dt);//uint8_t year, uint8_t month, uint8_t day);
// compare time only;
extern signed char TIMEDATE_CompareTime(TTimeDate *dt0, TTimeDate *dt1);
//compare date and time
extern signed char TIMEDATE_CompareDateTime(TTimeDate *dt0, TTimeDate *dt1);
// check if date is valid
extern TBool TIMEDATE_IsValidDate(TTimeDate *dt);
// set default valid date
void TIMEDATE_SetValidDate(TTimeDate *dt);
// check if time is valid
extern TBool TIMEDATE_IsValidTime(TTimeDate *dt);
// check if date and time are valid
extern TBool TIMEDATE_IsValid(TTimeDate *dt);
//validate date/time
extern void TIMEDATE_Validate(TTimeDate *dt);
//check if this year is a leap one
extern uint8_t TIMEDATE_IsLeapYear(uint8_t Year);
//increment date by one day
extern void TIMEDATE_IncDate(TTimeDate *dt);
// TIMEDATE_Encode - pack date and time into 5-byte structure
extern void TIMEDATE_Encode(TTimeDate *dt, uint8_t *Buffer);
// TIMEDATE_Decode - unpack date and time from 5-byte structure
extern TTimeDate TIMEDATE_Decode(uint8_t *Buffer);
// TIMEDATE_DaysFromNull - calc number of days from the 1st of January 2000
extern uint16_t TIMEDATE_DaysFromNull(TTimeDate *dt);
// TIMEDATE_SecondsFromNull - calc number of seconds from the 1st of January 2000
extern uint32_t TIMEDATE_SecondsFromNull(TTimeDate *dt);
// TIMEDATE_CalcDateFromDays - calc date from the number of days (1.1.2000)
extern void TIMEDATE_CalcDateFromDays(uint16_t days, TTimeDate *dt);
// TIMEDATE_CalcDateFromSeconds - calc date from the number of seconds (1.1.2000)
extern void TIMEDATE_CalcDateFromSeconds(uint32_t seconds, TTimeDate *dt);
// TIMEDATE_DaysInterval - calc interval in days between two dates
extern int TIMEDATE_DaysInterval(TTimeDate *dt0, TTimeDate *dt1);
// TIMEDATE_AddDays - add days to date
extern void TIMEDATE_AddDays(TTimeDate *dt, uint16_t days);
// TIMEDATE_CompareDateInterval - compare two dates with interval
extern signed char TIMEDATE_CompareDateInterval(TTimeDate *dt0, TTimeDate *dt1, int Interval);
// TIMEDATE_Date2Str - format date to string with EUR/USA/CHN format
extern void TIMEDATE_Date2Str(TTimeDate *dt, char *str, int Frmt);
// TIMEDATE_Time2Str - format time to string 
extern void TIMEDATE_Time2Str(TTimeDate *dt, char *str, int Frmt);
// TIMEDATE_RealTime2Str - format real time to string with EUR/USA/CHN format
extern void TIMEDATE_RealTime2Str(TTimeDate *dt, char *str, int Frmt);
// TIMEDATE_SimpleTime2Str - format time to string
extern void TIMEDATE_SimpleTime2Str(int dt, char *str);
// TIMEDATE_GetFatTime - convert time/date to a FAT time/date record
uint32_t TIMEDATE_GetFatTime(TTimeDate *dt);

#endif
