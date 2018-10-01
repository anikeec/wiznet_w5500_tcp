/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
/* Definitions ---------------------------------------------------------------*/	
#define _WIZCHIP_ 5500

#define SOCK_TCPS        0
#define SOCK_UDPS        1

#define DATA_BUF_SIZE   2048
#define TEMP_BUF_SIZE 	10

#define MESSAGE_MAX_LENGTH	100

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32f1xx_hal.h"
#include "./../Ethernet/socket.h"	// Just include one header for WIZCHIP
#include "./../Json/cJSON.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

GPIO_TypeDef* SPI_RST_PORT = GPIOA;
uint16_t SPI_RST_PIN = GPIO_PIN_2;
GPIO_TypeDef* SPI_INT_PORT = GPIOA;
uint16_t SPI_INT_PIN = GPIO_PIN_3;
GPIO_TypeDef* SPI_CS_PORT = GPIOA;
uint16_t SPI_CS_PIN = GPIO_PIN_4;
GPIO_TypeDef* SPI_SCK_PORT = GPIOA;
uint16_t SPI_SCK_PIN = GPIO_PIN_5;

uint8_t gDATABUF[DATA_BUF_SIZE];
uint8_t tempBuffer[TEMP_BUF_SIZE];

wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 0, 224},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 0, 1},
                            .dns = {8,8,8,8},
                            .dhcp = NETINFO_STATIC };

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

void  wizchipReset(void);
void  wizchipUnreset(void);
void  wizchipSelect(void);
void  wizchipDeselect(void);
void  wizchipWriteByte(uint8_t wb);
uint8_t wizchipReadByte();
void network_init(void);															// Initialize Network information and display it
int32_t loopback_tcps(uint8_t, uint8_t*, uint16_t);		// Loopback TCP server
int32_t loopback_udps(uint8_t, uint8_t*, uint16_t);		// Loopback UDP server
void jsonParserTest(void);
int createAccessMessage(
													char** result, 
													int deviceNumber,
													int packetNumber,
													char* cardNumber,
													char* eventType,
													int eventId);
int16_t receiveServerAnswer(uint8_t socketNumber, uint8_t* buf, uint16_t bufSize, uint16_t port);
int32_t waitServerAnswer(uint8_t socketNumber, uint8_t* buf, uint16_t bufSize, uint16_t port);
uint8_t getSocketStatus(uint8_t socketNumber);
int8_t	parseServerAnswer(uint8_t* dataBuffer, uint16_t dataAmount);

/* Main ----------------------------------------------------------------------*/
int main(void)
{	
	uint8_t tmp;
  int32_t ret = 0;
  uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};	
		
	char buffer[MESSAGE_MAX_LENGTH];	
	char* message;
	int* messageLength;
	char* cardNumber = "123456";
	int length = 0;
/*		
		while(1) {
			//jsonParserTest();
			createAccessMessage(&message,messageLength,1,2,cardNumber,6,7,8);
			length = strlen(message);
			length += 1;
		}
*/		
		/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

		/* Configure the system clock */
  SystemClock_Config();

		/* USER CODE BEGIN SysInit */

		/* USER CODE END SysInit */

		/* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
	
	wizchipReset();
	wizchipUnreset();
	
		// First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP
		/* Critical section callback - No use in this example */
		//reg_wizchip_cris_cbfunc(0, 0);
		/* Chip selection call back */
	#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
			reg_wizchip_cs_cbfunc(wizchipSelect, wizchipDeselect);
	#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
			reg_wizchip_cs_cbfunc(wizchipSelect, wizchipSelect);  // CS must be tried with LOW.
	#else
		 #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
				#error "Unknown _WIZCHIP_IO_MODE_"
		 #else
				reg_wizchip_cs_cbfunc(wizchipSelect, wizchipDeselect);
		 #endif
	#endif
		/* SPI Read & Write callback function */
  reg_wizchip_spi_cbfunc(wizchipReadByte, wizchipWriteByte);

		/* WIZCHIP SOCKET Buffer initialize */
  if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
  {
     //printf("WIZCHIP Initialized fail.\r\n");
     while(1);
  }

		/* PHY link status check */
  do
  {
      if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1) {			 
			}
        //printf("Unknown PHY Link stauts.\r\n");
  }while(tmp == PHY_LINK_OFF);

		/* Network initialization */
  network_init();
	
	uint8_t addr[4] = {192,168,0,125};
	uint16_t port = 65530;
	uint8_t retValue = 0;
	int8_t receivedResult = 0;	
		
  while (1)
  {
		while(1) {
			createAccessMessage(&message,1,2,cardNumber,"EXIT_QUERY",8);
			length = strlen(message);
			memset(buffer,NULL,MESSAGE_MAX_LENGTH);
			strlcpy(buffer,message,length+1);
			buffer[length++] = 0x0D;
			buffer[length++] = 0x0A;
			retValue = socket(0, Sn_MR_TCP, port, 0);
			retValue = connect(0, addr, port);
			if(retValue == SOCK_OK) {
				send(0,(uint8_t*)buffer,length);
			}
			
			memset(gDATABUF, NULL, DATA_BUF_SIZE);
			receivedResult = receiveServerAnswer(SOCK_TCPS, gDATABUF, DATA_BUF_SIZE, port);
			if(receivedResult > 0) {
				//we can parse result in the gDATABUF
				parseServerAnswer(gDATABUF, receivedResult);
			}				
			disconnect(0);
		}
		
		while(1) {		
				/* TCP Client */
			retValue = socket(0, Sn_MR_TCP, port, 0);
			retValue = connect(0, addr, port);
			if(retValue == SOCK_OK) {
				send(0,addr,4);
			}
			disconnect(0);
			
		}
		
		/* Loopback Test */
    	// TCP server loopback test
    	if( (ret = loopback_tcps(SOCK_TCPS, gDATABUF, 5000)) < 0) {
			//printf("SOCKET ERROR : %ld\r\n", (long)ret);
		}
  }

}

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
	dataAmount++;
	
	return 0;
}
													
/*------------------------------------------------------*/
//getSocketStatus
/* 
 * return values:
 * @par Normal status
 * - @ref SOCK_CLOSED 		: Closed
 * - @ref SOCK_INIT   		: Initiate state
 * - @ref SOCK_LISTEN    	: Listen state
 * - @ref SOCK_ESTABLISHED 	: Success to connect
 * - @ref SOCK_CLOSE_WAIT   : Closing state
 * - @ref SOCK_UDP   		: UDP socket
 * - @ref SOCK_MACRAW  		: MAC raw mode socket
 *@par Temporary status during changing the status of Socket n.
 * - @ref SOCK_SYNSENT   	: This indicates Socket n sent the connect-request packet (SYN packet) to a peer.
 * - @ref SOCK_SYNRECV    	: It indicates Socket n successfully received the connect-request packet (SYN packet) from a peer.
 * - @ref SOCK_FIN_WAIT		: Connection state
 * - @ref SOCK_CLOSING		: Closing state
 * - @ref SOCK_TIME_WAIT	: Closing state
 * - @ref SOCK_LAST_ACK 	: Closing state
*/
/*------------------------------------------------------*/
uint8_t getSocketStatus(uint8_t socketNumber) {
	return getSn_SR(socketNumber);
}

/*------------------------------------------------------*/
//receiveServerAnswer
/*
 * ret values
 * -1 - some error
 * >0 - receive OK and return amount of received bytes
*/
/*------------------------------------------------------*/
int16_t receiveServerAnswer(uint8_t socketNumber, uint8_t* buf, uint16_t bufSize, uint16_t port) {
	int32_t receiveSize = 0;
	uint16_t bufferPosition = 0;
	uint8_t i = 0, previousSymbol = NULL, currentSymbol = NULL;
	
	memset(tempBuffer, NULL, TEMP_BUF_SIZE);
	while(1) {		
		receiveSize = waitServerAnswer(socketNumber, tempBuffer, TEMP_BUF_SIZE, port);
		if(receiveSize > 0) {
			for(i=0; i<receiveSize; i++) {
				previousSymbol = currentSymbol;
				currentSymbol = tempBuffer[i];
				buf[bufferPosition++] = currentSymbol;
				if((previousSymbol == 0x0D) && (currentSymbol == 0x0A)) {
					return bufferPosition;
				}
			}
			memset(tempBuffer, NULL, TEMP_BUF_SIZE);
		} else if(receiveSize < 0) {
			return -1;
		}
	}
}
	
/*------------------------------------------------------*/
//waitServerAnswer
/*------------------------------------------------------*/
int32_t waitServerAnswer(uint8_t socketNumber, uint8_t* buf, uint16_t bufSize, uint16_t port)
{
   int32_t ret;
   uint16_t size = 0, sentsize=0;
	 uint8_t retValue = getSocketStatus(socketNumber);
   switch(retValue)
   {
      case SOCK_ESTABLISHED:
         if(getSn_IR(socketNumber) & Sn_IR_CON)
         {
            //Connected
            setSn_IR(socketNumber,Sn_IR_CON);
						/* In this case, if the interrupt of Socket n is activated, interrupt occurs. Refer to IR, IMR
								Sn_IMR and Sn_IR. */
         }
         if((size = getSn_RX_RSR(socketNumber)) > 0)
         {
            if(size > bufSize) { 
							size = bufSize;
						}
            ret = recv(socketNumber,buf,size);
            if(ret <= 0) 
							return ret;
						/*
            sentsize = 0;
            while(size != sentsize)
            {
               ret = send(socketNumber,buf+sentsize,size-sentsize);
               if(ret < 0)
               {
                  close(socketNumber);
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
						*/
						return ret;
         }
         break;
      case SOCK_CLOSE_WAIT:
         //CloseWait
         if((ret=disconnect(socketNumber)) != SOCK_OK) return -1;
         //Closed
         break;
      case SOCK_INIT:
    	  //Listen, port, socketNumber, port
         if( (ret = listen(socketNumber)) != SOCK_OK) return -1;
         break;
      case SOCK_CLOSED:
         //LBTStart
         if((ret=socket(socketNumber,Sn_MR_TCP,port,0x00)) != socketNumber)
            return -1;
         //Opened
         break;
      default:
         break;
   }
   return 0;
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
	char string[100];
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

	/** System Clock Configuration */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 8;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

	/* SPI1 init function */
static void MX_SPI1_Init(void)
{
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_RST_PORT, SPI_RST_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(SPI_SCK_PORT, SPI_SCK_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RST*/
  GPIO_InitStruct.Pin = SPI_RST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI_RST_PORT, &GPIO_InitStruct);

  /*Configure GPIO pin : INT */
  GPIO_InitStruct.Pin = SPI_INT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI_INT_PORT, &GPIO_InitStruct);
	
	/*Configure GPIO pins : CS */
  GPIO_InitStruct.Pin = SPI_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI_CS_PORT, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	wizchipUnreset();
}

/////////////////////////////////////////////////////////////////
// SPI Callback function for accessing WIZCHIP                 //
// WIZCHIP user should implement with your host spi peripheral //
/////////////////////////////////////////////////////////////////
void  wizchipReset(void)
{
   HAL_GPIO_WritePin(SPI_CS_PORT, SPI_RST_PIN, GPIO_PIN_RESET);
}

void  wizchipUnreset(void)
{
   HAL_GPIO_WritePin(SPI_CS_PORT, SPI_RST_PIN, GPIO_PIN_SET);
}

void  wizchipSelect(void)
{
   HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET);
}

void  wizchipDeselect(void)
{
   HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET);
}

void  wizchipWriteByte(uint8_t wb)
{
	HAL_SPI_Transmit(&hspi1, &wb, 1, 5);
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){};
}

uint8_t wizchipReadByte()
{
	uint8_t buffer[5];
	HAL_SPI_Receive(&hspi1, buffer, 1, 5);
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){};
	return buffer[0];
}

/////////////////////////////////////////////////////////////
// Intialize the network information to be used in WIZCHIP //
/////////////////////////////////////////////////////////////
void network_init(void)
{
   uint8_t tmpstr[6];
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);/*
	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	printf("======================\r\n");*/
}
/////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// Loopback Test Example Code using ioLibrary_BSD			 //
///////////////////////////////////////////////////////////////
int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t ret;
   uint16_t size = 0, sentsize=0;
		uint8_t retValue = getSn_SR(sn);
   switch(retValue)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
            //printf("%d:Connected\r\n",sn);
            setSn_IR(sn,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(sn)) > 0)
         {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = recv(sn,buf,size);
            if(ret <= 0) return ret;
            sentsize = 0;
            while(size != sentsize)
            {
               ret = send(sn,buf+sentsize,size-sentsize);
               if(ret < 0)
               {
                  close(sn);
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
         }
         break;
      case SOCK_CLOSE_WAIT :
         //printf("%d:CloseWait\r\n",sn);
         if((ret=disconnect(sn)) != SOCK_OK) return ret;
         //printf("%d:Closed\r\n",sn);
         break;
      case SOCK_INIT :
    	  //printf("%d:Listen, port [%d]\r\n",sn, port);
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
         //printf("%d:LBTStart\r\n",sn);
         if((ret=socket(sn,Sn_MR_TCP,port,0x00)) != sn)
            return ret;
         //printf("%d:Opened\r\n",sn);
         break;
      default:
         break;
   }
   return 1;
}

int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint16_t destport;
   //uint8_t  packinfo = 0;
   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) > 0)
         {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = recvfrom(sn,buf,size,destip,(uint16_t*)&destport);
            if(ret <= 0)
            {
               //printf("%d: recvfrom error. %ld\r\n",sn,(long)ret);
               return ret;
            }
            size = (uint16_t) ret;
            sentsize = 0;
            while(sentsize != size)
            {
               ret = sendto(sn,buf+sentsize,size-sentsize,destip,destport);
               if(ret < 0)
               {
                  //printf("%d: sendto error. %ld\r\n",sn,(long)ret);
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
         }
         break;
      case SOCK_CLOSED:
         //printf("%d:LBUStart\r\n",sn);
         if((ret=socket(sn,Sn_MR_UDP,port,0x00)) != sn)
            return ret;
         //printf("%d:Opened, port [%d]\r\n",sn, port);
         break;
      default :
         break;
   }
   return 1;
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif
