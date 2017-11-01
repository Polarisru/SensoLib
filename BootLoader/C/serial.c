#include "serial.h"

//------------------------------------------------------------------------------
// SERIAL_Init - initialize UART
//------------------------------------------------------------------------------
void SERIAL_Init()
{
  // turn on power for USART1 and GPIOA
  RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
  // configure pins
  GPIOA->CRH = 0x000004B0;
  //GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9);
  // configure baudrate
  USART1->BRR = ((SystemCoreClock + SERIAL_BAUDRATE/2)/SERIAL_BAUDRATE);
  // USART1 enable for tx/rx
  USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
  // 1 stop bit
  USART1->CR2 &= ~USART_CR2_STOP;
  // disable USART DMA
  USART1->CR3 = 0;
  
  /*
  GPIOA->CRH &= ~GPIO_CRH_MODE9;
  GPIOA->CRH &= ~GPIO_CRH_CNF9;
  GPIOA->CRH &= ~GPIO_CRH_MODE10;
  GPIOA->CRH &= ~GPIO_CRH_CNF10;
  GPIOA->CRH |= GPIO_CRH_MODE9_1; // ????? ?? 2 ???
  GPIOA->CRH |= GPIO_CRH_CNF9_1;
  GPIOA->CRH |= GPIO_CRH_CNF10_0;

  // 12 ???/16*115200 = 6.5
  USART1->BRR = (6 << 4) + 8;*/
  
}


//------------------------------------------------------------------------------
// SERIAL_SendChar - send one char
//------------------------------------------------------------------------------
void SERIAL_SendChar(char c)
{
  USART1->DR = c;

  // wait for TX
  while ((USART1->SR & USART_SR_TXE) == 0);
}


//------------------------------------------------------------------------------
// SERIAL_GetChar - receive one char
//------------------------------------------------------------------------------
char SERIAL_GetChar(void)
{
  // Wait until the data is ready to be received
  while ((USART1->SR & USART_SR_RXNE) == 0);

  // read RX data, combine with DR mask
  return (char)(USART1->DR & 0xFF);
}
