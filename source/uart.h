/*
 * uart.h
 *
 *  Created on: 27. apr. 2022
 *      Author: Mathias
 */
#include "MKL25Z4.h"

#ifndef UART_H_
#define UART_H_

#include <stdio.h>

void uart0_init(void);
void transmit_double(double number);
void transmit_data(char *pdata);

#endif /* UART_H_ */
