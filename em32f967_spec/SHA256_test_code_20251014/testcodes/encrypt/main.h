

#include <string.h>
#include <stdlib.h>
#include <CMSDK_CM4.h>
#include <stdio.h>        // for strcpy, strcmp 
#include <stdbool.h>
#include "em32f967.h"
#include "system.h"
#include "gpio.h"
#include "uart.h"
//#include "spi.h"
//#include "dma.h"


/* Compiler and system dependent definitions: */

                /* Use times(2) time function unless    */
                /* explicitly defined otherwise         */

#ifdef MSC_CLOCK
#undef HZ
#undef TIMES
#include <time.h>
#define HZ     CLOCKS_PER_SEC
#endif
               /* Use Microsoft C hi-res clock */

#ifdef TIMES
#include <sys/types.h>
#include <sys/times.h>
                /* for "times" */
#endif

#define Mic_secs_Per_Second     1000000.0
                /* Berkeley UNIX C returns process times in seconds/HZ */


#ifdef  NOSTRUCTASSIGN
#define structassign(d, s)      memcpy(&(d), &(s), sizeof(d))
#else
#define structassign(d, s)      (d = s)
#endif

#ifdef  NOENUM
#define Ident_1 0
#define Ident_2 1
#define Ident_3 2
#define Ident_4 3
#define Ident_5 4
  typedef int   Enumeration;
#else
  typedef       enum    {Ident_1, Ident_2, Ident_3, Ident_4, Ident_5}
                Enumeration;
#endif
        /* for boolean and enumeration types in Ada, Pascal */

/* General definitions: */

#include <stdio.h>
                /* for strcpy, strcmp */

#define Null 0
                /* Value of a Null pointer */
#define true  1
#define false 0

typedef int     One_Thirty;
typedef int     One_Fifty;
typedef char    Capital_Letter;
typedef int     Boolean;
typedef char    Str_30 [31];
typedef int     Arr_1_Dim [50];
typedef int     Arr_2_Dim [50] [50];

typedef struct record
    {
    struct record *Ptr_Comp;
    Enumeration    Discr;
    union {
          struct {
                  Enumeration Enum_Comp;
                  int         Int_Comp;
                  char        Str_Comp [31];
                  } var_1;
          struct {
                  Enumeration E_Comp_2;
                  char        Str_2_Comp [31];
                  } var_2;
          struct {
                  char        Ch_1_Comp;
                  char        Ch_2_Comp;
                  } var_3;
          } variant;
      } Rec_Type, *Rec_Pointer;




#define FPGA    0
#define FPGA_XTAL   24000      
//#define UART2Console
      
#define __nop10() __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
#define __nop100() __nop10();__nop10();__nop10();__nop10();__nop10();__nop10();__nop10();__nop10();__nop10();__nop10();
			
#define SCRReg			(*( volatile unsigned long *)0xe000ed10)
#define ICSR				(*( volatile unsigned long *)0xe000ed04)
//#define ICSR				(*( volatile unsigned long *)0xe000ed04)
	
// global memory
#define mem0				(*( volatile unsigned long *)0x20017800)
#define mem1				(*( volatile unsigned long *)0x20017804)
#define mem2				(*( volatile unsigned long *)0x20017808)
#define mem3				(*( volatile unsigned long *)0x2001780c)
#define mem4				(*( volatile unsigned long *)0x20017810)
#define mem5				(*( volatile unsigned long *)0x20017814)
#define mem6				(*( volatile unsigned long *)0x20017818)
#define mem7				(*( volatile unsigned long *)0x2001781c)
#define mem8				(*( volatile unsigned long *)0x20017820)
#define mem9				(*( volatile unsigned long *)0x20017824)
	
//__IO uint32_t membuf[256];
//__IO uint32_t TimingDelay;
	
#define flash_00			(*( volatile unsigned long *)0x00000700)
#define flash_01			(*( volatile unsigned long *)0x00000704)

// WDT register
//#define WDOGLOAD			(*( volatile unsigned long *)0x40025000)
//#define WDOGVALUE			(*( volatile unsigned long *)0x40025004)
//#define WDOGCONTROL		(*( volatile unsigned long *)0x40025008)
//#define WDOGINTCLR		(*( volatile unsigned long *)0x4002500c)
//#define WDOGRIS				(*( volatile unsigned long *)0x40025010)
//#define WDOGMIS				(*( volatile unsigned long *)0x40025014)
//#define WDOGLOCK			(*( volatile unsigned long *)0x40025c00)
	
// backup register
#define BackUpReg0		(*( volatile unsigned long *)0x40023000)
	
// AES register
#define AESEKR0				(*( volatile unsigned long *)0x40019004)
	
// UDC register
#define UDCCtrl				(*( volatile unsigned long *)0x40007000)
#define FCCAFR_VAL		(*( volatile unsigned long *)0x40007408)	
	
// flash control register
#define flash_key_1		(*( volatile unsigned long *)0x40024000)
#define flash_key_2		(*( volatile unsigned long *)0x40024004)
//#define flash_status	(*( volatile unsigned long *)0x40024008)
//#define flash0_ctrl0	(*( volatile unsigned long *)0x40024020)
//#define flash0_ctrl1	(*( volatile unsigned long *)0x40024024)
//#define flash0_din1  	(*( volatile unsigned long *)0x40024028)
//#define flash0_din0		(*( volatile unsigned long *)0x4002402C)
//#define flash0_dout1	(*( volatile unsigned long *)0x40024030)
//#define flash0_dout0	(*( volatile unsigned long *)0x40024034)
//#define flash1_ctrl0  (*( volatile unsigned long *)0x40024038)
//#define flash1_ctrl1	(*( volatile unsigned long *)0x4002403C)
//#define flash1_din1	  (*( volatile unsigned long *)0x40024040)
//#define flash1_din0	  (*( volatile unsigned long *)0x40024044)
//#define flash1_dout1  (*( volatile unsigned long *)0x40024048)
//#define flash1_dout0	(*( volatile unsigned long *)0x4002404C)
//#define BPT_PSWR			(*( volatile unsigned long *)0x40024050)	







// PWM register
#define PWM_PRDA			(*( volatile unsigned long *)0x4000c00c)

// RTC register
//#define RTC_RTSC 				(*( volatile unsigned long *)0x40022000)
//#define RTC_RTMNC 			(*( volatile unsigned long *)0x40022004)
//#define RTC_RTHRC 			(*( volatile unsigned long *)0x40022008)
//#define RTC_RTDYC 			(*( volatile unsigned long *)0x4002200c)
//#define RTC_ARSC				(*( volatile unsigned long *)0x40022010)
//#define RTC_ARMN 				(*( volatile unsigned long *)0x40022014)
//#define RTC_ARHR 				(*( volatile unsigned long *)0x40022018)
//#define RTC_RTRR 				(*( volatile unsigned long *)0x4002201c)
//#define RTC_RTCR  			(*( volatile unsigned long *)0x40022020)
//#define RTC_RTSCR  			(*( volatile unsigned long *)0x40022024)
//#define RTC_RTMNR 			(*( volatile unsigned long *)0x40022028)
//#define RTC_RTHRR 			(*( volatile unsigned long *)0x4002202c)
//#define RTC_RTDYR 			(*( volatile unsigned long *)0x40022030)
//#define RTC_RTITS  			(*( volatile unsigned long *)0x40022034)



#define Timer0_CTRL				(*( __IO uint32_t *)0x40000000)
#define Timer0_VALUE			(*( volatile unsigned long *)0x40000004)
#define Timer0_RELOAD			(*( volatile unsigned long *)0x40000008)
#define Timer0_INTSTATUS	(*( volatile unsigned long *)0x4000000c)
	
#define Timer1_CTRL				(*( volatile unsigned long *)0x40001000)
#define Timer1_VALUE			(*( volatile unsigned long *)0x40001004)
#define Timer1_RELOAD			(*( volatile unsigned long *)0x40001008)
#define Timer1_INTSTATUS	(*( volatile unsigned long *)0x4000100c)
	
#define Timer2_CTRL				(*( volatile unsigned long *)0x40010000)
#define Timer2_VALUE			(*( volatile unsigned long *)0x40010004)
#define Timer2_RELOAD			(*( volatile unsigned long *)0x40010008)
#define Timer2_INTSTATUS	(*( volatile unsigned long *)0x4001000c)
	
#define Timer3_CTRL				(*( volatile unsigned long *)0x40011000)
#define Timer3_VALUE			(*( volatile unsigned long *)0x40011004)
#define Timer3_RELOAD			(*( volatile unsigned long *)0x40011008)
#define Timer3_INTSTATUS	(*( volatile unsigned long *)0x4001100c)

// I2C register
#define I2CON_0						(*( volatile unsigned long *)0x40005000)
#define I2CSAR_0					(*( volatile unsigned long *)0x40005004)
#define I2CON_1						(*( volatile unsigned long *)0x40014000)
#define I2CSAR_1					(*( volatile unsigned long *)0x40014004)	
//#define I2CON0						(*( volatile unsigned long *)0x40005000)
//#define I2CON0						(*( volatile unsigned long *)0x40005000)
//#define I2CON0						(*( volatile unsigned long *)0x40005000)
//#define I2CON0						(*( volatile unsigned long *)0x40005000)


// SPI register
#define SSPCR0_0			(*( volatile unsigned short *)0x40004000)
#define SSPCR1_0			(*( volatile unsigned short *)0x40004004)
#define SSPDR_0				(*( volatile unsigned short *)0x40004008)
#define SSPSR_0				(*( volatile unsigned short *)0x4000400c)
#define SSPCPSR_0			(*( volatile unsigned short *)0x40004010)
#define SSPIMSC_0			(*( volatile unsigned short *)0x40004014)
#define SSPRIS_0			(*( volatile unsigned short *)0x40004018)	
#define SSPICR_0			(*( volatile unsigned short *)0x40004020)	
#define SSPDMACR_0		(*( volatile unsigned short *)0x40004024)
	

#define SSPCR0_1			(*( volatile unsigned short *)0x40013000)
#define SSPCR1_1			(*( volatile unsigned short *)0x40013004)
#define SSPDR_1				(*( volatile unsigned short *)0x40013008)
#define SSPSR_1				(*( volatile unsigned short *)0x4001300c)
#define SSPCPSR_1			(*( volatile unsigned short *)0x40013010)
#define SSPIMSC_1			(*( volatile unsigned short *)0x40013014)
#define SSPRIS_1			(*( volatile unsigned short *)0x40013018)	
#define SSPICR_1			(*( volatile unsigned short *)0x40013020)		
#define SSPDMACR_1		(*( volatile unsigned short *)0x40013024)	

// SPI register

//#define DMA_SAR0			(*( volatile unsigned long *)0x40035000)

// DMA register
#define DMA_SAR0			(*( volatile unsigned long long *)0x40035000)
#define DMA_DAR0			(*( volatile unsigned long long *)0x40035008)
#define DMA_LLP0			(*( volatile unsigned long long *)0x40035010)
#define DMA_CTL0			(*( volatile unsigned long long *)0x40035018)
//#define DMA_SSTAT0		(*( volatile unsigned long long *)0x40035020)	
//#define DMA_DSTAT0		(*( volatile unsigned long long *)0x40035028)
//#define DMA_SSTATAR0	(*( volatile unsigned long long *)0x40035030)
//#define DMA_DSTATAR0	(*( volatile unsigned long long *)0x40035038)
#define DMA_CFG0			(*( volatile unsigned long long *)0x40035040)
#define DMA_SGR0			(*( volatile unsigned long long *)0x40035048)
//#define DMA_DSR0			(*( volatile unsigned long long *)0x40035050)
#define DMA_DSR0			(*( __IO uint64_t *)0x40035050)
	
#define DMA_SAR1			(*( volatile unsigned long long *)0x40035058)
#define DMA_DAR1			(*( volatile unsigned long long *)0x40035060)
#define DMA_LLP1			(*( volatile unsigned long long *)0x40035068)
#define DMA_CTL1			(*( volatile unsigned long long *)0x40035070)
#define DMA_CFG1			(*( volatile unsigned long long *)0x40035098)
#define DMA_SGR1			(*( volatile unsigned long long *)0x400350a0)
#define DMA_DSR1			(*( volatile unsigned long long *)0x400350a8)
	
#define DMA_SAR2			(*( volatile unsigned long long *)0x400350b0)
#define DMA_DAR2			(*( volatile unsigned long long *)0x400350b8)
#define DMA_LLP2			(*( volatile unsigned long long *)0x400350c0)
#define DMA_CTL2			(*( volatile unsigned long long *)0x400350c8)
#define DMA_CFG2			(*( volatile unsigned long long *)0x400350f0)
#define DMA_SGR2			(*( volatile unsigned long long *)0x400350f8)
#define DMA_DSR2			(*( volatile unsigned long long *)0x40035100)	
	
#define DMA_SAR3			(*( volatile unsigned long long *)0x40035108)
#define DMA_DAR3			(*( volatile unsigned long long *)0x40035110)
#define DMA_LLP3			(*( volatile unsigned long long *)0x40035118)
#define DMA_CTL3			(*( volatile unsigned long long *)0x40035120)
#define DMA_CFG3			(*( volatile unsigned long long *)0x40035148)
#define DMA_SGR3			(*( volatile unsigned long long *)0x40035150)
#define DMA_DSR3			(*( volatile unsigned long long *)0x40035158)	
	
#define DMA_SAR4			(*( volatile unsigned long long *)0x40035160)
#define DMA_DAR4			(*( volatile unsigned long long *)0x40035168)
#define DMA_LLP4			(*( volatile unsigned long long *)0x40035170)
#define DMA_CTL4			(*( volatile unsigned long long *)0x40035178)
#define DMA_CFG4			(*( volatile unsigned long long *)0x400351a0)
#define DMA_SGR4			(*( volatile unsigned long long *)0x400351a8)
#define DMA_DSR4			(*( volatile unsigned long long *)0x400351b0)	
	
#define DMA_RawErr		(*( volatile unsigned long long *)0x400352e0)
	
#define DMA_RawTfr			(*( volatile unsigned long long *)0x400352c0)
#define DMA_RawBlock		(*( volatile unsigned long long *)0x400352c8)
#define DMA_RawSrcTran	(*( volatile unsigned long long *)0x400352d0)
#define DMA_RawDstTran	(*( volatile unsigned long long *)0x400352d8)

#define DMA_StatusErr		(*( volatile unsigned long long *)0x40035308)
	
#define DMA_MaskTfr			(*( volatile unsigned long long *)0x40035310)
#define DMA_MaskBlock		(*( volatile unsigned long long *)0x40035318)
#define DMA_MaskSrcTran	(*( volatile unsigned long long *)0x40035320)
#define DMA_MaskDstTran	(*( volatile unsigned long long *)0x40035328)

#define DMA_ClearTfr			(*( volatile unsigned long long *)0x40035338)
#define DMA_ClearBlock		(*( volatile unsigned long long *)0x40035340)
#define DMA_ClearSrcTran 	(*( volatile unsigned long long *)0x40035348)
#define DMA_ClearDstTran	(*( volatile unsigned long long *)0x40035350)
#define DMA_ClearErr			(*( volatile unsigned long long *)0x40035358)
	

#define DMA_Cfg_R			(*( volatile unsigned long long *)0x40035398)
#define DMA_ChEn_R		(*( volatile unsigned long long *)0x400353a0)
#define DMA_ID_R			(*( volatile unsigned long long *)0x400353a8)
#define DMA_TEST_R		(*( volatile unsigned long long *)0x400353b0)
/*
#define DMA_SSTAT0		(*( volatile unsigned long *)0x40035020)	
#define DMA_DSTAT0		(*( volatile unsigned long *)0x40035028)
#define DMA_SSTATAR0	(*( volatile unsigned long *)0x40035030)
#define DMA_DSTATAR0	(*( volatile unsigned long *)0x40035038)
#define DMA_CFG0			(*( volatile unsigned long *)0x40035040)
#define DMA_SGR0			(*( volatile unsigned long *)0x40035048)
#define DMA_DSR0			(*( volatile unsigned long *)0x40035050)
*/							
void SPIInit(void);	
void SPITest(void);	
void SPITest0(void);	
void SPITest1(void);
void SPIMasterTest(void);
void ChkSPIBusy1(void);
void ChkSPIBusy0(void);
void SPIBurstRead1(void);
void SPIBurstRead0(void);
void DMATest(void);
void SysTick_Init(void);
void ParameterInit(void);
void ParameterInit2(void);
void CLKOUTInit(void);
void WDTTest(void);
void TimerTest(void);
void PowerMKodeTest01(void);
void PowerMKodeTest02(void);
void PowerMKodeTest03(void);
void CLKGating(void);
void CLKTree(void);
void RTC_Test(void);

void UARTTest(void);
void SPIBurstWrite1(void);
void Remap(void);
void RemapTest(void);
unsigned int MemoryTest(void);
void FlashTest(void);
void AutoLoadTest(void);
void CLKTest(void);
void IAPTest(void);
void MasterSlaveSPI(void);

void Proc_1 (Rec_Pointer Ptr_Val_Par);
void Proc_2 (One_Fifty *Int_Par_Ref);
void Proc_3 (Rec_Pointer *Ptr_Ref_Par);
void Proc_4 (void);
void Proc_5 (void);
void Proc_6 (Enumeration  Enum_Val_Par, Enumeration *Enum_Ref_Par);
void Proc_7 (One_Fifty Int_1_Par_Val, One_Fifty Int_2_Par_Val, One_Fifty *Int_Par_Ref);
void Proc_8 (Arr_1_Dim Arr_1_Par_Ref, Arr_2_Dim Arr_2_Par_Ref, int Int_1_Par_Val, int Int_2_Par_Val);
Enumeration Func_1 (Capital_Letter Ch_1_Par_Val, Capital_Letter Ch_2_Par_Val);
Boolean Func_2 (Str_30 Str_1_Par_Ref, Str_30 Str_2_Par_Ref);
Boolean Func_3 (Enumeration Enum_Par_Val);


typedef unsigned char datum;    /* Set the data bus width to 8 bits.  */
;datum memTestDataBus(volatile datum * a);
;datum * memTestAddressBus(volatile datum * b, unsigned long );
			
			
void Delay10us(void);

void NVIC_EnableIRQ(IRQn_Type IRQn);// a Enables an interrupt or exception.
void NVIC_DisableIRQ(IRQn_Type IRQn);// a Disables an interrupt or exception.
void NVIC_SetPendingIRQ(IRQn_Type IRQn);// a Sets the pending status of interrupt or exception to 1
void NVIC_ClearPendingIRQ(IRQn_Type IRQn);// a Clears the pending status of interrupt or exception to 0.
uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn);// a Reads the pending status of interrupt or exception.
/*	
This function returns non-zero value if the pending status
is set to 1.
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) a Sets the priority of an interrupt or exception with
configurable priority level to 1.
uint32_t NVIC_GetPriority(IRQn_Type IRQn) a Reads the priority of an interrupt or exception with
configurable priority level.
This function return the current priority level.
*/

//void BuadRate( UART_TypeDef * UARTx, uint32_t BuadRate );
void getRandPattern(uint8_t* pbuf, int len);
