#ifndef _EEPROM_H
#define _EEPROM_H

#include "defines.h"
#include "platform.h"
#include "const.h"

#define EEPROM_ERRORS   (3)

// Define the STM32F10Xxx Flash page size depending on the used STM32 device
#if defined (STM32F10X_LD) || defined (STM32F10X_MD)
  #define PAGE_SIZE  (TWord)0x400  // Page size = 1KByte
#elif defined (STM32F10X_HD) || defined (STM32F10X_CL)
  #define PAGE_SIZE  (TWord)0x800  // Page size = 2KByte
#endif

// EEPROM start address in Flash
#define EEPROM_START_ADDRESS    ((uint32_t)0x08010000) // EEPROM emulation start address: after 64KByte of used Flash memory

// Pages 0 and 1 base and end addresses
#define PAGE0_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

#define EEPROM_COPY1    				(0x000)
#define EEPROM_COPY2      			(0x100)

// Used Flash pages for EEPROM emulation
enum EEPROM_PAGES
{
	EEPROM_PAGE_0,
	EEPROM_PAGE_1,
	EEPROM_PAGE_LAST
};

typedef struct
{
  char      SerialNumber[SERIAL_NUM_MAX_LEN+2];
  uint8_t     Empty[12];
  uint32_t    CRC32;
} _EEPROM_Data;

extern _EEPROM_Data EEPROM;

//extern __no_init __eeprom char PCBID[PCB_ID_MAX_LEN+1];

extern TBool EEPROM_WriteVar(void);
extern void EEPROM_Restore(void);
extern void EEPROM_LoadDefault(void);
extern TBool EEPROM_CheckIntegrity(void);
extern void EEPROM_Check(void);

#endif
