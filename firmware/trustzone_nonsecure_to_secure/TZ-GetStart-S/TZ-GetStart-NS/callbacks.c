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
		
		nsc_func_enc(key, 4, (uint8_t *)my_pt, ct_data + 16 * i);
		while (!adc_done);
	}
	return 0x00;
}

uint8_t get_multi_trace(uint8_t *n)
{
	while (!adc_done);
	SEGGER_RTT_Write(0, trace_data, 64*2*(ENC_PER_CALL));
	return 0x00;
}

uint8_t get_multi_ct(uint8_t *n)
{
	while (!adc_done);
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
	ADC->CTRLC.bit.FREERUN = 1;
	ADC->SWTRIG.bit.FLUSH = 1; //flush adc conversions
	asm volatile ("mov r1, r1");
	asm volatile ("mov r1, r1");
	nsc_func_enc(key, 4, pt, pt);
	simpleserial_put('r', 16, pt);
	return 0x00;
}

uint8_t get_key(uint8_t *k)
{
	for (int i = 0; i < 16; i++) {
		key[i] = k[i];
	}
}
