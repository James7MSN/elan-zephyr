#include "CMSDK_CM4.h"
#include "main.h"

#define _Module_UART 1

#if( _Module_UART )

__IO static bool UART1TxFinish,UART1RxFinish,UART2TxFinish,UART2RxFinish;
__IO static bool UART3TxFinish,UART3RxFinish;
__IO static char UARTConsole1;

void UARTTX1_Handler(void)
{
		__IO uint16_t status;
	
		status = UART1->INTSTACLR;
		UART1->INTSTACLR = 0x01;		// clear TX INt flag
    UART1TxFinish = 1;
}

void UARTRX2_Handler(void)
{
		__IO uint16_t status;
	
		status = UART2->INTSTACLR;
	  UART2->INTSTACLR = 0x02;		// clear RX INt flag
    UART2RxFinish = 1;
}
/*
void UARTRX3_Handler(void)
{
		__IO uint16_t status;
	
		status = UART3->INTSTACLR;
	  UART3->INTSTACLR = 0x02;		// clear RX INt flag
    UART3RxFinish = 1;
}

void UARTTX3_Handler(void)
{
		__IO uint16_t status;
	
		status = UART3->INTSTACLR;
		UART3->INTSTACLR = 0x01;		// clear TX INt flag
    UART3TxFinish = 1;
}
*/
void UARTTX2_Handler(void)
{
		__IO uint16_t status;
	
		status = UART2->INTSTACLR;
		UART2->INTSTACLR = 0x01;		// clear TX INt flag
    UART2TxFinish = 1;
}

void UARTRX1_Handler(void)
{
		__IO uint16_t status;
	
		status = UART1->INTSTACLR;
		UART1->INTSTACLR = 0x02;		// clear RX INt flag
    UART1RxFinish = 1;
}

void WaitUART1TXINTFlag(void)
{
    while(UART1->STATE_U.STATE_S.TXBUFFULL == 1);
}

void WaitUART2TXINTFlag(void)
{
    while(UART2->STATE_U.STATE_S.TXBUFFULL == 1);
}
/*
void WaitUART3TXINTFlag(void)
{
    while(UART3->STATE_U.STATE_S.TXBUFFULL == 1);
}
*/
void WaitUART1RXINTFlag(void)
{
    while(UART1->STATE_U.STATE_S.RXBUFFULL == 0);
}

void WaitUART2RXINTFlag(void)
{
    while(UART2->STATE_U.STATE_S.RXBUFFULL == 0);
}
/*
void WaitUART3RXINTFlag(void)
{
    while(UART3->STATE_U.STATE_S.RXBUFFULL == 0);
}
*/
void WaitUART1TxReady(void)
{
    while( UART1TxFinish == 0 );
}
void ClearUART1TxReady(void)
{
    UART1TxFinish = 0;
}
void WaitUART1RxReady(void)
{
    while( UART1RxFinish == 0 );
}
void ClearUART1RxReady(void)
{
    UART1RxFinish = 0;
}
void WaitUART2TxReady(void)
{
    while( UART2TxFinish == 0 );
}
void ClearUART2TxReady(void)
{
    UART2TxFinish = 0;
}
void WaitUART2RxReady(void)
{
    while( UART2RxFinish == 0 );
}
void ClearUART2RxReady(void)
{
    UART2RxFinish = 0;
}
void WaitUART3TxReady(void)
{
    while( UART3TxFinish == 0 );
}
void ClearUART3TxReady(void)
{
    UART3TxFinish = 0;
}
void WaitUART3RxReady(void)
{
    while( UART3RxFinish == 0 );
}
void ClearUART3RxReady(void)
{
    UART3RxFinish = 0;
}

void BuadRate( UART_TypeDef * UARTx, uint32_t BuadRate )
{
    uint32_t apb_clk,bauddiv;
  
    if((uint32_t)UARTx == UART2_BASE )
    {
      CLKGatingDisable(PCLKG_UART2);      
      if( IOShareCTRL->UART2_S )
      {  
        GPIOMUXSet(PORTA,GPIO_PINSOURCE4,GPIO_MUX02);   // set UART pin
        GPIOMUXSet(PORTA,GPIO_PINSOURCE5,GPIO_MUX02);
      }
      else      
      {  
        GPIOMUXSet(PORTA,GPIO_PINSOURCE14,GPIO_MUX02);   // set UART pin
        GPIOMUXSet(PORTA,GPIO_PINSOURCE15,GPIO_MUX02);
      } 
    }
    else if((uint32_t)UARTx == UART1_BASE )
    {  
      CLKGatingDisable(PCLKG_UART1);
      if( IOShareCTRL->UART1_S )      
      {  
        GPIOMUXSet(PORTA,GPIO_PINSOURCE1,GPIO_MUX02);
        GPIOMUXSet(PORTA,GPIO_PINSOURCE2,GPIO_MUX02);
      }
      else      
      {  
        GPIOMUXSet(PORTB,GPIO_PINSOURCE8,GPIO_MUX02);
        GPIOMUXSet(PORTB,GPIO_PINSOURCE9,GPIO_MUX02);
      }    
    }
  
    apb_clk = GetAPBFreq(); 
    bauddiv = ( apb_clk * 1000 + BuadRate/2 ) / BuadRate;
    if( bauddiv >= 16 )
      UARTx->BAUDDIV = bauddiv;
    else
      UARTx->BAUDDIV = 16;
}

void EnableUART( UART_TypeDef* UARTx, uint32_t Buadrate )
{
    __IO uint32_t status;
  
    BuadRate( UARTx,Buadrate );
    UARTx->CTRL = 0x0f;       // enable TX,RX & RX TX INT 
    if((uint32_t)UARTx == UART1_BASE )
    {
      //status = UART1->INTSTACLR;
      UART1->INTSTACLR = 0x03;		// clear RX/TX INt flag
      NVIC_EnableIRQ( UART1TX_Int_IRQn );
      NVIC_EnableIRQ( UART1RX_Int_IRQn );
    }
    else if((uint32_t)UARTx == UART2_BASE )
    {
      //status = UART1->INTSTACLR;
      UART2->INTSTACLR = 0x03;		// clear RX/TX INt flag
      NVIC_EnableIRQ( UART2TX_Int_IRQn );
      NVIC_EnableIRQ( UART2RX_Int_IRQn );
    }  
    /*
    else if((uint32_t)UARTx == UART3_BASE )
    {
      //status = UART1->INTSTACLR;
      UART3->INTSTACLR = 0x03;		// clear RX/TX INt flag
      NVIC_EnableIRQ( UART3TX_Int_IRQn );
      NVIC_EnableIRQ( UART3RX_Int_IRQn );
    } 
    */    
}

void DisableUART( UART_TypeDef* UARTx )
{
    UARTx->CTRL = 0;      // disable TX,RX & RX TX INT 
    if((uint32_t)UARTx == UART1_BASE )
    {
      CLKGatingEnable(PCLKG_UART1); 
      NVIC_DisableIRQ( UART1TX_Int_IRQn );
      NVIC_DisableIRQ( UART1RX_Int_IRQn );
    }
    else if((uint32_t)UARTx == UART2_BASE )
    {
      CLKGatingEnable(PCLKG_UART2); 
      NVIC_DisableIRQ( UART2TX_Int_IRQn );
      NVIC_DisableIRQ( UART2RX_Int_IRQn );
    }
    /*    
    else if((uint32_t)UARTx == UART3_BASE )
    {
      CLKGatingEnable(PCLKG_UART3); 
      NVIC_DisableIRQ( UART3TX_Int_IRQn );
      NVIC_DisableIRQ( UART3RX_Int_IRQn );
    }  
    */
}

void EnableUARTConsole( UART_TypeDef* UARTx, uint32_t Buadrate )
{
    BuadRate( UARTx,Buadrate );
    UARTx->CTRL = 0x0f;       // enable TX,RX & RX TX INT 
    //UARTx->CTRL = 0x03;       // enable TX,RX
    if((uint32_t)UARTx == UART1_BASE )
      UARTConsole1 = 1;
    else if((uint32_t)UARTx == UART2_BASE )
      UARTConsole1= 2;
    /*
    else if((uint32_t)UARTx == UART3_BASE )
      UARTConsole1= 3;
    */
}

int sendchar(int c)
{
    if( UARTConsole1 == 1 )
    {
      UART1->DATA = c;
      while((UART1->INTSTACLR&0x01)==0);    // wait TX int
      UART1->INTSTACLR = 0x01;              // clear TX int flag
    }
    else if( UARTConsole1 == 2 )
    {
      UART2->DATA = c;
      while((UART2->INTSTACLR&0x01)==0 );		// wait TX int
      UART2->INTSTACLR = 0x01;              // clear TX int flag 
    }
    /*
    else
    {
      UART3->DATA = c;
      while((UART3->INTSTACLR&0x01)==0 );		// wait TX int
      UART3->INTSTACLR = 0x01;              // clear TX int flag 
    }
    */
    return c;
}
int getkey(void)
{
    if( UARTConsole1 == 1 )
    {
      while((UART1->INTSTACLR&0x02)==0);	  // wait RX int	
      UART1->INTSTACLR = 0x02;              // clear RX int flag
      //while( UART1RxFinish == 0 );
      //UART1RxFinish = 0;  
      return UART1->DATA;
    }
    else if( UARTConsole1 == 2 )
    {
      while((UART2->INTSTACLR&0x02)==0);	  // wait RX int	
      UART2->INTSTACLR = 0x02;              // clear RX int flag
      return UART2->DATA;
    }
    /*
    else
    {
      while((UART3->INTSTACLR&0x02)==0);	  // wait RX int	
      UART3->INTSTACLR = 0x02;              // clear RX int flag
      return UART3->DATA;
    }
    */
		return 0;
}

void ShowChar(void)
{
    printf("I am Claire, and you ?\n" );
    printf("APB CLK: %6d\n", 24000);
    printf("I am ready\n" );
}
#else

int getkey(void)
{
  return 0;
}
int sendchar(int c)
{
  return 0;
}

#endif

