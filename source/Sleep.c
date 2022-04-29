/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    Sleep.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "color_rgb.h"
#include "adc_irq.h"
#include "LPTMR.h"
#include "uart.h"

/* TODO: insert other include files here. */

#define MASK(n) (1UL << n)
#define ADCBUFFER	100
#define ERROR -1
#define DEBUG0 20
#define DEBUG1 21
#define DEBUG2 22
#define DEBUG3 23

#define ADCSCALE 3.3/65535

unsigned short adc_read[ADCBUFFER];
unsigned short adc_idx;
int work;

void deepsleep(void);
void initdebug(void);
void initsleep(void);

int wakeup(void)
{
	if (!((((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2)
			&& (!(MCG->S & MCG_S_IREFST_MASK)) && (MCG->S & MCG_S_PLLST_MASK)
			&& (!(MCG->C2 & MCG_C2_LP_MASK))))
	{
		return ERROR;
	}

	//(2) Check the PLL state before transitioning to PEE mode, if not then error.
	for (int i = 0; i < 2000; i++)
	{
		if (MCG->S & MCG_S_LOCK0_MASK)
			break;
	}
	if (!(MCG->S & MCG_S_LOCK0_MASK))
		return ERROR;

	//(3) Clear CLKS to switch CLKS mux to select PLL as MCG_OUT.
	MCG->C1 &= ~MCG_C1_CLKS_MASK;

	//(4) Wait for clock status bits to update, if not then error.
	for (int i = 0; i < 2000; i++)
	{
		if (((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x3)
			break;
	}
	if (((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3)
		return ERROR;

	return 0;
}

void LLWU_IRQHandler()
{

	if (LLWU->F2 & LLWU_F2_WUF14_MASK)
	{
		// Clear Wake-up flag for pin
		LLWU->F2 |= LLWU_F2_WUF14_MASK;
		PMC->REGSC |= PMC_REGSC_ACKISO_MASK;
		// Disable sleep on exit
		SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
		work = 1;
		//wakeup();
	}
	else
		if (LLWU->F3 & LLWU_F3_MWUF0_MASK)
		{
			// write 1 to TCF to clear the LPT timer compare flag
			LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
			wakeup();
			//PTB->PTOR |= MASK(0);
			PMC->REGSC |= PMC_REGSC_ACKISO_MASK;
			//SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
		}
	PTE->PTOR |= MASK(DEBUG0);
}

void ADC0_IRQHandler()
{
	if (!work)
	{
		color_rgb(RED);
		SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
		//deepsleep();
	}
	else
	{
		SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
	}
	PTE->PTOR = MASK(DEBUG2);
	if (ADC0->SC1[0] & ADC_SC1_COCO_MASK)
	{
		adc_read[adc_idx] = (unsigned short) ADC0->R[0];
	}
	adc_idx++;
	if (adc_idx > ADCBUFFER - 1)
	{
		adc_idx = 0;
	}
}

/*
 * @brief   Application entry point.
 */
int main(void)
{
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif

	uint32_t masking_state;

	init_LED();

	color_rgb(BLACK);
	// Setup input pin
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
	/*
	 * Enable interrupt, MUX 1, Pull enable, pullup enable
	 */
	PORTD->PCR[4] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(1) |
	PORT_PCR_PE_MASK |
	PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x0A);

	initdebug();
	uart0_init();
	initLPTMR();
	initsleep();
	adcInit(0, 0, 0, 0);

	color_rgb(BLUE);
	//__WFI();

	while (1)
	{
		color_rgb(YELLOW);

		if (work)
		{
			PTE->PTOR |= MASK(DEBUG3);
			color_rgb(BLUE);
			int read_idx = adc_idx;
			// Save masking state
			masking_state = __get_PRIMASK();
			// Disable interrupts
			__disable_irq();
			// Perform atomic work
			for (int i = 0; i < ADCBUFFER; i++)
			{
				transmit_double(adc_read[read_idx] * ADCSCALE);
				read_idx++;
				if (read_idx > ADCBUFFER - 1)
				{
					read_idx = 0;
				}
			}
			// Restore masking state
			__set_PRIMASK(masking_state);
			__enable_irq();
			work = 0;

		}

		//color_rgb(BLACK);

		work = 0;
		//deepsleep();

	}
	return 0;
}
void initsleep(void)
{
	unsigned int dummyread;
	// External wakeup pin enabled
	LLWU->PE4 = LLWU_PE4_WUPE14(2);
	LLWU->ME = LLWU_ME_WUME0_MASK;

	// Set Interrupt priority
	NVIC_SetPriority(LLWU_IRQn, 0);
	// Clear any pending interrupts
	NVIC_ClearPendingIRQ(LLWU_IRQn);
	// Enable LPTMR0 interrupts
	NVIC_EnableIRQ(LLWU_IRQn);

	// Disable clock monitor (Apparently it restarts MCU if not)
	MCG->C6 &= ~MCG_C6_CME0_MASK;

	/* Allow LLS
	 * Allow VLPM
	 */
	SMC->PMPROT = SMC_PMPROT_ALLS_MASK;			// | SMC_PMPROT_AVLP_MASK;
	/* Clear and set
	 * Stop mode control to Low-Leakage-stop
	 */
	SMC->PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
	SMC->PMCTRL = SMC_PMCTRL_STOPM(3);
	/*wait for write to complete to SMC before stopping core */
	dummyread = SMC->PMCTRL;
	dummyread++;

	/* Set the SLEEPDEEP bit to enable deep sleep mode */
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;			// | SCB_SCR_SLEEPONEXIT_Msk;
}
void initdebug(void)
{
	PORTE->PCR[DEBUG0] = PORT_PCR_MUX(1);
	PORTE->PCR[DEBUG1] = PORT_PCR_MUX(1);
	PORTE->PCR[DEBUG2] = PORT_PCR_MUX(1);
	PORTE->PCR[DEBUG3] = PORT_PCR_MUX(1);
	PTE->PDDR |= MASK(DEBUG0) | MASK(DEBUG1) | MASK(DEBUG2) | MASK(DEBUG3);
}
/**
 * @brief Enter LLS
 */
void deepsleep(void)
{
	/* Set the SLEEPDEEP bit to enable deep sleep mode (STOP) */
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#ifdef CMSIS
__wfi();
#else
	/* WFI instruction will start entry into STOP mode */
	asm("WFI");
#endif
}
