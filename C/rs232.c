#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "global.h"
#include "rs232.h"
#include "rtos.h"
#include "stm32f10x_usart.h"

char RS232_Buffer[USART_LAST][RS232_BUFFER_SIZE];
char RS232_TxBuffer[USART_LAST][RS232_BUFFER_SIZE];

uint8_t RS232_IHead[USART_LAST];

//typedef void (*func_type)(void);
void (*RS232_Callback[USART_LAST])();
//func_type RS232_Callback[USART_LAST];
RS232_InitStructDef RS232_InitStructures[USART_LAST];

//------------------------------------------------------------------------------
// RS232_ResetRx - reset input buffer
//------------------------------------------------------------------------------
void RS232_ResetRx(int Num)
{
  if (Num>=USART_LAST)
    return;

  RS232_IHead[Num] = 0;
}
  
//------------------------------------------------------------------------------
// RS232_SendData - send serial data from buffer
//------------------------------------------------------------------------------
void RS232_SendData(int Num, char *buf, int len)
{
	DMA_InitTypeDef DMA_InitStructure;
  //USART_TypeDef* USART_Num;
  DMA_Channel_TypeDef* USART_DmaChannel;
  uint32_t DMA_PeriphBaseAddr;

  if (Num>=USART_LAST)
    return;
  
  switch (Num)
  {
    case USART_1:
      //USART_Num = USART1;
      USART_DmaChannel = USART1_Tx_DMA_Channel;
      DMA_PeriphBaseAddr = USART1_DR_Base;
      break;
    case USART_2:
      //USART_Num = USART2;
      USART_DmaChannel = USART2_Tx_DMA_Channel;
      DMA_PeriphBaseAddr = USART2_DR_Base;
      break;
    case USART_3:
      //USART_Num = USART3;
      USART_DmaChannel = USART3_Tx_DMA_Channel;
      DMA_PeriphBaseAddr = USART3_DR_Base;
      break;
    default:
      return;
  }
  
	while (USART_DmaChannel->CNDTR)
    //os_dly_wait(1);
    //osDelay(1);
    vTaskDelay(1);
  //while (DMA_GetFlagStatus(USART1_Tx_DMA_FLAG) == RESET)
	//	continue;
	
  if (len)
  {
    //os_evt_clr(0x0001, t_parser);
		memcpy(RS232_TxBuffer[Num], buf, len);
		
		// USART1 TX DMA1 Channel (triggered by USART1 Tx event) Config
		DMA_DeInit(USART_DmaChannel);  
		DMA_StructInit(&DMA_InitStructure);
		DMA_InitStructure.DMA_PeripheralBaseAddr = DMA_PeriphBaseAddr;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RS232_TxBuffer[Num];
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_BufferSize = len;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(USART_DmaChannel, &DMA_InitStructure);
	  DMA_Cmd(USART_DmaChannel, ENABLE);
  }
}

//------------------------------------------------------------------------------
// RS232_Init - init external connection
//------------------------------------------------------------------------------
void RS232_Init(int num, RS232_InitStructDef* Configuration, void (*callback)(void))
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
  uint32_t USART_Clk;
	uint16_t USART_RxPin;
	uint16_t USART_TxPin;
  GPIO_TypeDef* USART_Gpio;
  USART_TypeDef* USART_Num;
  uint8_t USART_IntChannel;
  
  if (num>=USART_LAST)
    return;
  
  switch (num)
  {
    case USART_1:
      USART_Clk = USART1_CLK;
      USART_RxPin = USART1_RxPin;
      USART_TxPin = USART1_TxPin;
      USART_Gpio = USART1_GPIO;
      USART_Num = USART1;
      USART_IntChannel = USART1_IRQn;
      // Enable clock
      RCC_APB2PeriphClockCmd(USART_Clk, ENABLE); 
      break;
    case USART_2:
      USART_Clk = USART2_CLK;
      USART_RxPin = USART2_RxPin;
      USART_TxPin = USART2_TxPin;
      USART_Gpio = USART2_GPIO;
      USART_Num = USART2;
      USART_IntChannel = USART2_IRQn;
      // Enable clock
      RCC_APB1PeriphClockCmd(USART_Clk, ENABLE); 
      break;
    case USART_3:
      USART_Clk = USART3_CLK;
      USART_RxPin = USART3_RxPin;
      USART_TxPin = USART3_TxPin;
      USART_Gpio = USART3_GPIO;
      USART_Num = USART3;
      USART_IntChannel = USART3_IRQn;
      // Enable clock
      RCC_APB1PeriphClockCmd(USART_Clk, ENABLE); 
      break;
    default:
      return;
  }
  
	RS232_IHead[num] = 0;
	
  //init USART
	GPIO_StructInit(&GPIO_InitStructure);
  // Configure USART1 Rx as input floating
  GPIO_InitStructure.GPIO_Pin = USART_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(USART_Gpio, &GPIO_InitStructure);
  // Configure USART1 Tx as alternate function push-pull
  GPIO_InitStructure.GPIO_Pin = USART_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(USART_Gpio, &GPIO_InitStructure);

	// Enable the USART1 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART_IntChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

  USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = Configuration->Baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  switch (Configuration->Mode)
  {
    case RS232_MODE_DUPLEX:
      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
      break;
    case RS232_MODE_RECEIVE:
      USART_InitStructure.USART_Mode = USART_Mode_Rx;
      break;
    case RS232_MODE_TRANSMIT:
      USART_InitStructure.USART_Mode = USART_Mode_Tx;
      break;
  }
  USART_Init(USART_Num, &USART_InitStructure);
		
  // Enable USART Receive interrupt
	USART_ITConfig(USART_Num, USART_IT_RXNE, ENABLE);
  // Enable the USART
  USART_Cmd(USART_Num, ENABLE);	
  
	// Enable USART DMA Tx request
  if (Configuration->Mode!=RS232_MODE_RECEIVE)
    USART_DMACmd(USART_Num, USART_DMAReq_Tx, ENABLE);
  
  RS232_InitStructures[num] = *Configuration;
  //if (callback!=NULL)
    RS232_Callback[num] = callback;
}

//------------------------------------------------------------------------------
// USART1_IRQHandler - RS232 interrupt handler
//------------------------------------------------------------------------------
void USART1_IRQHandler(void) 
{
	char c;
	
  if (USART_GetITStatus(USART1, USART_IT_RXNE)) 
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		c = USART_ReceiveData(USART1);
		
    if (c==CHAR_NL)
      return;
				
    if (c==RS232_InitStructures[USART_1].EscSign)
    {
      RS232_IHead[USART_1] = 0;
    }
		
    RS232_Buffer[USART_1][RS232_IHead[USART_1]++] = c;

    if (c==RS232_InitStructures[USART_1].EndSign)
    {
      RS232_Buffer[USART_1][RS232_IHead[USART_1]] = 0;
			RS232_IHead[USART_1] = 0;
      if (RS232_Callback[USART_1]!=NULL)
        RS232_Callback[USART_1]();
		}
  }

}

//------------------------------------------------------------------------------
// RS232_SendChar - send a char to RS232
//------------------------------------------------------------------------------
void RS232_SendChar(int Num, char data)
{                
  USART_TypeDef* USART_Num;

  if (Num>=USART_LAST)
    return;

  switch (Num)
  {
    case USART_1:
      USART_Num = USART1;
      break;
    case USART_2:
      USART_Num = USART2;
      break;
    case USART_3:
      USART_Num = USART3;
      break;
    default:
      return;
  }

	USART_SendData(USART_Num, data);
  //wait for transmition to end
}
