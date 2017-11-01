#ifndef _UTILS_H
#define _UTILS_H

#include "defines.h"

extern void UTILS_DecToHex(uint8_t data, char *s);
extern uint8_t UTILS_HexToDec(char *s);
extern TBool UTILS_StrToByte(uint8_t *Value, char *s);
extern uint8_t UTILS_StrToInt(int *Value, char *s);
extern char *UTILS_StrUpr(char *s);
extern char *UTILS_StrLwr(char *s);
extern int UTILS_StrPos(const char *string, char c);
extern const char *UTILS_Int2Bin(int Value, int Len);
extern uint16_t UTILS_Median3Filter(uint16_t a, uint16_t b, uint16_t c);
extern uint8_t UTILS_Bcd2Hex(uint8_t BCD);
extern uint8_t UTILS_Hex2Bcd(uint8_t Data);

#endif
