#ifndef _RS232_H
#define _RS232_H

#include "defines.h"

#define RS232_BUFFER_SIZE					(256)

#define RS232_BAUDRATE	        	(9600L)

enum
{
  USART_1,
  USART_2,
  USART_3,
  USART_LAST
};

enum
{
  RS232_MODE_DUPLEX,
  RS232_MODE_RECEIVE,
  RS232_MODE_TRANSMIT
};

#define USART1_GPIO              	GPIOA
#define USART1_CLK               	RCC_APB2Periph_USART1
#define USART1_RxPin             	GPIO_Pin_10
#define USART1_TxPin             	GPIO_Pin_9
#define USART1_IRQn              	USART1_IRQn
#define USART1_IRQHandler        	USART1_IRQHandler
#define USART1_DR_Base           	0x40013804
#define USART1_Tx_DMA_Channel			DMA1_Channel4
#define USART1_Tx_DMA_FLAG				DMA1_FLAG_TC4

#define USART2_GPIO              	GPIOA
#define USART2_CLK               	RCC_APB1Periph_USART2
#define USART2_RxPin             	GPIO_Pin_3
#define USART2_TxPin             	GPIO_Pin_2
#define USART2_IRQn              	USART2_IRQn
#define USART2_IRQHandler        	USART2_IRQHandler
#define USART2_DR_Base           	0x40004404
#define USART2_Tx_DMA_Channel			DMA1_Channel7
#define USART2_Tx_DMA_FLAG				DMA1_FLAG_TC7

#define USART3_GPIO              	GPIOB
#define USART3_CLK               	RCC_APB1Periph_USART3
#define USART3_RxPin             	GPIO_Pin_11
#define USART3_TxPin             	GPIO_Pin_10
#define USART3_IRQn              	USART3_IRQn
#define USART3_IRQHandler        	USART3_IRQHandler
#define USART3_DR_Base           	0x40004804
#define USART3_Tx_DMA_Channel			DMA1_Channel2
#define USART3_Tx_DMA_FLAG				DMA1_FLAG_TC2

#define CHAR_CR 		  0x0D
#define CHAR_NL			  0x0A
#define CHAR_ESC 		  0x1B
#define CHAR_START    '$'

typedef struct {
  uint32_t  Baudrate;         // Baudrate
  uint8_t   Mode;             // port working mode
  uint8_t   EndSign;          // sign to mark packet end
  uint8_t   EscSign;          // sign to reset receiving buffer
	uint8_t		Error;					  // if port read or write fails
} RS232_InitStructDef __attribute__ ((aligned (4)));

//RS-232 buffer
extern char RS232_Buffer[USART_LAST][RS232_BUFFER_SIZE];
extern uint8_t RS232_IHead[USART_LAST];

// RS232 init
extern void RS232_Init(int Num, RS232_InitStructDef* Configuration, void (*callback)(void));
// send serial data from buffer
extern void RS232_SendData(int Num, char *buf, int len);
// put char
extern void RS232_SendChar(int Num, char data);

#endif
