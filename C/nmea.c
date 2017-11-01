#include <string.h>
#include "nmea.h"
#include "utils.h"

//------------------------------------------------------------------------------
// NMEA_CalcCRC - calculate NMEA CRC summ
//------------------------------------------------------------------------------
TBool NMEA_CalcCRC(uint8_t *crc, uint8_t *data, uint16_t length)
{
  uint8_t crc_temp = NMEA_CRC_FILL;
  uint16_t n;
  
  if (!crc || !data || (length<4))
  {
    // Wrong attributes
    return false;
  }
  if (*data != '$')
  {
    // GPS data have to start with '$'
    return false;
  }

  for (n=1; n<length; n++)
  {
    if (data[n]=='*')
    {
      *crc = crc_temp;
      return true;
    }
    crc_temp ^= data[n];
  }
  
  return false;
}

//------------------------------------------------------------------------------
// NMEA_ParseRMC - parse NMEA RMC command
//------------------------------------------------------------------------------
signed char NMEA_ParseRMC(char *s, TDataPoint *Point)
{
  uint8_t tempVal;
  uint8_t len;
  int intVal;
  
  if (*s!=NMEA_SIGN_DELIM)
    return NMEA_PARSE_UNKNOWN;
  s++;
  
  if (!UTILS_StrToByte(&tempVal, s))
    return NMEA_PARSE_UNKNOWN;
  s += 2;
  Point->hour = tempVal;
  if (!UTILS_StrToByte(&tempVal, s))
    return NMEA_PARSE_UNKNOWN;
  s += 2;
	Point->minute = tempVal;
  if (!UTILS_StrToByte(&tempVal, s))
    return NMEA_PARSE_UNKNOWN;
  s += 2;
	Point->second = tempVal;
  
  s = strchr(s, NMEA_SIGN_DELIM);
  if (s==NULL)
    return NMEA_PARSE_UNKNOWN;
  s++;
  
  Point->valid = *s++;
  if (*s!=NMEA_SIGN_DELIM)
    return NMEA_PARSE_UNKNOWN;
  s++;
  
  if (!UTILS_StrToByte(&tempVal, s))
    return NMEA_PARSE_UNKNOWN;
  s += 2;
	Point->latitude_degree = tempVal;
  if (!UTILS_StrToByte(&tempVal, s))
    return NMEA_PARSE_UNKNOWN;
  s += 2;
	Point->latitude_minute = tempVal;
  if (*s++!=NMEA_SIGN_POINT)
    return NMEA_PARSE_UNKNOWN;
  len = UTILS_StrToInt(&intVal, s);
  if (len>0)
    Point->latitude_fraction = intVal;
  else
    Point->latitude_fraction = 0;
  s += len;
  
  if (*s!=NMEA_SIGN_DELIM)
    return NMEA_PARSE_UNKNOWN;
  s++;
  
  Point->NS = *s++;
  if (*s!=NMEA_SIGN_DELIM)
    return NMEA_PARSE_UNKNOWN;
  s++;

  s++;
  if (!UTILS_StrToByte(&tempVal, s))
    return NMEA_PARSE_UNKNOWN;
  s += 2;
	Point->longitude_degree = tempVal;
  if (!UTILS_StrToByte(&tempVal, s))
    return NMEA_PARSE_UNKNOWN;
  s += 2;
	Point->longitude_minute = tempVal;
  if (!UTILS_StrToByte(&tempVal, s))
    return NMEA_PARSE_UNKNOWN;
  s += 2;
	Point->longitude_fraction = tempVal;
  
  if (*s!=NMEA_SIGN_DELIM)
    return NMEA_PARSE_UNKNOWN;
  s++;

  Point->EW = *s++;

  return NMEA_PARSE_OK;
}

//------------------------------------------------------------------------------
// NMEA_ParseGGA - parse NMEA GGA command
//------------------------------------------------------------------------------
signed char NMEA_ParseGGA(char *s, TDataPoint *Point)
{
  //uint8_t tempVal;
  
  if (*s!=NMEA_SIGN_DELIM)
    return NMEA_PARSE_UNKNOWN;
  s++;

  return NMEA_PARSE_OK;
}

//------------------------------------------------------------------------------
// NMEA_ParseGLL - parse NMEA GLL command
//------------------------------------------------------------------------------
signed char NMEA_ParseGLL(char *s, TDataPoint *Point)
{
  //uint8_t tempVal;
  
  if (*s!=NMEA_SIGN_DELIM)
    return NMEA_PARSE_UNKNOWN;
  s++;

  return NMEA_PARSE_OK;
}
