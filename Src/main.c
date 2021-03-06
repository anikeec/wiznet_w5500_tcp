/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "definitions.h"
#include "main.h"
#include "network.h"
#include "hardware.h"
#include "engine.h"
#include "uart.h"
#include <string.h>

/* Public variables ----------------------------------------------------------*/
uint8_t gDATABUF[DATA_BUF_SIZE];
uint8_t tempBuffer[TEMP_BUF_SIZE];
char buffer[MESSAGE_MAX_LENGTH];
char messageBuffer[MESSAGE_MAX_LENGTH];
char cardNumber[CARD_BUFFER_LENGTH];

/* Main ----------------------------------------------------------------------*/
int main(void)
{		
	char* message = messageBuffer;
	//int* messageLength;
	uint8_t tempBuffer[20];
	int length = 0;
	int8_t	result = 0;
	
	uint8_t retValue = 0;
	int16_t receivedResult = 0;	
	uint8_t addr[4] = {NW_SERVER_ADDR0, NW_SERVER_ADDR1, NW_SERVER_ADDR2, NW_SERVER_ADDR3};
	extern uint8_t uartTxReady;

	hardwareInit();
	
	networkInit();
		
  while (1)
  {
			length = 0;
		/*
			do{
				length = UART1_GetDataLength();
			} while(length < 5);
			UART1_Read((uint8_t*)cardNumber, length);
		*/
			UART1_Receive((uint8_t*)tempBuffer, CARD_EMMARINE_NUMBER_LENGTH);
			hexBufferToSymbolBuffer((uint8_t*)tempBuffer, CARD_EMMARINE_NUMBER_LENGTH, (uint8_t*)cardNumber);
		
			createAccessMessage(message,MESSAGE_MAX_LENGTH,11,2,(char*)cardNumber,"ENTER_QUERY",3);
			length = strlen(message);
			memset(buffer,NULL,MESSAGE_MAX_LENGTH);
			strlcpy(buffer,message,length+1);
			buffer[length++] = 0x0D;
			buffer[length++] = 0x0A;
		
			UART1_SendData((uint8_t*)buffer, length);
			while(uartTxReady == FALSE) {};
		
			retValue = networkTcpConnect(NETWORK_SOCKET, addr, NW_SERVER_PORT);
			if(retValue == TRUE) {
				networkTcpSend(NETWORK_SOCKET,(uint8_t*)buffer,length);
			}
			
			//memset(gDATABUF, NULL, DATA_BUF_SIZE);
			receivedResult = receiveServerAnswer(NETWORK_SOCKET, gDATABUF, DATA_BUF_SIZE, NW_SERVER_PORT);
			if(receivedResult > 0) {
				//we can parse result in the gDATABUF
				UART1_SendData(gDATABUF, receivedResult);
				while(uartTxReady == FALSE) {};
				result = parseServerAnswer(gDATABUF, receivedResult);
				handleServerAnswer(result);
			}				
			
			networkTcpDisconnect(NETWORK_SOCKET);
  }

}

/* Functions -----------------------------------------------------------------*/



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
