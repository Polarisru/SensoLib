#ifndef _NMEA_H
#define _NMEA_H

#include "defines.h"

#define NMEA_CRC_FILL   0x00

#define NMEA_SIGN_START '$'
#define NMEA_SIGN_END   '*'
#define NMEA_SIGN_DELIM ','
#define NMEA_SIGN_POINT '.'

typedef struct
{
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char date_day;
	unsigned char date_month;
	unsigned char date_year;

	unsigned char latitude_degree;
	unsigned char latitude_minute;
	unsigned int latitude_fraction;
	unsigned char NS;

	unsigned char longitude_degree;
	unsigned char longitude_minute;
	unsigned int longitude_fraction;
	unsigned char EW;

	//unsigned short velocity_i;
	//unsigned char velocity_f;	// 00-99 (только сотые)
	
	//unsigned short angle_i;
	//unsigned char angle_f;	// 00-99 (только сотые)

	//unsigned char satn;

	//signed short hdop_i;
	//unsigned char hdop_f;

	//signed short altitude_i;
	//unsigned char altitude_f;

	//unsigned short magnetic_declination_i;
	//unsigned char magnetic_declination_f;	// 00-99 (только сотые)
	
	//unsigned char MEW;

	unsigned char valid;
	//unsigned char valid2;
} TDataPoint;

#define PARSER_RMC_START			  0
#define PARSER_RMC_DOLL			  	1
#define PARSER_RMC_G				    2		
#define PARSER_RMC_P				    3
#define PARSER_RMC_R				    4
#define PARSER_RMC_M				    5
#define PARSER_RMC_C				    6
#define PARSER_RMC_COMMA1			  7
#define PARSER_RMC_TIMEHH0			8
#define PARSER_RMC_TIMEHH1			9
#define PARSER_RMC_TIMEMM0			10
#define PARSER_RMC_TIMEMM1			11
#define PARSER_RMC_TIMESS0			12
#define PARSER_RMC_TIMESS1			13
#define PARSER_RMC_THOUS			  14
#define PARSER_RMC_COMMA2			  15
#define PARSER_RMC_STATUS			  16
#define PARSER_RMC_COMMA3			  17
#define PARSER_RMC_LATITUDE0		18
#define PARSER_RMC_LATITUDE1		19
#define PARSER_RMC_LATITUDE2		20
#define PARSER_RMC_LATITUDE3		21
#define PARSER_RMC_LATITUDEDOT	22
#define PARSER_RMC_LATITUDE4		23
#define PARSER_RMC_LATITUDE5		24
#define PARSER_RMC_LATITUDE6		25
#define PARSER_RMC_LATITUDE7		26
#define PARSER_RMC_COMMA4			  27
#define PARSER_RMC_NS				    28
#define PARSER_RMC_COMMA5			  29
#define PARSER_RMC_LONGITUDE0		30
#define PARSER_RMC_LONGITUDE1		31
#define PARSER_RMC_LONGITUDE2		32
#define PARSER_RMC_LONGITUDE3		33
#define PARSER_RMC_LONGITUDE4		34
#define PARSER_RMC_LONGITUDEDOT	35
#define PARSER_RMC_LONGITUDE5		36
#define PARSER_RMC_LONGITUDE6		37
#define PARSER_RMC_LONGITUDE7		38
#define PARSER_RMC_LONGITUDE8		39
#define PARSER_RMC_COMMA6			  40
#define PARSER_RMC_EW				    41
#define PARSER_RMC_COMMA7			  42
#define PARSER_RMC_VELOCITY0		43
#define PARSER_RMC_VELOCITYDOT	44
#define PARSER_RMC_VELOCITY1		45
#define PARSER_RMC_VELOCITY2		46
#define PARSER_RMC_COMMA8			  47
#define PARSER_RMC_ANGLE0			  48
#define PARSER_RMC_ANGLEDOT			49
#define PARSER_RMC_ANGLE1			  50
#define PARSER_RMC_ANGLE2			  51
#define PARSER_RMC_COMMA9			  52
#define PARSER_RMC_DATEDAY0			53
#define PARSER_RMC_DATEDAY1			54
#define PARSER_RMC_DATEYEAR0		55
#define PARSER_RMC_DATEYEAR1		56
#define PARSER_RMC_DATEMONTH0		57
#define PARSER_RMC_DATEMONTH1		58
#define PARSER_RMC_COMMA10			59
#define PARSER_RMC_MD0				  60
#define PARSER_RMC_MDDOT			  61
#define PARSER_RMC_MD1				  62
#define PARSER_RMC_MD2				  63
#define PARSER_RMC_COMMA11			64
#define PARSER_RMC_MEW				  65
#define PARSER_RMC_COMMA12			66
#define PARSER_RMC_VALID2			  67
#define PARSER_RMC_STAR				  70
#define PARSER_RMC_CS0				  71
#define PARSER_RMC_CS1				  72
#define PARSER_RMC_EOL				  73

#define NMEA_PARSE_UNKNOWN			(-1)
#define NMEA_PARSE_OK			      (0)

extern TBool NMEA_CalcCRC(uint8_t *crc, uint8_t *data, uint16_t length);
extern signed char NMEA_ParseRMC(char *s, TDataPoint *Point);
extern signed char NMEA_ParseGGA(char *s, TDataPoint *Point);
extern signed char NMEA_ParseGLL(char *s, TDataPoint *Point);

#endif
