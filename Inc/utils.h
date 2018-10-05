/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UTILS_H
#define __UTILS_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
	 
/* Definitions----------------------------------------------------------------*/

	 
/* Exported types ------------------------------------------------------------*/
struct DateTimeType {
	uint8_t		year;
	uint8_t 	month;
	uint8_t 	day;
	uint8_t		hours;
	uint8_t 	minutes;
	uint8_t 	seconds;
};

typedef struct DateTimeType	DateTime;
	 
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
char* dateTimeToJsonDate(DateTime dateTime);
void uint8ToChars(uint8_t temp_var, uint8_t *temp_chars);
void uint16ToChars(uint16_t temp_var, uint8_t *temp_chars);
void uint32ToChars(uint32_t temp_var, uint8_t *temp_chars);
void hexBufferToSymbolBuffer(uint8_t* hexBuffer, uint16_t len, uint8_t* symbBuffer);

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H */
