/*
 * gpio.h
 *
 *  Created on: Aug 1, 2023
 *      Author: kaustubh
 */

#ifndef PERIPHERALS_GPIO_INC_GPIO_H_
#define PERIPHERALS_GPIO_INC_GPIO_H_

void MX_GPIO_Init(void);

/* checking for user button debouncing */
void user_button_debounce_check(void);

#endif /* PERIPHERALS_GPIO_INC_GPIO_H_ */
