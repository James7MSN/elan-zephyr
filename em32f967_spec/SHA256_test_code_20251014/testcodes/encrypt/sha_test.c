//-----------------------------------------------------------------------------
//TEST
//-----------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "encrypt_reg.h"
#include "main.h"
#include "mbedtls/sha256.h"
//=============================================================================
//test

const	char *test[] = {
		"",
		"abc",
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		"The quick brown fox jumps over the lazy dog",
		"The quick brown fox jumps over the lazy cog", /* avalanche effect test */
		"bhn5bjmoniertqea40wro2upyflkydsibsk8ylkmgbvwi420t44cq034eou1szc1k0mk46oeb7ktzmlxqkbte2sy",
		"f35c950577c9ca29fdcc3572a45f401456dc4db1c54eb0a92b97434f38fa61bade8daa7bb06f85c11e21f68220667d5e9bd6a1db08259dba5a2551cd63ed99e25724f917b68a96ae43826606c912ed8892dc71fa8f3be66f5510f131054ee277c00eb614fc86bcde18e6456dbc069367ed9cf77a5fc66f85dda66505aefa8958d58f7d9d29e178f7a41bc7079ea538d61c2f6d21bfe0d9924af586f9612fba6a5e24229e59b52926d62a7447e691aaae2c15996f19db8d5f0ecd776bff1daabaeabf79b62861c195037c6f5edb449888d6ad95d469081b00487759d974e37ae357c23fd0c1721e3f014a64341119fcd875c72fcff9001e6c4094",
		"7bf5714d72647ded183db09fa678165bd703455bd33d0da4417765256cf4b95af3c543dc1c0fd9ba8270127d0ba838dd0df0217b844bc93e5775953662d0be0e2c9261ebe432877c1ba2ee465b70909af2a4194d55c824e9445e23a9a9eec5c06bece03bc620bc3eaf7f796586582e9b5d944eca5778a81201bb9f7e78342c6cbc265d1e457b091a82c138e75ce57319826fcd03c02538df52256f2f58a3492f821066ca3dd52aec071e5b826f3f4e5974739894698992fb7b741130c7ac3c7661bf04e84f33ca3b299e36efb5f0b3138a9ba0116e2df21b2daaae7e2a666ed1dfc3da718ac5c5213dbd7535406b457278fd5bb4adb71df0c869886b20e41237f63bdb8e3861f50ee81e49316bb47cf65eb9fc3e2cdb82a296b3521a5f58d353b238b23c77709b4dc8de7fcf874ba878212c0761e6a4c66eedbfd075d5055ba0617a74401e117ab54f26436e5c6b35b1368f958c4d0171cf66a5fd8ce7f150c509f2b684ffdb539b4b97f8ce5720b91916e692dc2123fa8eff1af46683b68bf1efe3c2fd8a74e2b015f68977e7fa96701491dc9b69aa7862c296834b7c254a7639152d37c1d7f13b84ff7260d4b1b8cd9d3b368bc7476102997347f5cbbdf4153e3eeb37239ef4d020131e35e77136f9a1fa1e34b67dac490b26cb60dbc0253af5af844384fc36f1e2ef10d716b05bb6592473f5a752fece61db80dd6a89c05030626512fa62ab" 
	};

const	uint8_t test_hash[8][32]={
		{0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,
		 0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55
		},
		{0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
		 0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad
		},
		{0x24,0x8d,0x6a,0x61,0xd2,0x06,0x38,0xb8,0xe5,0xc0,0x26,0x93,0x0c,0x3e,0x60,0x39,
		 0xa3,0x3c,0xe4,0x59,0x64,0xff,0x21,0x67,0xf6,0xec,0xed,0xd4,0x19,0xdb,0x06,0xc1
		},
		{0xd7,0xa8,0xfb,0xb3,0x07,0xd7,0x80,0x94,0x69,0xca,0x9a,0xbc,0xb0,0x08,0x2e,0x4f,
		 0x8d,0x56,0x51,0xe4,0x6d,0x3c,0xdb,0x76,0x2d,0x02,0xd0,0xbf,0x37,0xc9,0xe5,0x92
		},
		{0xe4,0xc4,0xd8,0xf3,0xbf,0x76,0xb6,0x92,0xde,0x79,0x1a,0x17,0x3e,0x05,0x32,0x11,
		 0x50,0xf7,0xa3,0x45,0xb4,0x64,0x84,0xfe,0x42,0x7f,0x6a,0xcc,0x7e,0xcc,0x81,0xbe
		},
		{0x90,0x85,0xdf,0x2f,0x02,0xe0,0xcc,0x45,0x59,0x28,0xd0,0xf5,0x1b,0x27,0xb4,0xbf,
		 0x1d,0x9c,0xd2,0x60,0xa6,0x6e,0xd1,0xfd,0xa1,0x1b,0x0a,0x3f,0xf5,0x75,0x6d,0x99
		},
		{0xA8,0x49,0x0A,0xEC,0x39,0xD1,0x06,0xDB,0x2A,0x41,0x7A,0x9F,0xD4,0xE9,0x90,0xDC,
		 0xF9,0x3C,0x02,0x51,0x6C,0xC3,0xEB,0x19,0xC9,0xDD,0xCA,0x5D,0xBF,0xDA,0x01,0x55
		},
		{0x9D,0x92,0x15,0x00,0xD3,0xD2,0x81,0xA0,0x86,0x17,0x8E,0x48,0x2F,0x0D,0x83,0x37,
		 0x77,0x8D,0x16,0xA8,0x1F,0x64,0x7F,0x47,0xDC,0x01,0x94,0xFD,0xA3,0xAE,0x35,0x87
		},
	};

extern uint8_t SRAM[32*1024] __attribute__((at(0x2000C000)));
	
#define TEST_BLK_SIZE	(16*256)
#define SHA256_BYTES    32
#define TEST_BLK_GAP  32

//-----------------------------------------------------------------------------
int do_run_sha_test1(void)
{
	uint32_t* hash = (uint32_t*)&SRAM[0*TEST_BLK_SIZE];
	size_t i;

	for (i = 1; i < (sizeof(test) / sizeof(test[0])); i += 1) {
		//mbedtls_sha256_ret((uint8_t*)test[i], strlen(test[i]), (uint8_t*)hash, 0);
		HAL_SHA_Transform((uint8_t*)test[i], strlen(test[i]), (uint8_t*)hash);
		if(memcmp((uint8_t*)hash,test_hash[i],SHA256_BYTES))
		{
			return -1;
		}	
	}
	return 0;
}

//-----------------------------------------------------------------------------
int do_run_sha_test2(void)
{
	uint32_t size;
	uint8_t gap;
	uint32_t* orbuf = (uint32_t*)&SRAM[0*TEST_BLK_SIZE];
	uint32_t* hash1 = (uint32_t*)&SRAM[1*TEST_BLK_SIZE];
	uint32_t* hash2 = (uint32_t*)&SRAM[2*TEST_BLK_SIZE];
	
	getRandPattern(&gap, 1);
	size = TEST_BLK_SIZE - TEST_BLK_GAP - gap;
	getRandPattern((uint8_t*)orbuf, size);
	
	mbedtls_sha256_ret((uint8_t*)orbuf, size, (uint8_t*)hash1, 0);
	HAL_SHA_Transform((uint8_t*)orbuf, size, (uint8_t*)hash2);
	
	if (memcmp(hash1, hash2, SHA256_BYTES))
	{
		return -1;
	}
	
	return 0;
	
}



//-----------------------------------------------------------------------------
volatile SHA_StatusTypedef shaErrStatus;
void testShaErr(SHA_StatusTypedef state)
{
	shaErrStatus = state;
	while(1);
}
int test_sha(void)
{
	uint32_t test_cnt;
	uint32_t i,temp;
	bool m;
	
	srand(0X23456);
	
	HAL_SHA_PeripheralsInit(testShaErr);

	printf("***** SHA-256 Pattern Test *****\n\r");
	if (do_run_sha_test1()){
		printf("Test Fail!\n\r");
		return -1;
	}
	printf("Test Pass!\n\r");

	printf("\n");
	printf("***** SHA-256 Software Encrypt Comparison Test *****\n\r");		
	for (test_cnt=0; test_cnt<1000; test_cnt++){
		printf("Test Progress : %d/1000 ",test_cnt + 1);
		if (do_run_sha_test2()){
			printf("\nTest Fail!\n\r");	
			return -1;
		}
		printf("\r");
	}
	printf("\nTest Pass!\n\r");		

	printf("\n");
	printf("***** SHA-256 IP Reset Test *****\n\r");		
	IPRESET(IPReset_ENCRYPT);
	m = true;
	
	temp=0;
	for(i=0;i<8;i++)
		temp = temp + SHA_OUT_POINT[i];
	if(temp != 0){
		printf("OUTPUT Registar : Fail\n\r");
		m = false;		
	}
	else
		printf("OUTPUT Registar : Pass\n\r");		

	if(SHA_IN != 0){
		printf("INPUT Registar : Fail\n\r");	
		m = false;	
//		return -1;
	}	
	else
		printf("INPUT Registar : Pass\n\r");	
	
	if(SHA_CTR->SHACTR.SHACTRBYTE != 0x08){
		printf("Control Registar : Fail\n\r");
		m = false;	
//		return -1;
	}	
	else	
		printf("Control Registar : Pass\n\r");
	
	if(SHA_DATALEN_HIGH != 0){
		printf("DATALEN High Registar : Fail\n\r");
		m = false;	
//		return -1;
	}
	else
		printf("DATALEN High Registar : Pass\n\r");	

	if(SHA_DATALEN_LOW != 0){
		printf("DATALEN Low Registar : Fail\n\r");
		m = false;	
//		return -1;
	}			
	else
		printf("DATALEN Low Registar : Pass\n\r");
	
	if(SHA_PAD_CTR->SHAPADCTR.SHAPADCTRBYTE != 0){
		printf("PAD Control Registar : Fail\n\r");
		m = false;	
//		return -1;
	}	
	else
		printf("PAD Control Registar : Pass\n\r");
	
	if (m){
		printf("\n");
		printf("<<< SHA-256 Test All Pass! >>>\n\r");
	}
	else
		printf("SHA-256 IP Reset Test Fail!\n\r");
	
	printf("\n");
	return 0;
	
}
