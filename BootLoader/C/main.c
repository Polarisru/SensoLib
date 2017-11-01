#include "programm.h"
#include "serial.h"
#include "defines.h"

int i, crc16;
char buffer[BLOCK_SIZE];

int main(void) 
{
  unsigned char val, val2, status;
  unsigned int * ptr;
  unsigned int address, size;
  unsigned int crc32;

  //get_mcu_type();
  
  ptr =(unsigned int *)RAM_START_ADDR;
  if(!(*ptr == BOOTLOADER_KEY1 && *(ptr+1) == BOOTLOADER_KEY2))
  {
    if (PROGRAMM_CodeChecksum())
    {
      PROGRAMM_ExecuteCode();
    }
  }
  
  SERIAL_Init();

  //LPC_GPIO1->FIODIR = (1<<pinLED);           		// LEDs on PORT1 defined as Output
  //LPC_GPIO1->FIOSET |= (1<<pinLED);

  for (;;)
  {
    
    if (SERIAL_GetChar()!=COMM_CONF1) continue;
    if (SERIAL_GetChar()!=COMM_CONF2) continue;
           
    val=SERIAL_GetChar(); // Wait for command character.
            
    // Check echo
    /*if(val==CMD_ECHO)
    {
      SERIAL_SendChar(ANSW_ECHO);
    }*/

    // Chip erase
    if(val==CMD_ERASE)
    {
      PROGRAMM_EraseFlash();
      SERIAL_SendChar(ANSW_OK); // Send OK back.
    }

    // Start block load.
    else if(val==CMD_WRITE)
    {
      crc16 = 0;
      val = SERIAL_GetChar();
      val2 = SERIAL_GetChar();
      address = ((unsigned int)val<<16) | ((unsigned int)val2<<8) | (unsigned int)SERIAL_GetChar(); // Get address
      val = SERIAL_GetChar();
      size = ((unsigned int)val<<8) | (unsigned int)SERIAL_GetChar(); // Get block size.
      val = SERIAL_GetChar(); // Get memtype
      crc16 = val + (size>>8) + (size&0xff) + (address>>16) + ((address>>8)&0xff) + (address&0xff);
      //address <<= 1;

      for (i=0; i<size; i++)
      {
        buffer[i] = SERIAL_GetChar();
        crc16 += buffer[i];
      }

      status = SERIAL_GetChar();
      i = (unsigned int)status | ((unsigned int)SERIAL_GetChar() << 8);

      if (i!=crc16)
      {
        status = ANSW_ERROR;
      } else
      {  
  	    if (val=='F')
        {
          address += USER_FLASH_ADD;
          if (address<USER_FLASH_START)
          {
            status = ANSW_OK;
          } else
          {
            //write flash
            status = PROGRAMM_WriteFlash(buffer, (void*)address, size);
            if (status==CMD_SUCCESS)
              status = ANSW_OK;
            else
              status = ANSW_ERROR;
          }
        }
      }

      SERIAL_SendChar(status);
    }

    // Return software version.
    else if(val==CMD_VER)
    {
      SERIAL_SendChar('0');
      SERIAL_SendChar('1');
    }

    // Return signature bytes.
    else if(val==CMD_ID)
    {							
      SERIAL_SendChar(SIGNATURE_BYTE_1);
      SERIAL_SendChar(SIGNATURE_BYTE_2);
      SERIAL_SendChar(SIGNATURE_BYTE_3);
    }

    // Return device signature
    else if(val==CMD_DEV)
    {							
      SERIAL_SendChar(DEVICE_ID);
    }

    // Reset controller
    else if(val==CMD_RESET)
    {
      //calculate checksum and write it to flash
      address = CHECKSUM_START;
      crc32 = PROGRAMM_CalcChecksum();
      status = PROGRAMM_WriteFlash((void*)&crc32, (void*)address, sizeof(unsigned int));

      PROGRAMM_ExecuteCode();
    }
    
    // stop bootloader, execute user code
    else if(val==CMD_STOP)
    {
      PROGRAMM_ExecuteCode();
    }

    // Unknown command
    else
    {
      SERIAL_SendChar(ANSW_UNKNOWN);
    }
  }

  execute_user_code();

  enter_usb_isp();
}
