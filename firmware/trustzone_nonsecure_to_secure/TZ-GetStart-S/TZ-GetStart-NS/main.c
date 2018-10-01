/**
 * \file
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include "atmel_start.h"
#include "trustzone_veneer.h"
#include "SEGGER_RTT.h"
#include "adc.h"
#include "simpleserial.h"
#include "callbacks.h"


void DMAC_0_Handler(void)
{
	PORT_SEC->Group->OUTSET.reg = 1 << 7;
	ADC->CTRLC.bit.FREERUN = 0;//disable freerun
	DMAC->CHINTFLAG.bit.TCMPL = 1; //clear transfer complete flag
	adc_done = 1;
}

//data register empty
void putch(char c)
{
	SEGGER_RTT_Write(0, &c, 1);
}

char getch(void)
{
	return SEGGER_RTT_WaitKey();
}

/* Non-secure main() */
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	SEGGER_RTT_Init();
	
	init_adc();

	simpleserial_addcmd('b', 0, send_adc_block);
	simpleserial_addcmd('p', 16,  get_pt);
	simpleserial_addcmd('k', 16,  get_key);
	simpleserial_addcmd('m', 4,  do_multi_encryption);
	simpleserial_addcmd('t', 0,  get_multi_trace);
	simpleserial_addcmd('c', 0,  get_multi_ct);
	//do_multi_encryption(mem);
	while (1)
	{
		simpleserial_get();
	}
}
