/*
 * adc.h
 *
 *  Created on: 28 Aug 2018
 *      Author: au263437
 */

#ifndef ADC_IRQ_H_
#define ADC_IRQ_H_

// NOTE: by including the below file, we get directly access to the defined struct "ADC0",
// a pointer to the "ADC_Type" that contains all registers in the ADC
#include "MKL25Z4.h"


// function for setting up the ADC
// param: channel selectes the channel, and pin on the KL25 Z board, to uniitalize
// The ADC is initalized in 16 bit mode
void adcInit(int channel, int continous, int avge, int samples);

// function for reading the ADC, in single ended mode
// param: channel selectes the channel, and pin on the KL25 Z board, to read from
// valid values are 0-5, that corrwsponds to A0-A5 pins on the board
// this function is blocking, and returns when a conversion is finished
void adcStart(int channel);



#endif /* ADC_IRQ_H_ */
