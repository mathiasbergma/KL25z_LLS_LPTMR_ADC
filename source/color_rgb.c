/*
 * color_rgb.c
 *
 *  Created on: 12. sep. 2021
 *      Author: Mathias
 */

#include "color_rgb.h"

void init_LED(void)
{
	//Enable clock to Port B & D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;

	//Make the pins GPIO through Port Control Register
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);

	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);

	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);

	//Setting output ports
	PTB->PDDR |= (1UL << RED_LED) | (1UL << GREEN_LED);
	PTD->PDDR |= (1UL << BLUE_LED);

	//Turn off leds
	PTB->PSOR = (1UL << RED_LED);
	PTD->PSOR = (1UL << BLUE_LED);
	PTB->PSOR = (1UL << GREEN_LED);
}

void color_rgb(int color)
{

	if (color & 1 << 0)
	{
		//Turn on led
		PTD->PCOR = (1UL << BLUE_LED);
	}
	else
	{
		//Turn off led
		PTD->PSOR = (1UL << BLUE_LED);
	}

	if (color & 1 << 1)
	{
		//Turn on led
		PTB->PCOR = (1UL << GREEN_LED);
	}
	else
	{
		//Turn off led
		PTB->PSOR = (1UL << GREEN_LED);
	}
	if (color & 1 << 2)
	{
		//Turn on led
		PTB->PCOR = (1UL << RED_LED);
	}
	else
	{
		//Turn off led
		PTB->PSOR = (1UL << RED_LED);
	}

	return;
}
