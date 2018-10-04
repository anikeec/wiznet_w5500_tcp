/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

/* Definitions of buffer------------------------------------------------------*/
//main
#define DATA_BUF_SIZE   2048
#define TEMP_BUF_SIZE 	10
#define MESSAGE_MAX_LENGTH	200

//network
//-

//hardware
//-

//engine
//-

//utils
#define DATE_TIME_SUF_SIZE	27

//uart
#define RING_BUFFER_LENGTH 200

/*
		summary:
			DATA_BUF_SIZE	- 2048
			TEMP_BUF_SIZE	- 10
			MESSAGE_MAX_LENGTH - 200
			DATE_TIME_SUF_SIZE - 27
			RING_BUFFER_LENGTH - 200*2 = 400
			------------------ - 2685
			
		heap:
						1024
		stack:
						2048
*/

/* Definitions----------------------------------------------------------------*/
#define _WIZCHIP_ 			5500

#define SPI_RST_PORT 		GPIOA
#define SPI_RST_PIN 		GPIO_PIN_2
#define SPI_INT_PORT 		GPIOA
#define SPI_INT_PIN 		GPIO_PIN_3
#define SPI_CS_PORT 		GPIOA
#define SPI_CS_PIN 			GPIO_PIN_4
#define SPI_SCK_PORT 		GPIOA
#define SPI_SCK_PIN 		GPIO_PIN_5

#define LD4_Pin 				GPIO_PIN_8
#define LD4_GPIO_Port 	GPIOC
#define LD3_Pin 				GPIO_PIN_9
#define LD3_GPIO_Port 	GPIOC
#define TMS_SWDIO_Pin 	GPIO_PIN_13
#define TMS_SWDIO_			GPIO_Port GPIOA
#define TCK_SWCLK_Pin 	GPIO_PIN_14
#define TCK_SWCLK_			GPIO_Port GPIOA

#define NW_MAC0					0x00
#define NW_MAC1					0x08
#define NW_MAC2					0xdc
#define NW_MAC3					0x00
#define NW_MAC4					0xab
#define NW_MAC5					0xcd

#define NW_DEVICE_ADDR0	192
#define NW_DEVICE_ADDR1	168
#define NW_DEVICE_ADDR2	0
#define NW_DEVICE_ADDR3	224

#define NW_GW_ADDR0			192
#define NW_GW_ADDR1			168
#define NW_GW_ADDR2			0
#define NW_GW_ADDR3			1

#define NW_MASK_ADDR0		255
#define NW_MASK_ADDR1		255
#define NW_MASK_ADDR2		255
#define NW_MASK_ADDR3		0

#define NW_DNS_ADDR0		8
#define NW_DNS_ADDR1		8
#define NW_DNS_ADDR2		8
#define NW_DNS_ADDR3		8

#define NW_SERVER_ADDR0	192
#define NW_SERVER_ADDR1	168
#define NW_SERVER_ADDR2	0
#define NW_SERVER_ADDR3	125
#define NW_SERVER_PORT	65530
#define NETWORK_SOCKET	0

#define TRUE						1
#define FALSE						0

#endif /* __DEFINITIONS_H */
