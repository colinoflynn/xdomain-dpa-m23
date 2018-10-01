/*
 * GccApplication1.c
 *
 * Created: 9/14/2018 11:51:06 AM
 * Author : Alex Dewar
 */ 

#include <string.h>
#include "sam.h"
#include "SEGGER_RTT.h"
#include "adc.h"
#include "simpleserial.h"
#include "callbacks.h"

//#define EXT_CLK

void trigger_setup(void)
{
	PORT_SEC->Group[0].DIRSET.reg = 1 << 22;
	PORT_SEC->Group[0].OUTCLR.reg = 1 << 22;
}

void trigger_low(void)
{
	PORT_SEC->Group[0].OUTCLR.reg = 1 << 22;
}

void trigger_high(void)
{
	PORT_SEC->Group[0].OUTSET.reg = 1 << 22;
}

#ifdef EXT_CLK
//use external clock
void platform_init(void)
{
	SystemInit();

	//enable ext clock and clock failure detection
	OSCCTRL->XOSCCTRL.reg = (1 << 1) | (1 << 3);

	//wait for clock to be ready and check for failure
	while (!OSCCTRL->STATUS.bit.XOSCRDY);
	while (OSCCTRL->STATUS.bit.XOSCFAIL);

	//MCLK->CPUDIV.reg = 0x02; //divide mclk by 2
	GCLK->GENCTRL[0].bit.SRC = 0; //switch to xosc

	PORT_SEC->Group[0].DIRSET.reg = 1 << 23;
	PORT_SEC->Group[0].OUTSET.reg = 1 << 23;

}
#else
//16MHz internal clock
void platform_init(void)
{
	SystemInit();

	//enable ext clock and clock failure detection
	OSCCTRL->OSC16MCTRL.bit.FSEL = 0b11; //16MHz internal clock
	//GCLK->GENCTRL[0].bit.DIV = 4; //16 divide

	PORT_SEC->Group[0].DIRSET.reg = 1 << 23;
	PORT_SEC->Group[0].OUTSET.reg = 1 << 23;

}
#endif


void DMAC_0_Handler(void)
{
	PORT_SEC->Group->OUTSET.reg = 1 << 7;
	ADC->CTRLC.bit.FREERUN = 0;//disable freerun
	DMAC->CHINTFLAG.bit.TCMPL = 1; //clear transfer complete flag
	adc_done = 1;
}

void init_uart(void)
{
	//PORT_SEC->Group[0].DIRSET.reg = 1 << 8;
	//multiplexing needs to be enabled before setting where stuff is mux'd to
	PORT_SEC->Group[0].PINCFG[16].reg = 0b11; //enable input read, mux
	PORT_SEC->Group[0].PINCFG[17].reg = 0b11; //enable input read, mux
	PORT_SEC->Group[0].PMUX[8].reg = 3 | (3 << 4); //SERCOM_alt mux for PORT

	//DON"T FORGET TO ENABLE PERIPH CLOCK
	MCLK->APBCMASK.bit.SERCOM1_ = 1; //on by default?
	GCLK->PCHCTRL[11].reg = (1 << 6) | 0; //enable generic clock, use src 0

	//LSB first, RX on pad[1] (PA9), Tx on pad[0], internal clock
	SERCOM0->USART.CTRLA.reg = (1 << 30) | (1 << 20) | (0 << 16) | (1 << 2);
	//enable TX and RX

	//SERCOM0->USART.BAUD.reg = 60073; //38400 baud
	SERCOM0->USART.BAUD.reg = 32756; //230400 baud

	//DEFAULT FOR ABOVE IS 8n1
	SERCOM0->USART.CTRLA.bit.ENABLE = 1; //enable
	SERCOM0->USART.CTRLB.reg = (1 << 17) | (1 << 16);
	while(SERCOM0->USART.SYNCBUSY.bit.CTRLB); //wait for tx and rx to be enabled
	while(SERCOM0->USART.SYNCBUSY.bit.ENABLE); //wait for enable to complete
	while(!(SERCOM0->USART.CTRLB.reg & (0b11 << 16))); //check for tx and rx enable
}

//data register empty
void putch(char c)
{
	//wait for data register to be empty
	//while(!SERCOM0->USART.INTFLAG.bit.DRE);
	//SERCOM0->USART.DATA.reg = c;
	SEGGER_RTT_Write(0, &c, 1);
}

char getch(void)
{
	//wait for data
	//while(!SERCOM0->USART.INTFLAG.bit.RXC);
	//return SERCOM0->USART.DATA.reg;
	return SEGGER_RTT_WaitKey();
}

#define TEST_SWITCHING_MODE 1

uint8_t mem[4];
int main(void)
{
    /* Initialize the SAM system */
    //SystemInit();
	SEGGER_RTT_Init();
	platform_init();
	
	/* Setup Port Security Attribution of I/O Pins */
	PORT_SEC->Group[0].NONSEC.reg = 0;;
	PORT_SEC->Group->DIRSET.reg = 1 << 7;
	PORT_SEC->Group->WRCONFIG.reg = (1 << 7) | (1 << 30);
	PORT_SEC->Group->OUTSET.reg = 1 << 7;
	//init_uart();
	
	trigger_setup();
    /* Replace with your application code */
	
#if TEST_SWITCHING_MODE
	//Set buck mode 
	SUPC->VREG.bit.SEL = 1;
	
	//Start the switch
	//SUPC->STATUS.bit.VREGRDY = 0;
	
	//Wait for switch done
	while(SUPC->STATUS.bit.VREGRDY == 0);
	while(SUPC->STATUS.bit.VCORERDY == 0);
#endif
	
	init_adc();
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
	simpleserial_addcmd('b', 0, send_adc_block);
	simpleserial_addcmd('p', 16,  get_pt);
	simpleserial_addcmd('k', 16,  get_key);
	simpleserial_addcmd('m', 4,  do_multi_encryption);
	simpleserial_addcmd('n', 4,  do_multi_tvla);
	simpleserial_addcmd('t', 0,  get_multi_trace);
	simpleserial_addcmd('c', 0,  get_multi_ct);
	//do_multi_encryption(mem);
    while (1) 
    {
		simpleserial_get();
    }
}
