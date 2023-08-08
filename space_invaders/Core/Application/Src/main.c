/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* to store the 12-bit ADC value */
uint16_t adc_val[1] = { 0 };

/* task handler for the application's primary task */
TaskHandle_t xTaskHandle_spaceInvaders = NULL;

/* TIM2 base timer handle */
extern TIM_HandleTypeDef htim2;

/* ADC handle */
extern ADC_HandleTypeDef hadc1;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* MCU configuration--------------------------------------------------------*/

	/* reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* configure the system clock */
	SystemClock_Config();

	/* configure the peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_SPI1_Init();
	MX_ADC1_Init();
	MX_TIM2_Init();

	/* initialize the Nokia 5110 LCD */
	Nokia5110_Init();

	/* start the HAL base timer - TIM2 */
	HAL_TIM_Base_Start(&htim2);

	/* start the sampling the ADC input */
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_val, 1);

	/* create the primary application task */
	assert_param(
			xTaskCreate((pdTASK_CODE)space_invaders_app, "Space Invaders", TASK_STACK_SIZE, NULL, 1, &xTaskHandle_spaceInvaders) == pdPASS);

	/* start the scheduler */
	vTaskStartScheduler();

	while(1);

	return 0;
}
