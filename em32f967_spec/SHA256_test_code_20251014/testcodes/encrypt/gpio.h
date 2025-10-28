
#ifndef _GPIO_H
#define _GPIO_H

#include <CMSDK_CM4.h>
#include <stdbool.h>


typedef enum
{ 
  FALLING	   	= 0x00, 
  RISING  	  = 0x01,
  LOWLEVEL	 	= 0x02, 
  HIGHLEVEL	  = 0x03
}GPIOINTDef;	

typedef enum
{ 
  GPIO_MUX00   	= 0x00, 
  GPIO_MUX01  	= 0x01,
  GPIO_MUX02   	= 0x02, 
  GPIO_MUX03  	= 0x03,
  GPIO_MUX04   	= 0x04, 
  GPIO_MUX05  	= 0x05,
  GPIO_MUX06   	= 0x06, 
  GPIO_MUX07  	= 0x07
}GPIOMUXDef;	
	
typedef enum
{ 
  GPIO_Mode_IN   = 0x00, /*!< GPIO Input Mode */
  GPIO_Mode_OUT  = 0x01, /*!< GPIO Output Mode */
}GPIOModeDef;
#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_Mode_IN)  || ((MODE) == GPIO_Mode_OUT) )


/** 
  * @brief  GPIO Configuration PullUp PullDown enumeration 
  */ 
typedef enum
{ 
  GPIO_PuPd_Floating 		  = 0x00,
  GPIO_PuPd_PullUp66K		  = 0x01,
  GPIO_PuPd_PullUp4_7K 	  = 0x02,
  GPIO_PuPd_Pulldown15K		= 0x03
}GPIOPuPdDef;
//#define IS_GPIO_PUPD(PUPD) (((PUPD) == GPIO_PuPd_NOPULL) || ((PUPD) == GPIO_PuPd_UP) || \
                            ((PUPD) == GPIO_PuPd_DOWN))

typedef enum
{ 
  GPIO_OD02 		= 0x00,
  GPIO_OD04 		= 0x01,
  GPIO_OD06 		= 0x02,
  GPIO_OD08 		= 0x03,
}GPIOODDef;

/** 
  * @brief  GPIO Bit SET and Bit RESET enumeration 
  */ 
typedef enum
{ 
  Bit_RESET = 0,
  Bit_SET = 1
}BitAction;
#define IS_GPIO_BIT_ACTION(ACTION) (((ACTION) == Bit_RESET) || ((ACTION) == Bit_SET))


typedef enum
{
   PORTA =      0x00,
   PORTB =      0x01,
   PORTANALOG = 0x02
}GPIOPortDef;

typedef enum
{
	 GPIO_PIN_0	  = 0x0001,
	 GPIO_PIN_1	  = 0x0002,
	 GPIO_PIN_2	  = 0x0004,
	 GPIO_PIN_3	  = 0x0008,
	 GPIO_PIN_4	  = 0x0010,
	 GPIO_PIN_5	  = 0x0020,
	 GPIO_PIN_6	  = 0x0040,
	 GPIO_PIN_7	  = 0x0080,
	 GPIO_PIN_8	  = 0x0100,
	 GPIO_PIN_9	  = 0x0200,
	 GPIO_PIN_10  = 0x0400,
	 GPIO_PIN_11  = 0x0800,
	 GPIO_PIN_12  = 0x1000,
	 GPIO_PIN_13  = 0x2000,
	 GPIO_PIN_14  = 0x4000,
	 GPIO_PIN_15  = 0x8000,
   GPIO_PIN_ALL = 0xffff
} GPIOPinBitDef;

typedef enum
{
	 GPIO_PIN_FUN_B_ALL = 0x0000,
   GPIO_PIN_FUN_A0	  = 0x0001,
	 GPIO_PIN_FUN_A1	  = 0x0002,
	 GPIO_PIN_FUN_A2	  = 0x0004,
	 GPIO_PIN_FUN_A3	  = 0x0008,
	 GPIO_PIN_FUN_A4	  = 0x0010,
	 GPIO_PIN_FUN_A5	  = 0x0020,
	 GPIO_PIN_FUN_A6	  = 0x0040,
	 GPIO_PIN_FUN_A7	  = 0x0080,
	 GPIO_PIN_FUN_A8	  = 0x0100,
	 GPIO_PIN_FUN_A9	  = 0x0200,
	 GPIO_PIN_FUN_A10   = 0x0400,
	 GPIO_PIN_FUN_A11   = 0x0800,
	 GPIO_PIN_FUN_A12   = 0x1000,
	 GPIO_PIN_FUN_A13   = 0x2000,
	 GPIO_PIN_FUN_A14   = 0x4000,
	 GPIO_PIN_FUN_A15   = 0x8000,
   GPIO_PIN_FUN_A_ALL = 0xffff
} GPIOPinFunDef;

typedef enum
{
	 GPIO_PIN_SMT_B0	  = 0x0001,
	 GPIO_PIN_SMT_B1	  = 0x0002,
	 GPIO_PIN_SMT_B2	  = 0x0004,
	 GPIO_PIN_SMT_B3	  = 0x0008,
	 GPIO_PIN_SMT_B4	  = 0x0010,
	 GPIO_PIN_SMT_B5	  = 0x0020,
	 GPIO_PIN_SMT_B6	  = 0x0040,
	 GPIO_PIN_SMT_B7	  = 0x0080,
	 GPIO_PIN_SMT_B8	  = 0x0100,
	 GPIO_PIN_SMT_B9	  = 0x0200,
	 GPIO_PIN_SMT_B10   = 0x0400,
	 GPIO_PIN_SMT_B11   = 0x0800,
	 GPIO_PIN_SMT_B12   = 0x1000,
	 GPIO_PIN_SMT_B13   = 0x2000,
	 GPIO_PIN_SMT_B14   = 0x4000,
	 GPIO_PIN_SMT_B15   = 0x8000,
   GPIO_PIN_SMT_A11   = 0xf001,
	 GPIO_PIN_SMT_A12   = 0xf002,
	 GPIO_PIN_SMT_A13   = 0xf004,
	 GPIO_PIN_SMT_A14   = 0xf008,
	 GPIO_PIN_SMT_A15   = 0xf010
} GPIOPinSMTDef;

typedef enum
{
  GPIO_PINSOURCE0  = 0x00,
  GPIO_PINSOURCE1  = 0x01,
  GPIO_PINSOURCE2  = 0x02,
  GPIO_PINSOURCE3  = 0x03,
  GPIO_PINSOURCE4  = 0x04,
  GPIO_PINSOURCE5  = 0x05,
  GPIO_PINSOURCE6  = 0x06,
  GPIO_PINSOURCE7  = 0x07,
  GPIO_PINSOURCE8  = 0x08,
  GPIO_PINSOURCE9  = 0x09,
  GPIO_PINSOURCE10 = 0x0a,
  GPIO_PINSOURCE11 = 0x0b,
  GPIO_PINSOURCE12 = 0x0c,
  GPIO_PINSOURCE13 = 0x0d,
  GPIO_PINSOURCE14 = 0x0e,
  GPIO_PINSOURCE15 = 0x0f
} GPIOPinNameDef;
  

#define IS_GPIO_PIN_SOURCE(PINSOURCE) (((PINSOURCE) == GPIO_PinSource0) || \
                                       ((PINSOURCE) == GPIO_PinSource1) || \
                                       ((PINSOURCE) == GPIO_PinSource2) || \
                                       ((PINSOURCE) == GPIO_PinSource3) || \
                                       ((PINSOURCE) == GPIO_PinSource4) || \
                                       ((PINSOURCE) == GPIO_PinSource5) || \
                                       ((PINSOURCE) == GPIO_PinSource6) || \
                                       ((PINSOURCE) == GPIO_PinSource7) || \
                                       ((PINSOURCE) == GPIO_PinSource8) || \
                                       ((PINSOURCE) == GPIO_PinSource9) || \
                                       ((PINSOURCE) == GPIO_PinSource10) || \
                                       ((PINSOURCE) == GPIO_PinSource11) || \
                                       ((PINSOURCE) == GPIO_PinSource12) || \
                                       ((PINSOURCE) == GPIO_PinSource13) || \
                                       ((PINSOURCE) == GPIO_PinSource14) || \
                                       ((PINSOURCE) == GPIO_PinSource15))

typedef struct
{
	GPIOPinNameDef GPIO_Pin;              
	GPIOModeDef GPIO_Mode;     
  GPIOPuPdDef GPIO_PuPd;     
}GPIO_InitTypeDef;


typedef enum
{ 
  EXTWAKEUP1	   	= 0x00, 
  EXTWAKEUP2	   	= 0x01,
  EXTWAKEUP3	   	= 0x02, 
  EXTWAKEUP4	   	= 0x03,
  EXTWAKEUP5	   	= 0x04, 
  EXTWAKEUP6	   	= 0x05,
  EXTWAKEUP7	   	= 0x06
}WakeUpNumDef;	

typedef enum
{ 
  FALLINGEDGE	   	= 0x00, 
  RISINGEDGE  	  = 0x01,
  BOTHEDGE		   	= 0x02, 
  BOTHEDGE1  	 	  = 0x03
}ExtWakeUpDef;	

typedef struct 
{
	unsigned long WAKEUPEN 		: 7;
	unsigned long WAKETYPE 		: 14;
	unsigned long WSTATUS 		: 7;
	unsigned long WSTATUSCLR 	: 1;
	unsigned long Reserved		: 2;
} ExtWake_Type;	

#define ExtWakeupCtrl     ((ExtWake_Type *)0x40031004  )



#define SYSREG					(*( __IO uint32_t *)0x40030000)
#define SYSSTATUS				(*( __IO uint32_t *)0x40030004)
#define MISCREG					(*( __IO uint32_t *)0x40030008)

#define IOMUXPACTRL		  (*( __IO uint32_t *)0x40030200)
#define IOMUXPACTRL2	  (*( __IO uint32_t *)0x40030204)
#define IOMUXPBCTRL     (*( __IO uint32_t *)0x40030208)
#define IOMUXPBCTRL2 	  (*( __IO uint32_t *)0x4003020c)
#define IOANAENCTRL 		(*( __IO uint32_t *)0x40030210)     
#define IOPUPDPACTRL    (*( __IO uint32_t *)0x40030214)                                        
#define IOPUPDPBCTRL    (*( __IO uint32_t *)0x40030218)     
#define IOHDPACRTL		  (*( __IO uint32_t *)0x4003021c)       
#define IOHDPBCRTL		  (*( __IO uint32_t *)0x40030220)       
#define IOSMTPACTRL	    (*( __IO uint32_t *)0x40030224)       
#define IOSMTPBCTRL	    (*( __IO uint32_t *)0x40030228)     
#define IOODEPACTRL     (*( __IO uint32_t *)0x4003022c)       
#define IOODEPBCTRL		  (*( __IO uint32_t *)0x40030230)       
#define IOOSEPACTRL	    (*( __IO uint32_t *)0x40030234)     
#define IOOSEPBCTRL	    (*( __IO uint32_t *)0x40030238) 

#define PDGPIOCTRL		  (*( __IO uint32_t *)0x40030240)  

#define PDPAOE		      (*( __IO uint32_t *)0x40030244)
#define PDPBOE		      (*( __IO uint32_t *)0x40030248)
#define PDPAOUT		      (*( __IO uint32_t *)0x4003024c)
#define PDPBOUT		      (*( __IO uint32_t *)0x40030250)

#define PB8CLOCKRATE	  (*( __IO uint32_t *)0x40030254)
#define PB9CLOCKRATE		(*( __IO uint32_t *)0x40030258)

#define IPGPIOADATA     (*( __IO uint32_t *)0x40020000)
#define IPGPIOADATAOUT  (*( __IO uint32_t *)0x40020004)
#define IPGPIOBDATA     (*( __IO uint32_t *)0x40021000)
#define IPGPIOBDATAOUT  (*( __IO uint32_t *)0x40021004)



typedef struct 
{
	__IO uint32_t bit0          : 1;
	__IO uint32_t bit1     	    : 1;
	__IO uint32_t bit32         : 2;
	__IO uint32_t bit4          : 1;
	__IO uint32_t bit5     	    : 1;
	__IO uint32_t bit76         : 2;
	__IO uint32_t Reserved 	    : 26;
} BIO_Type;	
#define GPIOBCTRL     ((BIO_Type *)0x40021004)



typedef struct 
{
	unsigned long Addr		 			: 3;
	unsigned long InvData 			: 10;
	unsigned long Read 					: 1;
	unsigned long Program 			: 1;
	unsigned long AutoLoad			: 1;
	unsigned long ReadyClr 			: 1;
	unsigned long LoadReady 		: 1;
	unsigned long DataOut 			: 10;
	unsigned long CounterEnable	: 1;
	unsigned long Dummy 			  : 3;
} eFuse_Type;	

#define eFuseProgram	  ((eFuse_Type *)0x40020064  )

//#define sip_set4				(*( __IO uint32_t *)0x40020050)
  

#define PD_WARMUP_CNT		(*( __IO uint32_t *)0x40031000)	
#define EXT_WAKEUP			(*( __IO uint32_t *)0x40031004)
//#define SIPSET9 				(*( __IO uint32_t *)0x40020064)
#define EFUSEDOUT0			(*( __IO uint32_t *)0x40046f04)
#define EFUSEDOUT1			(*( __IO uint32_t *)0x40036f08)
//#define EFUSEDOUT2			(*( __IO uint32_t *)0x40020070)
#define SYSSTATUSREG		(*( __IO uint32_t *)0x40030004)	  

typedef struct
{
	__IO uint16_t DATA;             // 0x00
	__IO uint16_t dummy0;
	__IO uint16_t DATAOUT;          // 0x04
	__IO uint16_t dummy1;
	__IO uint16_t Reserved0;
	__IO uint16_t dummy2;
	__IO uint16_t Reserved1;
	__IO uint16_t dummy3;
	__IO uint16_t DATAOUTSET;       // 0x10
	__IO uint16_t dummy4;
	__IO uint16_t DATAOUTCLR;       // 0x14
	__IO uint16_t dummy5;
	__IO uint16_t ALTFUNCSET;       // 0x18
	__IO uint16_t dummy6;
	__IO uint16_t ALTFUNCCLR;       // 0x1c
	__IO uint16_t dummy7;
	__IO uint16_t INTENSET;         // 0x20
	__IO uint16_t dummy8;
	__IO uint16_t INTENCLR;         // 0x24
	__IO uint16_t dummy9;
	__IO uint16_t INTTYPEEDGESET;   // 0x28
	__IO uint16_t dummy10;
	__IO uint16_t INTTYPEEDGECLR;   // 0x2c
	__IO uint16_t dummy11;
	__IO uint16_t INTPOLSET;        // 0x30
	__IO uint16_t dummy12;
	__IO uint16_t INTPOLCLR;        // 0x34
	__IO uint16_t dummy13;
	__IO uint16_t INTSTATUSANDCLR;  // 0x38
	__IO uint16_t dummy14;
}GPIO_IPType;


#define GPIOIPA (( GPIO_IPType * ) GPIOA_BASE)
#define GPIOIPB (( GPIO_IPType * ) GPIOB_BASE)

void ExtWakeUp( WakeUpNumDef WakeUpNum, ExtWakeUpDef TriggerEdge );
void DisExtWakeUp( WakeUpNumDef WakeUpNum );
void ClearExtWakeUpStatus( void );


void TOPREGTest(void);
void GPIOTest(void);




bool GPIO_ReadBit(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);
uint16_t GPIO_ReadPort(GPIO_IPType* GPIOx );
void GPIO_WritePort(GPIO_IPType* GPIOx, uint16_t PortVal);
void GPIOMUXSet(GPIOPortDef GPIOx, GPIOPinNameDef GPIOPin, GPIOMUXDef MUXNum);
void GPIO_WriteBit(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin, BitAction BitVal);
void GPIO_ToggleBits(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);
void GPIO_Init(GPIO_IPType * GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void EnableGPIOINT(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin, GPIOINTDef TriggerType);
void DisableGPIOINT(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);

void GPIO_Schmitt_Enable(GPIOPinSMTDef GPIOPin);
void GPIO_Schmitt_Disable(GPIOPinSMTDef GPIOPin);
void GPIO_HighDrive_Enable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);
void GPIO_HighDrive_Disable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);
void GPIO_OpenDrain_Enable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);
void GPIO_OpenDrain_Disable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);
void GPIO_OpenSource_Enable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);
void GPIO_OpenSource_Disable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin);

void GPIO_SetOuput2( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin );
void GPIO_SetInputFloat( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin );
void GPIO_SetInputPullDown( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin );

#endif

void GPIO_SetPB8Toggle( uint32_t us_31_25 );
void GPIO_SetPB9Toggle( uint32_t us_31_25 );
void GPIO_StopPB8Toggle(void);
void GPIO_StopPB9Toggle(void);
void GPIO_SetOutput( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin, GPIOPuPdDef GPIOAttr );
void GPIO_SetInput( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin, GPIOPuPdDef GPIOAttr );
void GPIO_PD_SetOuput( GPIOPortDef Portx, GPIOPinBitDef GPIOPin, BitAction BitVal );
void GPIO_PD_SetInput( GPIOPortDef Portx, GPIOPinBitDef GPIOPin );
void GPIO_PD_Disable( GPIOPortDef Portx );
void WOEMean( uint32_t pixel8_avg );
