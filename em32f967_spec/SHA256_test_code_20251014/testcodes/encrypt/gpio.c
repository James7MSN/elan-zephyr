#include "CMSDK_CM4.h"
#include "main.h"


void GPIOA_COMB_Handler(void)
{
		uint16_t status;
  
		status = GPIOIPA->INTSTATUSANDCLR;
		GPIOIPA->INTSTATUSANDCLR = status;			// clear Interrupt status

}

void GPIOB_COMB_Handler(void)
{
		uint16_t status;
  
		status = GPIOIPB->INTSTATUSANDCLR;
		GPIOIPB->INTSTATUSANDCLR = status;			// clear Interrupt status

}
void Wakeup_Handler( void )
{
		uint16_t status;
    __IO uint8_t temp1,temp2,temp[32];
  
		//IPGPIOBDATAOUT |= 0x1000;
    //IPGPIOBDATAOUT &= 0xffffefff;
    status = ExtWakeupCtrl->WSTATUS;
		ExtWakeupCtrl->WSTATUSCLR = 1;
    //while(1);
    if ( status == 0 )
    {  
      #if( _Module_USB )
      PHYTEST->PHY_TEST_.PHY_TESTBIT.USBWAKEUPEN = 0;
      #endif
      IPGPIOADATAOUT |= 0x20;
      IPGPIOADATAOUT &= 0xffffffdf;
    }

}


void GPIO_Schmitt_Enable(GPIOPinSMTDef GPIOPin)
{
    if(( GPIOPin & 0xf000 ) != 0 ) /*!< port A schmitt ctrl */
    {
        IOSMTPACTRL |= ( GPIOPin & 0xf000 );
    }
    else
    {
        IOSMTPBCTRL |= GPIOPin;    /*!< port B schmitt ctrl */
    }
}

void GPIO_Schmitt_Disable(GPIOPinSMTDef GPIOPin)
{
    if(( GPIOPin & 0xf000) != 0 )  /*!< port A schmitt ctrl */
    {
        IOSMTPACTRL &= ~( GPIOPin & 0xf000 );
    }
    else
    {
        IOSMTPBCTRL &= ~ GPIOPin;  /*!< port B schmitt ctrl */
    }
}


void GPIO_HighDrive_Enable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin)
{
    if((uint64_t)GPIOx == GPIOA_BASE )
		{
       IOHDPACRTL |= GPIOPin;
    }
    else if((uint64_t)GPIOx == GPIOB_BASE )
		{
       IOHDPBCRTL |= GPIOPin;
    }
}

void GPIO_HighDrive_Disable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin)
{
    if((uint64_t)GPIOx == GPIOA_BASE )
		{
       IOHDPACRTL &= ~GPIOPin;
    }
    else if((uint64_t)GPIOx == GPIOB_BASE )
		{
       IOHDPBCRTL &= ~GPIOPin;
    }
}

void GPIO_OpenDrain_Enable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin)
{
    if((uint64_t)GPIOx == GPIOA_BASE )
		{
       IOODEPACTRL |= GPIOPin;
    }
    else if((uint64_t)GPIOx == GPIOB_BASE )
		{
       IOODEPBCTRL |= GPIOPin;
    }
}

void GPIO_OpenDrain_Disable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin)
{
    if((uint64_t)GPIOx == GPIOA_BASE )
		{
       IOODEPACTRL &= ~GPIOPin;
    }
    else if((uint64_t)GPIOx == GPIOB_BASE )
		{
       IOODEPBCTRL &= ~GPIOPin;
    }
}

void GPIO_OpenSource_Enable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin)
{
    if((uint64_t)GPIOx == GPIOA_BASE )
		{
       IOOSEPACTRL |= GPIOPin;
    }
    else if((uint64_t)GPIOx == GPIOB_BASE )
		{
       IOOSEPBCTRL |= GPIOPin;
    }
}

void GPIO_OpenSource_Disable(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin)
{
    if((uint64_t)GPIOx == GPIOA_BASE )
		{
       IOOSEPACTRL &= ~GPIOPin;
    }
    else if((uint64_t)GPIOx == GPIOB_BASE )
		{
       IOOSEPBCTRL &= ~GPIOPin;
    }
}


void GPIO_ToggleBits(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin)
{
    int was_masked;
  
    was_masked = __disable_irq();
    if(( GPIOx->DATAOUT & GPIOPin ) != (uint16_t)Bit_RESET )
    {
         GPIOx->DATAOUT &= ~GPIOPin;
    }
    else
    {
         GPIOx->DATAOUT |= GPIOPin;
    }
    if( !was_masked )
        __enable_irq();
    
}

void GPIO_WriteBit(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin, BitAction BitVal)
{
    int was_masked;
  
    was_masked = __disable_irq();
    if( BitVal != Bit_RESET )
    {
        GPIOx->DATAOUT |= GPIOPin;
    }
    else
    {
        GPIOx->DATAOUT &= ~GPIOPin;
    }
    if( !was_masked )
        __enable_irq();
    
}

bool GPIO_ReadBit( GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin )
{
    bool bitstatus;
  
		if(( GPIOx->DATA & GPIOPin ) != (uint16_t)Bit_RESET )
    {
         bitstatus = (bool)Bit_SET;
    }
    else
    {
         bitstatus = (bool)Bit_RESET;
    }
    return bitstatus;
}

void GPIO_WritePort( GPIO_IPType* GPIOx, uint16_t PortVal )
{
    GPIOx->DATAOUT = PortVal;
}

uint16_t GPIO_ReadPort( GPIO_IPType* GPIOx )
{
	   return ((uint16_t)GPIOx->DATA);
}


void GPIO_Init(GPIO_IPType * GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
		uint32_t value_temp,value_mask;
	
	  //pin_t = GPIO_InitStruct->GPIO_Pin;
	
		// set IO mux = 0x00
		if((uint64_t)GPIOx == GPIOA_BASE )
		{
         CLKGatingDisable(HCLKG_GPIOA);
         if(( GPIO_InitStruct->GPIO_Pin == GPIO_PINSOURCE11 )|| \
           ( GPIO_InitStruct->GPIO_Pin == GPIO_PINSOURCE12 ))
            GPIOMUXSet( PORTA, GPIO_InitStruct->GPIO_Pin, GPIO_MUX01 );
         else
            GPIOMUXSet( PORTA, GPIO_InitStruct->GPIO_Pin, GPIO_MUX00 );
         GPIOMUXSet( PORTANALOG, GPIO_InitStruct->GPIO_Pin, GPIO_MUX00 );			   
		}
		else if( (uint64_t)GPIOx == GPIOB_BASE )
		{
         CLKGatingDisable(HCLKG_GPIOB); 
			   GPIOMUXSet( PORTB, GPIO_InitStruct->GPIO_Pin, GPIO_MUX00 );
		}
		
		// set IO mode
	
		value_mask = 0x01<<GPIO_InitStruct->GPIO_Pin;
		if( GPIO_InitStruct->GPIO_Mode  == GPIO_Mode_IN )
			GPIOx->DATAOUTCLR = value_mask;
		else
			GPIOx->DATAOUTSET = value_mask;
				
		// set IO PUPD
		value_temp = (GPIO_InitStruct->GPIO_PuPd)<<(GPIO_InitStruct->GPIO_Pin*2);
		value_mask = 0x03<<(GPIO_InitStruct->GPIO_Pin*2);
		if((uint64_t)GPIOx == GPIOA_BASE)
		{
				IOPUPDPACTRL = (IOPUPDPACTRL & ~value_mask) | value_temp ;
		}
		else if((uint64_t)GPIOx == GPIOB_BASE)
		{
				 IOPUPDPBCTRL = (IOPUPDPBCTRL & ~value_mask) | value_temp ;
		}
}


//PBx IO mux Swtich 3bits : 000~111
//PB0[2:0],PB1[6:4],PB2[10:8],PB3[14:12],PB4[18:16],PB5[22:20],PB6[26:24],PB7[30:28]
//PBx IO mux Swtich 3bits : 000~111
//PB8[2:0],PB9[6:4],PB10[10:8],PB11[14:12],PB12[18:16],PB13[22:20],PB14[26:24],PB15[30:28]

void GPIOMUXSet( GPIOPortDef GPIOx, GPIOPinNameDef GPIOPin, GPIOMUXDef MUXNum )
{
		uint32_t MUXValue,MUXShift,GPIOPinR;
		
		MUXShift = 0x07;
		MUXValue = MUXNum;
    if( GPIOPin < 8 )
      GPIOPinR = GPIOPin;
    else
      GPIOPinR = GPIOPin-8;
    
		MUXValue <<= ( GPIOPinR * 4 );
		MUXShift <<= ( GPIOPinR * 4 );
		if( GPIOx == PORTA )
		{
			if( GPIOPin < 8 )
        IOMUXPACTRL = ( IOMUXPACTRL & ~MUXShift ) | MUXValue;
      else
        IOMUXPACTRL2 = ( IOMUXPACTRL2 & ~MUXShift ) | MUXValue;
		}
		else if( GPIOx == PORTB )
		{
      if( GPIOPin < 8 )
        IOMUXPBCTRL = ( IOMUXPBCTRL & ~MUXShift ) | MUXValue;
      else
        IOMUXPBCTRL2 = ( IOMUXPBCTRL2 & ~MUXShift ) | MUXValue;
		}
    else if( GPIOx == PORTANALOG )
    {
      MUXShift = 0x03;
      MUXValue = MUXNum;
      MUXValue <<= ( GPIOPin * 2 );
      MUXShift <<= ( GPIOPin * 2 ); 
      IOANAENCTRL = ( IOANAENCTRL & ~MUXShift ) | MUXValue;
    }
}	

void EnableGPIOINT(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin, GPIOINTDef TriggerType)
{
    switch( TriggerType )
    {
      case FALLING :
      {
          GPIOx->INTTYPEEDGESET = GPIOPin; 
          GPIOx->INTPOLCLR = GPIOPin;
      }   break;
      case RISING :
      {
          GPIOx->INTTYPEEDGESET = GPIOPin; 
          GPIOx->INTPOLSET = GPIOPin;
      }   break;
      case LOWLEVEL :
      {
          GPIOx->INTTYPEEDGECLR = GPIOPin; 
          GPIOx->INTPOLCLR = GPIOPin;
      }   break;
      case HIGHLEVEL :
      {
          GPIOx->INTTYPEEDGECLR = GPIOPin; 
          GPIOx->INTPOLSET = GPIOPin;
      }   break;
      default: break;
    }      
    
    GPIOx->INTENSET = GPIOPin;
    if((uint32_t)GPIOx == GPIOA_BASE )
      NVIC_EnableIRQ( PORTA_Int_IRQn );		// enable GPIOA interrupt
    else
      NVIC_EnableIRQ( PORTB_Int_IRQn );		// enable GPIOB interrupt
    
}
void DisableGPIOINT(GPIO_IPType* GPIOx, GPIOPinBitDef GPIOPin)
{
    GPIOx->INTENCLR = GPIOPin;
    if(((uint32_t)GPIOx == GPIOA_BASE ) && (GPIOx->INTENSET == 0))
      NVIC_DisableIRQ( PORTA_Int_IRQn );		// disable GPIOA interrupt
    else if(((uint32_t)GPIOx == GPIOB_BASE ) && (GPIOx->INTENSET == 0))
      NVIC_DisableIRQ( PORTB_Int_IRQn );		// disable GPIOB interrupt
}

void ClearExtWakeUpStatus( void )
{
		ExtWakeupCtrl->WSTATUSCLR = 1;
}

void DisExtWakeUp( WakeUpNumDef WakeUpNum )
{
    ExtWakeupCtrl->WAKEUPEN &= ~(0x01<<WakeUpNum);
    ExtWakeupCtrl->WSTATUSCLR = 1;
    if( ExtWakeupCtrl->WAKEUPEN == 0 )
      NVIC_DisableIRQ( WakeUp_Int_IRQn );	// disable ext port change interrupt
    switch( WakeUpNum )
		{
				case EXTWAKEUP1:		
				  	GPIOMUXSet(PORTA, GPIO_PINSOURCE6, GPIO_MUX00); // wakeup0 = B4 MUX 3
            break;
				case EXTWAKEUP2:		
				  	GPIOMUXSet(PORTA, GPIO_PINSOURCE14, GPIO_MUX00); // wakeup1 = B5 MUX 3
            break;
        case EXTWAKEUP3:		
				  	GPIOMUXSet(PORTA, GPIO_PINSOURCE15, GPIO_MUX00); // wakeup2 = A13 MUX 3
            break;
        case EXTWAKEUP4:		
				  	GPIOMUXSet(PORTB, GPIO_PINSOURCE3, GPIO_MUX00); // wakeup3 = A3 MUX 3
            break;
        case EXTWAKEUP5:		
				  	GPIOMUXSet(PORTB, GPIO_PINSOURCE7, GPIO_MUX00); // wakeup4 = B3 MUX 3
            break;
        case EXTWAKEUP6:		
				  	GPIOMUXSet(PORTB, GPIO_PINSOURCE8, GPIO_MUX00); // wakeup5 = B9 MUX 3
            break;
        case EXTWAKEUP7:		
				  	GPIOMUXSet(PORTB, GPIO_PINSOURCE9, GPIO_MUX00); // wakeup6 = B11 MUX 3
            break;
        default:		
				  	break;
    }  
}

void ExtWakeUp( WakeUpNumDef WakeUpNum, ExtWakeUpDef TriggerEdge )
{
		uint32_t type_value, shift_value;
  
    //SYSREGCTRL->POWEN = 1;
    CLKGatingDisable(PCLKG_PWR);
  
    shift_value = 0x03;
    type_value = TriggerEdge;
    
		shift_value <<= WakeUpNum*2 ;
    type_value <<= WakeUpNum*2 ;
  
    ExtWakeupCtrl->WAKETYPE = (ExtWakeupCtrl->WAKETYPE & ~shift_value) | type_value;
    
    switch( WakeUpNum )
		{
				case EXTWAKEUP1:		
				  	GPIOMUXSet(PORTA, GPIO_PINSOURCE6, GPIO_MUX05); // wakeup0 = B4 MUX 3
            break;
				case EXTWAKEUP2:		
				  	GPIOMUXSet(PORTA, GPIO_PINSOURCE14, GPIO_MUX05); // wakeup1 = B5 MUX 3
            break;
        case EXTWAKEUP3:		
				  	GPIOMUXSet(PORTA, GPIO_PINSOURCE15, GPIO_MUX05); // wakeup2 = A13 MUX 3
            break;
        case EXTWAKEUP4:		
				  	GPIOMUXSet(PORTB, GPIO_PINSOURCE3, GPIO_MUX05); // wakeup3 = A3 MUX 3
            break;
        case EXTWAKEUP5:		
				  	GPIOMUXSet(PORTB, GPIO_PINSOURCE7, GPIO_MUX05); // wakeup4 = B3 MUX 3
            break;
        case EXTWAKEUP6:		
				  	GPIOMUXSet(PORTB, GPIO_PINSOURCE8, GPIO_MUX05); // wakeup5 = B9 MUX 3
            break;
        case EXTWAKEUP7:		
				  	GPIOMUXSet(PORTB, GPIO_PINSOURCE9, GPIO_MUX05); // wakeup6 = B11 MUX 3
            break;
        default:		
				  	break;
    }  
		ExtWakeupCtrl->WAKEUPEN |= 0x01<<WakeUpNum;
    ExtWakeupCtrl->WSTATUSCLR = 1;      // add at 20170908 
    NVIC_EnableIRQ( WakeUp_Int_IRQn );	// enable ext port change interrupt
}

void GPIO_SetOuput2( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin )
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GPIOPin;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_Floating;
    
    GPIO_Init( GPIOx, &GPIO_InitStructure );
  
}

void GPIO_SetOutput( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin, GPIOPuPdDef GPIOAttr )
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GPIOPin;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIOAttr;
    
    GPIO_Init( GPIOx, &GPIO_InitStructure );
  
}
void GPIO_SetInput( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin, GPIOPuPdDef GPIOAttr )
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GPIOPin;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIOAttr;
    
    GPIO_Init( GPIOx, &GPIO_InitStructure );
  
}
void GPIO_SetInputFloat( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin )
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GPIOPin;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_Floating;
    
    GPIO_Init( GPIOx, &GPIO_InitStructure );
}

void GPIO_SetInputPullDown( GPIO_IPType* GPIOx, GPIOPinNameDef GPIOPin )
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GPIOPin;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_Pulldown15K;
    
    GPIO_Init( GPIOx, &GPIO_InitStructure );
}

void GPIO_SetPB8Toggle( uint32_t us_31_25 )
{  
    //CLKGatingDisable(HCLKG_GPIOB);
    //PDPBOE |= 0x100;          // PB8 output enable
    PB8CLOCKRATE = us_31_25;
    PDGPIOCTRL |= 0x04;       // PB8 CLK OUT enable  
}

void GPIO_SetPB9Toggle( uint32_t us_31_25 )
{  
    //CLKGatingDisable(HCLKG_GPIOB);
    //PDPBOE |= 0x200;          // PB9 output enable
    PB9CLOCKRATE = us_31_25;
    PDGPIOCTRL |= 0x08;       // PB9 CLK OUT enable  
}

void GPIO_StopPB8Toggle(void)
{
    PDGPIOCTRL &= 0xfffffffb;       // PB8 CLK OUT disable
} 

void GPIO_StopPB9Toggle(void)
{
    PDGPIOCTRL &= 0xfffffff7;       // PB9 CLK OUT disable
}

void GPIO_PD_SetOuput( GPIOPortDef Portx, GPIOPinBitDef GPIOPin, BitAction BitVal )
{
   if( Portx ==  PORTA )  
   {
     PDPAOE |= GPIOPin;
     if( BitVal != Bit_RESET )
       PDPAOUT |= GPIOPin;
     else
       PDPAOUT &= ~GPIOPin;
     PDGPIOCTRL |= 0x01;       // PA pd enable 
   }
   else
   {
     PDPBOE |= GPIOPin;
     if( BitVal != Bit_RESET )
       PDPBOUT |= GPIOPin;
     else
       PDPBOUT &= ~GPIOPin;
     PDGPIOCTRL |= 0x02;       // PB pd enable 
   }
}

void GPIO_PD_SetInput( GPIOPortDef Portx, GPIOPinBitDef GPIOPin )
{
   if( Portx ==  PORTA )  
   {
     PDPAOE &= ~GPIOPin;
     PDGPIOCTRL |= 0x01;       // PA pd enable 
   }
   else
   {
     PDPBOE &= ~GPIOPin;
     PDGPIOCTRL |= 0x02;       // PB pd enable 
   }
}

void GPIO_PD_Disable( GPIOPortDef Portx )
{
    if( Portx ==  PORTA )  
      PDGPIOCTRL &= ~0x01;       // PA pd disable 
    else if( Portx ==  PORTB )  
      PDGPIOCTRL &= ~0x02;       // PB pd disable
}

