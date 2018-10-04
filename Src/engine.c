
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
													char* resultBuffer, 
													int resultBufferLength,
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
	int ret = TRUE;										
	
														
	cJSON *message = cJSON_CreateObject();													
	if (message == NULL) {
		ret = FALSE;
  }		
	
	if(ret != FALSE) {
		messageTypeJson = cJSON_CreateString("ACCESS");
		if (messageTypeJson == NULL) {
			ret = FALSE;
		}	else {
			cJSON_AddItemToObject(message, "mt", messageTypeJson);
		}
	}
	
	if(ret != FALSE) {
		deviceNumberJson = cJSON_CreateNumber(deviceNumber);
		if (deviceNumberJson == NULL) {
			ret = FALSE;
		}	else {
			cJSON_AddItemToObject(message, "dn", deviceNumberJson);
		}
	}
	
	if(ret != FALSE) {
		packetNumberJson = cJSON_CreateNumber(packetNumber);
		if (packetNumberJson == NULL) {
			ret = FALSE;
		}	else {
			cJSON_AddItemToObject(message, "pn", packetNumberJson);
		}
	}
	
	if(ret != FALSE) {
		cardNumberJson = cJSON_CreateString(cardNumber);
		if (cardNumberJson == NULL) {
			ret = FALSE;
		}	else {
			cJSON_AddItemToObject(message, "cn", cardNumberJson);
		}
	}
	
	if(ret != FALSE) {
		eventTypeJson = cJSON_CreateString(eventType);
		if (eventTypeJson == NULL) {
			ret = FALSE;
		}	else {
			cJSON_AddItemToObject(message, "et", eventTypeJson);
		}
	}
	
	if(ret != FALSE) {
		eventIdJson = cJSON_CreateNumber(eventId);
		if (eventIdJson == NULL) {
			ret = FALSE;
		}	else {
			cJSON_AddItemToObject(message, "ei", eventIdJson);
		}
	}
	
	if(ret != FALSE) {
		rtcGetDateTime(&dateTime);
		dateTimeBuffer = dateTimeToJsonDate(dateTime);
		dateJson = cJSON_CreateString(dateTimeBuffer);
		if (dateJson == NULL) {
			ret = FALSE;
		}	else {
			cJSON_AddItemToObject(message, "t", dateJson);
		}
	}
	
	if(ret != FALSE) {
		ret = cJSON_PrintPreallocated(message, resultBuffer, resultBufferLength, 0);
		/*
		if (string == NULL) {
			ret = FALSE;
		} else {
			*result = string;
		}
		*/
	}
	
	cJSON_Delete(message);
	
	return ret;
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

