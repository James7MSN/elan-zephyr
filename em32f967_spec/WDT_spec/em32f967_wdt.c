///////////////////////////////////////
// Watchdog

void NMI_Handler (void )
{
	WDOGLOCK = 0x1acce551;

  if( WDOGCONTROL & 0x02 )
	{
		WDOGLOAD = 0x3;????// update for cdc error 20240216
    WDOGLOAD = 0x3;????
		WDOGLOCK = 0;
    while(1);
	}
	else
	{
		WDOGINTCLR = 0x01;
		WDOGLOCK = 0;
	}
}
 
// update for cdc error 20240216
void WDTTimerReload(void)
{
    __disable_irq();
	  WDOGLOCK = 0x1acce551;
	  WDOGINTCLR = 0x01;
	  WDOGINTCLR = 0x01;
	  WDOGLOCK = 0;
	  __enable_irq();
}
 
void WatchDogDisable(void)
{
    WDOGLOCK = 0x1acce551;
    //Delay100us();
    WDOGCONTROL = 0;
    WDOGLOCK = 0;
    //Delay100us();
}
 
void WatchDogEnable( WDTMode mode, uint32_t msec )
{
    CLKGatingDisable(PCLKG_DWG); 
	  __disable_irq();??????????// for continuous load
		WDOGLOCK = 0x1acce551;
	  WDOGCONTROL = 0;??????????// update for CDC error, disable INT
	  WDOGLOAD = 32 * msec;     // use 32K CLK
	  WDOGLOAD = 32 * msec;??????// update for cdc error 20240216
    WDTTimer = 32 * msec; 
	  if( mode )
      WDOGCONTROL = 0x03;     // enable reset
    else
      WDOGCONTROL = 0x01;     // just enable int
    WDOGLOCK = 0;
		__enable_irq();
}
 
void WatchDogResetFast( void )
{
    CLKGATEREG &= ~(0x01<<PCLKG_DWG);
	  __disable_irq();
		WDOGLOCK = 0x1acce551;
	  WDOGCONTROL = 0;??????????// update for CDC error, disable INT
    WDOGLOAD = 3;     ????????// use 32K CLK
	  WDOGLOAD = 3;??????????????// update for cdc error 20240216
    WDTTimer = 1; 
    WDOGCONTROL = 0x03;   ????// enable reset
    WDOGLOCK = 0;
    while(1);
}
 
bool CheckWDTFlag(void )
{
    return SYSSTATUSCTRL->WDTRESETS;
}
 
bool CheckSWRFlag(void )
{
    return SYSSTATUSCTRL->SWRESETS;
}
 
void ClearWDTFlag(void)
{
    uint32_t temp;

    temp = (SYSSTATUS & 0xf8) | 0x02;
    SYSSTATUS = temp;         

    // cannot use structure, because will clear other
}

