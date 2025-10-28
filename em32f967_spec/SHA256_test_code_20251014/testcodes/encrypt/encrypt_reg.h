#ifndef _ENCRYPT_REG_H_
#define _ENCRYPT_REG_H_
#include <stdint.h>
#include "CMSDK_CM4.h"
#include "em32f967.h"


/** @brief AES encryption register define */

typedef enum
{ 
  AES128  = 4,
  AES256  = 8
}AesKeyLen;

typedef struct
{
	__IO uint32_t AES_STR									:		 1;
	__IO uint32_t AES_ECBMODE							:		 1;
	__IO uint32_t AES_DECODE							:		 1;
	__IO uint32_t AES_EXTPKCS							:		 1;
	__IO uint32_t AES_VALID_BYTE					:		 2;
	__IO uint32_t AESGCTRRESERVED1				:		 2;
	__IO uint32_t AES_KEYLEN    					:		 1;
	__IO uint32_t AESGCTRRESERVED2				:		23;
} AESGCTRBIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint32_t								AESGCTRBYTE;
		__IO AESGCTRBIT_TypeDef			AESGCTRBIT;
	}AESGCTR;
}AES_GCTR_;
#define AES_GCTR								(( AES_GCTR_ *)(ENCRYPT_BASE+0x34))

typedef struct
{
	__IO uint32_t AESCTRRESERVED1					:		 1;
	__IO uint32_t AES_INT_CLR							:		 1;
	__IO uint32_t AES_RST									:		 1;
	__IO uint32_t AES_READY								:		 1;
	__IO uint32_t AES_STA									:		 1;
	__IO uint32_t AES_INT_MASK						:		 1;
	__IO uint32_t AESCTRRESERVED2					:		 2;
	__IO uint32_t AES_WR_REV							:		 1;
	__IO uint32_t AES_RD_REV							:		 1;
	__IO uint32_t AESCTRRESERVED3					:		22;
} AESCTRBIT_TypeDef;

typedef struct
{
	union
	{
		__IO uint32_t								AESCTRBYTE;
		__IO AESCTRBIT_TypeDef			AESCTRBIT;
	}AESCTR;
}AES_CTR_;
#define AES_CTR								(( AES_CTR_ *)(ENCRYPT_BASE+0x38))

#define AES_IV_POINT					(( uint32_t *)(ENCRYPT_BASE+0x3C)) //4 word
#define AES_KEY_POINT					(( uint32_t *)(ENCRYPT_BASE+0x4C)) //8 word
#define AES_IN								(*( uint32_t *)(ENCRYPT_BASE+0x6C))
#define AES_OUT_POINT					(( uint32_t *)(ENCRYPT_BASE+0x70)) //4 word
#define AES_DATALEN						(*( uint32_t *)(ENCRYPT_BASE+0x80))
#define AAD_DATALEN						(*( uint32_t *)(ENCRYPT_BASE+0x98))

/** @brief SHA encryption register define */
typedef struct
{
	__IO uint32_t SHA_STR									:		 1;
	__IO uint32_t SHA_INT_CLR							:		 1;
	__IO uint32_t SHA_RST									:		 1;
	__IO uint32_t SHA_READY								:		 1;
	__IO uint32_t SHA_STA									:		 1;
	__IO uint32_t SHA_INT_MASK						:		 1;
	__IO uint32_t SHACTRRESERVED1					:		 2;
	__IO uint32_t SHA_WR_REV							:		 1;
	__IO uint32_t SHA_RD_REV							:		 1;
	__IO uint32_t SHACTRRESERVED2					:		22;
} SHACTRBIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint32_t								SHACTRBYTE;
		__IO SHACTRBIT_TypeDef			SHACTRBIT;
	}SHACTR;
}SHA_CTR_;

typedef struct
{
	__IO uint32_t SHA_PAD_PACKET					:		 5;
	__IO uint32_t SHAPADCTRRESERVED1			:		 3;
	__IO uint32_t SHA_VALID_BYTE					:		 2;
	__IO uint32_t SHAPADCTRRESERVED2			:		22;
} SHAPADCTRBIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint32_t									SHAPADCTRBYTE;
		__IO SHAPADCTRBIT_TypeDef			SHAPADCTRBIT;
	}SHAPADCTR;
}SHA_PAD_CTR_;
#define SHA_CTR								(( SHA_CTR_ *)ENCRYPT_BASE)
#define SHA_IN								(*( uint32_t *)(ENCRYPT_BASE+0x04))
#define SHA_OUT_POINT					(( uint32_t *)(ENCRYPT_BASE+0x08))
#define SHA_DATALEN_HIGH			(*( uint32_t *)(ENCRYPT_BASE+0x28))
#define SHA_DATALEN_LOW				(*( uint32_t *)(ENCRYPT_BASE+0x2C))
#define SHA_PAD_CTR						(( SHA_PAD_CTR_ *)(ENCRYPT_BASE+0x30))


/** @brief RSA encryption register define */
typedef enum
{ 
  RSA2048  = 64,
  RSA3072  = 96
}RsaKeyLen;

typedef struct
{
	__IO uint32_t RSA_STR									:		 1;
	__IO uint32_t RSA_INT_CLR							:		 1;
	__IO uint32_t RSA_RST									:		 1;
	__IO uint32_t RSACTRRESERVED1					:		 1;
	__IO uint32_t RSA_STA									:		 1;
	__IO uint32_t RSA_INT_MASK						:		 1;
	__IO uint32_t RSACTRRESERVED2					:		 2;
	__IO uint32_t RSA_WR_REV							:		 1;
	__IO uint32_t RSA_RD_REV							:		 1;
	__IO uint32_t RSA_MODE  							:		 1;
	__IO uint32_t RSACTRRESERVED3					:		21;
} RSACTRBIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint32_t								RSACTRBYTE;
		__IO RSACTRBIT_TypeDef			RSACTRBIT;
	}RSACTR;
}RSA_CTR_;
#define RSA_CTR							(( RSA_CTR_ *)(ENCRYPT_BASE+0xFC))
#define RSA_M_POINT					(( uint32_t *)(ENCRYPT_BASE+0x200)) //96 word
#define RSA_E_POINT					(( uint32_t *)(ENCRYPT_BASE+0x400)) //96 word
#define RSA_N_POINT					(( uint32_t *)(ENCRYPT_BASE+0x600)) //96 word
#define RSA_OUT_POINT				(( uint32_t *)(ENCRYPT_BASE+0x800)) //96 word


/** @brief DMA encryption register define */
#define CBC_128_ENCRYPT 0
#define CBC_128_DECRYPT 1
#define ECB_128_ENCRYPT 2
#define ECB_128_DECRYPT 3
#define CBC_256_ENCRYPT 4
#define CBC_256_DECRYPT 5
#define ECB_256_ENCRYPT 6
#define ECB_256_DECRYPT 7
#define CBC_128 0
#define ECB_128 2
#define CBC_256 4
#define ECB_256 6
#define ENCRYPT 0
#define DECRYPT 1

typedef struct
{
	__IO uint32_t DMA_STR									:		 1;
	__IO uint32_t DMA_INT_CLR							:		 1;
	__IO uint32_t DMA_RST									:		 1;
	__IO uint32_t DMACTRRESERVED1					:		 1;
	__IO uint32_t DMA_STA									:		 1;
	__IO uint32_t DMA_INT_MASK						:		 1;
	__IO uint32_t DMA_AES_BYPASS					:		 1;
	__IO uint32_t DMA_SHA_BYPASS					:		 1;
	__IO uint32_t DMA_WR_REV							:		 1;
	__IO uint32_t DMA_RD_REV							:		 1;
	__IO uint32_t DMACTRRESERVED2					:		22;
} DMACTRBIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint32_t								DMACTRBYTE;
		__IO DMACTRBIT_TypeDef			DMACTRBIT;
	}DMACTR;
}DMA_CTR_;
#define DMA_CTR							(( DMA_CTR_ *)(ENCRYPT_BASE+0x84))
#define DMA_SRC							(*( uint32_t *)(ENCRYPT_BASE+0x88))
#define DMA_DST							(*( uint32_t *)(ENCRYPT_BASE+0x8C))
#define DMA_RLEN						(*( uint32_t *)(ENCRYPT_BASE+0x90))
#define DMA_WLEN						(*( uint32_t *)(ENCRYPT_BASE+0x94))

#define DMA_RAM_BASE				(0x20028000)


/** @brief TRNG encryption register define */
typedef struct
{
	__IO uint8_t TRNG_STR									:		 1;
	__IO uint8_t TRNG_INT_MASK						:		 1;
	__IO uint8_t TRNG_POST								:		 1;
	__IO uint8_t TRNG_MODE								:		 1;
	__IO uint8_t TRNGCTRRESERVED					:		 4;
} TRNGCTRBIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint8_t								TRNGCTRBYTE;
		__IO TRNGCTRBIT_TypeDef			TRNGCTRBIT;
	}TRNGCTR;
}TRNG_CTR_;

typedef struct
{
	__IO uint8_t TRNG_DV									:		 1;
	__IO uint8_t TRNG_REP_ERR							:		 1;
	__IO uint8_t TRNG_PROPORTION_ERR			:		 1;
	__IO uint8_t TRNG_FAILURE_TIMEOUT			:		 1;
	__IO uint8_t TRNG_PP_KAT							:		 1;
	__IO uint8_t TRNGSRRESERVED						:		 3;
} TRNGSRBIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint8_t								TRNGSRBYTE;
		__IO TRNGSRBIT_TypeDef			TRNGSRBIT;
	}TRNGSR;
}TRNG_SR_;

typedef struct
{
	__IO uint32_t TRNG_REP_CUTOFF					:		 8;
	__IO uint32_t TRNG_ADAPT_CUTOFF				:		12;
	__IO uint32_t TRNG_ADAPT_WINDOW				:		12;
} TRNGMCRBIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint32_t								TRNGMCRBYTE;
		__IO TRNGMCRBIT_TypeDef			TRNGMCRBIT;
	}TRNGMCR;
}TRNG_MCR_;
#define TRNG_CTR							(( TRNG_CTR_ *)(TRNG_BASE+0x00))
#define TRNG_DLY							(*( uint8_t *)(TRNG_BASE+0x04))
#define TRNG_DIV							(*( uint8_t *)(TRNG_BASE+0x08))
#define TRNG_SR								(( TRNG_SR_ *)(TRNG_BASE+0x0C))
#define TRNG_OUT_POINT				(( uint32_t *)(TRNG_BASE+0x10)) //8 word
#define TRNG_MCR							(( TRNG_MCR_ *)(TRNG_BASE+0x30))
#define TRNG_MFTR							(*( uint32_t *)(TRNG_BASE+0x34))
#define TRNG_MAPC							(*( uint32_t *)(TRNG_BASE+0x38))


/** @brief ECC encryption register define */
typedef struct
{
	__IO uint32_t ECC_DONE								:		 1;
	__IO uint32_t ECC_BUSY								:		 1;
	__IO uint32_t ECCSTARESERVED2					:		30;
} ECCSTABIT_TypeDef;
typedef struct
{
	union
	{
		__IO uint32_t								ECCSTABYTE;
		__IO ECCSTABIT_TypeDef			ECCSTABIT;
	}ECCSTA;
}ECC_STA_;
#define ECC_STA								(( ECC_STA_ *)(ECC256_BASE+0x04))


//AES
typedef struct {
	uint32_t 	AESDKR[8];
	uint32_t  AESIV[4];
}AES_KeyTypedef;

typedef enum {
	AES_STATUS_OK, 
	AES_STATUS_ERR_NOT_CORRECT_ADDRESS,  
	AES_STATUS_ERR_BUFFER_SIZE_OVER_MAX_LEN,
	AES_STATUS_ERR_NOT_CORRECT_LEN, 
	AES_STATUS_ERR_BUSY,
}AES_StatusTypedef;
typedef void (*AES_StatusCBKTypeDef)(AES_StatusTypedef state);

//SHA
typedef enum {
	SHA_STATUS_OK, 
	SHA_STATUS_ERR_NOT_CORRECT_ADDRESS,  
	SHA_STATUS_ERR_BUFFER_SIZE_OVER_MAX_LEN,
	SHA_STATUS_ERR_NOT_CORRECT_LEN, 
	SHA_STATUS_ERR_BUSY,
}SHA_StatusTypedef;
typedef void (*SHA_StatusCBKTypeDef)(SHA_StatusTypedef state);

//RSA
typedef enum {
	RSA_STATUS_OK, 
	RSA_STATUS_ERR_NOT_CORRECT_ADDRESS,  
	RSA_STATUS_ERR_BUFFER_SIZE_OVER_MAX_LEN,
	RSA_STATUS_ERR_NOT_CORRECT_LEN, 
	RSA_STATUS_ERR_BUSY,
}RSA_StatusTypedef;
typedef void (*RSA_StatusCBKTypeDef)(RSA_StatusTypedef state);

//DMA
typedef enum {
	DMA_MUTEX_LOCK,
	DMA_MUTEX_UNLOCK,
}DMA_MutexTypedef;

typedef enum {
	DMA_LOCK_STATUS_OK, 
	DMA_LOCK_STATUS_ERR,
}DMA_LockStatusTypedef;

typedef enum {
	DMA_STATUS_OK, 
	DMA_STATUS_ERR_NOT_CORRECT_ADDRESS,  
	DMA_STATUS_ERR_BUFFER_SIZE_OVER_MAX_LEN,
	DMA_STATUS_ERR_NOT_CORRECT_LEN, 
	DMA_STATUS_ERR_BUSY,
}DMA_StatusTypedef;
typedef void (*DMA_StatusCBKTypeDef)(DMA_StatusTypedef state);

extern uint8_t AES_INT_FLAG, RSA_INT_FLAG, SHA_INT_FLAG, DMA_INT_FLAG, ECC_INT_FLAG, TRNG_INT_FLAG;

//AES Initial
void HAL_AES_PeripheralsInit(AES_StatusCBKTypeDef callback);

//AES ECB Mode Encrypted
AES_StatusTypedef HAL_AES_ECB_Encrypted(AES_KeyTypedef* pkey, //AES key
																				uint8_t *psrc_buf,    //Input data buffer point
																				uint8_t *pdest_buf,   //Output data buffer point
																				uint32_t* size,       //Input data size
																				uint8_t klen_word);   //AES key length,For example AES128 or AES256
//AES ECB Mode Decrypted
AES_StatusTypedef HAL_AES_ECB_Decrypted(AES_KeyTypedef* pkey, //AES key
																				uint8_t *psrc_buf,    //Input data buffer point
																				uint8_t *pdest_buf,   //Output data buffer point
																				uint32_t* size,       //Input data size
																				uint8_t klen_word);   //AES key length,For example AES128 or AES256
//AES CBC Mode Encrypted
AES_StatusTypedef HAL_AES_CBC_Encrypted(AES_KeyTypedef* pkey, //AES key(with iv)
																				uint8_t *psrc_buf,    //Input data buffer point
																				uint8_t *pdest_buf,   //Output data buffer point
																				uint32_t* size,       //Input data size
																				uint8_t klen_word);   //AES key length,For example AES128 or AES256
//AES CBC Mode Decrypted
AES_StatusTypedef HAL_AES_CBC_Decrypted(AES_KeyTypedef* pkey, //AES key(with iv)
																				uint8_t *psrc_buf,    //Input data buffer point
																				uint8_t *pdest_buf,   //Output data buffer point
																				uint32_t* size,       //Input data size
																				uint8_t klen_word);   //AES key length,For example AES128 or AES256
//AES GCM Mode Cryption (only 128bit)
AES_StatusTypedef HAL_AES_GCM_Crypt(AES_KeyTypedef* pkey,    //AES key(with iv, key=128bit, iv=96bit)
                                    uint8_t mode,            //GCM Encrypt or Decrypt mode
																		uint8_t *psrc_buf,       //Input data buffer point
																		uint8_t *pdest_buf,      //Output data buffer point
																		uint32_t data_size,      //Input data size
                                    uint8_t *aad_buf,        //Additional data buffer point
                                    uint32_t aad_size,       //Additional data size
                                    uint8_t *tag_buf);       //Authentication tag buffer point

//SHA Initial
extern uint8_t SHA_INT_FLAG;
void HAL_SHA_PeripheralsInit(SHA_StatusCBKTypeDef callback);

//SHA Transform
SHA_StatusTypedef HAL_SHA_Transform(uint8_t *input,      //Input data buffer point
																		uint32_t len,        //Input data size
																		uint8_t *output);    //Output data buffer point
SHA_StatusTypedef HAL_SHA_Transform1(uint8_t *input,      //Input data buffer point
																		uint32_t len,        //Input data size
																		uint8_t *output);    //Output data buffer point

//RSA Initial
extern uint8_t RSA_INT_FLAG;
void HAL_RSA_PeripheralsInit(RSA_StatusCBKTypeDef callback);

//RSA Process
RSA_StatusTypedef HAL_RSA_Process(uint32_t * n_buf,       //Modulus data buffer point
																	uint32_t * k_buf,       //RSA key data buffer point
																	uint32_t * psrc_buf,    //Input data buffer point
																	uint32_t * pdest_buf,   //Output data buffer point
																	uint8_t klen_word);     //RSA key length,For example RSA2048 or RSA3072
//RSA Process1
RSA_StatusTypedef HAL_RSA_Process1(uint32_t * n_buf,       //Modulus data buffer point
																		uint32_t * k_buf,      //RSA key data buffer point
																		uint32_t * psrc_buf,   //Input data buffer point
																		uint32_t * pdest_buf,  //Output data buffer point
																		uint8_t klen_word);    //RSA key length,For example RSA2048 or RSA3072

//DMA Initial
void HAL_DMA_PeripheralsInit(DMA_StatusCBKTypeDef callback);

//Return DMA Link RAM Address
uint32_t HAL_DMA_SideBand_Ram_Base(void);

//DMA Mode AES Encrypted
DMA_StatusTypedef HAL_DMA_AES_Encrypted(AES_KeyTypedef* pkey, //AES key(with vi)
																				uint8_t* psrc_buf,    //Input data buffer point
																				uint8_t* pdest_buf,   //Output data buffer point
																				uint32_t* inlen);       //Input/Output data size
//DMA Mode AES Decrypted
DMA_StatusTypedef HAL_DMA_AES_Decrypted(AES_KeyTypedef* pkey, //AES key(with vi)
																				uint8_t* psrc_buf,    //Input data buffer point
																				uint8_t* pdest_buf,   //Output data buffer point
																				uint32_t* inlen);       //Input/Output data size
//DMA Mode AES Cryption
DMA_StatusTypedef HAL_DMA_AES_Crypt(AES_KeyTypedef* pkey,    //AES key(with vi)
																		uint8_t mode,            //AES Mode(ECB or CBC, Encrypt or Decrypt)
																		uint8_t* psrc_buf,       //Input data buffer point
																		uint8_t* pdest_buf,      //AES encrypted data buffer point
																		uint32_t* inlen);        //Input/Output data size

//DMA Mode SHA Transform
DMA_StatusTypedef HAL_DMA_SHA_Transform(uint8_t *input,       //Input data buffer point
																				uint32_t len,         //Input data size
																				uint8_t *output);     //Output data buffer point
//DMA Mode SHA Transform and AES Cryption
DMA_StatusTypedef HAL_DMA_SHA_AES_Crypt(AES_KeyTypedef* pkey,    //AES key(with vi)
																						uint8_t mode,        //AES Mode(ECB or CBC, Encrypt or Decrypt)
																						uint8_t* psrc_buf,   //Input data buffer point
																						uint8_t* pdest_buf,  //AES encrypted data buffer point
																						uint32_t* inlen,     //Input/Output data size
																						uint8_t* sha_out);   //SHA output data buffer point


#endif
