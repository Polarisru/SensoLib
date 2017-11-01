#include <string.h>
#include "global.h"
#include "rs232.h"
#include "rtos.h"
#include "sim800.h"

uint8_t SIM800_PortNum;

//------------------------------------------------------------------------------
// SIM800_Callback - callback function for RS232 driver
//------------------------------------------------------------------------------
void SIM800_Callback(void)
{
  if (globalOSStarted)
    //isr_evt_set(0x0001, t_bt);
 	  xSemaphoreGiveFromISR(semSIM, NULL);
}

//------------------------------------------------------------------------------
// SIM800_Init - initialize GSM module with SIM800
//------------------------------------------------------------------------------
void SIM800_Init(uint8_t port)
{
  RS232_InitStructDef RS232_InitStructure;

  if (port>=USART_LAST)
    return;
  
  RS232_InitStructure.Baudrate = SIM800_BAUDRATE;
  RS232_InitStructure.Mode = RS232_MODE_DUPLEX;
  RS232_InitStructure.EndSign = CHAR_CR;
  RS232_InitStructure.EscSign = CHAR_ESC;
  RS232_InitStructure.Error = 0;
  RS232_Init(port, &RS232_InitStructure, SIM800_Callback);
  
  SIM800_PortNum = port;
}

//------------------------------------------------------------------------------
// SIM800_SendCmd - send command to SIM800 module
//------------------------------------------------------------------------------
void SIM800_SendCmd(char *buf)
{
  int len = strlen(buf);
  
  RS232_SendData(SIM800_PortNum, buf, len);
}

//------------------------------------------------------------------------------
// SIM800_WaitReply - wait for reply from SIM800 module
//------------------------------------------------------------------------------
void SIM800_WaitReply(char *buf, uint32_t Timeout)
{
  
}
