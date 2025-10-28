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

Stack_Size      EQU     0x00000200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000100

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
								DCD     PORTA_COMB_Int		  ; GPIO Port A Combined Int
                DCD     PORTB_COMB_Int    	; GPIO Port B Combined Int
								DCD     0 	
								DCD     WakeUp_Int					; external INT
								DCD			DMA_IntTfr					; DMA transfer end Int
								DCD			DMA_Intblock				; DMA block transfer end Int
								DCD			DMA_IntSrcTfr				; DMA source transfer end Int
								DCD			DMA_IntDstTfr				; DMA destination transfer end Int		
								DCD 		DMA_IntErr					; DMA tranfer error Int
								DCD     TIMER0_Int          ; TIMER 0 Int
								DCD     TIMER1_Int          ; TIMER 1 Int
								DCD     USB_Setup_Handler 	; setup
								DCD     USB_Suspend_Handler ; suspend
								DCD     USB_Resume_Handler 	; resume
								DCD     USB_Reset_Handler 	; reset
								DCD     USB_SofErrSe1_Handler	; se1
								DCD     USB_EPxInEmpty_Handler ; empty
								DCD     USB_EPxOut_Handler	; out
								DCD			USB_LPM_Handler 		; LPM
								DCD			SPI1_Int						; SPI 0 Interrupt Int
								DCD     I2C0TX_Int			    ; I2C 0 TX Int
                DCD     I2C0RX_Int	        ; I2C 0 RX Int
								DCD     0;AES_Encode_Int	    ; AES256 encode Int
								DCD     0;AES_Decode_Int	    ; AES256 decode Int
								DCD     LVD_Int	           	; LVD Int
								DCD     0;SystemHold_Int	    ; SystemHold_Int Int
                DCD     UART0RX_Int			  	; UART 0 RX Int
                DCD     UART0TX_Int         ; UART 0 TX Int
                DCD     UART0OV_Int         ; UART 0 overflow Int
								DCD     RTC_Int	           	; RTC Int	
								DCD     RTCAlarm_Int	      ; RTC alarm Int	
								DCD     ADC_Int			        ; ADC Int	
								DCD     CMP0_Int			      ; CMP 0 Int
								DCD     TIMER2_Int          ; TIMER 2 Int
                DCD     TIMER3_Int          ; TIMER 3 Int
								DCD			SPI2_Int						; SPI 1 Interrupt Int
								DCD     I2C1TX_Int	        ; I2C 1 TX Int
								DCD     I2C1RX_Int			    ; I2C 1 RX Int	
                DCD     UART1RX_Int         ; UART 1 RX Int
                DCD     UART1TX_Int         ; UART 1 TX Int
                DCD     UART1OV_Int         ; UART 1 overflow Int
								DCD     CMP1_Int	          ; CMP 1 Int		
								DCD     0;PWM0_Int	          ; PWM 0 Int	
								DCD     0;PWM1_Int	          ; PWM 1 Int	
								DCD     0;PWM2_Int	          ; PWM 2 Int	
                DCD			PORT_Reserved_Int			; 

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

;USB_Int					PROC
;                EXPORT  USB_Int            				 [WEAK]
;								IMPORT  USB_Int_S
;                BL			USB_Int_S
;								ENDP


PORTA_COMB_Int	PROC
                EXPORT  PORTA_COMB_Int            				 [WEAK]
								;IMPORT  PORTAInt
								B	      .
								ENDP
									
PORTB_COMB_Int	PROC
                EXPORT  PORTB_COMB_Int            				 [WEAK]
								;IMPORT  PORTAInt
								B	      .
								ENDP
									
DMA_IntTfr		PROC
                EXPORT  DMA_IntTfr      	      				 [WEAK]
								;IMPORT  PORTAInt
								B	      .
								ENDP	

DMA_Intblock		PROC
                EXPORT  DMA_Intblock      	      				 [WEAK]
								;IMPORT  PORTAInt
								B	      .
								ENDP		
									
DMA_IntSrcTfr		PROC
                EXPORT  DMA_IntSrcTfr      	      				 [WEAK]
								;IMPORT  PORTAInt
								B	      .
								ENDP		

DMA_IntDstTfr		PROC
                EXPORT  DMA_IntDstTfr      	      				 [WEAK]
								;IMPORT  PORTAInt
								B	      .
								ENDP		

;PORTA_COMB_Int	PROC
;                EXPORT  PORTA_COMB_Int            				 [WEAK]
;								IMPORT  PORTAInt
;								BL			PORTAInt
;								ENDP

;								IMPORT  PORTAInt
;PORTA_COMB_Int						
;								SUB 	lr, lr, #4
;								BL			PORTAInt
;								END


; Dummy Exception Handlers (infinite loops which can be modified)

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
								EXPORT WakeUp_Int								[WEAK]	
                EXPORT UART0RX_Int			           [WEAK]
                EXPORT UART0TX_Int                     [WEAK]
                EXPORT UART0OV_Int                     [WEAK]
                EXPORT UART1RX_Int                     [WEAK]
                EXPORT UART1TX_Int                     [WEAK]
                EXPORT UART1OV_Int                     [WEAK]
                EXPORT TIMER0_Int                      [WEAK]
                EXPORT TIMER1_Int                      [WEAK]
                EXPORT TIMER2_Int                      [WEAK]
                EXPORT TIMER3_Int                      [WEAK]
                EXPORT DMA_IntTfr			           [WEAK]
                EXPORT DMA_Intblock		               [WEAK]
                EXPORT DMA_IntSrcTfr		           [WEAK]
                EXPORT DMA_IntDstTfr		           [WEAK]
                EXPORT DMA_IntErr			           [WEAK]
								EXPORT USB_Setup_Handler          [WEAK]
                EXPORT USB_Suspend_Handler        [WEAK]
                EXPORT USB_Resume_Handler         [WEAK]
                EXPORT USB_Reset_Handler          [WEAK]
                EXPORT USB_SofErrSe1_Handler      [WEAK]
                EXPORT USB_EPxInEmpty_Handler     [WEAK]
                EXPORT USB_EPxOut_Handler         [WEAK]
                EXPORT USB_LPM_Handler            [WEAK]
                EXPORT SPI1_Int			               [WEAK]
                EXPORT SPI2_Int			               [WEAK]
                EXPORT I2C0RX_Int			           [WEAK]
                EXPORT I2C0TX_Int	                   [WEAK]
                EXPORT I2C1RX_Int			           [WEAK]
                EXPORT I2C1TX_Int	                   [WEAK]
                EXPORT USB_Int				           [WEAK]
                EXPORT PGA0_Int			               [WEAK]
                EXPORT PGA1_Int	                       [WEAK]
                EXPORT CMP0_Int			               [WEAK]
                EXPORT CMP1_Int	                       [WEAK]
                EXPORT ADC_Int			               [WEAK]
                EXPORT RTC_Int	           	           [WEAK]
                EXPORT RTCAlarm_Int	                   [WEAK]
                EXPORT LVD_Int	           	           [WEAK]
                EXPORT PORTA_COMB_Int		           [WEAK]
                EXPORT PORTB_COMB_Int    	           [WEAK]
                EXPORT PORT_Reserved_Int			   [WEAK]
                EXPORT AES256_Int	           		   [WEAK]
                EXPORT PWM0_0_Int	           		   [WEAK]
                EXPORT PWM0_1_Int	           		   [WEAK]
                EXPORT PWM1_0_Int	           		   [WEAK]
                EXPORT PWM1_1_Int	           		   [WEAK]
                EXPORT PWM2_0_Int	           		   [WEAK]
				EXPORT PWM2_1_Int	           		   [WEAK]  
WakeUp_Int					
UART0RX_Int			                    
UART0TX_Int          
UART0OV_Int          
UART1RX_Int          
UART1TX_Int          
UART1OV_Int          
TIMER0_Int           
TIMER1_Int           
TIMER2_Int           
TIMER3_Int           
USB_Setup_Handler
USB_Suspend_Handler
USB_Resume_Handler
USB_Reset_Handler
USB_SofErrSe1_Handler
USB_EPxInEmpty_Handler
USB_EPxOut_Handler
USB_LPM_Handler	 
DMA_IntErr			 
SPI1_Int			 
SPI2_Int			 
I2C0RX_Int			 
I2C0TX_Int	         
I2C1RX_Int			 
I2C1TX_Int	         
USB_Int				 
PGA0_Int			 
PGA1_Int	         
CMP0_Int			 
CMP1_Int	         
ADC_Int			     
RTC_Int	             
RTCAlarm_Int	     
LVD_Int	             
;PORTA_COMB_Int		 
;PORTB_COMB_Int       
PORT_Reserved_Int	 
AES256_Int	         
PWM0_0_Int	         
PWM0_1_Int	         
PWM1_0_Int	         
PWM1_1_Int	         
PWM2_0_Int	         
PWM2_1_Int	         
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
