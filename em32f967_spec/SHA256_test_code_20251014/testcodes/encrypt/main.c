/*
2021/12/27	:EM32F667 Encrypt Test code
*/

#include <stdio.h>
#include <stdint.h>
#include "CMSDK_CM4.h"
#include "em32f967.h"
#include "main.h"
#include "uart.h"

uint8_t SRAM[32*1024] __attribute__((at(0x2002C000)));

#ifdef TEST_ALL_ENTRYPT
#define TEST_AES
#define TEST_SHA
#define TEST_RSA
#define TEST_DMA
#define TEST_TRNG
//#define TEST_ECC
#endif

#ifdef TEST_AES
int  test_aes(void);
#endif

#ifdef TEST_SHA
int test_sha(void);
#endif

#ifdef TEST_RSA
int test_rsa(void);
#endif

#ifdef TEST_DMA
int test_dma(void);
#endif

#ifdef TEST_TRNG
int test_trng(void);
#endif

#ifdef TEST_TIME
int test_time(void);
#endif

#ifdef TEST_ECC
int ecc_main(void);
#endif

#ifdef TEST_I2C_WITH_DMA
void test_i2c_with_dma(void);
#endif
void ip_test_status(char* msg)
{
	volatile int i = 1;

	while(i);
}
//#define ioie_od_ctrl		(*( volatile unsigned int *)0x40020034)
	

#define REG(n)	(*((volatile unsigned int*)n))
//#define BIT(n)	(1<<n)
#define MASK(n)	~((1<<n))

//#define CACHECTRL			 (*( __IO uint32_t *)0x40037000)

void getRandPattern(uint8_t* pbuf, int len)
{
	int i;
	
	for (i=0; i<len; i++)
	{
		pbuf[i] =  rand()&0xFF;
	}
}


int main (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	CacheEnable();
//  REG_CLK_GATE.bf.cache = GATING_CLK_DISABLE;
//  CACHECTRL = 0x03;         // enable cache
	
	
//	SetMainFreq2(IRCHIGH,IRCHIGH96,DIV1); 
	SetMainFreq2(IRCLOW,IRCLOW24,DIV1); 
	
	SysTick_Config(1000);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PINSOURCE0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_Floating;
 	GPIO_Init(GPIOIPA, &GPIO_InitStructure);
	GPIO_WriteBit(GPIOIPA, GPIO_PIN_0, (BitAction)0);

	EnableUARTConsole(UART2,115200);
	
#ifdef TEST_AES	

	if (test_aes())
	{
		ip_test_status("err:aes_test");		
	}
	else
	{
#if !defined(TEST_ALL_ENTRYPT)	
		ip_test_status("pass:aes_test");
#endif
	}
#endif	

#ifdef TEST_SHA	
	if(test_sha())
	{
		ip_test_status("err:sha_test");
	}
	else
	{
#if !defined(TEST_ALL_ENTRYPT)	
		ip_test_status("pass:sha_test");
#endif
	}
#endif

#ifdef TEST_RSA	
	if(test_rsa())
	{
		ip_test_status("err:rsa_test");
	}
	else
	{
#if !defined(TEST_ALL_ENTRYPT)	
		ip_test_status("pass:rsa_test");
#endif
	}
#endif

#ifdef TEST_DMA

	if (test_dma())
	{
		ip_test_status("err:dma_test");		
	}
	else
	{
#if !defined(TEST_ALL_ENTRYPT)	
		ip_test_status("pass:dma_test");
#endif
	}
#endif		
	
#ifdef TEST_TRNG	

	if (test_trng())
	{
		ip_test_status("err:trng_test");		
	}
	else
	{
#if !defined(TEST_ALL_ENTRYPT)	
		ip_test_status("pass:trng_test");
#endif
	}
#endif	

#ifdef TEST_TIME	

	if (test_time())
	{
		ip_test_status("err:time_test");		
	}
	else
	{
#if !defined(TEST_ALL_ENTRYPT)	
		ip_test_status("pass:time_test");
#endif
	}
#endif	
	
#ifdef TEST_ECC	

	if (ecc_main())
	{
		ip_test_status("err:ecc_test");		
	}
	else
	{
		ip_test_status("pass:ecc_test");
	}
#endif	
	
	
}
