#include <stdio.h>
#include <string.h>
#include "bt.h"
#include "global.h"
#include "platform.h"
#include "rtos.h"
#include "usb2com.h"
#include "utils.h"
#include "version.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_endp.h"
//#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_istr.h"

#include "stm32f10x_exti.h"

uint8_t USB_Data_Received_Count;    /* Number of Bytes received from Virtual COM */
uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];

uint8_t  USB_Tx_State = 0;

char USB2COM_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
uint8_t USB2COM_Index;

const char USB2COM_CMD_AT[] = "AT";
const char USB2COM_CMD_ID[] = "ID";
const char USB2COM_CMD_VER[] = "VER";
const char USB2COM_CMD_DGPS[] = "DGPS";
const char USB2COM_CMD_DBT[] = "DBT";

const char USB2COM_ANS_OK[] = "Ok";
const char USB2COM_ANS_VER[] = "V%02d.%02d";

const char USB2COM_ERR_1[] = "E1";

const char USB2COM_END_STR[] = "\r";

//------------------------------------------------------------------------------
// USB_LP_IRQHandler - USB receive interrupt
//------------------------------------------------------------------------------
void USB_LP_CAN1_RX0_IRQHandler(void) 
{
  USB_Istr();	
}

void USB_FS_WKUP_IRQHandler(void)
{
  //EXTI_ClearITPendingBit(EXTI_Line18);
}

//------------------------------------------------------------------------------
// USB2COM_Init - init USB communication module
//------------------------------------------------------------------------------
void USB2COM_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;   
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  // Enable the USB Wake-up interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  
  USB2COM_Index = 0;
  
	USB_Init();
}

//------------------------------------------------------------------------------
// USB2CON_Send - send string to USB
//------------------------------------------------------------------------------
void USB2COM_Send(char *Buff)
{
  strcpy((char*)USB_Tx_Buffer, Buff);
  USB_Tx_length = strlen((char*)USB_Tx_Buffer);
}

//------------------------------------------------------------------------------
// USB2COM_Parse - parse received string
//------------------------------------------------------------------------------
void USB2COM_Parse(void)
{
  const char *errMsg;
  char buf[32];
  
  errMsg = NULL;
  
  UTILS_StrUpr(USB2COM_Buffer);
  if (strncmp(USB2COM_Buffer, USB2COM_CMD_AT, strlen(USB2COM_CMD_AT))==0)
  {
    //AT command, send to RS interface 
    BT_SendCmd(USB2COM_Buffer);
    strcpy(buf, USB2COM_ANS_OK);
    //strcpy((char*)USB_Tx_Buffer, USB2COM_ANS_OK);
    //USB_Tx_length = strlen((char*)USB_Tx_Buffer);
  } else
  if (strcmp(USB2COM_Buffer, USB2COM_CMD_ID)==0)
  {
    //strcpy((char*)USB_Tx_Buffer, USB2COM_ANS_ID);
    strcpy(buf, DEVICE_NAME);
  } else
  if (strcmp(USB2COM_Buffer, USB2COM_CMD_VER)==0)
  {
    //sprintf((char*)USB_Tx_Buffer, USB2COM_ANS_VER, DEVICE_VERSION[0], DEVICE_VERSION[1]);
    sprintf(buf, USB2COM_ANS_VER, DEVICE_VERSION[0], DEVICE_VERSION[1]);
  } else
  if (strcmp(USB2COM_Buffer, USB2COM_CMD_DGPS)==0)
  {
    globalDeviceStatus.debugMode = DEBUG_MODE_GPS;
    strcpy(buf, USB2COM_ANS_OK);
  } else
  if (strcmp(USB2COM_Buffer, USB2COM_CMD_DBT)==0)
  {
    globalDeviceStatus.debugMode = DEBUG_MODE_BT;
    strcpy(buf, USB2COM_ANS_OK);
  } else
  {
    errMsg = USB2COM_ERR_1;
  }
  if (errMsg)
  {
    //strcpy((char*)USB_Tx_Buffer, errMsg);
    strcpy(buf, errMsg);
  }
  //strcat((char*)USB_Tx_Buffer, USB2COM_END_STR);
  strcat(buf, USB2COM_END_STR);
  //USB_Tx_length = strlen((char*)USB_Tx_Buffer);
  USB2COM_Send(buf);
}

//------------------------------------------------------------------------------
// USB2COM_Task - USB communication RTOS task
//------------------------------------------------------------------------------
//__task void USB2COM_Task(void)
//void USB2COM_Task(void const *arg)
void USB2COM_Task(void *pvParameters)
{
  uint8_t count;
  //uint8_t flag;
  
  for (;;)
  {
    //os_evt_wait_or(0x0001, 0xffff);
    //osSignalWait(0x0001, osWaitForever);
    xSemaphoreTake(semUSB2COM, portMAX_DELAY);
    
    for (count=0; count<USB_Data_Received_Count; count++)
    {
      switch (USB_Rx_Buffer[count])
      {
        case '\x1b':
          //clear buffer
          USB2COM_Index = 0;
          break;
        case '\r':
          //process string
          USB2COM_Buffer[USB2COM_Index] = 0;
          USB2COM_Index = 0;            
          USB2COM_Parse();
          break;
        case '\n':
          //ignore
          break;
        default:
          //normal char
          USB2COM_Buffer[USB2COM_Index++] = USB_Rx_Buffer[count];
      }
    }
    /*os_dly_wait(10);
    if (USB_Data_Received_Flag)
    {
      flag = false;
      USB_Data_Received_Flag = 0;
      for (count=0; count<USB_Data_Received_Count; count++)
        if (USB_Rx_Buffer[count]=='\r')
        {  
          flag = true;
          break;
        }
      if (flag)
      {
        strcpy(USB_Tx_Buffer, "Ok\r");
        USB_Tx_length = 3;
      }
    }*/
  }
}
