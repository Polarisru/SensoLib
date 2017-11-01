#include "rtc.h"
#include "timedate.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rtc.h"

/*******************************************************************************
* Function Name  : RTC_SetDate
* Description    : Wrapper for AKC Software
* Input          : struct of type RTC_DATE
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_SetDate(TTimeDate *dt)
{
	TTimeDate dt_now;
  uint32_t utime = RTC_GetCounter();
  TIMEDATE_CalcDateFromSeconds(utime, &dt_now);
	
	dt_now.Day = dt->Day;
	dt_now.Month = dt->Month;
	dt_now.Year = dt->Year;
	//now.tm_wday	= dt->weekday;

  utime = TIMEDATE_SecondsFromNull(&dt_now);
  RTC_SetCounter(utime);
}

/*******************************************************************************
* Function Name  : RTC_SetTime
* Description    : Wrapper for AKC Software
* Input          : struct of type RTC_TIME
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_SetTime(TTimeDate *dt)
{
	TTimeDate dt_now;
  uint32_t utime = RTC_GetCounter();
  TIMEDATE_CalcDateFromSeconds(utime, &dt_now);
  
	dt_now.Second = dt->Second;
	dt_now.Minute = dt->Minute;
	dt_now.Hour = dt->Hour;
  
  utime = TIMEDATE_SecondsFromNull(&dt_now);
  RTC_SetCounter(utime);
}

void RTC_SetDateTime(TTimeDate *dt)
{
  uint32_t utime = TIMEDATE_SecondsFromNull(dt);
  
  RTC_SetCounter(utime);
}

/*******************************************************************************
* Function Name  : RTC_Set_DefaultTimeDate
* Description    : set default values, 01.01.2015, 01:00:00:00
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Set_DefaultTimeDate(void)
{
	TTimeDate	def_dt;

	def_dt.Day			=	1;
	def_dt.Month		= 1;
	def_dt.Year			=	TIMEDATE_DEF_YEAR;
	def_dt.Hour 		= 1;
	def_dt.Minute 	= 0;
	def_dt.Second 	=	0;
		
	RTC_SetDateTime(&def_dt);
}

/*******************************************************************************
* Function Name  : RTC_GetTime
* Description    : Wrapper for AKC Software
* Input          : struct of type RTC_TIME
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_GetTime(TTimeDate *dt)
{
	TTimeDate dt_now;
  uint32_t utime = RTC_GetCounter();
  TIMEDATE_CalcDateFromSeconds(utime, &dt_now);
  
	dt->Hour 	 = dt_now.Hour;
	dt->Minute = dt_now.Minute;
	dt->Second = dt_now.Second;
	//dt->milliseconds	= 0;
}

/*******************************************************************************
* Function Name  : RTC_GetDate
* Description    : Wrapper for AKC Software
* Input          : struct of type RTC_DATE
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_GetDate(TTimeDate *dt)
{
	TTimeDate dt_now;
  uint32_t utime = RTC_GetCounter();
  TIMEDATE_CalcDateFromSeconds(utime, &dt_now);
  
	dt->Day			= dt_now.Day;
	dt->Month		= dt_now.Month;
	dt->Year		= dt_now.Year;
	//dt->weekday = (U8) now.tm_wday;
}

void RTC_GetDateTime(TTimeDate *dt)
{
  uint32_t utime = RTC_GetCounter();
  TIMEDATE_CalcDateFromSeconds(utime, dt);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
//------------------------------------------------------------------------------
// RTC_Configuration - Configures the RTC
//------------------------------------------------------------------------------
void RTC_Configuration(void)
{
  // Enable PWR and BKP clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  // Allow access to BKP Domain
  PWR_BackupAccessCmd(ENABLE);
  // Reset Backup Domain
  //BKP_DeInit();
  // Enable LSE
  RCC_LSEConfig(RCC_LSE_ON);
  // Wait till LSE is ready
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
  // Select LSE as RTC Clock Source
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  // Enable RTC Clock
  RCC_RTCCLKCmd(ENABLE);
  // Wait for RTC registers synchronization
  RTC_WaitForSynchro();
  // Wait until last write operation on RTC registers has finished
  RTC_WaitForLastTask();
  // Enable the RTC Second
  RTC_ITConfig(RTC_IT_SEC, ENABLE);
  // Wait until last write operation on RTC registers has finished
  RTC_WaitForLastTask();
  // Set RTC prescaler: set RTC period to 1sec
  RTC_SetPrescaler(32767); // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
  // Wait until last write operation on RTC registers has finished 
  RTC_WaitForLastTask();
}

//------------------------------------------------------------------------------
// RTC_Init - initialize internal RTC
//------------------------------------------------------------------------------
void RTC_Init(void)
{
  RTC_Configuration();
  
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    // Backup data register value is not correct or not yet programmed (when the first time the program is executed)    
		RTC_Set_DefaultTimeDate();
		RTC_Configuration();
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);				// Adjust time by values entered by the user on the hyperterminal
  } else 
  {
    RTC_WaitForSynchro();														// Wait for RTC registers synchronization
		RTC_WaitForLastTask();													// Wait until last write operation on RTC registers has finished
		RCC_ClearFlag();																// Clear reset flags
  }
}
