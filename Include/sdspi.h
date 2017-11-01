#ifndef _SDSPI_H
#define _SDSPI_H

#include "platform.h"
#include "defines.h"

#define SPI_DUMMY_BYTE			0x00

#define SPI_SD              SPI2
#define SPI_SD_CLK          RCC_APB1Periph_SPI2
#define SPI_SD_GPIO         GPIOB
#define SPI_SD_PIN_SCK      GPIO_Pin_13
#define SPI_SD_PIN_MISO     GPIO_Pin_14
#define SPI_SD_PIN_MOSI     GPIO_Pin_15
#define SPI_SD_PIN_CS       GPIO_Pin_12

//#define SPI_MASTER_IRQn              SPI2_IRQn

// MMC/SD command
#define CMD0	 (0)			  // GO_IDLE_STATE
#define CMD1	 (1)			  // SEND_OP_COND (MMC)
#define	ACMD41 (0x80+41)	// SEND_OP_COND (SDC)
#define CMD8	 (8)			  // SEND_IF_COND
#define CMD9	 (9)			  // SEND_CSD
#define CMD10	 (10)		    // SEND_CID
#define CMD12	 (12)		    // STOP_TRANSMISSION
#define ACMD13 (0x80+13)	// SD_STATUS (SDC)
#define CMD16	 (16)		    // SET_BLOCKLEN
#define CMD17	 (17)		    // READ_SINGLE_BLOCK
#define CMD18	 (18)		    // READ_MULTIPLE_BLOCK
#define CMD23	 (23)		    // SET_BLOCK_COUNT (MMC)
#define	ACMD23 (0x80+23)	// SET_WR_BLK_ERASE_COUNT (SDC)
#define CMD24	 (24)		    // WRITE_BLOCK
#define CMD25	 (25)		    // WRITE_MULTIPLE_BLOCK
#define CMD32	 (32)		    // ERASE_ER_BLK_START
#define CMD33	 (33)		    // ERASE_ER_BLK_END
#define CMD38	 (38)		    // ERASE
#define CMD55	 (55)		    // APP_CMD
#define CMD58	 (58)		    // READ_OCR

enum SDSPI_STS
{
  SDSPI_STS_NONE,
  SDSPI_STS_WP,
  SDSPI_STS_ALL
};

extern void SDSPI_Init(void);
extern void SDSPI_SetStatus(int Status);
extern int SDSPI_GetStatus(void);
extern uint8_t SDSPI_Xchg(uint8_t data);
extern int SDSPI_WaitReady(int wt);
extern void SDSPI_Deselect(void);
extern int SDSPI_Select(void);
extern int SDSPI_PowerOn(void);
extern void SDSPI_PowerOff(void);
extern void SDSPI_ClkFast(void);
extern void SDSPI_ClkSlow(void);
extern void SDSPI_Receive(uint8_t *buff,	int btr);
extern int SDSPI_ReceiveBlock(uint8_t *buff, int btr);
extern int SDSPI_SendBlock(const uint8_t *buff, uint8_t token);
extern uint8_t SDSPI_SendCmd(uint8_t cmd, uint32_t arg);

#endif
