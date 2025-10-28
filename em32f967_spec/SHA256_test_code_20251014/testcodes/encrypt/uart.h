
#ifndef _UART_H
#define _UART_H

#include <CMSDK_CM4.h>
#include <stdbool.h>

#define xxxx

// UART register
typedef struct
{
  __IO uint32_t DATA;
  union
  {
    struct
    {
    __IO uint32_t TXBUFFULL    : 1;
    __IO uint32_t RXBUFFULL    : 1;
    __IO uint32_t TXBUFOVERRUN : 1;
    __IO uint32_t RXBUFOVERRUN : 1;
    __IO uint32_t Reserved     : 28;
    }STATE_S;
  __IO uint32_t STATE;
  }STATE_U;
 __IO uint32_t CTRL;
  __IO uint32_t INTSTACLR;
  __IO uint32_t BAUDDIV;
  __IO uint32_t Reserved[3];
  __IO uint32_t DMALENGTHL;
  __IO uint32_t DMALENGTHH;
  __IO uint32_t DMAWAITCNT   : 8;
  __IO uint32_t ReservedBit0 : 24;
	__IO uint32_t DMAENANLE    : 1;
  __IO uint32_t TXNRX        : 1;
  __IO uint32_t ReservedBit  : 30;
}UART_TypeDef;

#define UART1 (( UART_TypeDef * ) UART1_BASE)
#define UART2 (( UART_TypeDef * ) UART2_BASE)
#define UART3 (( UART_TypeDef * ) UART3_BASE)

void ShowChar(void);
int sendchar(int c);


#ifdef xxxx
//void InitUART2( uint32_t Buadrate );
void EnableUART( UART_TypeDef* UARTx, uint32_t BuadRate );
void DisableUART( UART_TypeDef* UARTx);
void BuadRate( UART_TypeDef * UARTx, uint32_t BuadRate );
void EnableUARTConsole( UART_TypeDef* UARTx, uint32_t Buadrate );



int getkey(void);

void WaitUART1TXINTFlag(void);
void WaitUART2TXINTFlag(void);
void WaitUART3TXINTFlag(void);
void WaitUART1RXINTFlag(void);
void WaitUART2RXINTFlag(void);
void WaitUART3RXINTFlag(void);

#endif
#endif
