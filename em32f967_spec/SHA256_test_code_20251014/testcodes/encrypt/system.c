#include "CMSDK_CM4.h"
#include "main.h"
#include "encrypt_reg.h"

__IO static bool bRTCAlarm;
__IO static uint32_t Count1ms,WDTTimer;
__IO uint32_t m_SystemTickCountL, m_SystemTickCountH;
static uint32_t AHBCount=6000; /* FPGA  */
//static uint32_t AHBCount=12000;
__IO static uint32_t IRCFreqs = IRCLOW12;

void NOPDelay(int32_t N)
{
    __IO uint32_t i;
  
    for( i=0; i<N; i++ )
      __nop();
}

void Delay100ms(void)
{
	__IO uint32_t i;
  for( i=0; i<100; i++ )
	{
			 Delay1ms();
	}
}

void Delay10ms(void)
{
	__IO uint32_t i;
  for( i=0; i<100; i++ )
	{
			 Delay100us();
	}
}
void Delay1ms(void)
{
	__IO uint32_t i;
  for( i=0; i<100; i++ )
	{
			 Delay10us();
	}
}

void Delay10us(void)
{
  
  if(( AHBCount == 0 ) || ( AHBCount > 120000 ))
  {
    BACKUPREGF = AHBCount;
    AHBCount = 12000;
  } 
  
  NOPDelay( AHBCount/1000 );
}

void Delay100us(void)
{
  if(( AHBCount == 0 ) || ( AHBCount > 120000 ))
  {
    BACKUPREGF = AHBCount;
    AHBCount = 12000;
  } 
   
  NOPDelay( AHBCount/100 );
}

void Delay1s(void)
{
	__IO uint32_t i;
	for( i=0; i<10000; i++ )
	{
			 Delay100us();
	}
}

void CLKGatingEnable( CLKGatingSwitch GatingN )
{
    if( GatingN == PCLKG_ALL )
    {
      CACHECTRL = 0x00;         // disable cache
      CLKGATEREG = 0xffffffff;
    }  
    else
      CLKGATEREG |= 0x01<<GatingN;
}

void CLKGatingDisable( CLKGatingSwitch GatingN )
{
     if( GatingN == PCLKG_ALL )
      CLKGATEREG = 0;
     else
      CLKGATEREG &= ~(0x01<<GatingN);
}

bool IsCLKGating( CLKGatingSwitch GatingN )
{
     return (bool)( CLKGATEREG & ~(0x01<<GatingN));
}

void IPRESET( IPResetSwitch IPN )
{
    IPRESETREG = ~(0x01<<IPN);
    __nop10();
    IPRESETREG = 0xffffffff;
    __nop10();
}

void SetMainFreq2( ClockSource CLKSource, Freq1Source FreqS, AHBPreScaler PreDIV )
{
	//bool bMainXTAL, bRetrim;
	bool bPLL;
  uint32_t temp;
	
  uint32_t *FlashAddr = (uint32_t *) 0x100A6090;    // temp address
	uint32_t *FlashAddr1 = (uint32_t *) 0x100A60F0;
	uint32_t *LJIRCCTRL_REG = (uint32_t *) 0x40036004;
	
	CLKGatingDisable(PCLKG_AIP);

//2021/07/06 add for check usb trimcode is match with flash trim code	
	if( FlashAddr[0] != 0xffffffff  )
  {
		temp = (FlashAddr[0]<<3)&0x0001fff8;
		LJIRCCTRL_REG[0] = ( LJIRCCTRL_REG[0]&(~0x0001fff8))|temp;
		
		PHYCTRL->PHYRTRIM=FlashAddr1[0]&0x0000000f;
  }
      
   // 20220208 adjust DIV, needn't change wait cycle
    // wait cycle : 3:<=128M, 2:<=96M, 1:<=64M, 0:<=32M

    if( FreqS == IRCFreqs )
    {
       SYSREGCTRL->HCLKDIV = PreDIV;
       return;
    }      
    
    MISCREGCTRL->WaitCountPass = 0x0a;  // password add at 20210119
    MISCREGCTRL->WaitCount = 3;         // max. wait cycle
    MISCREGCTRL->WaitCountSet = 1;      // wait count enable

	if( SYSREGCTRL->HCLKSEL == 0x01 )
	{  
		SYSREGCTRL->HCLKSEL = 0x00;    // select normal speed CLK
		Delay100us();
		SYSPLLCTRL->SYSPLLPD = 1;
		__nop10();
	}
    
	if( CLKSource == External1 )
	{
		SYSREGCTRL->HCLKSEL = 0x02;    // select external CLK
	}
	else
	{     
		if( FreqS >> 4 )
			bPLL = 1;
		else
			bPLL = 0;
      
		if(1)
		{  
			switch( FreqS )
			{
				case IRCLOW12:
				{
					MIRCCTRL_2->MIRCTall = MIRC12M_R_2->MIRC_Tall; 
          MIRCCTRL_2->MIRCTV12 = ~MIRC12M_R_2->MIRC_TV12; 
				}	
				break;
				
				case IRCLOW16:
				case IRCHIGH64:
				{
					MIRCCTRL_2->MIRCTall = MIRC16M_2->MIRC_Tall; 
					MIRCCTRL_2->MIRCTV12 = ~MIRC16M_2->MIRC_TV12; 
				}
				break;
				
				case IRCLOW20:
				case IRCHIGH80:  
				{
					MIRCCTRL_2->MIRCTall = MIRC20M_2->MIRC_Tall; 
					MIRCCTRL_2->MIRCTV12 = ~MIRC20M_2->MIRC_TV12;
				}					
				break;
				
				case IRCLOW24:   
				case IRCHIGH96:
				{
					MIRCCTRL_2->MIRCTall = MIRC24M_2->MIRC_Tall; 
					MIRCCTRL_2->MIRCTV12 = ~MIRC24M_2->MIRC_TV12; 
				}
				break;
					
				case IRCLOW28:
				case IRCHIGH112: 
				{
					MIRCCTRL_2->MIRCTall = MIRC28M_2->MIRC_Tall; 
					MIRCCTRL_2->MIRCTV12 = ~MIRC28M_2->MIRC_TV12; 
				}
				break;
				
				case IRCLOW32:
				case IRCHIGH128: 
				{
					MIRCCTRL_2->MIRCTall = MIRC32M_2->MIRC_Tall; 
					MIRCCTRL_2->MIRCTV12 = ~MIRC32M_2->MIRC_TV12; 
				}
				break;
				
				default: break;  
			}
		}
       
		Delay100us();               // need delay > 50us
		MIRCCTRL->MIRCRCM =( FreqS & 0x0f );       // set IRC freq  
      

		SYSREGCTRL->XTALHIRCSEL = 0;              // select IRC
		
		if( bPLL )
		{
			switch( FreqS )
			{
				case IRCHIGH64:  SYSPLLCTRL->SYSPLLFSET = 0; break;
				case IRCHIGH80:  SYSPLLCTRL->SYSPLLFSET = 1; break;
				case IRCHIGH96:  SYSPLLCTRL->SYSPLLFSET = 2; break;
				case IRCHIGH112: SYSPLLCTRL->SYSPLLFSET = 3; break;
				case IRCHIGH128: SYSPLLCTRL->SYSPLLFSET = 3; break;   // 20201210
				default: break; 
			}
        
			LDOPLL->PLLLDO_PD = 0;
			__nop10();
			__nop10();
			LDOPLL->PLLLDO_VP_SEL = 0;     // select internal regulator
			Delay10us();
			Delay10us();
			SYSPLLCTRL->SYSPLLPD = 0;
			__nop10();
			while( SYSPLLCTRL->SYSPLLSTABLE == 0 );  // wait PLL A stable
			__nop10();
			SYSREGCTRL->HCLKSEL = 0x01;    // select PLL high speed CLK
			__nop10();
		}
		else
		{
			SYSREGCTRL->HCLKSEL = 0x00;    // select normal speed CLK
			Delay100us();
			SYSPLLCTRL->SYSPLLPD = 1;
		}
		IRCFreqs = FreqS;
	}
/*        
	SYSREGCTRL->HCLKDIV = PreDIV;
	//__nop10();
	MISCREGCTRL->WaitCountSet = 0;
	MISCREGCTRL->WaitCountPass = 0;
	AHBCount = GetAHBFreq();
*/

    // 20220208 wait cycle auto set = 0, need set DIV trigger
    if( PreDIV == DIV128 )
      SYSREGCTRL->HCLKDIV = (PreDIV-1);
    else
      SYSREGCTRL->HCLKDIV = (PreDIV+1);
    MISCREGCTRL->WaitCountSet = 0;
    MISCREGCTRL->WaitCountPass = 0;
    SYSREGCTRL->HCLKDIV = PreDIV;
    AHBCount = GetAHBFreq();

}

uint32_t GetAPBFreq(void)
{
    uint32_t irc_freq,irc_pll_freq,main_freq,apb_freq;
      
    switch( MIRCCTRL->MIRCRCM )
    {
        case 0x00 : irc_freq = 12000; irc_pll_freq = 12000*16/2;  break; // 12M/120M
        case 0x01 : irc_freq = 16000; irc_pll_freq = 16000*16/4;  break; // 16M/80M
        case 0x02 : irc_freq = 20000; irc_pll_freq = 20000*16/4;  break; // 20M/100M
        case 0x03 : irc_freq = 24000; irc_pll_freq = 24000*16/4;  break; // 24M/120M
        case 0x04 : irc_freq = 28000; irc_pll_freq = 28000*16/6;  break; // 28M/93M
        case 0x05 : irc_freq = 32000; irc_pll_freq = 32000*16/6;  break; // 32M/107M
    }
    
    switch( SYSREGCTRL->HCLKSEL & 0x03 )
    {
        case 0x00 : main_freq = irc_freq;   break;
        case 0x01 :
        {
            if( SYSREGCTRL->XTALHIRCSEL )
              main_freq = 24000*5;
            else
              main_freq = irc_pll_freq;
        }   break;
        case 0x02 : main_freq = 0xffffffff;   break;  // external input CLK
        default: main_freq = 0;   break;      // undefined
    }
    
     if(FPGA)
    {
         if( SYSREGCTRL->HCLKSEL & 0x03 )
         {
             main_freq = FPGA_XTAL;   // 24M
         }
         else
         {
             main_freq = FPGA_XTAL/4; // 6M
         }
    }
    
    if(( main_freq == 0 ) || ( main_freq == 0xffffffff ))
    {
         apb_freq = main_freq;
    }
    else
    {
         main_freq = main_freq >> ( SYSREGCTRL->HCLKDIV );
         apb_freq =  main_freq/2;
    }
    return apb_freq;
}

uint32_t GetAHBFreq( void )
{
    return GetAPBFreq()*2;
}

uint32_t GetMAINFreq(void)
{
    uint32_t irc_freq,irc_pll_freq,main_freq;
      
    switch( MIRCCTRL->MIRCRCM )
    {
        case 0x00 : irc_freq = 12000; irc_pll_freq = 12000*16/2;  break; // 12M/120M
        case 0x01 : irc_freq = 16000; irc_pll_freq = 16000*16/4;  break; // 16M/80M
        case 0x02 : irc_freq = 20000; irc_pll_freq = 20000*16/4;  break; // 20M/100M
        case 0x03 : irc_freq = 24000; irc_pll_freq = 24000*16/4;  break; // 24M/120M
        case 0x04 : irc_freq = 28000; irc_pll_freq = 28000*16/6;  break; // 28M/93M
        case 0x05 : irc_freq = 32000; irc_pll_freq = 32000*16/6;  break; // 32M/107M
    }
    
    switch( SYSREGCTRL->HCLKSEL & 0x03 )
    {
        case 0x00 : main_freq = irc_freq;   break;
        case 0x01 :
        {
            if( SYSREGCTRL->XTALHIRCSEL )
              main_freq = 24000*5;
            else
              main_freq = irc_pll_freq;
        }   break;
        case 0x02 : main_freq = 0xffffffff;   break;  // external input CLK
        default: main_freq = 0;   break;      // undefined
    }
    
     if(FPGA)
    {
         if( SYSREGCTRL->HCLKSEL & 0x03 )
         {
             main_freq = FPGA_XTAL;   // 24M
         }
         else
         {
             main_freq = FPGA_XTAL/4; // 6M
         }
    }
    
    return main_freq;
}

#ifdef UART2Console
void UART1TX_Int( void )
{
		__IO uint16_t status;
	
		status = UART1->INTSTACLR;
		UART1->INTSTACLR = 0x01;		// clear TX INt flag
}
void UART2RX_Int( void )
{
		__IO uint16_t status;
	
		status = UART2->INTSTACLR;
	  UART2->INTSTACLR = 0x02;		// clear RX INt flag
    UART2RxFinish = 1;
}

void UART2TX_Int( void )
{
		__IO uint16_t status;
	
		status = UART2->INTSTACLR;
		UART2->INTSTACLR = 0x01;		// clear TX INt flag
    UART2TxFinish = 1;
}

void UART1RX_Int( void )
{
		__IO uint16_t status;
	
		status = UART1->INTSTACLR;
		UART1->INTSTACLR = 0x02;		// clear RX INt flag
}


void InitUART2( void )
{
    GPIOMUXSet(PORTA,GPIO_PINSOURCE9,GPIO_MUX01);   // set UART pin
    GPIOMUXSet(PORTA,GPIO_PINSOURCE10,GPIO_MUX01);
    //GPIOMUXSet(PORTB,GPIO_PINSOURCE8,GPIO_MUX02);
    //GPIOMUXSet(PORTB,GPIO_PINSOURCE9,GPIO_MUX02);
    
    // Buad rate test
    
    BuadRate( UART2,38400 );
    UART2->CTRL = 0x07;       // enable TX,RX & TX INT 
    NVIC_EnableIRQ( UART2TX_Int_IRQn );
    NVIC_EnableIRQ( UART2RX_Int_IRQn );
}

int sendchar(int c)
{
    UART2TxFinish = 0;
    UART2->DATA = c;
    while( UART2TxFinish == 0 );
    return (c*2)/2;
}
int getkey(void)
{
    UART2RxFinish = 0;
    while( UART2RxFinish == 0 );
    return UART2->DATA;
}
void ShowChar(void)
{
    printf("APB CLK: %3d\n", 244);
    printf("I am ready\n" );
    //printf("Characters: %c %c \n", 'a', 0x55);
    
    //sendchar(0x55);
    
    //sendchar(0x61);
    
    //sendchar(0x72);
    
}
#endif

void TIMER1_Int (void)
{
		TIMER1->INTSTACLR = 0x01;			  // clear timer status
		//GPIO_ToggleBits(GPIOIPA, GPIO_PIN_7);
	//	mem4 ++;
}
void TIMER2_Int (void)
{
		TIMER2->INTSTACLR = 0x01;			  // clear timer status
		//GPIO_ToggleBits(GPIOIPA, GPIO_PIN_7);
	//	mem5 ++;
}
void TIMER3_Int (void)
{
		TIMER3->INTSTACLR = 0x01;			  // clear timer status
		//GPIO_ToggleBits(GPIOIPA, GPIO_PIN_7);
	//	mem6 ++;
}
void TIMER4_Int (void)
{
		TIMER4->INTSTACLR = 0x01;			  // clear timer status
		//GPIO_ToggleBits(GPIOIPA, GPIO_PIN_7);
	//	mem7 ++;
}

void EnableTimer( Timer_TypeDef * Timerx, TimerSource Source, uint32_t usec_count )
{
    uint32_t timer_count;
  
    switch( (uint64_t) Timerx )
    {
      case TIMER1_BASE :  CLKGatingDisable( PCLKG_TMR1 ); break;
      case TIMER2_BASE :  CLKGatingDisable( PCLKG_TMR2 ); break;
      case TIMER3_BASE :  CLKGatingDisable( PCLKG_TMR3 ); break;
      case TIMER4_BASE :  CLKGatingDisable( PCLKG_TMR4 ); break;
      default : break;
    } 
    Timerx->VALUE = 0;        // if value != 0, reload will be ignore
    if( Source == EXTCLK )
      timer_count = usec_count-1;
    else
    {  
      if( usec_count > 35000 )
        timer_count = GetAPBFreq() * ( usec_count / 1000 );
      else
        timer_count = GetAPBFreq() * usec_count / 1000;
    }  
    Timerx->RELOAD = timer_count;
    
    if( Source != INTERNALCLK )
    {
      switch( (uint64_t) Timerx )
      {
      case TIMER1_BASE :  {
        GPIO_SetInputFloat( GPIOIPA, GPIO_PINSOURCE13);
        GPIOMUXSet(PORTB,GPIO_PINSOURCE1,GPIO_MUX02);  // for TC1
      }break;
      case TIMER2_BASE :  {
        GPIO_SetInputFloat( GPIOIPA, GPIO_PINSOURCE14);
        GPIOMUXSet(PORTB,GPIO_PINSOURCE2,GPIO_MUX02);  // for TC2
      }break;
      case TIMER3_BASE :  {
        GPIO_SetInputFloat( GPIOIPB, GPIO_PINSOURCE10);
        GPIOMUXSet(PORTB,GPIO_PINSOURCE10,GPIO_MUX02);  // for TC3
      }break;
      case TIMER4_BASE :  {
        GPIO_SetInputFloat( GPIOIPA, GPIO_PINSOURCE2);
        GPIOMUXSet(PORTA,GPIO_PINSOURCE2,GPIO_MUX02);   // for TC4
        // mux need set after gpio set
      }break;
      default : break;
      }
    }
    
    switch( (uint64_t) Timerx )
    {
      case TIMER1_BASE :  NVIC_EnableIRQ( TIMER1_IRQn ); break;
      case TIMER2_BASE :  NVIC_EnableIRQ( TIMER2_IRQn ); break;
      case TIMER3_BASE :  NVIC_EnableIRQ( TIMER3_IRQn ); break;
      case TIMER4_BASE :  NVIC_EnableIRQ( TIMER4_IRQn ); break;
      default : break;
    }
    
    if( Source == INTERNALCLK ) 
      Timerx->CTRL = 0x09;      // enable timer & timer interrupt
    else if( Source == EXTENABLE )
      Timerx->CTRL = 0x0b;      // enable timer & timer interrupt
    else  if( Source == EXTCLK )
      Timerx->CTRL = 0x0d;      // enable timer & timer interrupt
}

void DisableTimer( Timer_TypeDef * Timerx )
{
    Timerx->CTRL = 0x0;       // disable timer
    Timerx->INTSTACLR = 0x01; // clear interrupt ststus
    switch( (uint64_t) Timerx )
    {
      case TIMER1_BASE :  NVIC_DisableIRQ( TIMER1_IRQn );
                          CLKGatingEnable( PCLKG_TMR1 ); break;
      case TIMER2_BASE :  NVIC_DisableIRQ( TIMER2_IRQn ); 
                          CLKGatingEnable( PCLKG_TMR2 ); break;
      case TIMER3_BASE :  NVIC_DisableIRQ( TIMER3_IRQn );
                          CLKGatingEnable( PCLKG_TMR3 ); break;
      case TIMER4_BASE :  NVIC_DisableIRQ( TIMER4_IRQn );
                          CLKGatingEnable( PCLKG_TMR4 ); break;
      default : break;
    }
}


void SoftwareReset(void)
{
		MISCREGCTRL->SWRESTEN = 1;    // enable SW Reeset
		SYSREGCTRL->SWRESTN = 0;			// software reset 
    Delay1ms();
}
/*
void RTC_Int(void)
{
		uint16_t	status,i;
		
		status = RTC_RTITS;
		if( status & (0x01 << 1))
		{
				GPIO_ToggleBits( GPIOIPA, GPIO_PIN_7 );
				for( i=0; i<10; i++ )
				{
						 Delay10us();
				}
				mem1 ++;
		}
		if( status & 0x01 )
		{
				GPIO_ToggleBits( GPIOIPA, GPIO_PIN_7 );
				mem0++;
		}
		
		RTC_RTITS &=0xf0;
}
*/

void NMI_Handler (void )
{
		uint32_t	Value1;	
		
		Value1 = WDOGVALUE;
		GPIO_ToggleBits(GPIOIPA, GPIO_PIN_7);
		WDOGLOCK = 0x1acce551;
	  Value1 = WDOGCONTROL;
	  if( Value1 & 0x02 )
		{
				WDOGLOAD = 0x0001;
				WDOGLOCK = 0;
				while(1);
		}
		
	  //WDOGCONTROL = 0;
    	
		//WDOGLOAD = 0x0001;
		//WDOGLOCK = 0;
		if( Value1 != 0 )
		{
			  Value1 = 0;
		}
		
	  //while(1);
	  /*
	  WDOGLOCK = 0x1acce551;
		//WDOGCONTROL = 0x02;
		WDOGINTCLR = 0x01;				// clear interrupt & reload
		WDOGLOCK = 0;
	  GPIO_ToggleBits(GPIOIPA, GPIO_PIN_7);
		mem4 ++;
	  */
	  

}


void SysTick_Handler(void)
{
		//TimingDelay++;
//	GPIO_ToggleBits(GPIOIPA, GPIO_PIN_7);
	m_SystemTickCountL++;
	if(m_SystemTickCountL == 0)
		m_SystemTickCountH++;
}


uint8_t AES_INT_FLAG, RSA_INT_FLAG, SHA_INT_FLAG, DMA_INT_FLAG, ECC_INT_FLAG, TRNG_INT_FLAG;

uint32_t GetSHA_State(void)
{
	return SHA_INT_FLAG;
}

uint32_t GetAES_State(void)
{
	return AES_INT_FLAG;
}

uint32_t GetRSA_State(void)
{
	return RSA_INT_FLAG;
}

uint32_t GetDMA_State(void)
{
	return DMA_INT_FLAG;
}

uint32_t GetTRNG_State(void)
{
	return TRNG_INT_FLAG;
}

uint32_t GetECC_State(void)
{
	return ECC_INT_FLAG;
}

void ENCRYPT_Handler(void)
{

	if(DMA_CTR->DMACTR.DMACTRBIT.DMA_STA == 1)
	{
		DMA_CTR->DMACTR.DMACTRBIT.DMA_INT_CLR = 1;
		DMA_INT_FLAG = 1;
	}	
	if(SHA_CTR->SHACTR.SHACTRBIT.SHA_STA == 1)
	{
		SHA_CTR->SHACTR.SHACTRBIT.SHA_INT_CLR = 1;
		SHA_INT_FLAG = 1;
	}
	if(AES_CTR->AESCTR.AESCTRBIT.AES_STA == 1)
	{
		AES_CTR->AESCTR.AESCTRBIT.AES_INT_CLR = 1;
		AES_INT_FLAG = 1;
	}
	if(RSA_CTR->RSACTR.RSACTRBIT.RSA_STA == 1)
	{
		RSA_CTR->RSACTR.RSACTRBIT.RSA_INT_CLR = 1;
		RSA_INT_FLAG = 1;
	}
	
}

void TRNG_Handler(void)
{
	TRNG_SR->TRNGSR.TRNGSRBIT.TRNG_DV = 1;
	TRNG_INT_FLAG = 1;
}

void ECC_Handler(void)
{
	ECC_STA->ECCSTA.ECCSTABIT.ECC_DONE = 1;
	ECC_INT_FLAG = 1;
	
}


void CacheEnable(void)
{
    FLASH_SR1 &= 0xffffffd;         // disable accelerate
    CLKGatingDisable( PCLKG_CACHE );
    CACHECTRL = 0x03;         // enable cache
    __nop10();
}

void CacheDisable(void)
{
    CACHECTRL = 0x00;         // disable cache
    __nop10();
    CLKGatingEnable( PCLKG_CACHE );
    FLASH_SR1 |= 0x02;         // enable accelerate
}

void RTC_int_Handler(void)
{
		__IO uint16_t status;
    		
    status = RTCCTRL->RTITS_U.RTITS;
    RTC_RTITS &= 0xf0;
    status = RTC_RTITS;
    #ifdef TESTING
    uint32_t sec,min,hour;
    sec = RTCCTRL->RTSC;
    min = RTCCTRL->RTMNC;
    hour = RTCCTRL->RTHRC;
    if( hour )
    {  
      FastToggle(hour*2);
      FastToggle2(hour*2);
    }  
    if( min )
      FastToggle(min*2);
    FastToggle2((sec*2)%60);
    #endif
}

void EnableRTC(void)
{
    CLKGatingDisable(PCLKG_RTC);
    RTCCTRL->RTCR_U.RTCR_S.RTCRELOAD = 1;
    RTCCTRL->RTCR_U.RTCR_S.RTCENABLE = 1;
    RTCCTRL->RTCR_U.RTCR_S.RTCSECINTEN = 1;
    //RTCCTRL->RTCR_U.RTCR_S.RTCRELOAD = 1;
    //RTCCTRL->RTSCR = 1;
    //RTCCTRL->RTMNR = 1;
    RTCCTRL->RTITS_U.RTITS = 0;
    NVIC_EnableIRQ(RTC_Int_IRQn);
}

void DisableRTC(void)
{
    RTCCTRL->RTCR_U.RTCR = 0;
}

void WaitAlarm(void)
{
    while( bRTCAlarm == 0 );
}

void RTCAlarm( RTCTimerH Hour, RTCTimerM Minute, RTCTimerS Second )
{
    uint32_t hr,mn,sc;
  
    CLKGatingDisable(PCLKG_RTC);
    RTCCTRL->RTCR_U.RTCR_S.RTCENABLE = 0;
    
    hr = 0;
    mn = 0;
    sc = RTCCTRL->RTSC + Second;
    if( sc > 59 )
    {
        sc -= 60;
        mn++;
    }
    mn = mn + RTCCTRL->RTMNC + Minute;
    if( mn > 59 )
    {
        mn -= 60;
        hr++;
    }
    hr = hr + RTCCTRL->RTHRC + Hour;
    if( hr >23 )
        hr -= 24;
     
    RTCCTRL->ARHR = hr;
    RTCCTRL->ARMN = mn;
    RTCCTRL->ARSC = sc;
  
    bRTCAlarm = 0;
    NVIC_EnableIRQ( RTCAlarm_IRQn );	// RTC alarm interrupt
    RTCCTRL->RTCR_U.RTCR_S.RTCALARMEN = 1;
    RTCCTRL->RTCR_U.RTCR_S.RTCENABLE = 1;
    
    Count1ms = 0;
    
}


void SetLVE_High( void )
{
  __IO uint32_t LDOVT6_09,temp;
  bool bLDOVTB5_09;
  
  bLDOVTB5_09 = ~((LDO1_09V_auto&0x20)>>5);  // inverse bit5
  LDOVT6_09 = ( LDO1_09V_auto & 0x1f ) | ( bLDOVTB5_09<<5 );
  LDOChange->CHGCounter = 7;
  LDOChange->LDO1_CHG_EN = 1;
  Delay100us();
  PIN_CTRL = 0x00800010;
  while( PIN_CTRL != 0x00800010 );
  __nop10();
  Delay1ms();
  Delay1ms();
  LDOChange->LDO1_CHG_EN = 0;
  LDO1CTRL->LDO_VT = LDOVT6_09;
}

void SetLVE_Low( void )
{
  __IO uint32_t LDOVT6_11,temp;
  bool bLDOVTB5_11;
  
  bLDOVTB5_11 = ~((LDO1_11V_auto&0x20)>>5); 
  LDOVT6_11 = ( LDO1_11V_auto & 0x1f )|( bLDOVTB5_11<<5 );
  LDOChange->CHGCounter = 7;
  LDOChange->LDO1_CHG_EN = 1;
  Delay100us();
  PIN_CTRL = 0x00800030;
  while( PIN_CTRL != 0x00800030 );
  __nop10();
  Delay1ms();
  Delay1ms();
  LDOChange->LDO1_CHG_EN = 0;
  LDO1CTRL->LDO_VT = LDOVT6_11;
}


void PDSW2Sub(void)
{
    SetMainFreq2(IRCLOW,IRCLOW12,DIV1); 
    SetLVE_High();
    BOOTLVEPDPU->BOOT_LVE_PSWD = 0x38888192;
    BOOTLVEPDPU->BOOT_PDPU = 0;
    BOOTLVEPDPU->LVE_PDPU = 0;
    //EnableRTC();
    CLKGatingDisable(PCLKG_AIP); 
    AIP_Password_CS = 0x81948434;
    BORCTRL->BOR_BOREN = 0;
    LDO2CTRL->LDO2_IDLE = 1;
    //LDO2CTRL->LDO2_PD = 1;
    SYSREGCTRL->HCLKSEL = 0x03;    // select 32K CLK
    MIRCCTRL->MIRCPD = 1;
    //LDO1CTRL->LDO_IDLE = 1;
    //POWERSWCTRL->BORPD = 0; ????
    //POWERSWCTRL->HIRCPD = 1;
    //POWERSWCTRL->LDO2PD = 1;
    //POWERSWCTRL->LDOIdle = 1;
    //POWERSWCTRL->SIRC32PD = 1;    // add for RTC wakeup
    if(1)
    {
    RAMSave70CTRL_CS = 0x1ffff;      
    POWERSWCTRL->RAMPDEnable = 1;
    } 
    POWERSWCTRL->SIPPDEnable = 1;    
    CLKGatingEnable(PCLKG_AIP);   //
    POWERSWCTRL->LDOIdle = 1;
    POWERSWCTRL->POWERSW = 2;
    while(1);
}

void PowerDownSwitch( PDSwitch SW )
{
    CLKGatingDisable( PCLKG_PWR );
    CLKGatingDisable( PCLKG_BKP );
    CLKGatingDisable(PCLKG_AIP);
    SYSREGCTRL->POWEN = 1;        // enable power domain
    if( SW == PDSW2 )
      PDSW2Sub();
    if(( SW == PDSW2 ) || ( SW == PDSW4 ))
    {
      SetMainFreq2(IRCLOW,IRCLOW12,DIV1); 
      SetLVE_High();
      BOOTLVEPDPU->BOOT_LVE_PSWD = 0x38888192;
      BOOTLVEPDPU->BOOT_PDPU = 0;
      if(1)
        BOOTLVEPDPU->LVE_PDPU = 0;
      //EnableRTC();
      AIP_Password_CS = 0x81948434;
      if( SW == PDSW2 )
      //if(0)
      {  
        RAMSave90CTRL_CS = 0x1dffb; // only SRAM_128K_136K & UDC RAM alive
        SYSREGCTRL->USBReset_SEL = 1;  // only reset by power on reset
      }  
      else
        RAMSave90CTRL_CS = 0x1ffff;
      
      if(1)
      {  
      POWERSWCTRL->SIPPDEnable = 1;
      POWERSWCTRL->BORPD = 0;
      POWERSWCTRL->HIRCPD = 1;
      POWERSWCTRL->LDO2PD = 1;
      POWERSWCTRL->LDOIdle = 1;
      //POWERSWCTRL->SIRC32PD = 1;    // add for RTC wakeup
      POWERSWCTRL->RAMPDEnable = 1;
      CLKGatingEnable(PCLKG_AIP);   // add in 20201026 for minus ~20 uA
      }  
      POWERSWCTRL->POWERSW = SW;
      //POWERSWCTRL_CS = 0x00000018A;
      //POWERSWCTRL_CS = 0x00000010A;
      //POWERSWCTRL->POWERSW = SW;
    }
    else    
      POWERSWCTRL->POWERSW = SW;
    while( POWERSWCTRL->POWERSW != SW );
    Delay1ms();     // for FPGA
}


