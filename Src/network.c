
/* Includes ------------------------------------------------------------------*/
#include "definitions.h"
#include "main.h"
#include "network.h"
#include "hardware.h"
#include "./../Ethernet/socket.h"	// Just include one header for WIZCHIP
#include <string.h>

/* External variables --------------------------------------------------------*/
extern uint8_t tempBuffer[TEMP_BUF_SIZE];

/* Private variables ---------------------------------------------------------*/
wiz_NetInfo gWIZNETINFO = { .mac = {NW_MAC0, NW_MAC1, NW_MAC2, NW_MAC3, NW_MAC4, NW_MAC5},
                            .ip = {NW_DEVICE_ADDR0, NW_DEVICE_ADDR1, NW_DEVICE_ADDR2, NW_DEVICE_ADDR3},
                            .sn = {NW_MASK_ADDR0, NW_MASK_ADDR1, NW_MASK_ADDR2, NW_MASK_ADDR3},
                            .gw = {NW_GW_ADDR0, NW_GW_ADDR1, NW_GW_ADDR2, NW_GW_ADDR3},
                            .dns = {NW_DNS_ADDR0, NW_DNS_ADDR1, NW_DNS_ADDR2, NW_DNS_ADDR3},
                            .dhcp = NETINFO_STATIC };

/* Functions -----------------------------------------------------------------*/

/*------------------------------------------------------*/
//networkTcpConnect
/*------------------------------------------------------*/
uint8_t networkTcpConnect(uint8_t socketNumber, uint8_t* addr, uint16_t port) {
	uint8_t retValue = 0;
	
	retValue = socket(0, Sn_MR_TCP, port, 0);
	retValue = connect(0, addr, port);
	if(retValue == SOCK_OK) {
		return TRUE;
	} else {
		return FALSE;
	}
}	
														
/*------------------------------------------------------*/
//networkDisconnect
/*------------------------------------------------------*/
uint8_t networkTcpDisconnect(uint8_t socketNumber) {
	disconnect(0);
	return TRUE;
}

/*------------------------------------------------------*/
//networkTcpSend
/*------------------------------------------------------*/
uint8_t networkTcpSend(uint8_t socketNumber, uint8_t* buffer, uint16_t length) {
	send(socketNumber,buffer,length);
	return TRUE;
}
														
/*------------------------------------------------------*/
//networkInit
/*------------------------------------------------------*/
void networkInit(void) {
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};	
	uint8_t tmp;
	uint8_t tmpstr[6];	
	
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
   uint16_t size = 0;
	
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
