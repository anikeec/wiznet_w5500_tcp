/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_H
#define __HARDWARE_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "utils.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void hardwareInit(void);
void  wizchipReset(void);
void  wizchipUnreset(void);
void  wizchipSelect(void);
void  wizchipDeselect(void);
void  wizchipWriteByte(uint8_t wb);
uint8_t wizchipReadByte();
void rtcGetDateTime(DateTime* dateTime);

#ifdef __cplusplus
}
#endif

#endif /* __HARDWARE_H */
