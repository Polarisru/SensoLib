#ifndef _RTOS_H
#define _RTOS_H

//#define osObjectsExternal

//#include <RTL.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "Platform.h"
//#include "cmsis_os.h"

/*typedef struct
{
  int Type;
  char *Buffer;
} TParsItem;*/

/*extern OS_TID t_adc;         // ADC task
extern OS_TID t_comm;        // Communication task
extern OS_TID t_gps;         // GPS task
extern OS_TID t_bt;          // Bluetooth task
extern OS_TID t_sensors;		 // Sensors task
extern OS_TID t_usb2com;     // USB communication*/

/*extern osThreadId t_adc;         // ADC task
extern osThreadId t_comm;        // Communication task
extern osThreadId t_gps;         // GPS task
extern osThreadId t_sensors;		 // Sensors task
extern osThreadId t_usb2com;     // USB communication*/

//FreeRTOS
extern volatile uint16_t evtCOMM;

extern xSemaphoreHandle semADC;
extern xSemaphoreHandle semBT;
extern xSemaphoreHandle semGPS;
extern xSemaphoreHandle semCOMM;
extern xSemaphoreHandle semUSB2COM;

extern xTaskHandle t_adc;					//ADC task
extern xTaskHandle t_comm;			  //communication task
extern xTaskHandle t_gps;					//GPS task
extern xTaskHandle t_kbd;         //keyboard task
extern xTaskHandle t_bt;			    //Bluetooth task
extern xTaskHandle t_sensors;			//sensors task
extern xTaskHandle t_usb2com;		  //USB communication task

#endif
