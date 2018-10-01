/*
 * callbacks.c
 *
 * Created: 11/30/2018 5:23:27 PM
 *  Author: User
 */ 
#include "sam.h"
#include "SEGGER_RTT.h"
#include "adc.h"
#include "simpleserial.h"

//NORMAL = DELAY_16_CYCLE + DELAY_2_CYCLE + DELAY_1_CYCLE

#define DELAY_16_CYCLE
//#define DELAY_8_CYCLE
//#define DELAY_4_CYCLE
#define DELAY_2_CYCLE
#define DELAY_1_CYCLE

#define ENC_PER_CALL 32
static volatile uint16_t trace_data[64*(ENC_PER_CALL)]; //DMA destination
uint16_t *trace_data_addr = trace_data;
volatile uint8_t ct_data[16*ENC_PER_CALL];

static uint32_t state[4] = {0xDEADBEEF, 0xAA669911, 0xBE11EA27FF, 0xFF005522};
uint32_t xorshift32(void)
{
	uint32_t x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 15;
	*state = x;
	return x;
}

typedef void (*IDAU_AES_FUNC)(const uint8_t *keys, uint32_t key_len,
const uint8_t *src, uint8_t *dst);
IDAU_AES_FUNC idau_aes_enc = (IDAU_AES_FUNC)(IDAU_CRYA_AES_ENCRYPT_T | 0x01);
IDAU_AES_FUNC idau_aes_dec = (IDAU_AES_FUNC)(IDAU_CRYA_AES_DECRYPT_T | 0x01);
uint8_t key[24] = {0};
	
// do 32 encryptions and captures
uint8_t do_multi_encryption(uint8_t *seed)
{
	asm volatile ("isb"); //clear instruction pipeline
	*state = seed[0] | (seed[1] << 8) | (seed[2] << 16) | (seed[3] << 24);
	for (int i = 0; i < ENC_PER_CALL; i++) {
		
		uint32_t my_pt[4];
		while (!adc_done);
		adc_done = 0;
		//Update DMA destination (must be done before reenabling the channel)
		dmac_des.DSTADDR.reg = (uint32_t)(trace_data + i * 64 + 63);
		
		DMAC->CHCTRLA.bit.ENABLE = 1; //reenable channel
		
		//generate plaintext
		for (int j = 0; j < 4; j++) {
			my_pt[j] = xorshift32();
		}
		asm volatile ("isb"); //clear instruction pipeline
		
		//start recording ADC
		ADC->CTRLC.bit.FREERUN = 1;
		
		//Delay before encryption
		#ifdef DELAY_1_CYCLE
		asm volatile ("mov r1, r1");
		#endif
		
		#ifdef DELAY_2_CYCLE
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		#endif
		
		#ifdef DELAY_4_CYCLE
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		#endif
		
		#ifdef DELAY_8_CYCLE
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		#endif
		
		#ifdef DELAY_16_CYCLE
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		#endif
		
		idau_aes_enc(key, 4, (uint8_t *)my_pt, ct_data + 16 * i);
		while (!adc_done);
	}
	return 0x00;
}

uint8_t tvla_fixed_pt[16] = {0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90};

// do 32 encryptions and captures
uint8_t do_multi_tvla(uint8_t *seed)
{
	static uint8_t last_rndct_out[16];
	int is_rand;
	
	asm volatile ("isb"); //clear instruction pipeline
	*state = seed[0] | (seed[1] << 8) | (seed[2] << 16) | (seed[3] << 24);
	
	if(*state == 0){
		memset(last_rndct_out, 0, 16);
	}
	
	for (int i = 0; i < ENC_PER_CALL; i++) {
		
		uint32_t my_pt[4];
		while (!adc_done);
		adc_done = 0;
		//Update DMA destination (must be done before reenabling the channel)
		dmac_des.DSTADDR.reg = (uint32_t)(trace_data + i * 64 + 63);
		
		DMAC->CHCTRLA.bit.ENABLE = 1; //reenable channel
		
		//Is this random or fixed?
		if(xorshift32() & 0x01){
			//Random
			is_rand = 1;
			memcpy(my_pt, last_rndct_out, 16);
		} else {
			//fixed
			is_rand = 0;
			memcpy(my_pt, tvla_fixed_pt, 16);			
		}
		
		asm volatile ("isb"); //clear instruction pipeline
		
		//start recording ADC
		ADC->CTRLC.bit.FREERUN = 1;
		
		//Delay before encryption
		#ifdef DELAY_1_CYCLE
		asm volatile ("mov r1, r1");
		#endif
		
		#ifdef DELAY_2_CYCLE
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		#endif
		
		#ifdef DELAY_4_CYCLE
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		#endif
		
		#ifdef DELAY_8_CYCLE
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		#endif
		
		#ifdef DELAY_16_CYCLE
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		asm volatile ("mov r1, r1");
		#endif
		
		idau_aes_enc(key, 4, (uint8_t *)my_pt, ct_data + 16 * i);
		if(is_rand){
			memcpy(last_rndct_out, ct_data + 16 * i, 16);
		}
		while (!adc_done);
	}
	return 0x00;
}

uint8_t get_multi_trace(uint8_t *n)
{
	while (!adc_done);
	//simpleserial_put('r', 64*2*(ENC_PER_CALL - 1), trace_data);
	SEGGER_RTT_Write(0, trace_data, 64*2*(ENC_PER_CALL));
	return 0x00;
}

uint8_t get_multi_ct(uint8_t *n)
{
	while (!adc_done);
	//simpleserial_put('r', 16*ENC_PER_CALL, ct_data);
	SEGGER_RTT_Write(0, ct_data, 16*(ENC_PER_CALL));
	return 0x00;
}


uint8_t send_adc(uint8_t *pt)
{
	uint16_t res = ADC->RESULT.reg;
	
	simpleserial_put('r', 2, &res);
	return 0x00;
}

uint8_t send_adc_block(uint8_t *pt)
{
	while (!adc_done);
	simpleserial_put('r', 64*2, trace_data)	;
	return 0x00;
}



uint8_t get_pt(uint8_t *pt)
{
	while (!adc_done);
	adc_done = 0;
	trigger_high();
	ADC->CTRLC.bit.FREERUN = 1;
	ADC->SWTRIG.bit.FLUSH = 1; //flush adc conversions
	asm volatile ("mov r1, r1");
	asm volatile ("mov r1, r1");
	idau_aes_enc(key, 4, pt, pt);
	trigger_low();
	simpleserial_put('r', 16, pt);
	return 0x00;
}

uint8_t get_key(uint8_t *k)
{
	for (int i = 0; i < 16; i++) {
		key[i] = k[i];
	}
	return 0x00;
}
