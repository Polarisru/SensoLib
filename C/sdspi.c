#include "rtos.h"
#include "sdspi.h"
#include "stm32f10x_spi.h"

int SDSPI_CardStatus;
int Timer1, Timer2;

//------------------------------------------------------------------------------
// SDSPI_Init - initialize SPI module for SD card
//------------------------------------------------------------------------------
void SDSPI_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

	SPI_I2S_DeInit(SPI_SD);

  // Enable SPI_MASTER Periph cloc
  RCC_APB1PeriphClockCmd(SPI_SD_CLK, ENABLE);                           

	GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_SD_PIN_SCK | SPI_SD_PIN_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_SD_GPIO, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = SPI_SD_PIN_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SPI_SD_GPIO, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = SPI_SD_PIN_CS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(SPI_SD_GPIO, &GPIO_InitStructure);
  GPIO_WriteBit(SPI_SD_GPIO, SPI_SD_PIN_CS, Bit_SET);

  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  //SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI_SD, &SPI_InitStructure);
	
	SPI_CalculateCRC(SPI_SD, DISABLE);

	SPI_Cmd(SPI2, ENABLE);

}

//------------------------------------------------------------------------------
// SDSPI_SetStatus - set SD card status
//------------------------------------------------------------------------------
void SDSPI_SetStatus(int Status)
{
  SDSPI_CardStatus = Status;
}

//------------------------------------------------------------------------------
// SDSPI_GetStatus - get SD card status
//------------------------------------------------------------------------------
int SDSPI_GetStatus(void)
{
  return SDSPI_CardStatus;
}

//------------------------------------------------------------------------------
// SDSPI_Xchg - one byte send/receive
//------------------------------------------------------------------------------
uint8_t SDSPI_Xchg(uint8_t data)
{
  uint8_t Value;
  
	while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI_SD, data);
	while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_RXNE)==RESET);
	Value = SPI_I2S_ReceiveData(SPI_SD);

  return Value;
}

//------------------------------------------------------------------------------
// SDSPI_WaitReady - wait for SPI ready
//------------------------------------------------------------------------------
int SDSPI_WaitReady(int wt)
{
	uint8_t d;

	Timer2 = wt;
	do 
  {
		d = SDSPI_Xchg(0xFF);
    //os_dly_wait(1);                 // wait interval, 1 cycle
    //osDelay(1);
    vTaskDelay(1);
    Timer2--;
	} while (d != 0xFF && Timer2);	// Wait for card goes ready or timeout

	return (d == 0xFF) ? 1 : 0;
}

//------------------------------------------------------------------------------
// Deselect card and release SPI
//------------------------------------------------------------------------------
void SDSPI_Deselect(void)
{
  //put CS high
  GPIO_WriteBit(SPI_SD_GPIO, SPI_SD_PIN_CS, Bit_SET);
	SDSPI_Xchg(0xFF);	// Dummy clock (force DO hi-z for multiple slave SPI)
}

//------------------------------------------------------------------------------
// Select card and wait for ready
//------------------------------------------------------------------------------
int SDSPI_Select(void)
{
  //put CS low
  GPIO_WriteBit(SPI_SD_GPIO, SPI_SD_PIN_CS, Bit_RESET);
	SDSPI_Xchg(0xFF);	  // Dummy clock (force DO enabled)

	if (SDSPI_WaitReady(500)) 
    return 1; // Ok

	SDSPI_Deselect();

	return 0;	  // Timeout
}

//------------------------------------------------------------------------------
// SDSPI_PowerOn - Enable SD power function
//------------------------------------------------------------------------------
int SDSPI_PowerOn(void)
{
	//GPIO_ClearValue(SD1PWR_PORT, (1<<SD1PWR_PIN));

	Timer1 = 100;
	do 
  {
    //os_dly_wait(1);                 // wait interval, 1 cycle
    //osDelay(1);
    vTaskDelay(1);
    Timer1--;
	} while (Timer1);

  //if (GPIO_ReadValue(SD1CD_PORT)&(1<<SD1CD_PIN))
  //  return 0;
  
  /*if (GPIO_ReadValue(SD1WP_PORT)&(1<<SD1WP_PIN))
    SDSPI_CardStatus = SDSPI_STS_WP;
  else
    SDSPI_CardStatus = SDSPI_STS_ALL;*/
  
  return 1;
}

//------------------------------------------------------------------------------
// SDSPI_PowerOff - Disable SD power function
//------------------------------------------------------------------------------
void SDSPI_PowerOff(void)
{
	SDSPI_Select();				// Wait for card ready
	SDSPI_Deselect();
	//GPIO_SetValue(SD1PWR_PORT, (1<<SD1PWR_PIN));
}

//------------------------------------------------------------------------------
// SDSPI_ClkFast - set high clock speed for SPI
//------------------------------------------------------------------------------
void SDSPI_ClkFast(void)
{
  //SSP_SetClock(SD_SPI, SDSPI_CLK_FAST);
}

//------------------------------------------------------------------------------
// SDSPI_ClkSlow - set low clock speed for SPI
//------------------------------------------------------------------------------
void SDSPI_ClkSlow(void)
{
  //SSP_SetClock(SD_SPI, SDSPI_CLK_SLOW);
}

//------------------------------------------------------------------------------
// SDSPI_Receive - receive multiple bytes from SD card
//------------------------------------------------------------------------------
void SDSPI_Receive(uint8_t *buff,	int btr)
{
  int i;
  
  for (i=0; i<btr; i++)
  {
    while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE)==RESET);
    SPI_I2S_SendData(SPI_SD, SPI_DUMMY_BYTE);
    while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_RXNE)==RESET);
    *buff++ = SPI_I2S_ReceiveData(SPI_SD);
  }
}

//------------------------------------------------------------------------------
// SDSPI_Send - send data buffer to SD card
//------------------------------------------------------------------------------
void SDSPI_Send(const uint8_t *buff, int btx)
{
  int i;
  
  for (i=0; i<btx; i++)
  {
    while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE)==RESET);
    SPI_I2S_SendData(SPI_SD, *buff);
    buff++;
  }
}

//------------------------------------------------------------------------------
// SDSPI_ReceiveBlock - receive a data packet from the SD card
//------------------------------------------------------------------------------
int SDSPI_ReceiveBlock(uint8_t *buff, int btr)
{
	uint8_t token;

	Timer1 = 200;
	do 
  {							                  // Wait for DataStart token in timeout of 200ms
		token = SDSPI_Xchg(0xFF);
    //os_dly_wait(1);                 // wait interval, 1 cycle
    //osDelay(1);
    vTaskDelay(1);
    Timer1--;
	} while ((token == 0xFF) && Timer1);
	if(token != 0xFE) return 0;		  // Function fails if invalid DataStart token or timeout

	SDSPI_Receive(buff, btr);		    // Store trailing data to the buffer
	SDSPI_Xchg(0xFF); 
  SDSPI_Xchg(0xFF);			          // Discard CRC

	return 1;						            // Function succeeded
}

//------------------------------------------------------------------------------
// SDSPI_SendBlock - send a data packet to the SD card
//------------------------------------------------------------------------------
int SDSPI_SendBlock(const uint8_t *buff, uint8_t token)
{
	uint8_t resp;

	if (!SDSPI_WaitReady(500)) 
    return 0;		                  // Wait for card ready

	SDSPI_Xchg(token);					    // Send token
	if (token != 0xFD) 
  {				                        // Send data if token is other than StopTran
		SDSPI_Send(buff, 512);		    // Data
		SDSPI_Xchg(0xFF); 
    SDSPI_Xchg(0xFF);	            // Dummy CRC

    resp = SDSPI_Xchg(0xFF);			// Receive data resp
		if ((resp & 0x1F) != 0x05)		// Function fails if the data packet was not accepted
			return 0;
	}
	return 1;
}

//------------------------------------------------------------------------------
// SDSPI_SendCmd - send command to SD card
//------------------------------------------------------------------------------
uint8_t SDSPI_SendCmd(uint8_t cmd, uint32_t arg)
{
	uint8_t n, res;

	if (cmd & 0x80) 
  {	// Send a CMD55 prior to ACMD<n>
		cmd &= 0x7F;
		res = SDSPI_SendCmd(CMD55, 0);
		if (res > 1) 
      return res;
	}

	// Select the card and wait for ready except to stop multiple block read
	if (cmd != CMD12) 
  {
		SDSPI_Deselect();
		if (!SDSPI_Select()) 
      return 0xFF;
	}

	// Send command packet
	SDSPI_Xchg(0x40 | cmd);				  // Start + command index
	SDSPI_Xchg((uint8_t)(arg >> 24));  // Argument[31..24]
	SDSPI_Xchg((uint8_t)(arg >> 16));	// Argument[23..16]
	SDSPI_Xchg((uint8_t)(arg >> 8));		// Argument[15..8]
	SDSPI_Xchg((uint8_t)arg);				  // Argument[7..0]
	n = 0x01;							        // Dummy CRC + Stop
	if (cmd == CMD0) 
    n = 0x95;			              // Valid CRC for CMD0(0)
	if (cmd == CMD8) 
    n = 0x87;			              // Valid CRC for CMD8(0x1AA)
	SDSPI_Xchg(n);

	// Receive command resp
	if (cmd == CMD12) 
    SDSPI_Xchg(0xFF);	            // Diacard following one byte when CMD12
	n = 10;								        // Wait for response (10 bytes max)
	do
		res = SDSPI_Xchg(0xFF);
	while ((res & 0x80) && --n);

	return res;							      // Return received response
}
