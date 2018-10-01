/*
 * adc.c
 *
 * Created: 11/30/2018 5:10:29 PM
 *  Author: User
 */ 
#include "sam.h"
#include "adc.h"
#include "callbacks.h"
volatile DmacDescriptor2 dmac_des = {0};
volatile DmacDescriptor2 dmac_wb = {0};
volatile int adc_done = 1;
void init_opamp(void);

/*
Setups up the DMAC descriptor, which holds capture size, transfer size, src, dest addr
*/
void init_dmac_desc(void)
{
	//memset(trace_data_src, 0xFF, sizeof(trace_data_src));
	memset(&dmac_des, 0, sizeof(dmac_des));
	dmac_des.BTCTRL.bit.BEATSIZE = 1; //16-bit DMA transfers
	

	/*
	When one adc capture finishes, the destination memory address (where the value
	is put) is supposed to increment by BEATSIZE * STEPSIZE. It actually decrements 
	instead, so we need to set the destination to be the end of the memory we want to write to
	not the beginning. 
	*/
	dmac_des.BTCTRL.bit.STEPSIZE = 0; //next_addr = addr - 2; (in bytes)
	dmac_des.BTCTRL.bit.DSTINC = 1; //increment destination addr
	dmac_des.BTCTRL.bit.VALID = 1; //mark descriptor as valid
	uint32_t dest_addr = trace_data_addr;
	dest_addr += (63*2); 
	
	dmac_des.DSTADDR.reg = dest_addr; //DMA destination is trace_data
	dmac_des.SRCADDR.reg = &(ADC->RESULT.reg); //DMA source is ADC result
	
	dmac_des.BTCNT.reg = 64; //Do 64 captures
}


/*
Initialize ADC and DMAC. Turns everything on, but doesn't start capturing
To start capturing, enable ADC freerun
*/
void init_adc(void)
{
	
	//setup ADC clock
	MCLK->APBCMASK.bit.ADC_ = 1; 
	GCLK->PCHCTRL[16].reg = 1 << 6;
	NVIC->ISER[0] = 0xF800; //enable trasaction complete interrupt
	
	//calibrate ADC
	uint32_t *bias_addr = 0x806020; //location of calibration values in flash
	uint8_t bias_comp = (*bias_addr & (0x07 << 3)) >> 3;
	uint8_t bias_ref = *bias_addr & 0x07;
	
	//PA3 as analog input (if used)
	PORT_SEC->Group[0].PMUX[1].reg = (2 << 4); //ADC mux for PA3
	PORT_SEC->Group[0].PINCFG[3].reg = 0b1; //enable mux
	
	DMAC->CHID.reg = 0; //select DMA channel 0
	DMAC->CHINTENSET.bit.TCMPL = 1; //enable interrupt when transfer completes
	DMAC->CHCTRLB.bit.TRIGSRC = 0x13; //trigger upon ADC result ready
	DMAC->CHCTRLB.bit.TRIGACT = 0x02; //one trigger per beat (16-bits)
	DMAC->CTRL.bit.LVLEN0 = 1; //enable level 0 DMA
	
	DMAC->BASEADDR.reg = &dmac_des; //set dmac_des as DMA descriptor
	DMAC->WRBADDR.reg = &dmac_wb; //set dmac_des as DMA descriptor writeback
	
	init_dmac_desc();
	DMAC->CTRL.bit.DMAENABLE = 1; //enable DMA
	
	//ADC->REFCTRL.bit.REFSEL = 0x02; //ref is VDDANA/2
	
	ADC->INPUTCTRL.bit.MUXNEG = 0x18; //neg input is internal ground
	//ADC->INPUTCTRL.bit.MUXPOS = 0x01; //pos input is AIN1 --> OP-AMP INPUT
	//ADC->INPUTCTRL.bit.MUXPOS = 0x01; //pos input is AIN3 (UNCONNECTED INPUT NEED TO CONFIG PIN)
	ADC->INPUTCTRL.bit.MUXPOS = 0x1A; //pos input is VDDCORE/4
	//ADC->INPUTCTRL.bit.MUXPOS = 0x1F; //pos input is OPAMP2
	
	ADC->CALIB.bit.BIASCOMP = bias_comp;
	ADC->CALIB.bit.BIASREFBUF = bias_ref;
	ADC->CTRLA.bit.ENABLE = 1; //finally enable adc
	
	//ADC->CTRLC.bit.FREERUN = 1; //enable freerunning mode
	
}

//change to inst amp
void init_opamp(void)
{
	//opamp 2 inputs?
	//Actually, maybe want int ref, since that won't be on vddana supply (vddio instead w/ int reg)
	
	SUPC->VREF.bit.ONDEMAND = 0; //ref always on
	SUPC->VREF.bit.SEL = 0x01; //1.1V reference (3.3V/3)
	
	
	
	
	//clock should already be setup
	//                  R2       |      R1
	//setup:   VDDANA /\/\/ OPAMP NEG /\/\/ GND
	OPAMP->OPAMPCTRL[2].bit.RES1MUX = 0x03; //R1 con to GND
	OPAMP->OPAMPCTRL[2].bit.RES2VCC = 1; //R2 con to VDDANA
	OPAMP->OPAMPCTRL[2].bit.RES1EN = 1; //enable R1
	OPAMP->OPAMPCTRL[2].bit.POTMUX = 0x01; //R2 = 4, R1 = 12 (1/3 volt div)
	
	OPAMP->OPAMPCTRL[2].bit.MUXNEG = 2; //Conn to VREF (int reference)
	OPAMP->OPAMPCTRL[2].bit.MUXPOS = 1; //Conn between R1 and R2
	
	OPAMP->OPAMPCTRL[2].bit.ANAOUT = 1; //enable analog out (for ADC)
	OPAMP->OPAMPCTRL[2].bit.BIAS = 0x03; //max current, fastest speed
	OPAMP->OPAMPCTRL[2].bit.ENABLE = 1; //enable opamp 2
	OPAMP->CTRLA.bit.ENABLE = 1;
}

void init_inst_amp(void) //x7 gain
{		
	OPAMP->OPAMPCTRL[0].bit.MUXPOS = 0x02; //POS is Vddref
	OPAMP->OPAMPCTRL[0].bit.MUXNEG = 0x03;
	OPAMP->OPAMPCTRL[0].bit.RES1MUX = 0x02;
	OPAMP->OPAMPCTRL[0].bit.POTMUX = 0x00;
	OPAMP->OPAMPCTRL[0].bit.RES2OUT = 1;
	OPAMP->OPAMPCTRL[0].bit.RES1EN = 1;
	
	OPAMP->OPAMPCTRL[1].bit.MUXPOS = 1; //OA1 Tap
	OPAMP->OPAMPCTRL[1].bit.MUXNEG = 0x03;
	OPAMP->OPAMPCTRL[1].bit.RES1MUX = 0x03; //GND
	OPAMP->OPAMPCTRL[1].bit.RES2VCC = 1;
	OPAMP->OPAMPCTRL[1].bit.POTMUX = 0x04; // 1/2 voltage divider -> 1.65
	OPAMP->OPAMPCTRL[1].bit.RES1EN = 1;
	
	OPAMP->OPAMPCTRL[2].bit.MUXPOS = 0b111;
	OPAMP->OPAMPCTRL[2].bit.MUXNEG = 0b1;
	OPAMP->OPAMPCTRL[2].bit.RES1MUX = 0b10;
	OPAMP->OPAMPCTRL[2].bit.POTMUX = 0x07;
	OPAMP->OPAMPCTRL[2].bit.RES2OUT = 1;
	OPAMP->OPAMPCTRL[2].bit.RES1EN = 1;
	OPAMP->RESCTRL.bit.REFBUFLEVEL = 0x02; //refbuf is 1.6V
	
	OPAMP->OPAMPCTRL[0].bit.ANAOUT = 0;
	OPAMP->OPAMPCTRL[1].bit.ANAOUT = 0;
	OPAMP->OPAMPCTRL[2].bit.ANAOUT = 1;
	
	OPAMP->OPAMPCTRL[0].bit.ONDEMAND = 0;
	OPAMP->OPAMPCTRL[1].bit.ONDEMAND = 0;
	OPAMP->OPAMPCTRL[2].bit.ONDEMAND = 0;
	
	OPAMP->OPAMPCTRL[0].bit.BIAS = 0x03; //max current, fastest speed
	OPAMP->OPAMPCTRL[1].bit.BIAS = 0x03; //max current, fastest speed
	OPAMP->OPAMPCTRL[2].bit.BIAS = 0x03; //max current, fastest speed
	
	OPAMP->OPAMPCTRL[0].bit.ENABLE = 1; //enable opamp 0
	OPAMP->OPAMPCTRL[1].bit.ENABLE = 1; //enable opamp 1
	OPAMP->OPAMPCTRL[2].bit.ENABLE = 1; //enable opamp 2
	
	OPAMP->CTRLA.bit.ENABLE = 1;
}