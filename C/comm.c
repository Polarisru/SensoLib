#include "bmp180.h"
#include "bt.h"
#include "comm.h"
#include "const.h"
#include "ds3231.h"
#include "ff.h"
#include "global.h"
#include "hmc5883l.h"
#include "mpu6050.h"
#include "rtc.h"
#include "rtos.h"
#include "tm1637.h"

FATFS FAT_Sd;
const char PATH_Sd[] = "0:";
const char MAIN_File[] = "0:test.txt";

//------------------------------------------------------------------------------
// COMM_Init - init communication module
//------------------------------------------------------------------------------
void COMM_Init(void)
{
}

//------------------------------------------------------------------------------
// COMM_Task - communication RTOS task
//------------------------------------------------------------------------------
//__task void COMM_Task(void)
//void COMM_Task(void const *arg)
void COMM_Task(void *pvParameters)
{
  TTimeDate dt, dt1;
  uint8_t Point = TM1637_POINT_ON;
  uint8_t ModeCounter = 0; 
  uint32_t free_cl;
  FATFS *fs;
  FIL   FILE_Dest;
  FRESULT res;
  UINT bw;
  
  /*os_dly_wait(50);
  BT_SendCmd("AT+NAMEHryu\r\n");
  os_dly_wait(50);
  BT_SendCmd("AT+TYPE1\r\n");
  os_dly_wait(50);
  BT_SendCmd("AT+ROLE0\r\n");
  os_dly_wait(50);
  BT_SendCmd("AT+START\r\n");
  //BT_Switch(0);*/
  
  //vTaskSetApplicationTaskTag(NULL, (void *)2);

  f_mount(&FAT_Sd, PATH_Sd, 1);
  fs = &FAT_Sd;
  if (f_getfree(PATH_Sd, (DWORD*)&free_cl, &fs)!=FR_OK)
  {
    // SD card not properly mounted
    free_cl++;
  }
  res = f_open(&FILE_Dest, MAIN_File, FA_CREATE_ALWAYS | FA_WRITE);
  if (res==FR_OK)
  {
    bw = f_puts("Lesha durak\r\nKurit tabak\r\n", &FILE_Dest);
  }    
  f_close(&FILE_Dest);

  //DS3231_GetDateTime(&globalTimeDate);
  RTC_GetDateTime(&dt1);
  RTC_SetDateTime(&globalTimeDate);
  
  for (;;)
  {
    //os_dly_wait(1000);
    //osDelay(1000);
    //os_evt_wait_or(0x0001, 1000);	// wait for reply command
    //osSignalWait(0x0001, 1000);
    xSemaphoreTake(semCOMM, 1000);
    
    dt = globalTimeDate;
    
    HMC5883L_Read();
    MPU6050_Read();
    //BMP180_Read();
    //RTC_GetDateTime(&dt1);
    //DS3231_GetDateTime(&dt);
    
    switch (globalDisplayMode)
    {
      case DISPLAY_MODE_TIME:
        ModeCounter = 0;
        TM1637_SetPoint(Point);
        if (Point==TM1637_POINT_ON)
          Point = TM1637_POINT_OFF;
        else
          Point = TM1637_POINT_ON;
        TM1637_Display(0, dt.Hour/10);
        TM1637_Display(1, dt.Hour%10);
        TM1637_Display(2, dt.Minute/10);
        TM1637_Display(3, dt.Minute%10);
        break;
      case DISPLAY_MODE_DATE:
        TM1637_SetPoint(TM1637_POINT_ON);
        TM1637_Display(0, dt.Day/10);
        TM1637_Display(1, dt.Day%10);
        TM1637_Display(2, dt.Month/10);
        TM1637_Display(3, dt.Month%10);
        break;
      case DISPLAY_MODE_YEAR:
        TM1637_SetPoint(TM1637_POINT_OFF);
        TM1637_Display(0, 2);
        TM1637_Display(1, 0);
        TM1637_Display(2, dt.Year/10);
        TM1637_Display(3, dt.Year%10);
        break;
    }
    ModeCounter++;
    if (ModeCounter>3)
    {
      ModeCounter = 0;
      globalDisplayMode = DISPLAY_MODE_TIME;
    }  
  }
}
