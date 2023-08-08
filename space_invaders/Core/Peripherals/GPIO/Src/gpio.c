/*
 * gpio.c
 *
 *  Created on: Aug 1, 2023
 *      Author: kaustubh
 */

#include "stm32f4xx_hal.h"
#include "error_handler.h"

/* debounce limit count */
#define DEBOUNCE_COUNT	10

typedef struct user_button_status
{
	uint8_t current_state;	// =0 if pressed, 1 if not pressed
	uint32_t count;
	uint8_t button_state;	// final state of button
} user_button_status_t;

static user_button_status_t missile_button_stats;

extern volatile uint8_t user_missile_flag;

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA8 PA9 */
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void user_button_debounce_check(void) {
	missile_button_stats.current_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4);
	if (missile_button_stats.current_state
			!= missile_button_stats.button_state) {
		// button state is about to be changed, increase counter
		missile_button_stats.count++;
		if (missile_button_stats.count >= DEBOUNCE_COUNT) {
			// the button has not bounced for (DEBOUNCE_COUNT), change state
			missile_button_stats.button_state =
					missile_button_stats.current_state;
			if(missile_button_stats.button_state == 0)
			{
				user_missile_flag = 1;
			}
			missile_button_stats.count = 0;
		}
	} else {
		// reset counter
		missile_button_stats.count = 0;
	}
}
