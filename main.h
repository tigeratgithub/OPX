#include "stm8s.h"

#ifndef 	__UARTBUF
#define 	__UARTBUF
#define TXBUF_LEN	50
#define RXBUF_LEN	50

typedef enum 
{
	A5IDLE	= 0x00,
	PCENQ	= 0x01,
	PCEOT	= 0x02,
	PCDREQ	= 0x03,
	A5ACK	= 0x04,
	A5ANT	= 0x14,
	PCWAIT  = 0x05,
	A5ENQ	= 0x06,
	A5EOT	= 0x07,
	A5DRSP	= 0x08,
	PCACK	= 0x09,
	PCANT	= 0x19
} A5Session_TypeDef;

typedef struct bufpipe
{
	uint8_t		txBuf[TXBUF_LEN];
	uint8_t		txCursor;
	uint8_t		txLen;
	uint8_t		txRetryCount;
	uint8_t		rxBuf[RXBUF_LEN];
	uint8_t		rxCursor;
	A5Session_TypeDef sid;
	_Bool		txOK;
	_Bool		rxOK;
	_Bool		rxBufOver;
	_Bool		rxOver;
} BUFPIPE_TypeDef;

typedef struct uartcfg
{
	uint32_t	baud;
	UART2_WordLength_TypeDef 	wordLen;
	UART2_StopBits_TypeDef		stopBits;
	UART2_Parity_TypeDef		parity;
	uint8_t						timeOut;	//unit ms
	uint8_t						retry;
} CHANNEL_TypeDef;



#endif