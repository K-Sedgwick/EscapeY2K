/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile int forwardRising = 1;

// This is a helper method to enable all of the LEDS on GPIOC.
void configureLEDS()
{
    // CONFIGURE MODER
    // Set bit 12, 14, 16, and 18
    GPIOC->MODER |= ((1 << 12) | (1 << 14) | (1 << 16) | (1 << 18));
    // Clear bit 13, 15, 17, and 19
    GPIOC->MODER &= ~((1 << 13) | (1 << 15) | (1 << 17) | (1 << 19));

    // CONFIGURE OTYPE
    // Clear bits 6, 7, 8, and 9
    GPIOC->OTYPER &= ~((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9));

    // CONFIGURE OSPEEDR
    // Clear bits 12, 14, 16, and 18 (the others dont matter)
    GPIOC->OSPEEDR &= ~((1 << 12) | (1 << 14) | (1 << 16) | (1 << 18));

    // CONFIGURE PUPDR
    // Clear bits 12, 13, 14, 15, 16, 17, 18, and 19
    GPIOC->PUPDR &= ~((1 << 12) | (1 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19));
}

void configureTimers()
{
	// Enable timers
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	// Set TIM3 to one-pulse mode
	TIM3->CR1 |= TIM_CR1_OPM;
	
	// Get TIM2 to 1 Hz
	TIM2->PSC = 7999;
	TIM2->ARR = 1000;
	
	// Get TIM3 to 5 Hz
	TIM3->PSC = 7999;
	TIM3->ARR = 200;
	
	// Enable update interrupts
	TIM2->DIER |= TIM_DIER_UIE;
	TIM3->DIER |= TIM_DIER_UIE;
	
	// Enable interrupt handlers
	NVIC_SetPriority(TIM2_IRQn, 3);
	NVIC_SetPriority(TIM3_IRQn, 3);
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);
}

// This method toggles an LED
void toggleLED(int pin)
{
    GPIOC->BSRR = ((GPIOC->ODR & pin) << 16U) | (~GPIOC->ODR & pin);
}

// Turn off an LED
// The name of the method is in Spanish because it was shorter than an English counterpart name :P
void apagaLED(int pin)
{
    GPIOC->BSRR = (uint32_t)(pin << 16); // We shift left 16 places here to access the "turn off" registers for the LEDs
}

// Turn on an LED
// The name of the method is in Spanish because it was shorter than an English counterpart name :P
void prendeLED(int pin)
{
    GPIOC->BSRR = (uint32_t)pin;
}

// Returns state of LED
int checkLED(int pin)
{
		return GPIOC->ODR & pin;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    HAL_Init();           // Reset of all peripherals, init the Flash and Systick
    SystemClock_Config(); // Configure the system clock

    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;        // Configure the clock for the GPIOC pin
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;        // Configure the clock for the GPIOA pin
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;        // Configure the clock for the GPIOA pin
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN; // Enable peripheral clock for the SYSCFG peripheral

    // USER BUTTON SECTION
    // CONFIGURE MODER
    // Clear bits 0 and 1
    GPIOA->MODER &= ~((1 << 0) | (1 << 1));
    // CONFIGURE OSPEEDR
    // Clear bit 0
    GPIOA->OSPEEDR &= ~(1 << 0);

    // CONFIGURE PUPDR
    // Set bit 1
    GPIOA->PUPDR |= (1 << 1);
    // Clear bit 0
    GPIOA->PUPDR &= ~(1 << 0);

    // CONFIGURE MODER
    // Clear bits 0 and 1
    GPIOA->MODER &= ~((1 << 8) | (1 << 9));
    // CONFIGURE OSPEEDR
    // Clear bit 0
    GPIOA->OSPEEDR &= ~(1 << 8);

    // CONFIGURE PUPDR
    // Set bit 1
    GPIOA->PUPDR |= (1 << 9);
    // Clear bit 0
    GPIOA->PUPDR &= ~(1 << 8);

    // END USER BUTTON SECTION

    // LED SECTION

    configureLEDS();

    // END LED SECTION
		
		// TIMER SECTION
		
		configureTimers();
		
		// END TIMER SECTION

    // Button 1
    // Rising edge of button
    EXTI->IMR |= EXTI_IMR_IM0;
    EXTI->RTSR |= EXTI_RTSR_RT0;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;

    // Falling edge of button
    EXTI->FTSR |= EXTI_FTSR_FT0;
    NVIC_SetPriority(EXTI0_1_IRQn, 2);
    NVIC_EnableIRQ(EXTI0_1_IRQn);

    // Button 2
    // Rising edge of button
    EXTI->IMR |= EXTI_IMR_IM4;
    EXTI->RTSR |= EXTI_RTSR_RT4;
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PA;

    // Falling edge of button
    EXTI->FTSR |= EXTI_FTSR_FT4;
    NVIC_SetPriority(EXTI4_15_IRQn, 2);
    NVIC_EnableIRQ(EXTI4_15_IRQn);

    // END EXTI SECTION
		
		// Start Clock
		TIM2->CR1 |= TIM_CR1_CEN;
    while (1)
    {
    }
}

/*
 * This function runs every time the USER button is pressesd or released!
 * The code that exists inside is multi-functional, and changes state back and forth
 *  between two states (the button being pressed and the button being released).
 */
void EXTI0_1_IRQHandler()
{
		TIM2->CNT = 0;
    // Toggle the blue LED and turn the red LED off
    toggleLED(GPIO_PIN_7);
    apagaLED(GPIO_PIN_6);
    toggleLED(GPIO_PIN_9);
    // Clear the flag
    EXTI->PR |= 1;
}

/*
 * This function runs every time the USER button is pressesd or released!
 * The code that exists inside is multi-functional, and changes state back and forth
 *  between two states (the button being pressed and the button being released).
 */
void EXTI4_15_IRQHandler()
{
		TIM2->CNT = 0;
    // Toggle the orange LED
		if (GPIOA->IDR & GPIO_IDR_4) {
			prendeLED(GPIO_PIN_6);
			prendeLED(GPIO_PIN_8);
		}
		else {
			apagaLED(GPIO_PIN_6);
			apagaLED(GPIO_PIN_8);
		}
    
    //  Clear the flag
    EXTI->PR |= (1 << 2);
}

void TIM2_IRQHandler()
{
		// Only tick if clock isn't time traveling
		if (!checkLED(GPIO_PIN_6) && !checkLED(GPIO_PIN_7)) {
			prendeLED(GPIO_PIN_6);
			TIM3->CR1 |= TIM_CR1_CEN;
		}
		
		TIM2->SR &= ~TIM_SR_UIF;
}

void TIM3_IRQHandler()
{
		// Stop motor tick after certain amount of time
		apagaLED(GPIO_PIN_6);
	
		TIM3->SR &= ~TIM_SR_UIF;
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
