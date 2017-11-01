#include "i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

//------------------------------------------------------------------------------
// I2C_Initialize - init I2C module
//------------------------------------------------------------------------------
void I2C_Initialize(int Num, int Address, int Speed)
{
  I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
  uint32_t RCC_BusPeriph;
  I2C_TypeDef* I2Cx;
  uint16_t I2C_Pins;
  
  if (Num>=I2C_NUM_LAST)
    return;
  
  switch (Num)
  {
    case I2C_NUM_1:
      RCC_BusPeriph = RCC_APB1Periph_I2C1;
      I2Cx = I2C1;
      I2C_Pins = GPIO_Pin_6 | GPIO_Pin_7; //GPIO_Pin_8 | GPIO_Pin_9
      break;
    case I2C_NUM_2:
      RCC_BusPeriph = RCC_APB1Periph_I2C2;
      I2Cx = I2C2;
      I2C_Pins = GPIO_Pin_10 | GPIO_Pin_11;
      break;
  }
  
	RCC_APB1PeriphClockCmd(RCC_BusPeriph, ENABLE);
  
  //port configuration
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  I2C_Pins;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  RCC_APB1PeriphResetCmd(RCC_BusPeriph, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_BusPeriph, DISABLE);

	// I2C configuration
  I2C_DeInit(I2Cx);
	I2C_SoftwareResetCmd(I2Cx, ENABLE);
	I2C_SoftwareResetCmd(I2Cx, DISABLE);

  I2C_Cmd(I2Cx, ENABLE);

  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = Address;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = Speed;
  I2C_Init(I2Cx, &I2C_InitStructure);
	
  I2C_AcknowledgeConfig(I2Cx, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_BusPeriph, DISABLE);
}

//------------------------------------------------------------------------------
// I2C_WriteData - write data to I2C module
//------------------------------------------------------------------------------
int I2C_WriteData(int Num, int Address, char *pBuffer, int Len)
{
  uint32_t RCC_BusPeriph;
  I2C_TypeDef* I2Cx;
	int i;
	int Result;

  if (Num>=I2C_NUM_LAST)
    return I2C_ERR_PORT;

  switch (Num)
  {
    case I2C_NUM_1:
      RCC_BusPeriph = RCC_APB1Periph_I2C1;
      I2Cx = I2C1;
      break;
    case I2C_NUM_2:
      RCC_BusPeriph = RCC_APB1Periph_I2C2;
      I2Cx = I2C2;
      break;
  }

	RCC_APB1PeriphClockCmd(RCC_BusPeriph, ENABLE); 

	Result = I2C_ERR_OK;  
		
	while (1)
	{
		I2C_GenerateSTART(I2Cx, ENABLE);
		i = 0;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)&& i++<1000);  
    if (i>=1000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_START;
      break;
    }

		I2C_Send7bitAddress(I2Cx, Address, I2C_Direction_Transmitter);
		i = 0;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && i++<1000);
    if (i>=1000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_SENDADDR;
      break;
    }

    I2C_Cmd(I2Cx, ENABLE);
    
		while (Len--)
		{
			I2C_SendData(I2Cx, *pBuffer); 
			pBuffer++; 
			i = 0;
			while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && i++<3000);
			if (i>=3000)
			{
				//tsk_unlock();
				Result = I2C_ERR_SEND;
				break;
			}		
		}
		
		if (Result!=I2C_ERR_OK)
			break;
		
		I2C_GenerateSTOP(I2Cx, ENABLE);
			
		break;
	}
	
	RCC_APB1PeriphClockCmd(RCC_BusPeriph, DISABLE);
	
	return Result;
}

//------------------------------------------------------------------------------
// I2C_ReadData - read data from I2C module
//------------------------------------------------------------------------------
int I2C_ReadData(int Num, int Address, int Start, char *pBuffer, int Len)
{
  uint32_t RCC_BusPeriph;
  I2C_TypeDef* I2Cx;
	int i;
	int Result;

  if (Num>=I2C_NUM_LAST)
    return I2C_ERR_PORT;

  switch (Num)
  {
    case I2C_NUM_1:
      RCC_BusPeriph = RCC_APB1Periph_I2C1;
      I2Cx = I2C1;
      break;
    case I2C_NUM_2:
      RCC_BusPeriph = RCC_APB1Periph_I2C2;
      I2Cx = I2C2;
      break;
  }
  
	RCC_APB1PeriphClockCmd(RCC_BusPeriph, ENABLE);   
		
	Result = I2C_ERR_OK;   
  
	while (1)
	{
    i = 0;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) && i++<1000);
    if (i>=1000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_BUSY;
      break;
    }
  
    I2C_GenerateSTART(I2Cx, ENABLE);
		i = 0;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) && i++<1000);  
    if (i>=1000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_START;
      break;
    }

		I2C_Send7bitAddress(I2Cx, Address, I2C_Direction_Transmitter);
		i = 0;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && i++<1000);
    if (i>=1000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_SENDADDR;
      break;
    }
		
    I2C_Cmd(I2Cx, ENABLE);
    
		I2C_SendData(I2Cx, (uint8_t)(Start & 0x00FF));  
	  i = 0;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && i++<1000);  
    if (i>=1000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_SEND;
      break;
    }

		I2C_GenerateSTART(I2Cx, ENABLE);
		i = 0;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) && i++<1000);
    if (i>=1000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_START;
      break;
    }
		
		I2C_Send7bitAddress(I2Cx, Address, I2C_Direction_Receiver);
		i = 0;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && i++<1000);
    if (i>=1000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_SENDADDR;
      break;
    }
		
    i = 0;
    while (Len && i++<3000)
    {
      if (Len == 1)
      {
        I2C_AcknowledgeConfig(I2Cx, DISABLE);
        I2C_GenerateSTOP(I2Cx, ENABLE);
      }

      if (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
      { 
        *pBuffer = I2C_ReceiveData(I2Cx);
        pBuffer++; 
        Len--; 
			  i = 0;
      }   
    }
    if (i>=3000) 
    {
      //tsk_unlock();
      Result = I2C_ERR_RECEIVE;
      break;
    }
    
    break;
	}
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_BusPeriph, DISABLE); 

	return Result;
}
