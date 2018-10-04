
/* Includes ------------------------------------------------------------------*/
#include "definitions.h"
#include "main.h"
#include "uart.h"
#include "stm32f1xx_hal.h"
#include "ringbuffer.h"

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* Public variables ----------------------------------------------------------*/

/* Private definitions--------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
uint8_t UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t len);

/* Private variables ---------------------------------------------------------*/
uint8_t	uartReady = FALSE;
uint8_t uartTxReady = TRUE;
RingBuffer uartTxBuf, uartRxBuf;
uint8_t uartTxData;
char readBuf[1];

/* Functions -----------------------------------------------------------------*/

/*------------------------------------------------------*/
//UART1_SendStringAndWait
/*------------------------------------------------------*/
uint8_t UART1_SendStringAndWait(uint8_t *pData, uint16_t len) {
	uint8_t strEnd[2] = {0x0A, 0x0D};
	UART_Transmit(&huart1, pData, len);
	while(uartTxReady == FALSE) {};
	UART_Transmit(&huart1, strEnd, 2);
	while(uartTxReady == FALSE) {};
	return TRUE;
}

/*------------------------------------------------------*/
//UART1_SendData
/*------------------------------------------------------*/
uint8_t UART1_SendData(uint8_t *pData, uint16_t len) {
	return UART_Transmit(&huart1, pData, len);
}

/*------------------------------------------------------*/
//UART_Transmit
/*------------------------------------------------------*/
uint8_t UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t len) {
  if(HAL_UART_Transmit_IT(huart, pData, len) != HAL_OK) {
    if(RingBuffer_Write(&uartTxBuf, pData, len) != RING_BUFFER_OK)
      return 0;
  }
	uartTxReady = FALSE;
  return 1;
}

/*------------------------------------------------------*/
//HAL_UART_RxCpltCallback
//Rx Transfer completed callbacks.
/*------------------------------------------------------*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
	/* Set transmission flag: transfer complete*/
	uartReady = TRUE;
}

/*------------------------------------------------------*/
//HAL_UART_TxCpltCallback
//Tx Transfer completed callbacks
/*------------------------------------------------------*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if(RingBuffer_GetDataLength(&uartTxBuf) > 0) {
    RingBuffer_Read(&uartTxBuf, &uartTxData, 1);
    HAL_UART_Transmit_IT(huart, &uartTxData, 1);
  } else {
		uartTxReady = TRUE;
	}
}

/*------------------------------------------------------*/
//UART_Transmit
/*------------------------------------------------------*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  if(huart->ErrorCode == HAL_UART_ERROR_ORE)
    HAL_UART_Receive_IT(huart, (uint8_t *)readBuf, 1);
}
