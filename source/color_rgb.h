/*
 * color_rgb.h
 *
 *  Created on: 12. sep. 2021
 *      Author: Mathias
 */

#ifndef COLOR_RGB_H_
#define COLOR_RGB_H_

#include <stdio.h>
#include "board.h"

#define RED_LED (18) //Port B
#define GREEN_LED (19) //Port B
#define BLUE_LED (1) //Port D


#define BLACK	0
#define BLUE	1
#define GREEN	2
#define CYAN	3
#define RED		4
#define MAGENTA	5
#define YELLOW	6
#define WHITE	7

void init_LED(void);
void color_rgb(int color);

#endif /* COLOR_RGB_H_ */
