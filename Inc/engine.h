/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENGINE_H
#define __ENGINE_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
	 
/* Definitions----------------------------------------------------------------*/
#define ACCESS_MESSAGE_TYPE			"ACCESS"
#define INFO_MESSAGE_TYPE				"INFO"
#define SERVICE_MESSAGE_TYPE		"SERVICE"	 
	 
#define MESSAGE_TYPE_LENGTH		10
#define CARD_NUMBER_LENGTH		10
#define EVENT_TYPE_LENGTH			10
	 
/* Exported types ------------------------------------------------------------*/
struct AccessPacket {
	char			messageType[MESSAGE_TYPE_LENGTH];
	uint32_t 	deviceNumber;
	uint32_t 	packetNumber;
	char			cardNumber[CARD_NUMBER_LENGTH];
	char 			eventType[EVENT_TYPE_LENGTH];
	uint32_t 	eventId;
};

struct InfoPacket {
	char			messageType[MESSAGE_TYPE_LENGTH];
	
};

struct ServicePacket {
	char			messageType[MESSAGE_TYPE_LENGTH];
	
};
	 
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void jsonParserTest(void);
int createAccessMessage(
													char** result, 
													int deviceNumber,
													int packetNumber,
													char* cardNumber,
													char* eventType,
													int eventId);

int8_t	parseServerAnswer(uint8_t* dataBuffer, uint16_t dataAmount);

#ifdef __cplusplus
}
#endif

#endif /* __ENGINE_H */
