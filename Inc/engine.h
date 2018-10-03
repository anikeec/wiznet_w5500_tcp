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
	 
#define EVENT_TYPE_ACCESS_ALLOW		1
#define EVENT_TYPE_ACCESS_DENIED	2
#define EVENT_TYPE_ENTER_QUERY		3
#define EVENT_TYPE_EXIT_QUERY			4
	 
#define MESSAGE_TYPE_LENGTH		10
#define CARD_NUMBER_LENGTH		10
#define EVENT_TYPE_LENGTH			10
	 
/* Exported types ------------------------------------------------------------*/
struct AccessPacketType {
	char			messageType[MESSAGE_TYPE_LENGTH];
	uint32_t 	deviceNumber;
	uint32_t 	packetNumber;
	char			cardNumber[CARD_NUMBER_LENGTH];
	char 			eventType[EVENT_TYPE_LENGTH];
	uint32_t 	eventId;
};

typedef struct AccessPacketType	AccessPacket;

struct InfoPacketType {
	char			messageType[MESSAGE_TYPE_LENGTH];
	uint32_t 	deviceNumber;
	uint32_t 	packetNumber;
	char			cardNumber[CARD_NUMBER_LENGTH];
	uint32_t	userId;
	char 			eventType[EVENT_TYPE_LENGTH];
	uint32_t 	eventId;
};

typedef struct InfoPacketType		InfoPacket;

struct ServicePacketType {
	char			messageType[MESSAGE_TYPE_LENGTH];
	uint32_t 	deviceNumber;
	uint32_t 	packetNumber;
	
};

typedef struct ServicePacketType	ServicePacket;
	 
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
void 		handleServerAnswer(int8_t receivedPacketType);

#ifdef __cplusplus
}
#endif

#endif /* __ENGINE_H */
