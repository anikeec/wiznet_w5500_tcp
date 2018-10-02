
/* Includes ------------------------------------------------------------------*/
#include "definitions.h"
#include "main.h"
#include "network.h"
#include "engine.h"
#include "./../Json/cJSON.h"
#include <string.h>

/* External variables --------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
int8_t	parseAccessPacket(struct AccessPacket* accessPacket);
int8_t	parseInfoPacket(struct InfoPacket* infoPacket);
int8_t	parseServicePacket(struct ServicePacket* servicePacket);

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
	struct AccessPacket accessPacket;
	struct InfoPacket infoPacket;
	struct ServicePacket servicePacket;
	cJSON *messageTypeJson = NULL;	
	int compare = 0;
	char	messageType[MESSAGE_TYPE_LENGTH];
	
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
	
	compare = strcmp(messageType,ACCESS_MESSAGE_TYPE);
	if(compare == 0) {
		return parseAccessPacket(&accessPacket);
	}
	
	compare = strcmp(messageType,INFO_MESSAGE_TYPE);
	if(compare == 0) {
		return parseInfoPacket(&infoPacket);
	}
	
	compare = strcmp(messageType,SERVICE_MESSAGE_TYPE);
	if(compare == 0) {
		return parseServicePacket(&servicePacket);
	}
	
	return 0;
}

/*------------------------------------------------------*/
//parseAccessPacket
/*------------------------------------------------------*/
int8_t	parseAccessPacket(struct AccessPacket* accessPacket) {
	return 1;
}

/*------------------------------------------------------*/
//parseInfoPacket
/*------------------------------------------------------*/
int8_t	parseInfoPacket(struct InfoPacket* infoPacket) {
	return 2;
}

/*------------------------------------------------------*/
//parseServicePacket
/*------------------------------------------------------*/
int8_t	parseServicePacket(struct ServicePacket* servicePacket) {
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
