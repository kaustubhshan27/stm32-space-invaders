/*
 * gpio.h
 *
 *  Created on: Aug 1, 2023
 *      Author: kaustubh
 */

#ifndef PERIPHERALS_GPIO_INC_GPIO_H_
#define PERIPHERALS_GPIO_INC_GPIO_H_

void MX_GPIO_Init(void);

/* checking user button accounting for debounce */
void user_button_debounce_check(void);

/* return the current state of the user button */
uint8_t get_user_button_status(void);

#endif /* PERIPHERALS_GPIO_INC_GPIO_H_ */
