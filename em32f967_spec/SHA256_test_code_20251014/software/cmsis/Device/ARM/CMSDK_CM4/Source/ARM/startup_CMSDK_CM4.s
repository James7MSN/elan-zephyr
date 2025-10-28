;/**************************************************************************//**
; * @file     startup_CMSDK_CM4.s
; * @brief    CMSIS Cortex-M4 Core Device Startup File for
; *           Device CMSDK_CM4
; * @version  V3.01
; * @date     06. March 2012
; *
; * @note
; * Copyright (C) 2012 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/

;/*****************************************************************************/
;/* Startup.s: Startup file for ARM Cortex-M4 Device Family                   */
;/*****************************************************************************/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00002800
;Stack_Size      EQU     0x00001000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

;Heap_Size       EQU     0x00001000
Heap_Size       EQU     0x00030000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                ;0~10
                DCD   GPIOA_COMB_Handler             ; 16+ 0: GPIOA Combined Handler     
                DCD   GPIOB_COMB_Handler             ; 16+ 1: GPIOB Combined Handler     
                DCD   DMA_Wrapper_Handler            ; 16+ 2: DMA Wrapper Handler
                DCD   Wakeup_Handler                 ; 16+ 3: Wakeup Handler             
                DCD   USB_Ctrl_Handler               ; 16+ 4: USB Setup/Suspend/Resume/Reset Handler      
                DCD   USB_EPx_In_Handler             ; 16+16: USB EPx_In Handler         
                DCD   USB_EPx_Out_Handler            ; 16+17: USB EPx_Out Handler        
				DCD   USB_SOF_Handler                ; 16+15: USB SOF Handler            
                DCD   USB_ErrSE1_Handler             ; 16+15: USB SOF Handler            
                DCD   USB_LPM_RESUME_EXTPCKG_Handler ; 16+18: USB LPM RESUME EXTPCKG Handler 
                DCD   TIMER1_Handler                 ; 16+ 9: TIMER1 Handler             
				;11~20
                DCD   TIMER2_Handler                 ; 16+10: TIMER2 Handler             
                DCD   SPI1_Handler                   ; 16+19: SPI1 Handler               
				DCD   UARTRX1_Handler                ; 16+26: UART1 RX Handler           
                DCD   UARTTX1_Handler                ; 16+27: UART1 TX Handler           
                DCD   UARTOVF1_Handler               ; 16+28: UART1 Overflow Handler     
				DCD   UARTRX2_Handler                ; 16+38: UART2 RX Handler           
                DCD   UARTTX2_Handler                ; 16+39: UART2 TX Handler           
                DCD   UARTOVF2_Handler               ; 16+40: UART2 Overflow Handler     
                DCD   I2C1_TX_Handler                ; 16+20: I2C1_TX Handler            
                DCD   I2C1_RX_Handler                ; 16+21: I2C1_RX Handler            
			    ;21~30
				DCD   I2C1_SMTOIF_Handler
                DCD   RTC_int_Handler                ; 16+29: RTC_int Handler            
                DCD   RTC_alarm_Handler              ; 16+30: RTC_alarm Handler          
                DCD   PWMA_Handler                   ; 16+42: PWMA Handler               
                DCD   PWMB_Handler                   ; 16+43: PWMB Handler               
                DCD   PWMC_Handler                   ; 16+44: PWMC Handler               
                DCD   TIMER3_Handler                 ; 16+33: TIMER3 Handler             
                DCD   TIMER4_Handler                 ; 16+34: TIMER4 Handler             
                DCD   SPI2_Handler                   ; 16+35: SPI2 Handler               
                DCD   I2C2_TX_Handler                ; 16+36: I2C2_TX Handler            
				;31~40
                DCD   I2C2_RX_Handler                ; 16+37: I2C2_RX Handler            
				DCD   I2C2_SMTOIF_Handler
				DCD   ISO7816_1Finish_Handler
				DCD   ISO7816_1Err_Handler
				DCD   ISO7816_2Finish_Handler
				DCD   ISO7816_2Err_Handler
				DCD   ECC_Handler
				DCD   TRNG_Handler
				DCD   ENCRYPT_Handler
				DCD   LVDUSB_Handler
                ;41~50
				DCD   LVD_ADC_Handler    
                DCD   LVD_Flash_Handler              ; 16+24: LVD Handler                
                DCD   Systemhold_Handler             ; 16+25: Systemhold Handler        
                DCD   QSPI_Handler                   ; 16+41: CMP2 Handler               
                DCD   ALG1_Handler                   ; 16+52: ALG1 Handler               
                DCD   ALG2_Handler                   ; 16+53: ALG2 Handler               
                DCD   ALG3_Handler                   ; 16+54: ALG3 Handler 
				DCD   USART_TX_Handler                ; 16+49: UART3 RX Handler               
                DCD   USART_RX_Handler                ; 16+50: UART3 TX Handler              
				DCD   USART_ERR_Handler               ; 16+51: UART2 Overflow Handler               
                ;51~60
				
				
					
				DCD   DMA_TRF_Handler                ; 16+ 4: DMA TRF Handler            
                DCD   DMA_BLOCK_Handler              ; 16+ 5: DMA BLOCK Handler          
                DCD   DMA_SRCTRAN_Handler            ; 16+ 6: DMA SRCTRAN Handler        
                DCD   DMA_DSTTRAN_Handler            ; 16+ 7: DMA DSTTRAN Handler        
                DCD   DMA_ERR_Handler                ; 16+ 8: DMA ERR Handler                    
				
                DCD   I2C1_WKUP_Handler              ; 16+47: I2C1 WKUP Handler               
                DCD   I2C2_WKUP_Handler              ; 16+48: I2C2 WKUP Handler 
				DCD   AES256_EN_Handler              ; 16+22: AES256 EN Handler          
                DCD   AES256_DE_Handler              ; 16+23: AES256 DE Handler       
				DCD   USB_Setup				
				DCD   USB_Suspend_Handler            ; 16+12: USB Suspend Handler        
                DCD   USB_Resume_Handler             ; 16+13: USB Resume Handler         
                DCD   USB_Reset_Handler              ; 16+14: USB Reset Handler          	
					
				DCD   ALG4_Handler                   ; 16+55: ALG4 Handler               
                DCD   LVD2_Handler                   ; 16+56: LVD2 Handler 
					
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

jump_to_app		PROC
                EXPORT  jump_to_app
  LDR R0,=0xE000ED08 ; Set R0 to VTOR address
  LDR R1,=0x10000000 ; User’s flash memory based address
  STR R1, [R0] ; Define beginning of user’s flash memory as vector table
  LDR R0,[R1] ; Load initial MSP value
  MOV SP, R0 ; Set SP value (assume MSP is selected)
  LDR R0,[R1, #4] ; Load reset vector
  BX R0 ; Branch to reset handler in user’s flash
; Dummy Exception Handlers (infinite loops which can be modified)
				ENDP
					
jump_to_app_B	PROC
                EXPORT  jump_to_app_B
  LDR R0,=0xE000ED08 ; Set R0 to VTOR address
  LDR R1,=0x10078000 ; User’s flash memory based address
  STR R1, [R0] ; Define beginning of user’s flash memory as vector table
  LDR R0,[R1] ; Load initial MSP value
  MOV SP, R0 ; Set SP value (assume MSP is selected)
  LDR R0,[R1, #4] ; Load reset vector
  BX R0 ; Branch to reset handler in user’s flash
; Dummy Exception Handlers (infinite loops which can be modified)
				ENDP					
					
NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
                ;0~10
                EXPORT GPIOA_COMB_Handler         [WEAK]
                EXPORT GPIOB_COMB_Handler         [WEAK]
				EXPORT DMA_Wrapper_Handler		  [WEAK]
                EXPORT Wakeup_Handler             [WEAK]
				EXPORT USB_Ctrl_Handler           [WEAK]
                EXPORT DMA_TRF_Handler            [WEAK]
                EXPORT DMA_BLOCK_Handler          [WEAK]
                EXPORT DMA_SRCTRAN_Handler        [WEAK]
                EXPORT DMA_DSTTRAN_Handler        [WEAK]
                EXPORT DMA_ERR_Handler            [WEAK]
                EXPORT TIMER1_Handler             [WEAK]
                EXPORT TIMER2_Handler             [WEAK]
                ;11~20
                EXPORT USB_Setup_Handler          [WEAK]
                EXPORT USB_Suspend_Handler        [WEAK]
                EXPORT USB_Resume_Handler         [WEAK]
                EXPORT USB_Reset_Handler          [WEAK]
                EXPORT USB_SOF_Handler            [WEAK]
                EXPORT USB_EPx_In_Handler         [WEAK]
                EXPORT USB_EPx_Out_Handler        [WEAK]
                EXPORT USB_LPM_RESUME_EXTPCKG_Handler [WEAK]
				EXPORT USB_ErrSE1_Handler		  [WEAK]
                EXPORT SPI1_Handler               [WEAK]
                EXPORT I2C1_TX_Handler            [WEAK]
                ;21~30
                EXPORT I2C1_RX_Handler            [WEAK]
				EXPORT I2C1_SMTOIF_Handler        [WEAK]
                EXPORT AES256_EN_Handler          [WEAK]
                EXPORT AES256_DE_Handler          [WEAK]
                EXPORT LVD_Handler                [WEAK]
                EXPORT Systemhold_Handler         [WEAK]
                EXPORT UARTRX1_Handler            [WEAK]
                EXPORT UARTTX1_Handler            [WEAK]
                EXPORT UARTOVF1_Handler           [WEAK]
                EXPORT RTC_int_Handler            [WEAK]
                EXPORT RTC_alarm_Handler          [WEAK]
                ;31~40
                EXPORT ADC_Handler                [WEAK]
                EXPORT CMP1_Handler               [WEAK]
                EXPORT TIMER3_Handler             [WEAK]
                EXPORT TIMER4_Handler             [WEAK]
                EXPORT SPI2_Handler               [WEAK]
                EXPORT I2C2_TX_Handler            [WEAK]
                EXPORT I2C2_RX_Handler            [WEAK]
				EXPORT I2C2_SMTOIF_Handler        [WEAK]	
                EXPORT UARTRX2_Handler            [WEAK]
                EXPORT UARTTX2_Handler            [WEAK]
                EXPORT UARTOVF2_Handler           [WEAK]
                ;41~50
                EXPORT CMP2_Handler               [WEAK]
                EXPORT PWMA_Handler               [WEAK]
                EXPORT PWMB_Handler               [WEAK]
                EXPORT PWMC_Handler               [WEAK]
                ;EXPORT PGA1_Handler               [WEAK]
                ;EXPORT PGA2_Handler               [WEAK]
				EXPORT I2C1_SM_Handler            [WEAK]
                EXPORT I2C2_SM_Handler            [WEAK]
                EXPORT I2C1_WKUP_Handler          [WEAK]
                EXPORT I2C2_WKUP_Handler          [WEAK]
				EXPORT UARTRX3_Handler            [WEAK]               
                EXPORT UARTTX3_Handler            [WEAK]              
				EXPORT ISO7816_1Finish_Handler	  [WEAK]
				EXPORT ISO7816_1Err_Handler		  [WEAK]
				EXPORT ISO7816_2Finish_Handler	  [WEAK]
				EXPORT ISO7816_2Err_Handler		  [WEAK]
				EXPORT ECC_Handler				  [WEAK]
				EXPORT TRNG_Handler				  [WEAK]
				EXPORT ENCRYPT_Handler			  [WEAK]	
				;51~60
				EXPORT UARTOVF3_Handler           [WEAK]              
                EXPORT ALG1_Handler               [WEAK]               
                EXPORT ALG2_Handler               [WEAK]               
                EXPORT ALG3_Handler               [WEAK]  
				EXPORT ALG4_Handler               [WEAK]               
                EXPORT LVD2_Handler      		  [WEAK]
				

;0~10
GPIOA_COMB_Handler         
GPIOB_COMB_Handler
DMA_Wrapper_Handler
Wakeup_Handler             
DMA_TRF_Handler            
DMA_BLOCK_Handler          
DMA_SRCTRAN_Handler        
DMA_DSTTRAN_Handler        
DMA_ERR_Handler            
TIMER1_Handler             
TIMER2_Handler             
;11~20
USB_Setup
USB_Ctrl_Handler
USB_Setup_Handler          
USB_Suspend_Handler        
USB_Resume_Handler         
USB_Reset_Handler          
USB_SOF_Handler            
USB_EPx_In_Handler         
USB_EPx_Out_Handler        
USB_LPM_RESUME_EXTPCKG_Handler 
SPI1_Handler               
I2C1_TX_Handler            
;21~30
I2C1_RX_Handler            
AES256_EN_Handler          
AES256_DE_Handler          
LVD_Handler                
Systemhold_Handler         
UARTRX1_Handler            
UARTTX1_Handler            
UARTOVF1_Handler           
RTC_int_Handler            
RTC_alarm_Handler          
;31~40
ADC_Handler                
CMP1_Handler               
TIMER3_Handler             
TIMER4_Handler             
SPI2_Handler               
I2C2_TX_Handler            
I2C2_RX_Handler            
UARTRX2_Handler            
UARTTX2_Handler            
UARTOVF2_Handler
I2C1_SMTOIF_Handler
I2C2_SMTOIF_Handler
ISO7816_1Finish_Handler
ISO7816_1Err_Handler
ISO7816_2Finish_Handler
ISO7816_2Err_Handler
ECC_Handler
TRNG_Handler
ENCRYPT_Handler
LVDUSB_Handler
USB_ErrSE1_Handler
LVD_ADC_Handler    
LVD_Flash_Handler             
QSPI_Handler                
USART_TX_Handler              
USART_RX_Handler                
USART_ERR_Handler      
;41~50
CMP2_Handler               
PWMA_Handler               
PWMB_Handler               
PWMC_Handler               
;PGA1_Handler               
;PGA2_Handler               
I2C1_SM_Handler
I2C2_SM_Handler
I2C1_WKUP_Handler
I2C2_WKUP_Handler
UARTRX3_Handler
UARTTX3_Handler
;51~60
UARTOVF3_Handler
ALG1_Handler
ALG2_Handler
ALG3_Handler
ALG4_Handler
LVD2_Handler


                B       .
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
