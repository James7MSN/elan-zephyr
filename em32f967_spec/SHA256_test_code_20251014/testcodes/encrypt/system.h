
#include <CMSDK_CM4.h>
#include <stdbool.h>


typedef enum
{ 
  NORMALMODE = 8,
  IDRAMMODE  = 4,
  SYSRAMMODE = 2
}RemapMode;

typedef struct 
{
	__IO uint32_t XTALHIRCSEL	  : 1;  // [0]
	__IO uint32_t XTALLJIRCSEL  : 1;  // [1]
	__IO uint32_t HCLKSEL			  : 2;  // [3:2]
	__IO uint32_t USBCLKSEL		  : 1;  // [4]
	__IO uint32_t HCLKDIV			  : 3;  // [7:5]
  __IO uint32_t QSPICLK_SEL	  : 1;  // [8]
	__IO uint32_t ACC1CLK_SEL		: 1;  // [9]
	__IO uint32_t ENCRYPT_SEL   : 1;  // [10]
	__IO uint32_t Timer1_SEL 	  : 1;  // [11]
  __IO uint32_t Timer2_SEL 	  : 1;  // [12]
  __IO uint32_t Timer3_SEL 	  : 1;  // [13]
  __IO uint32_t Timer4_SEL 	  : 1;  // [14]
  __IO uint32_t QSPICLK_DIV	  : 1;  // [15]
  __IO uint32_t ACC1CLK_DIV	  : 1;  // [16]
  __IO uint32_t EncryptCLK_DIV: 1;  // [17]
  __IO uint32_t RTC_SEL   	  : 1;  // [18]
  __IO uint32_t I2C1Reset_SEL : 1;  // [19]
  __IO uint32_t USBReset_SEL  : 1;  // [20]
  __IO uint32_t HIRC_TESTV 	  : 1;  // [21]
	__IO uint32_t SWRESTN		    : 1;  // [22]
  __IO uint32_t DEEPSLPCLKOFF : 1;  // [23]
  __IO uint32_t ClearECCKey   : 1;  // [24]
	__IO uint32_t POWEN				  : 1;  // [25]
  __IO uint32_t RESETOP	      : 1;  // [26]
  __IO uint32_t PMUCTRL			  : 1;  // [27]
	__IO uint32_t REAMPMODE			: 4;	// [31:28]
} SysReg_Type;	

#define BOOTFROMPIN     0xff
#define BOOTFROMOPTION  0xfe
#define BOOTFROMMAIN    0xfc
#define BOOTFROMBOOT    0xfe



#define SYSREGCTRL      ((SysReg_Type *)0x40030000)

typedef enum
{ 
  USERMODE    = 0,
  OPTIONMODE  = 1,
  ERASEMODE   = 2
}FlashMode;


typedef struct 
{
	__IO uint32_t LEVELRSTS   : 1;
  __IO uint32_t WDTRESETS 	: 1;
	__IO uint32_t SWRESETS	 	: 1;
	 __I uint32_t FLASHWCOUNT : 2;
  __IO uint32_t Reserved 	  : 27;
} SysStatus_Type;	
#define SYSSTATUSCTRL   ((SysStatus_Type *)0x40030004)

typedef struct 
{
	__IO uint32_t WaitCount		        : 3;
  __IO uint32_t WaitCountSet        : 1;
  __IO uint32_t WaitCountPass       : 4;
	__IO uint32_t Reserved   	        : 18;
  __IO uint32_t Remap_SYSR_Boot     : 1;
  __IO uint32_t Remap_IDR_Boot      : 1;
	__IO uint32_t Gating_CPU_CLK      : 1;
  __IO uint32_t Remap_Switch        : 1;
  __IO uint32_t CPUReady_SkipArbiter: 1;
	__IO uint32_t SWRESTEN	 	        : 1;
} MISCReg_Type;	
#define MISCREGCTRL     ((MISCReg_Type *)0x40030008)

typedef struct 
{
	__IO uint32_t BOOT_LVE_PSWD	: 30;
	__IO uint32_t LVE_PDPU   		: 1;
	__IO uint32_t BOOT_PDPU 		: 1;
} BOOTLVEPDPU_Type;	
#define BOOTLVEPDPU     ((BOOTLVEPDPU_Type *)0x4003025c)

typedef struct 
{
	__IO uint32_t CLKOUT0SEL 		: 4;
	__IO uint32_t CLKOUT1SEL 		: 4;
	__IO uint32_t CLKOUT0DIV 		: 7;
	__IO uint32_t CLKOUT1DIV 		: 7;
  __IO uint32_t Reserved 	    : 10;
} TopTest_Type;	
#define TOPTEST         ((TopTest_Type *)0x40030300)


#define RAMSave50CTRL     ((RAMSave_Type *)0x40036E04 )
#define RAMSave70CTRL     ((RAMSave_Type *)0x40036E08 )
#define RAMSave90CTRL     ((RAMSave_Type *)0x40036E0C )

#define RAMSave50CTRL_CS  (*( __IO uint32_t *)0x40036E04 )
#define RAMSave70CTRL_CS  (*( __IO uint32_t *)0x40036E08 )
#define RAMSave90CTRL_CS  (*( __IO uint32_t *)0x40036E0C )
#define AIP_Password_CS   (*( __IO uint32_t *)0x40036F00 )

typedef enum
{ 
  PDSW0 = 0x00,
  PDSW1 = 0x01,
  PDSW2 = 0x02,
  PDSW3 = 0x03,
  PDSW4 = 0x04
}PDSwitch;

typedef struct 
{
	__IO uint32_t POWERSW 		  : 3;
	__IO uint32_t WARMUPCNT		  : 3;
  __IO uint32_t PD_SW_ACK_EN  : 1;
	__IO uint32_t StandBy1_S    : 1;
  __IO uint32_t StandBy2_S    : 1;
  __IO uint32_t SIPPDEnable   : 1;
  __IO uint32_t LDOIdle       : 1;
	__IO uint32_t HIRCPD  	    : 1;
  __IO uint32_t SIRC32PD 	    : 1;
  __IO uint32_t BORPD    	    : 1;
  __IO uint32_t LDO2PD  	    : 1;
  __IO uint32_t RAMPDEnable   : 1;
	__IO uint32_t Reserved		  : 17;
} PowerSW_Type;	

#define POWERSWCTRL     ((PowerSW_Type *)0x40031000  )
#define POWERSWCTRL_CS  (*( __IO uint32_t *)0x40031000  )

typedef struct 
{
	__IO uint32_t MIRCPD 	    : 1;
	__IO uint32_t HIRC_TESTV  : 1;
	__IO uint32_t MIRCRCM		  : 3;
	__IO uint32_t MIRCCA		  : 6;
  __IO uint32_t MIRCTBG	  	: 2;
	__IO uint32_t MIRCTCF	   	: 2;
	__IO uint32_t MIRCTV12	  : 3;
	__IO uint32_t Reserved1	 	: 14;
} MIRC_Type;	
#define MIRCCTRL        ((MIRC_Type *)0x40036000)

typedef struct 
{
	__IO uint32_t MIRCPD 	    : 1;
	__IO uint32_t HIRC_TESTV  : 1;
	__IO uint32_t MIRCRCM		  : 3;
	__IO uint32_t MIRCTall	  : 10;
	__IO uint32_t MIRCTV12	  : 3;
	__IO uint32_t Reserved1	 	: 14;
} MIRC_Type2;	
#define MIRCCTRL_2      ((MIRC_Type2 *)0x40036000)

typedef struct 
{
	__IO uint32_t LJIRCPD 		    : 1;
	__IO uint32_t LJIRCRCM		    : 2;
	__IO uint32_t LJIRCFR       	: 4;
	__IO uint32_t LJIRCCA 	 	    : 5;
	__IO uint32_t LJIRCFC         : 3;
	__IO uint32_t LJIRCTMV10 	    : 2;
  __IO uint32_t LJIRCTESTV10B   : 1;
	__IO uint32_t Reserved	 	    : 14;
} LJIRC_Type;	
#define LJIRCCTRL       ((LJIRC_Type *)0x40036004)

typedef struct 
{
	__IO uint32_t SIRC512KPD	    : 1;
	__IO uint32_t SIRC512KCA 	    : 5;
	__IO uint32_t SIRC512KPSET    : 2;
  __IO uint32_t PLLCLKDuty6_4   : 1;      // ECO for timing 112M
	__IO uint32_t Reserved	 	    : 23;
} SIRC512K_Type;	
#define SIRC512KCTRL    ((SIRC512K_Type *)0x40036100)

typedef struct 
{
	__IO uint32_t SIRC32KPD	      : 1;
	__IO uint32_t SIRC32KCA 	    : 5;
	__IO uint32_t SIRC32KPSET     : 2;
	__IO uint32_t Reserved	 	    : 24;
} SIRC32K_Type;	
#define SIRC32KCTRL     ((SIRC32K_Type *)0x40036104)

typedef struct 
{
	__IO uint32_t XTALFREQSEL	    : 2;
	__IO uint32_t XTALPD     	    : 1;
	__IO uint32_t XTALHZ          : 1;
   __I uint32_t XTALSTABLE      : 1;
  __IO uint32_t XTALCounter	    : 2;
	__IO uint32_t Reserved	 	    : 25;
} XTAL_Type;	
#define XTALCTRL        ((XTAL_Type *)0x40036200)

typedef enum
{ 
  LDO1VS_009V  = 7,
  LDO1VS_120V  = 6,
  LDO1VS_110V  = 5,
  LDO1VS_105V  = 4,
  LDO1VS_080V  = 3,
  LDO1VS_090V  = 2,
  LDO1VS_095V  = 1,
  LDO1VS_100V  = 0
}LDOVSMode;

typedef struct 
{
	__IO uint32_t LDO_IDLE    : 1;// need password, cannot change IDLE & VS at the same time, change VS need wait 10us
	__IO uint32_t LDO_VS      : 3;// 7:0.9V; 6:1.2V; 5:1.1V(LVE=0,default); 4:1.05V; 3:0.8V; 2:0.9V(LVE=1,default); 1:0.95V; 0:1V
	__IO uint32_t LDO_VT      : 6;
  __IO uint32_t Reserved	 	: 22;
} LDO1_Type;	
#define LDO1CTRL   ((LDO1_Type *)0x40036300)

typedef struct 
{
	__IO uint32_t LDO_IDLE    : 1;// need password, cannot change IDLE & VS at the same time, change VS need wait 10us
	__IO uint32_t LDO_VS      : 3;// 7:0.9V; 6:1.2V; 5:1.1V(LVE=0,default); 4:1.05V; 3:0.8V; 2:0.9V(LVE=1,default); 1:0.95V; 0:1V
  __IO uint32_t LDO_VT5     : 5;
  __IO uint32_t LDO_VTH     : 1;
  __IO uint32_t Reserved	 	: 22;
} LDO1Auto_Type;	
#define LDO1AutoCTRL   ((LDO1Auto_Type *)0x40036300)
// for autoload VT bit5 inverse problem

typedef enum
{ 
  LDO2_195V  = 3,
  LDO2_180V  = 2,
  LDO2_215V  = 1,
  LDO2_205V  = 0
}LDO2VSMode;

typedef struct 
{
	__IO uint32_t LDO2_IDLE     : 1;
	__IO uint32_t LDO2_PD       : 1;
	__IO uint32_t LDO2_VREF_EN  : 1;
  __IO uint32_t LDO2_VT       : 4;
	__IO uint32_t LDO2_VSEL     : 2;  // 00:2.05V(0.95V up); 01: 2.15V(0.95V); 10: 1.80V(0.95V down); 11: 1.95V(0.9v)
  __IO uint32_t Reserved	 	  : 13;
} LDO2_Type;	
#define LDO2CTRL        ((LDO2_Type *)0x40036304)

typedef struct 
{
	__IO uint32_t DCDC_EN       : 1;
	__IO uint32_t DCDC_EN_PRE   : 1;    // enable this bit first
	__IO uint32_t DCDC_NOVT     : 2;
  __IO uint32_t DCDC_VFCP_O   : 5;
	__IO uint32_t DCDC_CLK_SEL  : 2;    // 0:512K; 1:32K, 2:pin CLK, 3:no CLK
   __I uint32_t DCDC_PUMPEN   : 1;    // read only for trim
  __IO uint32_t Reserved	 	  : 20;
} DCDC_Type;	
#define DCDCCTRL        ((DCDC_Type *)0x40036308)


typedef struct 
{
	__IO uint32_t PLLLDO_PD     : 1;
	__IO uint32_t PLLLDO_VP_SEL : 1;
	__IO uint32_t PLLLDO_VS     : 3;  // only use 0.9V
  __IO uint32_t PLLLDO_TV12   : 4;
  __IO uint32_t Reserved	 	  : 23;
} LDOPLL_Type;	
#define LDOPLL        ((LDOPLL_Type *)0x4003630c)

typedef struct 
{
	__IO uint32_t LDO1_CHG_EN : 1;
	__IO uint32_t CHGCounter  : 9;
  __IO uint32_t Reserved	 	: 22;
} LDOSwitch_Type;	
#define LDOChange     ((LDOSwitch_Type *)0x40036310)

typedef struct 
{
  __IO uint32_t USBPLLPD         : 1;
  __IO uint32_t USBPLLFASTLOCK   : 1;
  __IO uint32_t USBPLLPSET       : 3;
   __I uint32_t USBPLLSTABLECNT  : 2;
   __I uint32_t USBPLLSTABLE     : 1;
  __IO uint32_t Reserved         : 24;
} USBPLL_Type;        

#define USBPLLCTRL       ((USBPLL_Type *)0x40036400)

typedef struct 
{
  __IO uint32_t SYSPLLPD         : 1;
  __IO uint32_t SYSPLLPSET       : 2;
  __IO uint32_t SYSPLLFSET       : 4;
   __I uint32_t SYSPLLSTABLECNT  : 2;
   __I uint32_t SYSPLLSTABLE     : 1;
  __IO uint32_t Reserved         : 22;
} SYSPLL_Type;         

#define SYSPLLCTRL       ((SYSPLL_Type *)0x40036404)

typedef struct 
{
	__IO uint32_t BOR_BOREN       : 1;
  __IO uint32_t BOR_EN_LTCH     : 1;
  __IO uint32_t BOR_PD_VREF     : 1;
  __IO uint32_t BOR_R_TRIM      : 4;
	__IO uint32_t BOR_reset	      : 1;
  __IO uint32_t Reserved	 	    : 16;
} BOR_Type;	
#define BORCTRL        ((BOR_Type *)0x40036500)

typedef enum
{ 
  //LVDADC_13V = 0,
  //LVDADC_14V = 1,
  LVDADC_15V = 2,
  LVDADC_16V = 3,
  LVDADC_17V = 4,
  LVDADC_18V = 5,
  LVDADC_19V = 6,
  LVDADC_20V = 7,
  LVDADC_21V = 8,
  LVDADC_22V = 9,
  LVDADC_23V = 10,
  LVDADC_24V = 11,
  LVDADC_25V = 12,
  LVDADC_26V = 13,
  LVDADC_27V = 14,
  LVDADC_28V = 15
}LVDADCLevel;

typedef struct 
{
	__IO uint32_t LVDENABLE       : 1;
	__IO uint32_t LVDRTRIM        : 5;
	__IO uint32_t LVDACTRISE      : 1;
	__IO uint32_t LVDACTFALL      : 1;
	__IO uint32_t LVDACTRISEINT   : 1;
	__IO uint32_t LVDACTFALLINT   : 1;
	__IO uint32_t LVDACNT			    : 4;
  __IO uint32_t LVDRESET        : 1;
	__IO uint32_t LVDACNT_PWDIO   : 6;
  __IO uint32_t LVDPWDIOOff     : 1;
  __IO uint32_t LVDFCTRLOff     : 1;
  __IO uint32_t LVD_hold_clock  : 1;
	__IO uint32_t Reserved	 	    : 8;
} LVDFLASH_Type;	
#define LVDFLASHCTRL      ((LVDFLASH_Type *)0x40036504)


typedef struct 
{
	__IO uint32_t LVDENABLE       : 1;
	__IO uint32_t LVDRTRIM        : 4;
	__IO uint32_t LVDACTRISE      : 1;
	__IO uint32_t LVDACTFALL      : 1;
	__IO uint32_t LVDACTRISEINT   : 1;
	__IO uint32_t LVDACTFALLINT   : 1;
	__IO uint32_t LVDACNT			    : 4;
  __IO uint32_t LVDUSBDetectOff : 1;
	__IO uint32_t Reserved	 	    : 18;
} LVDUSB_Type;	
#define LVDUSBCTRL      ((LVDUSB_Type *)0x40036508)

typedef struct 
{
	__IO uint32_t LVDENABLE       : 1;
	__IO uint32_t LVDRTRIM        : 4;
	__IO uint32_t LVDOPTION       : 4;
	__IO uint32_t LVDACTRISE      : 1;
	__IO uint32_t LVDACTFALL      : 1;
	__IO uint32_t LVDACTRISEINT   : 1;
	__IO uint32_t LVDACTFALLINT   : 1;
	__IO uint32_t LVDACNT			    : 4;
	__IO uint32_t Reserved	 	    : 15;
} LVD_Type;	

#define LVDDCDCCTRL       ((LVD_Type *)0x4003650c)
#define LVDVDD33CTRL      ((LVD_Type *)0x40036510)
#define LVDADCCTRL        ((LVD_Type *)0x40036514)

typedef struct 
{
	__IO uint32_t EFTPD           : 1;
	__IO uint32_t EFTVS04         : 1;
  __IO uint32_t EFTVS05         : 1;
  __IO uint32_t EFTVS06         : 1;
  __IO uint32_t EFTVS07         : 1;
	__IO uint32_t SYSHOLDEFTACT	  : 1;
  __IO uint32_t SYSHOLDACT	    : 1;
  __IO uint32_t Reserved	 	    : 25;
} SYSHOLD_Type;	
#define SYSHOLDCTRL       ((SYSHOLD_Type *)0x40036600)

typedef struct 
{
	__IO uint32_t PHYBUFNSEL      : 2;
	__IO uint32_t PHYBUFPSEL      : 2;
	__IO uint32_t PHYRTRIM        : 4;
	__IO uint32_t USBPHYPDB	 	    : 1;
	__IO uint32_t USBPHYRESET     : 1;
	__IO uint32_t USBPHYRSW		    : 1;
  __IO uint32_t Reserved	 	    : 21;
} PHY_Type;	
#define PHYCTRL           ((PHY_Type *)0x40036700)

typedef struct 
{
	__IO uint32_t ADCEN 		    : 1;
	__IO uint32_t ADCCHSEL 	    : 4;
	__IO uint32_t ADCSEREFT		  : 2;
	__IO uint32_t ADCFREERUN    : 1;
	__IO uint32_t ADCSTCH     	: 2;
	__IO uint32_t ADCDOUT	 	    : 12;
	__IO uint32_t ADCSEREFN     : 1;
	__IO uint32_t ADCCALI		 	  : 1;
	__IO uint32_t ADCSIGN  	    : 1;
	__IO uint32_t ADCVOF	 	    : 3;
  __IO uint32_t HALFVDDPD	 	  : 1;
	__IO uint32_t HALFVDDT0	    : 1;
	__IO uint32_t Reserved 	    : 1;
  __IO uint32_t ADCINTFLAG    : 1;
} ADC_Type;	
#define ADCCTRL       ((ADC_Type *)0x40036800)

typedef struct 
{
	__IO uint32_t ADCLKDIV	 	  : 8;
  __IO uint32_t Reserved	 	  : 24;
} ADCDIV_Type;	
#define ADCDIVCTRL    ((ADCDIV_Type *)0x40036804)

typedef struct 
{
	__IO uint32_t ADC_TEST	 	  : 1;
	__IO uint32_t ADC_DELAY	 	  : 4;
  __IO uint32_t Reserved	 	  : 27;
} ADCTEST_Type;	
#define ADCTEST_REG    ((ADCTEST_Type *)0x40036808)

typedef struct 
{
	__IO uint32_t VREF_BG_PD    : 1;
	__IO uint32_t VREF_PD 	    : 1;
	__IO uint32_t VREF_EN_LPF   : 1;
	__IO uint32_t VREF_SEL      : 1;
	__IO uint32_t VREF_TRIM_A  	: 3;
	__IO uint32_t VREF_TRIME    : 4;
	__IO uint32_t VREF_PAD_EN   : 1;
	__IO uint32_t Reserved 	    : 20;
} BGVREF_Type;	
#define BGVREF_REG     ((BGVREF_Type *)0x40036900)

typedef struct 
{
	__IO uint32_t FLASH_VREF_PD      : 1;
	__IO uint32_t FLASH_VREF_TRIM    : 4;
	__IO uint32_t FLASH_VREF_EN_PAD  : 1;
  __IO uint32_t Reserved	 	       : 26;
} VREF_Type;	
#define VREFCTRL        ((VREF_Type *)0x40036D00)

typedef struct 
{
	__IO uint32_t MIRC_CA   : 6;
	__IO uint32_t MIRC_TBG  : 2;
	__IO uint32_t MIRC_TCF  : 2;
  __IO uint32_t MIRC_TV12 : 2;
  __IO uint32_t Reserved	: 18;
} MIRCTRIM_Type;	

typedef struct 
{
	__IO uint32_t MIRC_Tall : 10;
  __IO uint32_t MIRC_TV12 : 3;
  __IO uint32_t Reserved	: 17;
} MIRCTRIM_Type2;

typedef struct 
{
  __IO uint32_t Reserved	: 6;
  __IO uint32_t LJIRC_CA  : 5;
  __IO uint32_t LJIRC_FC  : 3;
  __IO uint32_t LJIRC_CT  : 2;
} INFO_LJIRC_Type;        

#define LDO1_08V       (*( __IO uint32_t *)0x100a6030)
#define LDO1_095V      (*( __IO uint32_t *)0x100a6034)
#define LDO1_10V       (*( __IO uint32_t *)0x100a6038)
#define LDO1_12V       (*( __IO uint32_t *)0x100a603c)

#define SIRC512K_09V   ((SIRCTRIM_Type *)0x100a6040)
#define SIRC512K_11V   ((SIRCTRIM_Type *)0x100a6044)

#define LDO2_18V       (*( __IO uint32_t *)0x100a6050)
#define LDO2_195V      (*( __IO uint32_t *)0x100a6054)
#define LDO2_205V      (*( __IO uint32_t *)0x100a6058)
#define LDO2_215V      (*( __IO uint32_t *)0x100a605c)

#define DCDC_095V      (*( __IO uint32_t *)0x100a6060)
#define DCDC_11V       (*( __IO uint32_t *)0x100a6064)

#define LDO1_105V      (*( __IO uint32_t *)0x100a6068)

#define MIRC16M        ((MIRCTRIM_Type *)0x100a6070)
#define MIRC20M        ((MIRCTRIM_Type *)0x100a6074)
#define MIRC24M        ((MIRCTRIM_Type *)0x100a6078)
#define MIRC28M        ((MIRCTRIM_Type *)0x100a607c)
#define MIRC32M        ((MIRCTRIM_Type *)0x100a6080)

#define MIRC16M_2      ((MIRCTRIM_Type2 *)0x100a6070)
#define MIRC20M_2      ((MIRCTRIM_Type2 *)0x100a6074)
#define MIRC24M_2      ((MIRCTRIM_Type2 *)0x100a6078)
#define MIRC28M_2      ((MIRCTRIM_Type2 *)0x100a607c)
#define MIRC32M_2      ((MIRCTRIM_Type2 *)0x100a6080)

#define LJIRC          ((INFO_LJIRC_Type *)0x100a6090)

//#define PLLUSB_09V     (*( __IO uint32_t *)0x100a6074)
//#define PLLUSB_11V     (*( __IO uint32_t *)0x100a6078)
//#define SYSPLL_09V     (*( __IO uint32_t *)0x100a607c)
//#define SYSPLL_11V     (*( __IO uint32_t *)0x100a6080)
#define LVD_FLASH      (*( __IO uint32_t *)0x100a60a0)
#define LVD_USB        (*( __IO uint32_t *)0x100a60b0)
#define LVD_DCDC       (*( __IO uint32_t *)0x100a60c0)
#define LVD_VDD33      (*( __IO uint32_t *)0x100a60d0)
#define LVD_ADC        (*( __IO uint32_t *)0x100a60e0)

#define USB_PHY        (*( __IO uint32_t *)0x100a60f0)

#define LDO_PLL        (*( __IO uint32_t *)0x100a6100)

#define SRAM_09V       ((SRAMTRIM_Type *)0x100a6110)
#define SRAM_11V       ((SRAMTRIM_Type *)0x100a6114)

#define MIRC12M_R      ((MIRCTRIM_Type *)0x10087f60) // first trim
#define MIRC12M_R_2    ((MIRCTRIM_Type2 *)0x10087f60) // first trim
//#define LJIRC18V       ((INFO_LJIRC_Type *)0x1008050c)

// autoload
#define LDO1_09V_auto  (*( __IO uint32_t *)0x100a7f68)
#define LDO1_11V_auto  (*( __IO uint32_t *)0x100a7f6c)
/*
#define Regutror12_R   (*( __IO uint32_t *)0x100804d8)
#define MIRC12M_R      ((MIRCTRIM_Type *)0x100804c8)
#define MIRC12M_R      ((MIRCTRIM_Type *)0x100804c8)
#define MIRC16M        ((MIRCTRIM_Type *)0x1008045c)
#define MIRC20M        ((MIRCTRIM_Type *)0x10080468)
#define MIRC24M        ((MIRCTRIM_Type *)0x1008046c)
#define MIRC28M        ((MIRCTRIM_Type *)0x10080478)
#define MIRC32M        ((MIRCTRIM_Type *)0x1008047c)
#define LJIRC          ((INFO_LJIRC_Type *)0x10080488)
*/

typedef enum
{ 
  WDTINT = 0,
  WDTRESET = 1
}WDTMode;

typedef enum
{ 
  DIV1	   	= 0x00, 
  DIV2	   	= 0x01,
  DIV4	   	= 0x02, 
  DIV8	   	= 0x03,
  DIV16	   	= 0x04,
  DIV32	   	= 0x05, 
  DIV64   	= 0x06,
  DIV128   	= 0x07  
}AHBPreScaler;

typedef enum
{ 
  NormalFreq 	= 0x0, 
  HighPLLFreq	= 0x1
}FastNormalFreq;	

typedef enum
{ 
  IRC12 	  = 0x00, 
  IRC16 	  = 0x01,
  IRC20 	  = 0x02, 
  IRC24 	  = 0x03, 
  IRC28 	  = 0x04,
  IRC32 	  = 0x05, 
  XTAL24    = 0x11,
  XTAL12    = 0x13,
  External  = 0x20
}FreqSource;

typedef enum
{ 
  IRCLOW 	    = 0x00, 
  IRCHIGH	    = 0x01,
//  XTAL12HIGH 	= 0x13, 
//  XTAL24HIGH  = 0x11, 
  External1   = 0x20
}ClockSource;

typedef enum
{ 
  IRCLOW12 	    = 0x00, 
  IRCLOW16	    = 0x01,
  IRCLOW20 	    = 0x02, 
  IRCLOW24      = 0x03, 
  IRCLOW28 	    = 0x04, 
  IRCLOW32      = 0x05,
//  XTALHIGH96    = 0x10,      
  IRCHIGH64 	  = 0x11,      //  64 = IRC16/4*16; 16/4=4, FSET : 0000
  IRCHIGH80 	  = 0x12,      //  80 = IRC20/4*16; 20/4=5, FSET : 0001
  IRCHIGH96 	  = 0x13,      //  96 = IRC24/4*16; 24/4=6, FSET : 0010
  IRCHIGH112    = 0x14,      // 112 = IRC28/4*16; 28/4=7, FSET : 0011
  IRCHIGH128    = 0x15,      // 128 = IRC32/4*16; 32/4=8, FSET : 0100
  IRCHIGH96Q 	  = 0x16
  
}Freq1Source;

typedef enum
{ 
  Freq80M 	  = 0x01, 
  Freq40M 	  = 0x02,
  Freq20M 	  = 0x03
}FreqValue;

typedef enum
{ 
  HCLKG_DMA 	            =	0x00,
  HCLKG_GPIOA             =	0x01,
  HCLKG_GPIOB             =	0x02,
  PCLKG_LPC               =	0x03,
  HCLKG_7816_1            =	0x04,
  HCLKG_7816_2            =	0x05,
  HCLKG_ENCRYPT           =	0x06,
  PCLKG_USART             =	0x07,
  PCLKG_TMR1              =	0x08,
  PCLKG_TMR2	            =	0x09,
  PCLKG_TMR3	            =	0x0a,
  PCLKG_TMR4	            =	0x0b,
  PCLKG_UART1	            =	0x0c,
  PCLKG_UART2	            =	0x0d,
  PCLKG_RVD1	            =	0x0e,
  HCLKG_ESPI1	            =	0x0f,
  PCLKG_SSP2	            =	0x10,
  PCLKG_I2C1	            =	0x11,
  PCLKG_I2C2	            =	0x12,
  PCLKG_PWM	              =	0x13,
  PCLKG_RVD2              =	0x14,
  PCLKG_UDC	              =	0x15,
  PCLKG_ATRIM	            =	0x16,
  PCLKG_RTC	              =	0x17,
  PCLKG_BKP	              =	0x18,
  PCLKG_DWG	              =	0x19,
  PCLKG_PWR	              =	0x1a,
  PCLKG_CACHE             = 0x1b,
  PCLKG_AIP               = 0x1c,
  PCLKG_ECC               = 0x1d,
  PCLKG_TRNG              = 0x1e,
  HCLKG_EXTSPI            =	0x1f,
  HCLKG_GHM_ACC1          =	0x20,
  HCLKG_GHM_ACC2          =	0x21,
  HCLKG_GHM_ACC3          =	0x22,
  HCLKF_GHM_IP            =	0x23,
  HCLKF_FLASH_BIST        =	0x24,
  HCLKF_GHM_RANSAC        =	0x25,
  HCLKF_SWSPI             =	0x26,
  HCLKF_GHM_DOUBLE        =	0x27,
  HCLKF_GHM_DISTINGUISH   =	0x28,
  HCLKF_GHM_LSE           =	0x29,
  HCLKF_GHM_SAD           =	0x2a,
  HCLKF_GHM_M2D           =	0x2b,
  PCLKG_SSP1	            =	0x30,   // dummy
  PCLKG_ALL               = 0xffff
}CLKGatingSwitch;

#define CLKGATEREG			(*( __IO uint32_t *)0x40030100)
#define CLKGATEREG2			(*( __IO uint32_t *)0x40030104)

typedef enum
{ 
  IPReset_DMA 	  =	0x00,
  IPReset_GPIOA   =	0x01,
  IPReset_GPIOB   =	0x02,
  IPReset_GHM    =	0x03,
  IPReset_7816_1  =	0x04,
  IPReset_7816_2  =	0x05,
  IPReset_ENCRYPT =	0x06,
  IPReset_USART   =	0x07,
  IPReset_TMR1    =	0x08,
  IPReset_TMR2	  =	0x09,
  IPReset_TMR3	  =	0x0a,
  IPReset_TMR4	  =	0x0b,
  IPReset_UART1	  =	0x0c,
  IPReset_UART2	  =	0x0d,
  IPReset_RVD1	  =	0x0e,
  IPReset_ESPI1	  =	0x0f,
  IPReset_SSP2	  =	0x10,
  IPReset_I2C1	  =	0x11,
  IPReset_I2C2	  =	0x12,
  IPReset_PWM	    =	0x13,
  IPReset_SYSRAM  =	0x14,       // system RAM reset for side band error
  IPReset_UDC	    =	0x15,
  IPReset_ATRIM	  =	0x16,
  IPReset_RTC	    =	0x17,
  IPReset_LPC	    =	0x18,     
  IPReset_DWG	    =	0x19,
  IPReset_PWR	    =	0x1a,
  IPReset_CACHE   = 0x1b,
  IPReset_SWAPI   = 0x1c,
  IPReset_ECC     = 0x1d,
  IPReset_TRNG    = 0x1e,
  IPReset_EXTSPI  =	0x1f
}IPResetSwitch;

#define IPRESETREG			(*( __IO uint32_t *)0x40030108)

typedef struct 
{
	__IO uint32_t ESPI_S2		    : 2; // [1:0]
  __IO uint32_t SSP_S2        : 2; // [3:2] 
  __IO uint32_t ISO7816_1_S   : 1; // [4]
  __IO uint32_t ISO7816_2_S   : 1; // [5]
	__IO uint32_t UART1_S   	  : 1; // [6]
  __IO uint32_t UART2_S   	  : 1; // [7]
  __IO uint32_t I2C1_S 	      : 1; // [8]
	__IO uint32_t I2C2_S	 	    : 1; // [9]
  __IO uint32_t USART_S	 	    : 1; // [10]
  __IO uint32_t DMA_CS_S2		  : 2; // [12:11]
  __IO uint32_t SSP_2_4_S	 	  : 1; // [13]
  __IO uint32_t Flash_JTAG	  : 1; // [14]
  __IO uint32_t PWM_D_A1_S	  : 1; // [15]
  __IO uint32_t PWM_E_B1_S	 	: 1; // [16]
  __IO uint32_t PWM_F_C1_S	 	: 1; // [17]
  __IO uint32_t PWM_S	        : 1; // [18]
  __IO uint32_t LPC_PWMAB8_S  : 1; // [19]
  __IO uint32_t LPC_PWMBB9_S  : 1; // [20]
  __IO uint32_t LPC_Test1_S	 	: 2; // [22:21]
  __IO uint32_t LPC_Test2_S	 	: 2; // [24:23]
  __IO uint32_t Reserved   	  : 7; // [31:25]
} IOShare_Type;	
#define IOShareCTRL     ((IOShare_Type *)0x4003023c)

typedef enum
{ 
  H0 = 0x00, H1 = 0x01, H2 = 0x02, H3 = 0x03, H4 = 0x04, H5 = 0x05, H6 = 0x06,
  H7 = 0x07, H8 = 0x08, H9 = 0x09, H10= 0x0a, H11= 0x0b, H12= 0x0c, H13= 0x0d,
  H14= 0x0e, H15= 0x0f, H16= 0x10, H17= 0x11, H18= 0x12, H19= 0x13, H20= 0x14,
  H21= 0x15, H22= 0x16, H23= 0x17
}RTCTimerH;

typedef enum
{ 
  M0 = 0x00, M1 = 0x01, M2 = 0x02, M3 = 0x03, M4 = 0x04, M5 = 0x05, M6 = 0x06,
  M7 = 0x07, M8 = 0x08, M9 = 0x09, M10= 0x0a, M11= 0x0b, M12= 0x0c, M13= 0x0d,
  M14= 0x0e, M15= 0x0f, M16= 0x10, M17= 0x11, M18= 0x12, M19= 0x13, M20= 0x14,
  M21= 0x15, M22= 0x16, M23= 0x17, M24= 0x18, M25= 0x19, M26= 0x1a, M27= 0x1b, 
  M28= 0x1c, M29= 0x1d, M30= 0x1e, M31= 0x1f, M32= 0x20, M33= 0x21, M34= 0x22,
  M35= 0x23, M36= 0x24, M37= 0x25, M38= 0x26, M39= 0x27, M40= 0x28, M41= 0x29,
  M42= 0x2a, M43= 0x2b, M44= 0x2c, M45= 0x2d, M46= 0x2e, M47= 0x2f, M48= 0x30,
  M49= 0x31, M50= 0x32, M51= 0x33, M52= 0x34, M53= 0x35, M54= 0x36, M55= 0x37,
  M56= 0x38, M57= 0x39, M58= 0x3a, M59= 0x3b
}RTCTimerM;

typedef enum
{ 
  S0 = 0x00, S1 = 0x01, S2 = 0x02, S3 = 0x03, S4 = 0x04, S5 = 0x05, S6 = 0x06,
  S7 = 0x07, S8 = 0x08, S9 = 0x09, S10= 0x0a, S11= 0x0b, S12= 0x0c, S13= 0x0d,
  S14= 0x0e, S15= 0x0f, S16= 0x10, S17= 0x11, S18= 0x12, S19= 0x13, S20= 0x14,
  S21= 0x15, S22= 0x16, S23= 0x17, S24= 0x18, S25= 0x19, S26= 0x1a, S27= 0x1b, 
  S28= 0x1c, S29= 0x1d, S30= 0x1e, S31= 0x1f, S32= 0x20, S33= 0x21, S34= 0x22,
  S35= 0x23, S36= 0x24, S37= 0x25, S38= 0x26, S39= 0x27, S40= 0x28, S41= 0x29,
  S42= 0x2a, S43= 0x2b, S44= 0x2c, S45= 0x2d, S46= 0x2e, S47= 0x2f, S48= 0x30,
  S49= 0x31, S50= 0x32, S51= 0x33, S52= 0x34, S53= 0x35, S54= 0x36, S55= 0x37,
  S56= 0x38, S57= 0x39, S58= 0x3a, S59= 0x3b
}RTCTimerS;



#define BACKUPREG0  (*( __IO uint32_t *)( BACKUP_BASE + 0 ))
#define BACKUPREG1  (*( __IO uint32_t *)( BACKUP_BASE + 4 ))
#define BACKUPREG2  (*( __IO uint32_t *)( BACKUP_BASE + 8 ))
#define BACKUPREG3  (*( __IO uint32_t *)( BACKUP_BASE + 12 ))
#define BACKUPREG4  (*( __IO uint32_t *)( BACKUP_BASE + 16 ))
#define BACKUPREG5  (*( __IO uint32_t *)( BACKUP_BASE + 20 ))
#define BACKUPREG6  (*( __IO uint32_t *)( BACKUP_BASE + 24 ))
#define BACKUPREG7  (*( __IO uint32_t *)( BACKUP_BASE + 28 ))
#define BACKUPREG8  (*( __IO uint32_t *)( BACKUP_BASE + 32 ))
#define BACKUPREG9  (*( __IO uint32_t *)( BACKUP_BASE + 36 ))
#define BACKUPREGA  (*( __IO uint32_t *)( BACKUP_BASE + 40 ))
#define BACKUPREGB  (*( __IO uint32_t *)( BACKUP_BASE + 44 ))
#define BACKUPREGC  (*( __IO uint32_t *)( BACKUP_BASE + 48 ))
#define BACKUPREGD  (*( __IO uint32_t *)( BACKUP_BASE + 52 ))
#define BACKUPREGE  (*( __IO uint32_t *)( BACKUP_BASE + 56 ))
#define BACKUPREGF  (*( __IO uint32_t *)( BACKUP_BASE + 60 ))

// WDT register
#define WDOGLOAD			(*( __IO uint32_t *)0x40035000)
#define WDOGVALUE			(*( __IO uint32_t *)0x40035004)
#define WDOGCONTROL		(*( __IO uint32_t *)0x40035008)
#define WDOGINTCLR		(*( __IO uint32_t *)0x4003500c)
#define WDOGRIS				(*( __IO uint32_t *)0x40035010)
#define WDOGMIS				(*( __IO uint32_t *)0x40035014)
#define WDOGLOCK			(*( __IO uint32_t *)0x40035c00)

// Cache Register
#define CACHECTRL			 (*( __IO uint32_t *)0x40037000)
#define CACHECOUNTBASE (*( __IO uint32_t *)0x40037004)
#define CACHECOUNTHIT  (*( __IO uint32_t *)0x40037008)

#define UARTCID_1			(*( __IO uint32_t *)0x40012ff0)
  
// Timer register

typedef enum
{ 
  INTERNALCLK 	= 0x0, 
  EXTENABLE   	= 0x2,
  EXTCLK        = 0x3
}TimerSource;

typedef struct
{
  __IO uint32_t CTRL;
  __IO uint32_t VALUE;
  __IO uint32_t RELOAD;
  __IO uint32_t INTSTACLR;
}Timer_TypeDef;

#define TIMER1 (( Timer_TypeDef * ) TIMER1_BASE)
#define TIMER2 (( Timer_TypeDef * ) TIMER2_BASE)
#define TIMER3 (( Timer_TypeDef * ) TIMER3_BASE)
#define TIMER4 (( Timer_TypeDef * ) TIMER4_BASE)


typedef struct
{
  __IO uint32_t RTSC;
  __IO uint32_t RTMNC;
  __IO uint32_t RTHRC;
  __IO uint32_t RTDYC;
  __IO uint32_t ARSC;
  __IO uint32_t ARMN;
  __IO uint32_t ARHR;
  __IO uint32_t RTRR;
  union
  {
    struct
    {
    __IO uint32_t RTCENABLE   : 1;
    __IO uint32_t RTCSECINTEN : 1;
    __IO uint32_t RTCMININTEN : 1;
    __IO uint32_t RTCHRINTEN  : 1;
    __IO uint32_t RTCDAYINTEN : 1;
    __IO uint32_t RTCALARMEN  : 1;
    __IO uint32_t RTCRELOAD   : 1;
    __IO uint32_t RRCReserved : 25;
    }RTCR_S;
  __IO uint32_t RTCR;
  }RTCR_U;
  __IO uint32_t RTSCR;
  __IO uint32_t RTMNR;
  __IO uint32_t RTHRR;
  __IO uint32_t RTDYR;
  union
  {
  struct
    {
    __IO uint32_t RTCSECINT   : 1;
    __IO uint32_t RTCMININT   : 1;
    __IO uint32_t RTCHRINT    : 1;
    __IO uint32_t RTCDAYINT   : 1;
    __IO uint32_t RTCALARMINT : 1;
    __IO uint32_t RTCReserved : 27;
    }RTITS_S;
  __IO uint32_t RTITS;
  }RTITS_U;
}RTC_TypeDef;

#define RTCCTRL ((RTC_TypeDef *) RTC_BASE)

// RTC register
#define RTC_RTSC 			(*( __IO uint32_t *)0x40032000)
#define RTC_RTMNC 		(*( __IO uint32_t *)0x40032004)
#define RTC_RTHRC 		(*( __IO uint32_t *)0x40032008)
#define RTC_RTDYC 		(*( __IO uint32_t *)0x4003200c)
#define RTC_ARSC			(*( __IO uint32_t *)0x40032010)
#define RTC_ARMN 			(*( __IO uint32_t *)0x40032014)
#define RTC_ARHR 			(*( __IO uint32_t *)0x40032018)
#define RTC_RTRR 			(*( __IO uint32_t *)0x4003201c)
#define RTC_RTCR  		(*( __IO uint32_t *)0x40032020)
#define RTC_RTSCR  		(*( __IO uint32_t *)0x40032024)
#define RTC_RTMNR 		(*( __IO uint32_t *)0x40032028)
#define RTC_RTHRR 		(*( __IO uint32_t *)0x4003202c)
#define RTC_RTDYR 		(*( __IO uint32_t *)0x40032030)
#define RTC_RTITS  		(*( __IO uint32_t *)0x40032034)
/*
union
{  
struct
{
  __IO uint32_t STARTADDR : 19;
  __IO uint32_t SECTORS   : 8;
  __IO uint32_t Reserved  : 4;
  __IO uint32_t ENABLE    : 1;
}Protect_S;
__IO uint32_t Protect;
}Protect_U;
*/



// flash control register
#define FLASHKEY1		  (*( __IO uint32_t *)0x40034000)
#define FLASHKEY2		  (*( __IO uint32_t *)0x40034004)
#define flash_status	(*( __IO uint32_t *)0x40034008)
#define FLASH_SR0     (*( __IO uint32_t *)0x40034010)
#define FLASH_SR1     (*( __IO uint32_t *)0x40034014)
#define flash0_ctrl0	(*( __IO uint32_t *)0x40034020)
#define flash0_ctrl1	(*( __IO uint32_t *)0x40034024)
#define flash0_din1  	(*( __IO uint32_t *)0x40034028)
#define flash0_din0		(*( __IO uint32_t *)0x4003402C)
#define flash0_dout1	(*( __IO uint32_t *)0x40034030)
#define flash0_dout0	(*( __IO uint32_t *)0x40034034)
#define flash1_ctrl0  (*( __IO uint32_t *)0x40034038)
#define flash1_ctrl1	(*( __IO uint32_t *)0x4003403C)
#define flash1_din1	  (*( __IO uint32_t *)0x40034040)
#define flash1_din0	  (*( __IO uint32_t *)0x40034044)
#define flash1_dout1  (*( __IO uint32_t *)0x40034048)
#define flash1_dout0	(*( __IO uint32_t *)0x4003404C)
#define BPT_PSWR		  (*( __IO uint32_t *)0x40034050)	

#define EFUSE_OUT1    (*( __IO uint32_t *)0x40036f04)
#define EFUSE_OUT2    (*( __IO uint32_t *)0x40036f08)

#define PIN_CTRL      (*( __IO uint32_t *)0x40034118)

typedef struct 
{
	__IO uint32_t WARMUPTIME      : 2;
	__IO uint32_t RESETTIME	 	    : 2;
	__IO uint32_t SYSHOLDTIME     : 2;
	__IO uint32_t SYSHOLDENB	    : 1;
  __IO uint32_t SYSHOLDUDC	    : 1;
  __IO uint32_t SYSHOLDRSTUSB   : 1;
  __IO uint32_t SYSHOLDSEL	    : 1;
  __IO uint32_t DEFAULTCONFIG   : 2;
  __IO uint32_t Reserved	 	    : 20;
} Option1_Type;	
#if( EM32F867A )
#define Option1CTRL           ((Option1_Type *)0x10080000)
#else
#define Option1CTRL           ((Option1_Type *)0x00080000)
#endif

typedef struct 
{
	__IO uint32_t BMSO            : 1;
  __IO uint32_t BMS             : 1;
  __IO uint32_t Reserved	 	    : 30;
} Option2_Type;	
#if( EM32F867A )
#define Option2CTRL           ((Option1_Type *)0x10080004)
#else
#define Option2CTRL           ((Option1_Type *)0x00080004)
#endif


typedef struct 
{
	__IO uint32_t CHIPPTB         : 8;
  __IO uint32_t Reserved	 	    : 24;
} CHIPPT_Type;	
#if( EM32F867A )
#define CHIPPTCTRL           ((Option1_Type *)0x10080010)
#else
#define CHIPPTCTRL           ((Option1_Type *)0x00080010)
#endif


#if( EM32F867A )
#define INFOSTART     (( __IO uint32_t *)0x10080000)
#define OPTION2       (( __IO uint32_t *)0x10080004)
#define CHIPPT        (( __IO uint32_t *)0x10080010)
#define FRDP1         (( __IO uint32_t *)0x10080020)
#define FRDP2         (( __IO uint32_t *)0x10080024)
#define FWRP1         (( __IO uint32_t *)0x10080030)
#define FWRP2         (( __IO uint32_t *)0x10080034)
#define LOCKAREA      (( __IO uint32_t *)0x10080040)

#else
#define INFOSTART     (( __IO uint32_t *)0x00080000)
#define OPTION2       (( __IO uint32_t *)0x00080004)
#define CHIPPT        (( __IO uint32_t *)0x00080010)
#define FRDP1         (( __IO uint32_t *)0x00080020)
#define FRDP2         (( __IO uint32_t *)0x00080024)
#define FWRP1         (( __IO uint32_t *)0x00080030)
#define FWRP2         (( __IO uint32_t *)0x00080034)
#define LOCKAREA      (( __IO uint32_t *)0x00080040)
#endif

#define GPIOBPID1     (*( __IO uint32_t *)0x40021fe4)
#define GPIOBCID1     (*( __IO uint32_t *)0x40021ff4)

// Algoritm control register
#define alg_en		            (*( __IO uint32_t *)0x40016000)
#define alg1_w0		            (*( __IO uint32_t *)0x40016004)
#define alg1_operate_n		    (*( __IO uint32_t *)0x40016008)
#define alg1_line1_addr	      (*( __IO uint32_t *)0x4001600c)
#define alg1_line2_addr	      (*( __IO uint32_t *)0x40016010)
#define alg1_line3_addr	      (*( __IO uint32_t *)0x40016014)
#define alg1_gxgx_addr	      (*( __IO uint32_t *)0x40016018)
#define alg1_gygy_addr        (*( __IO uint32_t *)0x4001601c)
#define alg1_gxgy_addr        (*( __IO uint32_t *)0x40016020)
#define alg2_w	              (*( __IO uint32_t *)0x40016024)
#define alg2_n		            (*( __IO uint32_t *)0x40016028)
#define alg2_pix_count        (*( __IO uint32_t *)0x4001602c)
#define alg2_gxxsumline_addr	(*( __IO uint32_t *)0x40016030)
#define alg2_gyysumline_addr	(*( __IO uint32_t *)0x40016034)
#define alg2_gxysumline_addr	(*( __IO uint32_t *)0x40016038)
#define alg2_result1_addr     (*( __IO uint32_t *)0x4001603c)
#define alg2_result2_addr     (*( __IO uint32_t *)0x40016040)
#define alg2_result3_addr	    (*( __IO uint32_t *)0x40016044)
#define alg2_result4_addr	    (*( __IO uint32_t *)0x40016048)
#define alg2_result5_addr     (*( __IO uint32_t *)0x4001604c)
#define alg3_w1	              (*( __IO uint32_t *)0x40016050)
#define alg3_w2	              (*( __IO uint32_t *)0x40016054)
#define alg3_line_idx	        (*( __IO uint32_t *)0x40016058)
#define alg3_KoefsSum         (*( __IO uint32_t *)0x4001605c)
#define alg3_Orient_addr		  (*( __IO uint32_t *)0x40016060)
#define alg3_SrcImage_addr    (*( __IO uint32_t *)0x40016064)
#define alg3_DstLine_addr     (*( __IO uint32_t *)0x40016068)
#define alg4_width	          (*( __IO uint32_t *)0x4001606c)
#define alg4_wSumSqr	        (*( __IO uint32_t *)0x40016070)
#define alg4_gxxResult_addr	  (*( __IO uint32_t *)0x40016074)
#define alg4_gyyResult_addr	  (*( __IO uint32_t *)0x40016078)
#define alg4_gxyResult_addr   (*( __IO uint32_t *)0x4001607c)
#define alg4_pModule_addr     (*( __IO uint32_t *)0x40016080)
#define alg4_ResultB_addr	    (*( __IO uint32_t *)0x40016084)
#define alg4_ResultA_addr		  (*( __IO uint32_t *)0x40016088)



void EnableTimer( Timer_TypeDef * Timerx, TimerSource Source, uint32_t usec_count );
void DisableTimer( Timer_TypeDef * Timerx );
void CLKGatingEnable( CLKGatingSwitch GatingN );
void CLKGatingDisable( CLKGatingSwitch GatingN );
void IPRESET( IPResetSwitch IPN );
void SetMainFreq2( ClockSource CLKSource, Freq1Source FreqS, AHBPreScaler PreDIV );
uint32_t GetAHBFreq( void );
uint32_t GetAPBFreq( void );
uint32_t GetMAINFreq(void);
int sendchar(int c);
int getkey(void);
void NOPDelay(int32_t N);
void Delay10us(void);
void Delay100us(void);
void Delay1ms(void);
void Delay10ms(void);
void Delay100ms(void);
void Delay1s(void);
void InitUART2( void );
void ShowChar(void);

void PowerDownSwitch( PDSwitch SW );
void EnableRTC( void );
void DisableRTC( void );
void WaitAlarm( void );
void RTCAlarm( RTCTimerH Hour, RTCTimerM  Minute, RTCTimerS Second );
bool CheckAlarmOn(void);

void SoftwareReset(void);

uint32_t GetSHA_State(void);
uint32_t GetAES_State(void);
uint32_t GetRSA_State(void);
uint32_t GetDMA_State(void);
uint32_t GetTRNG_State(void);
uint32_t GetECC_State(void);
