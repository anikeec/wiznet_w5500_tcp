
/* Includes ------------------------------------------------------------------*/
#include "definitions.h"
#include "main.h"
#include "utils.h"
#include <string.h>

/* Definitions----------------------------------------------------------------*/
#define DATE_TIME_SUF_SIZE	27

/* External variables --------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
char dateTimeBuffer[DATE_TIME_SUF_SIZE];


void insert2NumberToBuffer(uint8_t number, char* buffer);

/* Functions -----------------------------------------------------------------*/

/*------------------------------------------------------*/
//dateTimeToJsonDate
//example return: "Sep 30, 2018 10:21:13 AM"
//Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
/*------------------------------------------------------*/
char* dateTimeToJsonDate(DateTime dateTime) {
	char* monthJson;
	uint16_t bufPtr = 0;
	memset(dateTimeBuffer, NULL, DATE_TIME_SUF_SIZE);

	//month
	switch(dateTime.month) {
		case 0x01:
						monthJson = "Jan";
						break;
		case 0x02:
						monthJson = "Feb";
						break;
		case 0x03:
						monthJson = "Mar";
						break;
		case 0x04:
						monthJson = "Apr";
						break;
		case 0x05:
						monthJson = "May";
						break;
		case 0x06:
						monthJson = "Jun";
						break;
		case 0x07:
						monthJson = "Jul";
						break;
		case 0x08:
						monthJson = "Aug";
						break;
		case 0x09:
						monthJson = "Sep";
						break;
		case 0x10:
						monthJson = "Oct";
						break;
		case 0x11:
						monthJson = "Nov";
						break;
		case 0x12:
						monthJson = "Dec";
						break;
		default:
						monthJson = "XXX";
						break;
	}
	dateTimeBuffer[bufPtr++] = monthJson[0];
	dateTimeBuffer[bufPtr++] = monthJson[1];
	dateTimeBuffer[bufPtr++] = monthJson[2];
	
	dateTimeBuffer[bufPtr++] = ' ';
	
	insert2NumberToBuffer(dateTime.day, &dateTimeBuffer[bufPtr]);
	bufPtr += 2;
	
	dateTimeBuffer[bufPtr++] = ',';
	dateTimeBuffer[bufPtr++] = ' ';
	
	insert2NumberToBuffer(20, &dateTimeBuffer[bufPtr]);
	bufPtr += 2;
	
	insert2NumberToBuffer(dateTime.year, &dateTimeBuffer[bufPtr]);
	bufPtr += 2;
	
	dateTimeBuffer[bufPtr++] = ' ';
	
	insert2NumberToBuffer(dateTime.hours, &dateTimeBuffer[bufPtr]);
	bufPtr += 2;
	
	dateTimeBuffer[bufPtr++] = ':';
	
	insert2NumberToBuffer(dateTime.minutes, &dateTimeBuffer[bufPtr]);
	bufPtr += 2;
	
	dateTimeBuffer[bufPtr++] = ':';
	
	insert2NumberToBuffer(dateTime.seconds, &dateTimeBuffer[bufPtr]);
	bufPtr += 2;
	
	dateTimeBuffer[bufPtr++] = ' ';
	
	dateTimeBuffer[bufPtr++] = 'A';
	dateTimeBuffer[bufPtr++] = 'M';
														
	return dateTimeBuffer;
}

/*------------------------------------------------------*/
//insert2NumberToBuffer
//input: 0..99
/*------------------------------------------------------*/
void insert2NumberToBuffer(uint8_t number, char* buffer) {
	char tempBuffer[3];
	if(number > 99)
		number = number%100;
	uint8ToChars(number, (uint8_t*)tempBuffer);
	buffer[0] = '0' + tempBuffer[1];
	buffer[1] = '0' + tempBuffer[0];
}

/*------------------------------------------------------*/
//uint8ToChars
//min input buffer length: 3
/*------------------------------------------------------*/
void uint8ToChars(uint8_t temp_var, uint8_t *temp_chars)
{
	uint8_t	i = 0, mult = 0;
	
	mult = 100;
	for(i=3;i>0;i--)
	{		
		temp_chars[i-1] = 0;
		while(temp_var>=mult)
		{
			temp_var = temp_var - mult;
			temp_chars[i-1]++;
		};
		mult = mult/10;
	}	
}

/*------------------------------------------------------*/
//uint16ToChars
//min input buffer length: 5
/*------------------------------------------------------*/
void uint16ToChars(uint16_t temp_var, uint8_t *temp_chars)
{
	uint16_t	i = 0, mult = 0;
	
	mult = 10000;
	for(i=5;i>0;i--)
	{		
		temp_chars[i-1] = 0;
		while(temp_var>=mult)
		{
			temp_var = temp_var - mult;
			temp_chars[i-1]++;
		};
		mult = mult/10;
	}	
}

/*------------------------------------------------------*/
//uint32ToChars
//min input buffer length: 10
/*------------------------------------------------------*/
void uint32ToChars(uint32_t temp_var, uint8_t *temp_chars)
{
	uint32_t	i = 0, mult = 0;
	
	mult = 1000000000;
	for(i=10;i>0;i--)
	{		
		temp_chars[i-1] = 0;
		while(temp_var>=mult)
		{
			temp_var = temp_var - mult;
			temp_chars[i-1]++;
		};
		mult = mult/10;
	}	
}

