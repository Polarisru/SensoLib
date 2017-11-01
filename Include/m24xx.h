#ifndef _M24XX_H
#define _M24XX_H

#include "platform.h"
#include "defines.h"

#define M24XX_FLAG_TIMEOUT			(100000)
#define M24XX_I2C_SPEED					(100000)

#define M24XX_FLASH_SIZE        (512)
#define M24XX_BLOCK_SIZE        (256)
#define M24XX_FLASH_PAGESIZE    (16)

#define M24XX_ADDRESS						(0xA0)

#define M24XX_GPIO				      GPIOB
#define M24XX_PIN_SCL           GPIO_Pin_10
#define M24XX_PIN_SDA           GPIO_Pin_11

#define I2C_EE						      I2C2
#define RCC_APBPeriph_I2C_EE    RCC_APB1Periph_I2C2

extern void M24XX_Init(void);
extern void M24XX_ReadBuffer(uint16_t ReadAddr, uint16_t NumByteToRead, uint8_t *Buff);
extern void M24XX_ByteWrite(uint16_t WriteAddr, uint8_t Buff);
extern void M24XX_PageWrite(uint16_t WriteAddr, uint16_t NumByteToWrite, uint8_t *pBuffer);
extern void M24XX_BufferWrite(uint16_t WriteAddr, uint16_t NumByteToWrite, uint8_t *pBuffer);

#endif
