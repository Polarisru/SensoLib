/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2012        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a grue function rather than modifying it.   */
/* This is an example of grue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

//#include "clock.h"
#include "diskio.h"		// FatFs lower layer API
#include "ffconf.h"
#include "global.h"
//#include "timedate.h"
#include "sdspi.h"
#include "utils.h"

static volatile DSTATUS DISKIO_Stat[DISK_LAST] = {STA_NOINIT};

static uint8_t CardType;

//static DISK_HANDLE_T *hDisk;

//-----------------------------------------------------------------------
// Inidialize a Drive                                                    
//-----------------------------------------------------------------------
DSTATUS disk_initialize(BYTE drv)
{
  uint8_t cmd, type;
  int n;
  uint8_t ocr[4];
  int Timer1;

	switch (drv) 
  {
    case DISK_SDSPI:
      SDSPI_PowerOn();							// Initialize SPI

      if (DISKIO_Stat[DISK_SDSPI] & STA_NODISK)  // Is card existing in the soket?
      {  
        return DISKIO_Stat[DISK_SDSPI];
      }

      SDSPI_ClkSlow();
      for (n = 10; n; n--) 
        SDSPI_Xchg(0xFF);	      // Send 80 dummy clocks

      type = 0;
      if (SDSPI_SendCmd(CMD0, 0) == 1)     // Put the card SPI/Idle state
      {			
        Timer1 = 1000;						    // Initialization timeout = 1 sec
        if (SDSPI_SendCmd(CMD8, 0x1AA) == 1) 
        {	// SDv2?
          for (n = 0; n < 4; n++) 
            ocr[n] = SDSPI_Xchg(0xFF);	// Get 32 bit return value of R7 resp
          if (ocr[2] == 0x01 && ocr[3] == 0xAA)  // Is the card supports vcc of 2.7-3.6V?
          {				
            while (Timer1 && SDSPI_SendCmd(ACMD41, 1UL << 30));	// Wait for end of initialization with ACMD41(HCS)
            if (Timer1 && SDSPI_SendCmd(CMD58, 0) == 0)    // Check CCS bit in the OCR
            {
              for (n = 0; n < 4; n++) 
                ocr[n] = SDSPI_Xchg(0xFF);
              type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	// Card id SDv2
            }
          }
        } else 
        {	// Not SDv2 card
          if (SDSPI_SendCmd(ACMD41, 0) <= 1) 	
          {	// SDv1 or MMC?
            type = CT_SD1; cmd = ACMD41;	// SDv1 (ACMD41(0))
          } else 
          {
            type = CT_MMC; cmd = CMD1;	// MMCv3 (CMD1(0))
          }
          while (Timer1 && SDSPI_SendCmd(cmd, 0)) ;		    // Wait for end of initialization
          if (!Timer1 || SDSPI_SendCmd(CMD16, 512) != 0)	// Set block length: 512
            type = 0;
        }
      }
      CardType = type;	// Card type
      SDSPI_Deselect();

      if (type) 
      {			// OK
        SDSPI_ClkFast();			// Set fast clock
        DISKIO_Stat[DISK_SDSPI] &= ~STA_NOINIT;	// Clear STA_NOINIT flag
      } else 
      {			// Failed
        SDSPI_PowerOff();
        DISKIO_Stat[DISK_SDSPI] = STA_NOINIT;
      }
      
      return DISKIO_Stat[DISK_SDSPI];
	}
  
	return STA_NOINIT;
}

//-----------------------------------------------------------------------
// Get Disk Status                                                       
//-----------------------------------------------------------------------
DSTATUS disk_status(BYTE drv)
{
	switch (drv) 
  {
    case DISK_SDSPI:
      return DISKIO_Stat[drv];
	}
  
	return STA_NOINIT;
}

//-----------------------------------------------------------------------
// Read Sector(s)                                                        
//-----------------------------------------------------------------------

DRESULT disk_read(BYTE drv,	BYTE *buff,	DWORD sector,	BYTE count)
{
  //uint8_t counter = 0;

  if (!count)
    return RES_PARERR;

  if (DISKIO_Stat[drv] & STA_NOINIT)
    return RES_NOTRDY;
    
	switch (drv) 
  {
    case DISK_SDSPI:
      if (!(CardType & CT_BLOCK)) sector *= 512;	// LBA ot BA conversion (byte addressing cards) */

      if (count == 1) 
      {	      // Single sector read
        if ((SDSPI_SendCmd(CMD17, sector) == 0) && SDSPI_ReceiveBlock(buff, 512))
          count = 0;
      } else 
      {				// Multiple sector read
        if (SDSPI_SendCmd(CMD18, sector) == 0) 
        {
          do 
          {
            if (!SDSPI_ReceiveBlock(buff, 512)) 
              break;
            buff += 512;
          } while (--count);
          SDSPI_SendCmd(CMD12, 0);				// STOP_TRANSMISSION
        }
      }
      SDSPI_Deselect();
      if (!count)
        return RES_OK;
      return RES_ERROR;
	}
  
	return RES_PARERR;
}

//-----------------------------------------------------------------------
// Write Sector(s)                                                       
//-----------------------------------------------------------------------
#if _USE_WRITE
DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
  //uint8_t counter = 0;
  
  if (!count)
    return RES_PARERR;

  if (DISKIO_Stat[drv] & STA_NOINIT)
    return RES_NOTRDY;
  
  switch (drv) 
  {
    case DISK_SDSPI:
      if (SDSPI_GetStatus()==SDSPI_STS_WP)
      {
        DISKIO_Stat[drv] = STA_PROTECT;
        return RES_WRPRT;	// Check write protect
      }  

      if (!(CardType & CT_BLOCK)) 
        sector *= 512;	// LBA ==> BA conversion (byte addressing cards)

      if (count == 1) 
      {	      // Single sector write
        if ((SDSPI_SendCmd(CMD24, sector) == 0) && SDSPI_SendBlock(buff, 0xFE))
          count = 0;
      } else 
      {				// Multiple sector write
        if (CardType & CT_SDC) 
          SDSPI_SendCmd(ACMD23, count);	// Predefine number of sectors
        if (SDSPI_SendCmd(CMD25, sector) == 0) 
        {
          do 
          {
            if (!SDSPI_SendBlock(buff, 0xFC)) 
              break;
            buff += 512;
          } while (--count);
          if (!SDSPI_SendBlock(0, 0xFD))	// STOP_TRAN token
            count = 1;
        }
      }
      SDSPI_Deselect();
      return RES_OK;
	}
  
	return RES_PARERR;
}
#endif


//-----------------------------------------------------------------------
// Miscellaneous Functions                                               
//-----------------------------------------------------------------------
#if _USE_IOCTL
DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
	DRESULT res;
  //int counter;
  uint8_t csd[16];
  uint32_t *dp, st, ed, csize;
  uint8_t n, *ptr = buff;

  if (DISKIO_Stat[drv] & STA_NOINIT)
    return RES_NOTRDY;
  
	switch (drv) 
  {
    case DISK_SDSPI:
      switch (ctrl)
      {
        case CTRL_SYNC :		// Wait for end of internal write process of the drive
          if (SDSPI_Select()) 
            res = RES_OK;
          break;
        
        case GET_SECTOR_COUNT :	// Get drive capacity in unit of sector (DWORD)
          if ((SDSPI_SendCmd(CMD9, 0) == 0) && SDSPI_ReceiveBlock(csd, 16)) 
          {
            if ((csd[0] >> 6) == 1) 
            {	// SDC ver 2.00
              csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
              *(DWORD*)buff = csize << 10;
            } else 
            {					// SDC ver 1.XX or MMC ver 3
              n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
              csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
              *(DWORD*)buff = csize << (n - 9);
            }
            res = RES_OK;
          }
          break;

        case GET_BLOCK_SIZE :	// Get erase block size in unit of sector (DWORD)
          if (CardType & CT_SD2) 
          {	// SDC ver 2.00
            if (SDSPI_SendCmd(ACMD13, 0) == 0) 
            {	// Read SD status
              SDSPI_Xchg(0xFF);
              if (SDSPI_ReceiveBlock(csd, 16)) 
              {				// Read partial block
                for (n = 64 - 16; n; n--) 
                  SDSPI_Xchg(0xFF);	// Purge trailing data
                *(DWORD*)buff = 16UL << (csd[10] >> 4);
                res = RES_OK;
              }
            }
          } else 
          {					// SDC ver 1.XX or MMC
            if ((SDSPI_SendCmd(CMD9, 0) == 0) && SDSPI_ReceiveBlock(csd, 16)) 
            {	// Read CSD
              if (CardType & CT_SD1) 
              {	// SDC ver 1.XX
                *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
              } else {					// MMC
                *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
              }
              res = RES_OK;
            }
          }
          break;

        case CTRL_ERASE_SECTOR :	// Erase a block of sectors (used when _USE_ERASE == 1)
          if (!(CardType & CT_SDC)) 
            break;				// Check if the card is SDC
          if (disk_ioctl(drv, MMC_GET_CSD, csd)) 
            break;	// Get CSD
          if (!(csd[0] >> 6) && !(csd[10] & 0x40)) 
            break;	// Check if sector erase can be applied to the card
          dp = buff; st = dp[0]; ed = dp[1];				// Load sector block
          if (!(CardType & CT_BLOCK)) 
          {
            st *= 512; ed *= 512;
          }
          if (SDSPI_SendCmd(CMD32, st) == 0 && SDSPI_SendCmd(CMD33, ed) == 0 && SDSPI_SendCmd(CMD38, 0) == 0 && SDSPI_WaitReady(30000))	// Erase sector block
            res = RES_OK;	// FatFs does not check result of this command
          break;

        // Following commands are never used by FatFs module

        case MMC_GET_TYPE :		// Get MMC/SDC type (BYTE)
          *ptr = CardType;
          res = RES_OK;
          break;

        case MMC_GET_CSD :		// Read CSD (16 bytes)
          if (SDSPI_SendCmd(CMD9, 0) == 0		// READ_CSD
            && SDSPI_ReceiveBlock(ptr, 16))
            res = RES_OK;
          break;

        case MMC_GET_CID :		// Read CID (16 bytes)
          if (SDSPI_SendCmd(CMD10, 0) == 0 && SDSPI_ReceiveBlock(ptr, 16))
            res = RES_OK;
          break;

        case MMC_GET_OCR :		// Read OCR (4 bytes)
          if (SDSPI_SendCmd(CMD58, 0) == 0) 
          {	// READ_OCR
            for (n = 4; n; n--) 
              *ptr++ = SDSPI_Xchg(0xFF);
            res = RES_OK;
          }
          break;

        case MMC_GET_SDSTAT :	// Read SD status (64 bytes)
          if (SDSPI_SendCmd(ACMD13, 0) == 0) 
          {	// SD_STATUS
            SDSPI_Xchg(0xFF);
            if (SDSPI_ReceiveBlock(ptr, 64))
              res = RES_OK;
          }
          break;

        default:
          res = RES_PARERR;
      }
      return res;
	}
	return RES_PARERR;
}
#endif

//-----------------------------------------------------------------------
// get FAT time/date
//-----------------------------------------------------------------------
DWORD disk_getfattime(void)
{
  uint32_t Result = 0;

  /*CLOCK_GetTime(&globalDateTime);
  if (TIMEDATE_IsValid(&globalDateTime))
    Result = TIMEDATE_GetFatTime(&globalDateTime);*/

  return Result;
}
