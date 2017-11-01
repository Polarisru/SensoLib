#include <stdio.h>
#include <string.h>
#include "bt.h"
#include "const.h"
#include "global.h"
#include "rs232.h"
#include "rtos.h"
#include "stm32f10x_gpio.h"
#include "usb2com.h"
#include "utils.h"
#include "version.h"

int BT_Port;

const char BT_CMD_TIME[] = "TIME";
const char BT_CMD_VER[] = "VER";

const char BT_INP_TIME[] = "%d:%d";

const char BT_ANS_OK[] = "Ok";
const char BT_ANS_VER[] = "V%02d.%02d";

const char BT_ERR_1[] = "E1";
const char BT_ERR_2[] = "E2";

const char BT_END_STR[] = "\r\n";

char BT_Buffer[64];

//------------------------------------------------------------------------------
// BT_Send - send command to bluetooth device
//------------------------------------------------------------------------------
void BT_Send(char *cmd)
{
  int len;
  
  strcpy(BT_Buffer, cmd);
  len = strlen(BT_Buffer);
  
  if (len>0)
    RS232_SendData(BT_Port, BT_Buffer, len);
}

//------------------------------------------------------------------------------
// BT_Parse - parse received string
//------------------------------------------------------------------------------
void BT_Parse(void)
{
  const char *errMsg;
  char buf[32];
  uint16_t intVal0, intVal1;
  
  errMsg = NULL;
  
  UTILS_StrUpr(BT_Buffer);
  if (strncmp(BT_Buffer, BT_CMD_TIME, strlen(BT_CMD_TIME))==0)
  {
    if (2!=sscanf(&BT_Buffer[strlen(BT_CMD_TIME)], BT_INP_TIME, &intVal0, &intVal1))
    {
      errMsg = BT_ERR_2;
    } else
    {
      globalTimeDate.Hour = intVal0;
      globalTimeDate.Minute = intVal1;
      strcpy(buf, BT_ANS_OK);
    }
  } else
  if (strncmp(BT_Buffer, BT_CMD_VER, strlen(BT_CMD_VER))==0)
  {
    sprintf(buf, BT_ANS_VER, DEVICE_VERSION[0], DEVICE_VERSION[1]);
  } else
  {
    errMsg = BT_ERR_1;
  }
  if (errMsg)
  {
    //strcpy((char*)USB_Tx_Buffer, errMsg);
    strcpy(buf, errMsg);
  }
  //strcat((char*)USB_Tx_Buffer, USB2COM_END_STR);
  strcat(buf, BT_END_STR);
  //USB_Tx_length = strlen((char*)USB_Tx_Buffer);
  BT_Send(buf);
}

//------------------------------------------------------------------------------
// BT_Task
//------------------------------------------------------------------------------
//__task void BT_Task(void)
void BT_Task(void *pvParameters)
{
  for (;;)
  {
	  //os_evt_wait_or(0x0001, 0xffff);	// wait for reply command
    //osSignalWait(0x0001, osWaitForever);
    xSemaphoreTake(semBT, portMAX_DELAY);
    
    strcpy(BT_Buffer, RS232_Buffer[BT_Port]);
    if (globalDeviceStatus.debugMode==DEBUG_MODE_BT)
      USB2COM_Send(BT_Buffer);
    BT_Parse();
  }
}

//------------------------------------------------------------------------------
// BT_Callback - callback function for RS232 driver
//------------------------------------------------------------------------------
void BT_Callback(void)
{
  if (globalOSStarted)
    //isr_evt_set(0x0001, t_bt);
 	  xSemaphoreGiveFromISR(semBT, NULL);
}

//------------------------------------------------------------------------------
// BT_Init - Bluetooth init
//------------------------------------------------------------------------------
void BT_Init(int port)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RS232_InitStructDef RS232_InitStructure;

  if (port>=USART_LAST)
    return;
	
	GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  BT_PIN_EN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BT_EN_GPIO, &GPIO_InitStructure);
  GPIO_WriteBit(BT_EN_GPIO, BT_PIN_EN, Bit_SET);

  RS232_InitStructure.Baudrate = 9600;
  RS232_InitStructure.Mode = RS232_MODE_DUPLEX;
  RS232_InitStructure.EndSign = CHAR_CR;
  RS232_InitStructure.EscSign = CHAR_ESC;
  RS232_InitStructure.Error = 0;
  RS232_Init(port, &RS232_InitStructure, BT_Callback);
  
  BT_Port = port;
  
}

//------------------------------------------------------------------------------
// BT_SendCmd - send command to bluetooth device
//------------------------------------------------------------------------------
void BT_SendCmd(char *cmd)
{
  int len;
  
  strcpy(BT_Buffer, cmd);
  strcat(BT_Buffer, BT_END_STR);
  len = strlen(BT_Buffer);
  
  if (len>0)
    RS232_SendData(BT_Port, BT_Buffer, len);
}

//------------------------------------------------------------------------------
// BT_Switch - switch bluetooth module on/off
//------------------------------------------------------------------------------
void BT_Switch(int On)
{
  if (On)
    GPIO_WriteBit(BT_EN_GPIO, BT_PIN_EN, Bit_SET);
  else
    GPIO_WriteBit(BT_EN_GPIO, BT_PIN_EN, Bit_RESET);
}
