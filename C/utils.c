#include <ctype.h>
#include <string.h>
#include "utils.h"

//------------------------------------------------------------------------------
// UTILS_DecToHex - convert byte to HEX string
//------------------------------------------------------------------------------
void UTILS_DecToHex(uint8_t data, char *s)
{
  uint8_t temp;
  
  temp = (data&0xF0)>>4;
  if (temp<9)
   *s  = 0x30 + temp;
  else
   *s = 0x41 + temp - 10;
  s++;
  
  temp = data&0x0F;
  if (temp<9)
   *s = 0x30 + temp;
  else
   *s = 0x41 + temp - 10;
}

//------------------------------------------------------------------------------
uint8_t toint_new(uint8_t datab)
{
  uint8_t input=0;

  if ((datab>=0x30)&&(datab<=0x39))
    input = datab - 0x30;
  else
  if ((datab>=0x41)&&(datab<=0x46))
    input = datab - 0x37;
  else
  if ((datab>=0x61)&&(datab<=0x66))
    input = datab - 0x57;
  
  return input;
}

//------------------------------------------------------------------------------
// UTILS_HexToDec - convert HEX string to dec value
//------------------------------------------------------------------------------
uint8_t UTILS_HexToDec(char *s)
{
  uint8_t retval;
  uint8_t byteH, byteL;

  // get a two-byte ASCII representation of a char from the UART
  byteH = *s++;
  byteL = *s++;

  // convert to a single 8 bit result
  retval = (uint8_t) toint_new(byteH) * 16;
  retval += (uint8_t) toint_new(byteL);
  
  return retval;
}

//------------------------------------------------------------------------------
// UTILS_StrToByte - convert string to byte value
//------------------------------------------------------------------------------
TBool UTILS_StrToByte(uint8_t *Value, char *s)
{
  uint8_t tempVal = 0;
  int i;
  
  for (i=0; i<2; i++)
  {
    tempVal *= 10;
    if (*s==0)
      return false;
    if ((*s<0x30)||(*s>0x39))
      return false;
    tempVal += *s - 0x30;
    s++;
  }
  
  *Value = tempVal;
  
  return true;
}

//------------------------------------------------------------------------------
// UTILS_StrToInt - convert string to int value
//------------------------------------------------------------------------------
uint8_t UTILS_StrToInt(int *Value, char *s)
{
  int tempVal = 0;
  int i;

  for (i=0; i<6; i++)
  {
    tempVal *= 10;
    if (*s==0)
      break;
    if (*s==',')
      break;
    if ((*s<0x30)||(*s>0x39))
      break;
    tempVal += *s - 0x30;
    s++;
  }
  
  *Value = tempVal;
  
  return (uint8_t)i;
}

//------------------------------------------------------------------------------
// UTILS_StrUpr - convert string to upper case
//------------------------------------------------------------------------------
char *UTILS_StrUpr(char *s)
{
  char *str=s;

  while (*str)
  {
    *str = toupper(*str);
    str++;
  }
  
  return s;
}

//------------------------------------------------------------------------------
// UTILS_StrLwr - convert string to lower case
//------------------------------------------------------------------------------
char *UTILS_StrLwr(char *s)
{
  char *str=s;

  while (*str)
  {
    *str = tolower(*str);
    str++;
  }
  
  return s;
}

//------------------------------------------------------------------------------
// UTILS_StrPos - find char position in the string
//------------------------------------------------------------------------------
int UTILS_StrPos(const char *string, char c)
{
  int i;

  i = strlen(string);
  for (;i>=0;i--)
  {
    if (string[i]==c)
      return i;
  }
  
  return -1;
}

//------------------------------------------------------------------------------
// UTILS_Int2Bin - make string value for the binary number
//------------------------------------------------------------------------------
const char *UTILS_Int2Bin(int Value, int Len)
{
	static char b[32];
  int z, i;
  
  b[0] = '\0';
  z = 1;
  
  //for (i=1; i<Len; i++)
  //  z <<= 1;
  
  //for (; z>0; z >>= 1)
  for (i=0; i<Len; i++)
  {
    strcat(b, ((Value & z) == z) ? "1" : "0");
    z <<= 1;
  }

  return b;
}

//------------------------------------------------------------------------------
// UTILS_Median3Filter - return median value for 3 values
//------------------------------------------------------------------------------
uint16_t UTILS_Median3Filter(uint16_t a, uint16_t b, uint16_t c)
{
  int Temp = 0;
  
  if (a>b)
    Temp += 4;
  if (a>c)
    Temp += 2;
  if (b>c)
    Temp += 1;
  
  switch (Temp)
  {
    case 0:
    case 7:
      return b;
    case 3:
    case 4:
      return a;
    default:
      return c;
  }
}

//------------------------------------------------------------------------------
// UTILS_Bcd2Hex - BCD -> HEX
//------------------------------------------------------------------------------
uint8_t UTILS_Bcd2Hex(uint8_t BCD)
{
  uint8_t HEX;

  HEX=0;
  if (BCD&0x01)
    HEX+=1;
  if (BCD&0x02)
    HEX+=2;
  if (BCD&0x04)
    HEX+=4;
  if (BCD&0x08)
    HEX+=8;
  if (BCD&0x10)
    HEX+=10;
  if (BCD&0x20)
    HEX+=20;
  if (BCD&0x40)
    HEX+=40;
  if (BCD&0x80)
    HEX+=80;
  return HEX;
}

//------------------------------------------------------------------------------
// UTILS_Hex2Bcd - BCD -> HEX
//----------------------------------------------------------------------------
uint8_t UTILS_Hex2Bcd(uint8_t Data)
{
  return ((Data/10)<<4)+Data%10;
}
