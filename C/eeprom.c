#include <string.h>
#include "eeprom.h"
#include "crc32.h"
#include "const.h"
#include "m24xx.h"

const _EEPROM_Data EEPROM_Default = 
{
  "000000",           //serial number of the device
  {0,0,0,0,0,0,0,0,0,0,0,0},  //place for adding new values
  0                   //CRC32
};

_EEPROM_Data EEPROM;

//__root __eeprom char PCBID[PCB_ID_MAX_LEN+1] @PCBID_ADDR;// = "0000000000";

//------------------------------------------------------------------------------
// EEPROM_Erase - erase copy of EEPROM in nonvolatile memory
//------------------------------------------------------------------------------
int EEPROM_Erase(int Num)
{
	/*uint16_t FlashStatus;
	
  if (Num==EEPROM_PAGE_0)
  {
		FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
    // If erase operation was failed, error code is returned
    if (FlashStatus != FLASH_COMPLETE)
      return 1;
  } else
  {
		FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
    // If erase operation was failed, error code is returned
    if (FlashStatus != FLASH_COMPLETE)
      return 1;
  }*/
  return 0;
}

//------------------------------------------------------------------------------
// EEPROM_Write - write copy of EEPROM to nonvolatile memory
//------------------------------------------------------------------------------
int EEPROM_Write(uint8_t *Buffer, int Num)
{
	//uint32_t Address;
  uint16_t Address;
	//int i;
	
  if (Num==EEPROM_PAGE_0)
		//Address = PAGE0_BASE_ADDRESS;
    Address = EEPROM_COPY1;
  else
		//Address = PAGE1_BASE_ADDRESS;
    Address = EEPROM_COPY2;
	
	/*for (i=0; i<sizeof(_EEPROM_Data); i+=sizeof(uint32_t))
	{
		FLASH_ProgramWord(Address, *((uint32_t*)Buffer));
		Buffer += sizeof(uint32_t);
		Address += sizeof(uint32_t);
	}*/
  M24XX_BufferWrite(Address, sizeof(_EEPROM_Data), Buffer);
	
  return 0;
}

//------------------------------------------------------------------------------
// EEPROM_Read - read copy of EEPROM from nonvolatile memory
//------------------------------------------------------------------------------
void EEPROM_Read(uint8_t *Buffer, int Num)
{
  if (Num==EEPROM_PAGE_0)
    //memcpy(Buffer, (uint8_t*)PAGE0_BASE_ADDRESS, sizeof(_EEPROM_Data));
    M24XX_ReadBuffer(EEPROM_COPY1, sizeof(_EEPROM_Data), Buffer);
  else										 
    //memcpy(Buffer, (uint8_t*)PAGE1_BASE_ADDRESS, sizeof(_EEPROM_Data));
    M24XX_ReadBuffer(EEPROM_COPY2, sizeof(_EEPROM_Data), Buffer);
}

//------------------------------------------------------------------------------
// EEPROM_WriteVar - write changed values to EEPROM
//------------------------------------------------------------------------------
TBool EEPROM_WriteVar(void)
{
  uint32_t CRC32;
  
	//FLASH_Unlock();
  CRC32 = CRC32_Calc((uint8_t*)&EEPROM, sizeof(_EEPROM_Data)-sizeof(uint32_t));
  EEPROM.CRC32 = CRC32;
  EEPROM_Erase(EEPROM_PAGE_0);
  EEPROM_Write((uint8_t*)&EEPROM, EEPROM_PAGE_0);
  EEPROM_Erase(EEPROM_PAGE_1);
  EEPROM_Write((uint8_t*)&EEPROM, EEPROM_PAGE_1);
	//FLASH_Lock();

  return true;
}

//------------------------------------------------------------------------------
// EEPROM_Restore - completely restore EEPROM from the firmware
//------------------------------------------------------------------------------
void EEPROM_Restore()
{
  uint32_t CRC32;

	//FLASH_Unlock();
  CRC32 = CRC32_Calc((uint8_t*)&EEPROM_Default, sizeof(_EEPROM_Data)-sizeof(uint32_t));
  memcpy((uint8_t*)&EEPROM, (uint8_t*)&EEPROM_Default, sizeof(_EEPROM_Data));
  EEPROM.CRC32 = CRC32;
  //write first copy of EEPROM
  EEPROM_Erase(EEPROM_PAGE_0);
  EEPROM_Write((uint8_t*)&EEPROM, EEPROM_PAGE_0);
  //write second copy of EEPROM
  EEPROM_Erase(EEPROM_PAGE_1);
  EEPROM_Write((uint8_t*)&EEPROM, EEPROM_PAGE_1);
	//FLASH_Lock();
}

//------------------------------------------------------------------------------
// EEPROM_LoadDefault - load default EEPROM values from the firmware
//------------------------------------------------------------------------------
void EEPROM_LoadDefault()
{
  //uint32_t CRC32;
  
	/*FLASH_Unlock();
  memcpy((uint8_t*)&EEPROM.PumpRatio, (uint8_t*)&EEPROM_Default.PumpRatio, (uint32_t)((uint32_t)&EEPROM_Default.CRC32-(uint32_t)&EEPROM_Default.PumpRatio));
  CRC32 = CRC32_Calc((uint8_t*)&EEPROM, sizeof(_EEPROM_Data)-sizeof(uint32_t));
  EEPROM.CRC32 = CRC32;
  //write first copy of EEPROM
  EEPROM_Erase(EEPROM_PAGE_0);
  EEPROM_Write((uint8_t*)&EEPROM, EEPROM_PAGE_0);
  //write second copy of EEPROM
  EEPROM_Erase(EEPROM_PAGE_1);
  EEPROM_Write((uint8_t*)&EEPROM, EEPROM_PAGE_1);
	FLASH_Lock();*/
  EEPROM_Restore();
}

//------------------------------------------------------------------------------
// EEPROM_CheckIntegrity - check integrity of EEPROM and try to restore it
//------------------------------------------------------------------------------
TBool EEPROM_CheckIntegrity()
{
  uint32_t CRC32;
  _EEPROM_Data EEPROM_Copy;

  //check first copy of EEPROM
  EEPROM_Read((uint8_t*)&EEPROM, EEPROM_PAGE_0);
  CRC32 = CRC32_Calc((uint8_t*)&EEPROM, sizeof(_EEPROM_Data)-sizeof(uint32_t));
  if (CRC32!=EEPROM.CRC32)
  {
    EEPROM_Read((uint8_t*)&EEPROM_Copy, EEPROM_PAGE_1);
    CRC32 = CRC32_Calc((uint8_t*)&EEPROM_Copy, sizeof(_EEPROM_Data)-sizeof(uint32_t));
    if (CRC32!=EEPROM_Copy.CRC32)
	  //both copies of EEPROM are damaged
			return false;
		//write first copy of EEPROM
		//FLASH_Unlock();
		EEPROM_Erase(EEPROM_PAGE_0);
  	EEPROM_Write((uint8_t*)&EEPROM, EEPROM_PAGE_0);
		//FLASH_Lock();
  }
  //check second copy of EEPROM
  EEPROM_Read((uint8_t*)&EEPROM_Copy, EEPROM_PAGE_1);
  CRC32 = CRC32_Calc((uint8_t*)&EEPROM_Copy, sizeof(_EEPROM_Data)-sizeof(uint32_t));
  if (CRC32!=EEPROM_Copy.CRC32)
  {
		//write second copy of EEPROM
		//FLASH_Unlock();
		EEPROM_Erase(EEPROM_PAGE_1);
    EEPROM_Write((uint8_t*)&EEPROM, EEPROM_PAGE_1);
		//FLASH_Lock();
  }
  return true;
}

//------------------------------------------------------------------------------
// EEPROM_Check - check content of EEPROM (for new firmware versions)
//------------------------------------------------------------------------------
void EEPROM_Check(void)
{
}

