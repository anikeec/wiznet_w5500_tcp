
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
//parseServerAnswer
/*------------------------------------------------------*/
int8_t	parseServerAnswer(uint8_t* dataBuffer, uint16_t dataAmount) {
	AccessPacket accessPacket;
	InfoPacket infoPacket;
	ServicePacket servicePacket;
	cJSON *messageTypeJson = NULL;
	cJSON *deviceNumberJson = NULL;
	cJSON *packetNumberJson = NULL;	
	int compare = 0;
	char	messageType[MESSAGE_TYPE_LENGTH];
	int deviceNumber = 0;
	int packetNumber = 0;
	
	cJSON *json_result = cJSON_Parse((char*)dataBuffer);
	if(json_result == NULL) {
		return FALSE;
	}
	
	messageTypeJson = cJSON_GetObjectItemCaseSensitive(json_result, "mt");
	if (cJSON_IsString(messageTypeJson) && (messageTypeJson->valuestring != NULL)) {
		memset(messageType, NULL, MESSAGE_TYPE_LENGTH);
    strlcpy(messageType, messageTypeJson->valuestring, strlen(messageTypeJson->valuestring) + 1);
  } else {
		return FALSE;
	}
	
	deviceNumberJson = cJSON_GetObjectItemCaseSensitive(json_result, "dn");
	if (cJSON_IsNumber(deviceNumberJson) && (deviceNumberJson->valueint != NULL)) {
		deviceNumber = deviceNumberJson->valueint;
  } else {
		return FALSE;
	}
	
	packetNumberJson = cJSON_GetObjectItemCaseSensitive(json_result, "pn");
	if (cJSON_IsNumber(packetNumberJson) && (packetNumberJson->valueint != NULL)) {
		packetNumber = packetNumberJson->valueint;
  } else {
		return FALSE;
	}
	
	compare = strcmp(messageType,ACCESS_MESSAGE_TYPE);
	if(compare == 0) {
		strlcpy(accessPacket.messageType, messageType, strlen(accessPacket.messageType));
		accessPacket.deviceNumber = deviceNumber;
		accessPacket.packetNumber = packetNumber;
		return parseAccessPacket(json_result, &accessPacket);
	}
	
	compare = strcmp(messageType,INFO_MESSAGE_TYPE);
	if(compare == 0) {
		strlcpy(infoPacket.messageType, messageType, strlen(accessPacket.messageType));
		infoPacket.deviceNumber = deviceNumber;
		infoPacket.packetNumber = packetNumber;
		return parseInfoPacket(json_result, &infoPacket);
	}
	
	compare = strcmp(messageType,SERVICE_MESSAGE_TYPE);
	if(compare == 0) {
		strlcpy(servicePacket.messageType, messageType, strlen(accessPacket.messageType));
		servicePacket.deviceNumber = deviceNumber;
		servicePacket.packetNumber = packetNumber;
		return parseServicePacket(json_result, &servicePacket);
	}
	
	return 0;
}

/*------------------------------------------------------*/
//parseAccessPacket
//{"mt":"ACCESS","ei":1,"dn":11,"pn":2,"t":"Oct 3, 2018 11:44:08 AM"}
/*------------------------------------------------------*/
int8_t	parseAccessPacket(cJSON *json, AccessPacket* accessPacket) {
	cJSON *eventIdJson = NULL;	
	int eventId = 0;	
		
	eventIdJson = cJSON_GetObjectItemCaseSensitive(json, "ei");
	if (cJSON_IsNumber(eventIdJson) && (eventIdJson->valueint != NULL)) {
		accessPacket->eventId = eventIdJson->valueint;
  } else {
		return FALSE;
	}	
	
	return 1;
}

/*------------------------------------------------------*/
//parseInfoPacket
/*------------------------------------------------------*/
int8_t	parseInfoPacket(cJSON *json, InfoPacket* infoPacket) {
	return 2;
}

/*------------------------------------------------------*/
//parseServicePacket
/*------------------------------------------------------*/
int8_t	parseServicePacket(cJSON *json, ServicePacket* servicePacket) {
	return 3;
}

/*------------------------------------------------------*/
//jsonParserTest
/*------------------------------------------------------*/
void jsonParserTest(void) {
	char *JSON_STRING = 
		"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
		"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";
		//"{\"JSON Test Pattern pass3\":{\"The outermost value\":\"must be an object or array.\",\"In this test\":\"It is an object.\"}}";
	int i;
	//char string[100];
	char* str;
	cJSON *user = NULL;		
	cJSON *groups = NULL;
	cJSON *group = NULL;

	cJSON *json_result = cJSON_Parse(JSON_STRING);
	//if(json_result == NULL) {
	//	return;
	//}
	user = cJSON_GetObjectItemCaseSensitive(json_result, "user");
	if (cJSON_IsString(user) && (user->valuestring != NULL)) {
		str = user->valuestring;
    //printf("Checking \"%s\"\n", user->valuestring);
  }
	
	groups = cJSON_GetObjectItemCaseSensitive(json_result, "groups");
	int size = cJSON_GetArraySize(groups);
	for(i=0;i<size;i++) {
		group = cJSON_GetArrayItem(groups, i);
		if (cJSON_IsString(group) && (group->valuestring != NULL)) {
			str = group->valuestring;
		}
	}
	
	cJSON_Delete(json_result);
	
}
