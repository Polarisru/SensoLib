#include "m24xx.h"
#include "stm32f10x_i2c.h"

int M24XX_Timeout;
uint8_t M24XX_Address;

void Delay_ms(uint32_t ms)
{
	volatile uint32_t nCount;
  RCC_ClocksTypeDef RCC_Clocks;
  
	RCC_GetClocksFreq (&RCC_Clocks);

  nCount = (RCC_Clocks.HCLK_Frequency/10000)*ms;
  for (; nCount!=0; nCount--);
}

//------------------------------------------------------------------------------
// M24XX_Init - init serial EEPROM module
//------------------------------------------------------------------------------
void M24XX_Init(void)
{
  I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);
  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  M24XX_PIN_SCL | M24XX_PIN_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  RCC_APB1PeriphResetCmd(RCC_APBPeriph_I2C_EE, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APBPeriph_I2C_EE, DISABLE);

	// I2C configuration
  I2C_DeInit(I2C_EE);
	I2C_SoftwareResetCmd(I2C_EE, ENABLE);
	I2C_SoftwareResetCmd(I2C_EE, DISABLE);

  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = M24XX_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = M24XX_I2C_SPEED;
  I2C_Init(I2C_EE, &I2C_InitStructure);

	//I2C_Cmd(I2C_EE, ENABLE);
  //I2C_StretchClockCmd(I2C_EE, ENABLE);

  M24XX_Address = M24XX_ADDRESS;
	
  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);
  
  //I2C_DMACmd(sEE_I2C, ENABLE);
}

//------------------------------------------------------------------------------
// M24XX_WaitEepromStandbyState - wait for EEPROM ready status
//------------------------------------------------------------------------------
void M24XX_WaitEepromStandbyState(void) 
{
 	RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   
	
  do 
  {
		I2C_ClearFlag(I2C_EE, I2C_FLAG_AF);
  
		I2C_GenerateSTART(I2C_EE, ENABLE);
		while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT)); 
		
		I2C_Send7bitAddress(I2C_EE, M24XX_Address, I2C_Direction_Transmitter);
		while (!I2C_CheckEvent(I2C_EE, 0x00070080) && !I2C_CheckEvent(I2C_EE, 0x00030400));  

	} while (I2C_GetFlagStatus(I2C_EE, I2C_FLAG_AF)==SET);
  
  I2C_GenerateSTOP(I2C_EE, ENABLE);  

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);
}

//------------------------------------------------------------------------------
// M24XX_ReadBuffer - read buffer from serial EEPROM
//------------------------------------------------------------------------------
void M24XX_ReadBuffer(uint16_t ReadAddr, uint16_t NumByteToRead, uint8_t *Buff)
{
  int Block = 0;

	if (!NumByteToRead)
		return;
	
	if (ReadAddr>=M24XX_FLASH_SIZE)
    return;

  if (ReadAddr>=M24XX_BLOCK_SIZE)
    Block = 2;

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   

  I2C_GenerateSTART(I2C_EE, ENABLE);
  while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
   
  I2C_Send7bitAddress(I2C_EE, M24XX_Address + Block, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  I2C_Cmd(I2C_EE, ENABLE);

  I2C_SendData(I2C_EE, (uint8_t)(ReadAddr & 0x00FF));    
  while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTART(I2C_EE, ENABLE);
  while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
  
  I2C_Send7bitAddress(I2C_EE, M24XX_Address + Block + 0x01, I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  while (NumByteToRead)
  {
    if (NumByteToRead==1)
    {
    	I2C_AcknowledgeConfig(I2C_EE, DISABLE);
      I2C_GenerateSTOP(I2C_EE, ENABLE);
    }

    if(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_RECEIVED)) 
    {
     	*Buff = I2C_ReceiveData(I2C_EE);
		  Buff++; 
	    NumByteToRead--;        
    }   
  }
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE); 
}

//------------------------------------------------------------------------------
// M24XX_ByteWrite - write one byte to serial EEPROM
//------------------------------------------------------------------------------
void M24XX_ByteWrite(uint16_t WriteAddr, uint8_t Buff)
{
  int Block = 0;
  
	if (WriteAddr>=M24XX_FLASH_SIZE)
    return;
  
  if (WriteAddr>=M24XX_BLOCK_SIZE)
    Block = 2;

	RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   
		
 	I2C_GenerateSTART(I2C_EE, ENABLE);
 	while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));  

	I2C_Send7bitAddress(I2C_EE, M24XX_Address + Block, I2C_Direction_Transmitter);
 	while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

 	I2C_SendData(I2C_EE, (uint8_t)(WriteAddr & 0x00FF));
 	while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

 	I2C_SendData(I2C_EE, Buff); 
 	while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
 	I2C_GenerateSTOP(I2C_EE, ENABLE);
	
  M24XX_WaitEepromStandbyState();
	
  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);
}

//------------------------------------------------------------------------------
// M24XX_PageWrite - write page to serial EEPROM
//------------------------------------------------------------------------------
void M24XX_PageWrite(uint16_t WriteAddr, uint16_t NumByteToWrite, uint8_t *pBuffer) 
{ 
  int Block = 0;

	if (!NumByteToWrite)
		return;
	
	if (WriteAddr>=M24XX_FLASH_SIZE)
    return;

  if (WriteAddr>=M24XX_BLOCK_SIZE)
    Block = 2;

 	RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   

	while (I2C_GetFlagStatus(I2C_EE, I2C_FLAG_BUSY))
		__nop();
  
  I2C_GenerateSTART(I2C_EE, ENABLE);
  while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT)); 
  
  I2C_Send7bitAddress(I2C_EE, M24XX_Address + Block, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  

  I2C_SendData(I2C_EE, (uint8_t)(WriteAddr & 0x00FF));
  while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	while (NumByteToWrite--)
  {
  	I2C_SendData(I2C_EE, *pBuffer); 
   	pBuffer++; 
   	while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  I2C_GenerateSTOP(I2C_EE, ENABLE);
  
  M24XX_WaitEepromStandbyState();

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);   
}

//------------------------------------------------------------------------------
// M24XX_BufferWrite - write buffer to serial EEPROM
//------------------------------------------------------------------------------
void M24XX_BufferWrite(uint16_t WriteAddr, uint16_t NumByteToWrite, uint8_t *pBuffer)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;

	if (!NumByteToWrite)
		return;
	
	if (WriteAddr>=M24XX_FLASH_SIZE)
    return;

 	RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   
	
  Addr = WriteAddr % M24XX_FLASH_PAGESIZE;
  count = M24XX_FLASH_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / M24XX_FLASH_PAGESIZE;
  NumOfSingle = NumByteToWrite % M24XX_FLASH_PAGESIZE;
 
  if (Addr == 0) 
  {
   	if (NumOfPage == 0)
    {
   		M24XX_PageWrite(WriteAddr, NumOfSingle, pBuffer);
   	} else
    {
    	while (NumOfPage--) 
      {
      	M24XX_PageWrite(WriteAddr, M24XX_FLASH_PAGESIZE, pBuffer); 
        WriteAddr +=  M24XX_FLASH_PAGESIZE;
        pBuffer += M24XX_FLASH_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        M24XX_PageWrite(WriteAddr, NumOfSingle, pBuffer);
      }
    }
  }	else 
  {
    if (NumOfPage==0)
    {
      if (NumByteToWrite > count) 
      {
        M24XX_PageWrite(WriteAddr, count, pBuffer);
      
        M24XX_PageWrite((WriteAddr + count), (NumByteToWrite - count), (uint8_t*)(pBuffer + count));
      }	else 
      {
        M24XX_PageWrite(WriteAddr, NumOfSingle, pBuffer);
      }     
    }	else 
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / M24XX_FLASH_PAGESIZE;
      NumOfSingle = NumByteToWrite % M24XX_FLASH_PAGESIZE;
      
      if (count != 0) 
      {  
        M24XX_PageWrite(WriteAddr, count, pBuffer);
        WriteAddr += count;
        pBuffer += count;
      } 
  
      while (NumOfPage--) 
      {
        M24XX_PageWrite(WriteAddr, M24XX_FLASH_PAGESIZE, pBuffer);
        WriteAddr +=  M24XX_FLASH_PAGESIZE;
        pBuffer += M24XX_FLASH_PAGESIZE;  
      }
      if (NumOfSingle != 0)
      {
        M24XX_PageWrite(WriteAddr, NumOfSingle, pBuffer); 
      }
    }
  }  
  
  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);   
}
