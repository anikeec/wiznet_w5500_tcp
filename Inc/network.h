/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETWORK_H
#define __NETWORK_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void networkInit(void);
int16_t receiveServerAnswer(uint8_t socketNumber, uint8_t* buf, uint16_t bufSize, uint16_t port);
int32_t waitServerAnswer(uint8_t socketNumber, uint8_t* buf, uint16_t bufSize, uint16_t port);
uint8_t getSocketStatus(uint8_t socketNumber);
uint8_t networkTcpConnect(uint8_t socketNumber, uint8_t* addr, uint16_t port);
uint8_t networkTcpDisconnect(uint8_t socketNumber);
uint8_t networkTcpSend(uint8_t socketNumber, uint8_t* buffer, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* __NETWORK_H */
