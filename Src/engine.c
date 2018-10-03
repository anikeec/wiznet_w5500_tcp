
/* Includes ------------------------------------------------------------------*/
#include "definitions.h"
#include "main.h"
#include "network.h"
#include "engine.h"
#include "hardware.h"
#include "utils.h"
#include "./../Json/cJSON.h"
#include <string.h>

/* External variables --------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
AccessPacket accessPacket;
InfoPacket infoPacket;
ServicePacket servicePacket;

/* Private definitions--------------------------------------------------------*/
#define ACCESS_MESSAGE			1
#define INFO_MESSAGE				2
#define	SERVICE_MESSAGE			3

/* Private functions ---------------------------------------------------------*/
int8_t	parseAccessPacket(cJSON *json, AccessPacket* accessPacket);
int8_t	parseInfoPacket(cJSON *json, InfoPacket* infoPacket);
int8_t	parseServicePacket(cJSON *json, ServicePacket* servicePacket);

/* Private variables ---------------------------------------------------------*/
DateTime dateTime;

/* Functions -----------------------------------------------------------------*/

/*------------------------------------------------------*/
//createAccessMessage
/*------------------------------------------------------*/
int createAccessMessage(
													char** result, 
													int deviceNumber,
													int packetNumber,
													char* cardNumber,
													char* eventType,
													int eventId) {
	char *string = NULL;
	cJSON *messageTypeJson = NULL;		
	cJSON *deviceNumberJson = NULL;													
	cJSON *packetNumberJson = NULL;	
	cJSON *cardNumberJson = NULL;
	cJSON *eventTypeJson = NULL;
	cJSON *eventIdJson = NULL;
	cJSON *dateJson = NULL;	
	char *dateTimeBuffer; 
														
	cJSON *message = cJSON_CreateObject();													
	if (message == NULL) {
		return 0;
  }		
	
	messageTypeJson = cJSON_CreateString("ACCESS");
	if (messageTypeJson == NULL) {
		return 0;
  }	
	cJSON_AddItemToObject(message, "mt", messageTypeJson);
	
	deviceNumberJson = cJSON_CreateNumber(deviceNumber);
	if (deviceNumberJson == NULL) {
		return 0;
  }	
	cJSON_AddItemToObject(message, "dn", deviceNumberJson);
	
	packetNumberJson = cJSON_CreateNumber(packetNumber);
	if (packetNumberJson == NULL) {
		return 0;
  }	
	cJSON_AddItemToObject(message, "pn", packetNumberJson);
	
	cardNumberJson = cJSON_CreateString(cardNumber);
	if (cardNumberJson == NULL) {
		return 0;
  }	
	cJSON_AddItemToObject(message, "cn", cardNumberJson);
	
	eventTypeJson = cJSON_CreateString(eventType);
	if (eventTypeJson == NULL) {
		return 0;
  }	
	cJSON_AddItemToObject(message, "et", eventTypeJson);
	
	eventIdJson = cJSON_CreateNumber(eventId);
	if (eventIdJson == NULL) {
		return 0;
  }	
	cJSON_AddItemToObject(message, "ei", eventIdJson);
	
	rtcGetDateTime(&dateTime);
	dateTimeBuffer = dateTimeToJsonDate(dateTime);
	dateJson = cJSON_CreateString(dateTimeBuffer);
	if (dateJson == NULL) {
		return 0;
  }	
	cJSON_AddItemToObject(message, "t", dateJson);
	
	string = cJSON_Print(message);
	if (string == NULL) {
		return 0;
  }
	
	*result = string;
	
	cJSON_Delete(message);
	
	return 1;
}	

/*------------------------------------------------------*/
//handleServerAnswer
/*------------------------------------------------------*/
void handleServerAnswer(int8_t receivedPacketType) {
	if(receivedPacketType == ACCESS_MESSAGE) {
		
	} else
	if(receivedPacketType == INFO_MESSAGE) {
		
	} else	
	if(receivedPacketType == SERVICE_MESSAGE) {
		
	} else {
		
	}
}

/*------------------------------------------------------*/
//parseServerAnswer
/*------------------------------------------------------*/
int8_t	parseServerAnswer(uint8_t* dataBuffer, uint16_t dataAmount) {	
	cJSON *messageTypeJson = NULL;
	cJSON *deviceNumberJson = NULL;
	cJSON *packetNumberJson = NULL;	
	int compare = 0;
	char	messageType[MESSAGE_TYPE_LENGTH];
	int deviceNumber = 0;
	int packetNumber = 0;
	int8_t retValue = TRUE;
	
	cJSON *json_result = cJSON_Parse((char*)dataBuffer);
	if(json_result == NULL) {
		retValue = FALSE;
	}
	
	if(retValue != FALSE) {
		messageTypeJson = cJSON_GetObjectItemCaseSensitive(json_result, "mt");
		if (cJSON_IsString(messageTypeJson) && (messageTypeJson->valuestring != NULL)) {
			memset(messageType, NULL, MESSAGE_TYPE_LENGTH);
			strlcpy(messageType, messageTypeJson->valuestring, strlen(messageTypeJson->valuestring) + 1);
		} else {
			retValue = FALSE;
		}
	}
	
	if(retValue != FALSE) {
		deviceNumberJson = cJSON_GetObjectItemCaseSensitive(json_result, "dn");
		if (cJSON_IsNumber(deviceNumberJson) && (deviceNumberJson->valueint != NULL)) {
			deviceNumber = deviceNumberJson->valueint;
		} else {
			retValue = FALSE;
		}
	}
	
	if(retValue != FALSE) {
		packetNumberJson = cJSON_GetObjectItemCaseSensitive(json_result, "pn");
		if (cJSON_IsNumber(packetNumberJson) && (packetNumberJson->valueint != NULL)) {
			packetNumber = packetNumberJson->valueint;
		} else {
			retValue = FALSE;
		}
	}
	
	if(retValue != FALSE) {
		compare = strcmp(messageType,ACCESS_MESSAGE_TYPE);
		if(compare == 0) {
			strlcpy(accessPacket.messageType, messageType, strlen(accessPacket.messageType));
			accessPacket.deviceNumber = deviceNumber;
			accessPacket.packetNumber = packetNumber;
			retValue = parseAccessPacket(json_result, &accessPacket);
		}
		
		compare = strcmp(messageType,INFO_MESSAGE_TYPE);
		if(compare == 0) {
			strlcpy(infoPacket.messageType, messageType, strlen(infoPacket.messageType));
			infoPacket.deviceNumber = deviceNumber;
			infoPacket.packetNumber = packetNumber;
			retValue = parseInfoPacket(json_result, &infoPacket);
		}

		compare = strcmp(messageType,SERVICE_MESSAGE_TYPE);
		if(compare == 0) {
			strlcpy(servicePacket.messageType, messageType, strlen(servicePacket.messageType));
			servicePacket.deviceNumber = deviceNumber;
			servicePacket.packetNumber = packetNumber;
			retValue = parseServicePacket(json_result, &servicePacket);
		}
	}
	
	cJSON_Delete(json_result);
	
	return retValue;
}

/*------------------------------------------------------*/
//parseAccessPacket
//{"mt":"ACCESS","ei":1,"dn":11,"pn":2,"t":"Oct 3, 2018 11:44:08 AM"}
/*------------------------------------------------------*/
int8_t	parseAccessPacket(cJSON *json, AccessPacket* accessPacket) {
	cJSON *eventIdJson = NULL;		
		
	eventIdJson = cJSON_GetObjectItemCaseSensitive(json, "ei");
	if (cJSON_IsNumber(eventIdJson) && (eventIdJson->valueint != NULL)) {
		accessPacket->eventId = eventIdJson->valueint;
  } else {
		return FALSE;
	}	
	
	return ACCESS_MESSAGE;
}

/*------------------------------------------------------*/
//parseInfoPacket
/*------------------------------------------------------*/
int8_t	parseInfoPacket(cJSON *json, InfoPacket* infoPacket) {
	cJSON *cardNumberJson = NULL;
	cJSON *userIdJson = NULL;
	cJSON *eventTypeJson = NULL;
	cJSON *eventIdJson = NULL;
	
	cardNumberJson = cJSON_GetObjectItemCaseSensitive(json, "cn");
	if (cJSON_IsString(cardNumberJson) && (cardNumberJson->valuestring != NULL)) {
    strlcpy(infoPacket->cardNumber, cardNumberJson->valuestring, strlen(infoPacket->cardNumber));
  }
	
	userIdJson = cJSON_GetObjectItemCaseSensitive(json, "ui");
	if (cJSON_IsNumber(userIdJson) && (userIdJson->valueint != NULL)) {
    infoPacket->userId = userIdJson->valueint;
  }
	
	eventTypeJson = cJSON_GetObjectItemCaseSensitive(json, "et");
	if (cJSON_IsString(eventTypeJson) && (eventTypeJson->valuestring != NULL)) {
    strlcpy(infoPacket->eventType, eventTypeJson->valuestring, strlen(infoPacket->eventType));
  }
	
	eventIdJson = cJSON_GetObjectItemCaseSensitive(json, "ei");
	if (cJSON_IsNumber(eventIdJson) && (eventIdJson->valueint != NULL)) {
    infoPacket->eventId = eventIdJson->valueint;
  }
	
	return INFO_MESSAGE;
}

/*------------------------------------------------------*/
//parseServicePacket
/*------------------------------------------------------*/
int8_t	parseServicePacket(cJSON *json, ServicePacket* servicePacket) {
	return SERVICE_MESSAGE;
}

