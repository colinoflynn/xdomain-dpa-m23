/*
 * adc.h
 *
 * Created: 11/30/2018 5:13:22 PM
 *  Author: User
 */ 


#ifndef ADC_H_
#define ADC_H_

void init_adc(void);
typedef struct {  /* Direct Memory Access Controller */
	__IO DMAC_BTCTRL_Type               BTCTRL;         /**< Offset: 0x00 (R/W  16) Block Transfer Control */
	__IO DMAC_BTCNT_Type                BTCNT;          /**< Offset: 0x02 (R/W  16) Block Transfer Count */
	__IO DMAC_SRCADDR_Type              SRCADDR;        /**< Offset: 0x04 (R/W  32) Block Transfer Source Address */
	__IO DMAC_DSTADDR_Type              DSTADDR;        /**< Offset: 0x08 (R/W  32) Block Transfer Destination Address */
	__IO DMAC_DESCADDR_Type             DESCADDR;       /**< Offset: 0x0C (R/W  32) Next Descriptor Address */
} DmacDescriptor2
#ifdef __GNUC__
__attribute__ ((aligned (16)))
#endif

;

extern volatile DmacDescriptor2 dmac_des;
extern volatile DmacDescriptor2 dmac_wb;
extern volatile int adc_done;

#endif /* ADC_H_ */