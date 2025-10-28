

#include <string.h>
#include <stdlib.h>

#include <time.h>

#include "encrypt_reg.h"
#include "main.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
// <c> Used SHA_STATUS_CALLBACK
#define __HAL_SHA_STATUS_CALLBACK
// </c>

// <c> Enable SHA IRQ
#define _SHA_IRQ_ENABLE
// </c>


#ifdef __HAL_SHA_STATUS_CALLBACK
#define _HAL_SHA_STATUS(status)	_HAL_SHA_Status(status)
#else
#define _HAL_SHA_STATUS(status)	status
#endif

static SHA_StatusCBKTypeDef SHA_StatusCB;


static SHA_StatusTypedef _HAL_SHA_Status(SHA_StatusTypedef status)
{

	if ((SHA_StatusCB)&& (status != SHA_STATUS_OK))
	{
		SHA_StatusCB(status);
	}
	return status;
}

void HAL_SHA_WAIT(void)
{
#ifdef _SHA_IRQ_ENABLE
	while(GetSHA_State() == 0);
	SHA_INT_FLAG = 0;
#else	
	while(SHA_CTR->SHACTR.SHACTRBIT.SHA_STA == 0); // wait till SHA completes
	SHA_CTR->SHACTR.SHACTRBIT.SHA_INT_CLR = 1;  //Clear SHA flag
#endif
}
	
//-----------------------------------------------------------------------------

void HAL_SHA_PeripheralsInit(SHA_StatusCBKTypeDef callback)
{
	
	CLKGatingDisable(HCLKG_ENCRYPT);

#ifdef _SHA_IRQ_ENABLE
	printf(">SHA_IRQ [Enable]\n\r");
	SHA_INT_FLAG = 0;
	SHA_CTR->SHACTR.SHACTRBIT.SHA_INT_MASK = 1;
	NVIC_EnableIRQ(ENCRYPT_IRQn);
#else
	printf(">SHA_IRQ [Disable]\n\r");
#endif

	SHA_StatusCB = callback;
	_HAL_SHA_Status(SHA_STATUS_OK);
}

//------------------------------------------------------------------------------
SHA_StatusTypedef HAL_SHA_Transform(uint8_t *input, 
																		uint32_t len, 
																		uint8_t *output)
{
	uint32_t sha_length_word, sha_length_bit, i;

//	SHA_CTR->SHACTR.SHACTRBIT.SHA_RST = 1;  //SHA Reset
//	while(SHA_CTR->SHACTR.SHACTRBIT.SHA_RST == 1);

	SHA_CTR->SHACTR.SHACTRBIT.SHA_WR_REV = 1;    //SHA IN REV
	SHA_CTR->SHACTR.SHACTRBIT.SHA_RD_REV = 1;    //SHA OUT REV	
	
	sha_length_word = len/4;
	if((len%4) !=0)
		sha_length_word++;
	
	sha_length_bit = len * 8;
	SHA_DATALEN_LOW	= sha_length_word;
	SHA_DATALEN_HIGH	= 0;

	SHA_PAD_CTR->SHAPADCTR.SHAPADCTRBIT.SHA_VALID_BYTE = len%4;
	
	if( (sha_length_bit % 512) < 448 )
		SHA_PAD_CTR->SHAPADCTR.SHAPADCTRBIT.SHA_PAD_PACKET = (uint32_t)((512 - (sha_length_bit % 512) - 64) / 32);
	else
		SHA_PAD_CTR->SHAPADCTR.SHAPADCTRBIT.SHA_PAD_PACKET = (uint32_t)((512 - (sha_length_bit % 512) + 448) / 32);

	SHA_CTR->SHACTR.SHACTRBIT.SHA_STR = 1; //SHA Start
	
//* 2202.5.19 Modify fix SHA WAIT stuck issue
	for(i=4; i<len+4; i+=4)	// input data.
	{	
		SHA_IN = *(uint32_t*)(input+i-4);
		if(i%64 == 0)
		{
			for(int j=0;j<12;j++)
				__NOP();
			while(SHA_CTR->SHACTR.SHACTRBIT.SHA_READY == 0);
		}
	}

	HAL_SHA_WAIT();
	
	for(i=0; i<32; i+=4)	// move the result to Buffer
		*(uint32_t*)(output+i) = SHA_OUT_POINT[i/4];

	return _HAL_SHA_STATUS(SHA_STATUS_OK);
}


//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

