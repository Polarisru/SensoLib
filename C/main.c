#include <stdio.h>
#include "adc.h"
#include "bmp180.h"
#include "bt.h"
#include "comm.h"
#include "const.h"
#include "defines.h"
#include "ds3231.h"
#include "eeprom.h"
#include "ff.h"
#include "global.h"
#include "gps.h"
#include "hmc5883l.h"
#include "i2c.h"
#include "kbd.h"
#include "m24xx.h"
#include "mpu6050.h"
#include "platform.h"
#include "rs232.h"
#include "rtc.h"
#include "rtos.h"
#include "sdspi.h"
//#include "sensors.h"
#include "timer.h"
#include "tm1637.h"
#include "usb2com.h"
#include "version.h"

//FreeRTOS
char xTasksArray[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

xTaskHandle t_adc = NULL;
xTaskHandle t_comm = NULL;
xTaskHandle t_gps = NULL;
xTaskHandle t_kbd = NULL;
xTaskHandle t_bt = NULL;
xTaskHandle t_sensors = NULL;
xTaskHandle t_usb2com = NULL;

volatile uint16_t evtCOMM;

xSemaphoreHandle semADC;
xSemaphoreHandle semBT;
xSemaphoreHandle semGPS;
xSemaphoreHandle semCOMM;
xSemaphoreHandle semUSB2COM;

void setTasksArray(uint8_t val, uint8_t num)
{
  if (num>=10)
    return;
  xTasksArray[num] = val;
}

/*OS_TID t_adc;         // ADC task
OS_TID t_comm;        // Communication task
OS_TID t_gps;         // GPS task
OS_TID t_sensors;		  // Sensors task
OS_TID t_kbd;		      // Keyboard task
OS_TID t_usb2com;     // USB communication
OS_TID t_bt;          // Bluetooth task*/

/*osThreadId t_adc;         // ADC task
osThreadId t_comm;        // Communication task
osThreadId t_gps;         // GPS task
osThreadId t_sensors;		  // Sensors task
osThreadId t_kbd;		      // Keyboard task
osThreadId t_usb2com;     // USB communication*/

/*static char stkCOMM[3072]; 
static char stkGPS[1024];
static char stkBT[1024];
static char stkUSB2COMM[1024];*/

//__task void init(void);              // Function prototypes

//------------------------------------------------------------------------------
void RCC_Configuration(void)
{
  // PCLK2 = HCLK/2
  RCC_PCLK2Config(RCC_HCLK_Div2); 
	
	GPIO_AFIODeInit();
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
}

//------------------------------------------------------------------------------
void NVIC_Configuration(void)
{
  //NVIC_InitTypeDef NVIC_InitStructure;
	
}

//------------------------------------------------------------------------------
void GPIO_Configuration(void)
{
	
}

//------------------------------------------------------------------------------
// DeviceInit - init the device
//------------------------------------------------------------------------------
void DeviceInit(void)
{
  //U32 SWOPrescaler;
  //U32 SWOSpeed;

	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	
  /*SWOSpeed = 1161290;
  *((volatile unsigned *)0xE000EDFC) = 0x01000000;   // "Debug Exception and Monitor Control Register (DEMCR)"
  *((volatile unsigned *)0xE0042004) = 0x00000027;
  *((volatile unsigned *)0xE00400F0) = 0x00000002;   // "Selected PIN Protocol Register": Select which protocol to use for trace output (2: SWO)
  SWOPrescaler = (72000000 / SWOSpeed) - 1;  // SWOSpeed in Hz
  *((volatile unsigned *)0xE0040010) = SWOPrescaler; // "Async Clock Prescaler Register". Scale the baud rate of the asynchronous output
  *((volatile unsigned *)0xE0000FB0) = 0xC5ACCE55;   // ITM Lock Access Register, C5ACCE55 enables more write access to Control Register 0xE00 :: 0xFFC
  *((volatile unsigned *)0xE0000E80) = 0x0001000D;   // ITM Trace Control Register
  *((volatile unsigned *)0xE0000E40) = 0x0000000F;   // ITM Trace Privilege Register
  *((volatile unsigned *)0xE0000E00) = 0x00000001;   // ITM Trace Enable Register. Enabled tracing on stimulus ports. One bit per stimulus port.
  *((volatile unsigned *)0xE0001000) = 0x400003FE;   // DWT_CTRL
  *((volatile unsigned *)0xE0040304) = 0x00000100;   // Formatter and Flush Control Register
  */
  
	//M24xx EEPROM init
	/*M24XX_Init();

  //read globals
  if (!EEPROM_CheckIntegrity())
    EEPROM_Restore();
  EEPROM_Check();*/
  
  //configure peripherals
  //USB device
  USB2COM_Init();
  //timers
  TIMER_Init();
  //RTC
  RTC_Init();
	//ADC
	ADC_Initialize();
  //Keyboard
  KBD_Init();
  //SD card
  SDSPI_Init();
  //RS232
  //RS232_Init(USART_1, 9600, 0);
  //I2C
  I2C_Initialize(I2C_NUM_1, 0x00, 400000);
  //HMC5883L
  HMC5883L_Init(I2C_NUM_1);
  //I2C
  I2C_Initialize(I2C_NUM_2, 0x00, 400000);
  //MPU6050
  MPU6050_Init(I2C_NUM_2);
  //BM180
  //BMP180_Init(I2C_NUM_1);
  //DS3231
  //DS3231_Init(I2C_NUM_1);
  //inductive sensors
  //SENSORS_Init();
  //Bluetooth
  BT_Init(USART_1);
  // GPS init
  //GPS_Init(USART_1);
  //TM1637 display
  TM1637_Init();
  //communication module
  COMM_Init();  
  //
}

//------------------------------------------------------------------------------
// initializing task
//------------------------------------------------------------------------------
/*__task void init(void) 
{
  t_adc = os_tsk_create(ADC_Task,  1); 				                                  // start ADC task
  t_comm = os_tsk_create_user(COMM_Task, 10, stkCOMM, sizeof(stkCOMM));         // start communication task
  t_gps = os_tsk_create_user(GPS_Task, 9, stkGPS, sizeof(stkGPS));              // start GPS task
  t_kbd = os_tsk_create(KBD_Task, 11);                                          // start Keyboard task
  t_bt = os_tsk_create_user(BT_Task, 9, stkBT, sizeof(stkBT));                  // start GPS task
  //t_sensors = os_tsk_create(SENSORS_Task, 2); 		                              // start Sensors task
  t_usb2com = os_tsk_create_user(USB2COM_Task, 2, stkUSB2COMM, sizeof(stkUSB2COMM)); // start USB communication task
  os_dly_wait(10);

  globalOSStarted = true;
    
  os_tsk_delete_self ();                 // stop init task (no longer needed)
}*/

//------------------------------------------------------------------------------
// main routine
//------------------------------------------------------------------------------
int main(void)
{ 
  //osKernelInitialize();
  
  globalOSStarted = false;
  globalDisplayMode = DISPLAY_MODE_TIME;
  globalDeviceStatus.debugMode = DEBUG_MODE_NONE;

  DeviceInit();
    
  TIMER_Start(TIMER_GENERAL, TO_500MSEC);
  while(!TIMER_TimeOut(TIMER_GENERAL));
  TIMER_Stop(TIMER_GENERAL);
  
  /*osThreadDef(ADC, ADC_Task, osPriorityLow, 0, 128);
  osThreadDef(COMM, COMM_Task, osPriorityHigh, 0, 3072);
  osThreadDef(GPS, GPS_Task, osPriorityHigh, 0, 1024);
  //osThreadDef(SENSORS_Task, osPriorityBelowNormal , 0, 128);
  osThreadDef(KBD, KBD_Task, osPriorityRealtime, 0, 128);
  osThreadDef(USB2COM, USB2COM_Task, osPriorityBelowNormal , 0, 1024);*/

  /*t_adc = osThreadCreate(osThread(ADC), NULL);
  t_comm = osThreadCreate(osThread(COMM), NULL);
  t_gps = osThreadCreate(osThread(GPS), NULL);
  t_kbd = osThreadCreate(osThread(KBD), NULL);
  //t_sensors = osThreadCreate(osThread(SENSORS), NULL);
  t_usb2com = osThreadCreate(osThread(USB2COM), NULL);*/
  
  //osDelay(10);
  
  //FreeRTOS
	semADC = xSemaphoreCreateCounting(1, 1);
	semBT = xSemaphoreCreateCounting(1, 1);
	semGPS = xSemaphoreCreateCounting(1, 1);
  semCOMM = xSemaphoreCreateCounting(1, 1);
  semUSB2COM = xSemaphoreCreateCounting(1, 1);

  xTaskCreate(ADC_Task, (const char*)"ADC", configMINIMAL_STACK_SIZE, NULL,  1 | portPRIVILEGE_BIT, &t_adc);
	xTaskCreate(COMM_Task, (const char*)"COMM", 3072/4, NULL,  10 | portPRIVILEGE_BIT, &t_comm);
  xTaskCreate(GPS_Task, (const char*)"GPS", 1024/4, NULL,  9 | portPRIVILEGE_BIT, &t_gps);
  xTaskCreate(KBD_Task, (const char*)"KBD", configMINIMAL_STACK_SIZE, NULL,  11 | portPRIVILEGE_BIT, &t_kbd);
  xTaskCreate(BT_Task, (const char*)"BT", 1024/4, NULL,  9 | portPRIVILEGE_BIT, &t_bt);
  //xTaskCreate(SENSORS_Task, (const char*)"SENSORS", configMINIMAL_STACK_SIZE, NULL,  2 | portPRIVILEGE_BIT, &t_sensors);
  xTaskCreate(USB2COM_Task, (const char*)"USB2COM", 1024/4, NULL,  2 | portPRIVILEGE_BIT, &t_usb2com);

  globalOSStarted = true;
  
	//os_sys_init(init);                     // init and start with task 'INIT'
  //osKernelStart();
  vTaskStartScheduler();

  //for (;;);
}
