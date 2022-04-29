/*
 * uart.c
 *
 *  Created on: 27. apr. 2022
 *      Author: Mathias
 */

#include "uart.h"

/**
 * @brief transmits array of doubles on uart (polled)
 * @param number Number to transmit
 * @param size Size of array
 */
void transmit_double(double number)
{
	char num_str[32];
	int temp;
	int temp2;
	double local_number;

	local_number = number;
	temp = local_number;
	local_number -= temp;
	temp2 = local_number * 100;

	sprintf(num_str, "%d.%d\n", temp, temp2);

	transmit_data(num_str);

}
/**
 * @brief Transmit char array on uart
 * @param pdata
 */
void transmit_data(char *pdata)
{
	uint32_t masking_state;
	// Wait until complete string is transmitted on serial port
	// and every byte is shifted out of Transmit buffer before
	// loading new byte
	while (*pdata)
	{
		// Save masking state
		masking_state = __get_PRIMASK();
		// Disable interrupts
		__disable_irq();
		UART0->D = *pdata;

		// Wait until byte is transmitted from Tx Buffer
		while (!(UART0->S1 & UART_S1_TDRE_MASK))
		{
		}
		// Restore masking state
		__set_PRIMASK(masking_state);
		__enable_irq();

		pdata++;
	}
}
/**
 * @brief Initializes UART0
 */
void uart0_init(void)
{
	// Select MCGFLLCLK as UART0 clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);

	// Enable UART0 Clock
	SIM->SCGC4 |= SIM_SCGC4_UART0(1);

	// Enable PORTA clock
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1);

	PORTA->PCR[1] |= PORT_PCR_MUX(2); /* PTA1 as ALT2 (UART0) */
	PORTA->PCR[2] |= PORT_PCR_MUX(2); /* PTA2 as ALT2 (UART0) */

	// Configure Baud Rate as 115200
	UART0->BDL = 0x1a;
	UART0->BDH = 0x0;

	// Configure Serial Port as 8-N-1
	// (8 data bits, No parity and 1 stop bit)
	UART0->C1 = 0x00;

	// Configure Tx/Rx Interrupts
	UART0->C2 |= UART_C2_TIE(0);  // Tx Interrupt disabled
	UART0->C2 |= UART_C2_TCIE(0); // Tx Complete Interrupt disabled
	UART0->C2 |= UART_C2_RIE(1);     // Rx Interrupt enabled

	// Configure Transmitter/Receiever
	UART0->C2 |= UART_C2_TE(1);  // Tx Enabled
	UART0->C2 |= UART_C2_RE(1);  // Rx Enabled

	// Enable UART0 Interrupt
	//__NVIC_EnableIRQ(UART0_IRQn);
}
