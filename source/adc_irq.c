/**
 * @file adc_irq.c
 * @author Bergma
 * @date nov 17 2021
 */

#include <adc_irq.h>

#define N_CHANNEL_MAX 5
const char channelMap[6] = {0x08,0x09,0x0C,0x0D,0x0B,0x0F};


/**
 * @param channel number 0-5, corresponding to available pins on the KL25Z board
 * @param continous Conversion enable (1 or 0)
 * @param avge Enable hardware averaging (1 or 0)
 * @param samples : 4, 8, 16 or 32 samples averaged. Must be 0 if Hardware averaging is disabled
 * @brief this function initalizes ADC0, one of the two converters on the chip
 *
 * for channel map, see ch 28.3.1
 * A0 = ADC0_SE8 - PTB0 - ADCH = 01000
 * A1 = ADC0_SE9 - PTB1 - ADCH = 01001
 * A2 = ADC0_SE12 - PTB2 - ADCH = 01100
 * A3 = ADC0_SE13 - PTB3 - ADCH = 01101
 * A4 = ADC0_SE11 - PTC2 - ADCH = 01011
 * A5 = ADC0_SE15 - PTC1 - ADCH = 01111
 * Temperature sensor - 11010
 * KL25P80M48SF0RM - chapter 28 describes the ADC, and corresponding registers
 *
 * */
void adcInit(int channel, int continous, int avge, int samples)
{
	/* is the channel number valid ? */
	if (!(channel >= 0 && channel < 6))
	{
		return;
	}
	if (!(samples == 4  || samples == 8 || samples == 16 || samples == 32 || samples == 0))
	{
		return;
	}
	/* enable clock for port B & C */
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK;
	/* enable clock (power) to the ADC*/
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	/* setup the converter - ADC0
	 * Low power config
	 * long sample time
	 * 16 Bit SE
	 * Bus clk / 2
	 * Interrupt Enable*/
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(3) |
	ADC_CFG1_ADICLK(1);

	/* Software trigger
	 * compare func. disabled
	 * DMA dsabled
	 * Voltgae ref - VREFH and VREFL*/
	ADC0->SC2 = ADC_SC2_ADTRG(0) | ADC_SC2_ACFE(0) | ADC_SC2_DMAEN(0) | ADC_SC2_REFSEL(0);

	/* Analog Interrupt enable*/
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | channelMap[channel];

	/*
	 * Continous conversion enable
	 * Hardware avarage enable
	 * Hardware average select 8 samples
	 */
	if (continous)
	{
		ADC0->SC3 |= ADC_SC3_ADCO_MASK;
	}
	if (avge)
	{
		ADC0->SC3 |= ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(samples);;
	}


	/* 6: Setup port control register to analog funcitons, correspodingly
	 * for analog functionality, PINMUX bits 8-11 must be set to 000,
	 * all the digital functions, drive strength, filters etc are disables*/
	if (channel < 4)
	{
		PORTB->PCR[channel] &= ~PORT_PCR_MUX_MASK;
		PORTB->PCR[channel] |= PORT_PCR_MUX(0);
	} else
	{
		PORTC->PCR[channel] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[channel] |= PORT_PCR_MUX(0);
	}

	NVIC_SetPriority(ADC0_IRQn, 10);
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	NVIC_EnableIRQ(ADC0_IRQn);
}

/**
 * @param channel number 0-5, corresponding to available pins on the KL25Z board
 * @brief this function reads from on of multiplexed channels on ADC0
 *
 * Read from ADC pins, each channel is selected by writing the ADCH bits in the SC
 * A0 = ADC0_SE8 - PTB0 - ADCH = 01000
 * A1 = ADC0_SE9 - PTB1 - ADCH = 01001
 * A2 = ADC0_SE12 - PTB2 - ADCH = 01100
 * A3 = ADC0_SE13 - PTB3 - ADCH = 01101
 * A4 = ADC0_SE11 - PTC2 - ADCH = 01011
 * A5 = ADC0_SE15 - PTC1 - ADCH = 01111
 */
void adcStart(int channel)
{
	/* 1: is the channel valid ? */
	if (channel > N_CHANNEL_MAX)
	{
		return;
	}

	/* 3: start the selected channel, lookup in channel map, to enable to correct ADC channel*/

	ADC0->SC1[0] |= channelMap[channel];

	/* 4: polled implementation, wait for COCO (COnversion COmplete) bit to be set */
/*	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
		;
*/
	/* 5: return result, just the entire 32 bit register, casted as unsigned short, since unused bits 16:31 are cleeared by HW */
//	res = (unsigned short) ADC0->R[0];

	return;

}

