#include "spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

//------------------------------------------------------------------------------
// SPI_Initialize - 
//------------------------------------------------------------------------------
void SPI_Initialize(uint8_t Num, uint8_t Mode, uint16_t Prescaler)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_TypeDef* SPIx;
  uint16_t SPI_InPins;
  uint16_t SPI_OutPins;
  GPIO_TypeDef* GPIOx;
  
  if (Num>=SPI_NUM_LAST)
    return;
  
  if (Mode>=SPI_MODE_LAST)
    return;

  SPI_I2S_DeInit(SPIx);

  switch (Num)
  {
    case SPI_NUM_1:
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
      SPIx = SPI1;
      SPI_OutPins = GPIO_Pin_5 | GPIO_Pin_7;  //GPIO_Pin_3 | GPIO_Pin_5
      SPI_InPins = GPIO_Pin_6;                //GPIO_Pin_4
      GPIOx = GPIOA;                          //GPIOB
      break;
    case SPI_NUM_2:
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
      SPIx = SPI2;
      SPI_OutPins = GPIO_Pin_13 | GPIO_Pin_15;
      SPI_InPins = GPIO_Pin_14;
      GPIOx = GPIOB;
      break;
  }

	GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_OutPins;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOx, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI_InPins;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOx, &GPIO_InitStructure);
  
	SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  switch (Mode)
  {
    case SPI_MODE_0:
      SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
      SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
      break;
    case SPI_MODE_1:
      SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
      SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
      break;
    case SPI_MODE_2:
      SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
      SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
      break;
    case SPI_MODE_3:
      SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
      SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
      break;
  }
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = Prescaler;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPIx, &SPI_InitStructure);
	
	SPI_CalculateCRC(SPIx, DISABLE);

	SPI_Cmd(SPIx, ENABLE);
}

//------------------------------------------------------------------------------
// SPI_ReadData - read data from SPI
//------------------------------------------------------------------------------
void SPI_ReadData(uint8_t Num, uint8_t* Data, uint16_t Len)
{
  SPI_TypeDef* SPIx;
  
  if (Num>=SPI_NUM_LAST)
    return;
  
  if (Len==0)
    return;

  switch (Num)
  {
    case SPI_NUM_1:
      SPIx = SPI1;
      break;
    case SPI_NUM_2:
      SPIx = SPI2;
      break;
  }

  while (Len--)
  {
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE)==RESET);
    SPI_I2S_SendData (SPIx, SPI_DUMMY_BYTE);
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE)==RESET);
    *Data++ = SPI_I2S_ReceiveData(SPIx);
  }
}

//------------------------------------------------------------------------------
// SPI_WriteData - 
//------------------------------------------------------------------------------
void SPI_WriteData(uint8_t Num, uint8_t* Data, uint16_t Len)
{
  SPI_TypeDef* SPIx;
  
  if (Num>=SPI_NUM_LAST)
    return;
  
  if (Len==0)
    return;

  switch (Num)
  {
    case SPI_NUM_1:
      SPIx = SPI1;
      break;
    case SPI_NUM_2:
      SPIx = SPI2;
      break;
  }
  
  while (Len--)
  {
    SPI_I2S_SendData(SPIx, *Data++);
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE)==RESET);
  }
}
