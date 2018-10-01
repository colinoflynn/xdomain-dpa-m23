/*
 * callbacks.h
 *
 * Created: 11/30/2018 5:22:56 PM
 *  Author: User
 */ 


#ifndef CALLBACKS_H_
#define CALLBACKS_H_

extern  uint16_t *trace_data_addr;
uint8_t do_multi_encryption(uint8_t *seed);
uint8_t do_multi_tvla(uint8_t * seed);
uint8_t get_multi_trace(uint8_t *n);
uint8_t get_multi_ct(uint8_t *n);
uint8_t send_adc(uint8_t *pt);
uint8_t send_adc_block(uint8_t *pt);
uint8_t get_pt(uint8_t *pt);
uint8_t get_key(uint8_t *k);


#endif /* CALLBACKS_H_ */