/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t UART1_SendData(uint8_t *pData, uint16_t len);
uint8_t UART1_SendStringAndWait(uint8_t *pData, uint16_t len);
//uint16_t UART1_GetDataLength();
//uint16_t UART1_Read(uint8_t *buffer, uint16_t len);
uint8_t UART1_Receive(uint8_t *pData, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
