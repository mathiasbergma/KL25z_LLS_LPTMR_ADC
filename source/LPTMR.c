/*
 * LPTMR.c
 *
 *  Created on: 20. apr. 2022
 *      Author: Mathias
 */
#include "LPTMR.h"
#include "MKL25Z4.h"
#include "adc_irq.h"


void LPTMR0_IRQHandler(void)
{
	SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
	// write 1 to TCF to clear the LPT timer compare flag
	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
	//SMC->PMCTRL |= SMC_PMCTRL_RUNM(2);
	PTE->PTOR |= (1UL << 21);
	// Initialize conversion
	adcStart(0);
}



/**
 * @brief Initializes low power timer for 5 second interrupt
 * @return void
 */
void initLPTMR(void)
{
	// Enable clock to Low Power Timer
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;
	// Gate System Oscillator
	SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(3);
	// Reset registers
	LPTMR0->CSR = 0;
	// Load compare value
	LPTMR0->CMR = 999;
	/* Bypass prescaler
	 * Pulse counter input 1 select (1kHz)
	 */
	LPTMR0->PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;
	//LPTMR0->CNR = 0;
	/* Enable interrupt
	 * Enable timer
	 */
	LPTMR0->CSR = LPTMR_CSR_TIE_MASK;
	LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
	// Set Interrupt priority
	NVIC_SetPriority(LPTMR0_IRQn, 1);
	// Clear any pending interrupts
	NVIC_ClearPendingIRQ(LPTMR0_IRQn);
	// Enable LPTMR0 interrupts
	NVIC_EnableIRQ(LPTMR0_IRQn);
}
