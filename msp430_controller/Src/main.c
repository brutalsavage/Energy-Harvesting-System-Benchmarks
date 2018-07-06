/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
/* Different benchmark saves -------------------------------------------------*/
#define MIDI_Save 926
#define MIDI_Restore 1757
#define MIDI_numbersaves 11250
#define MIDI_minus 600000

#define DS_Save 1415
#define DS_Restore 2574
#define DS_numbersaves 2200
#define DS_minus 550000
#define DS_minus2 670000

#define AR_Save 1498
#define AR_Restore 2933
#define AR_numbersaves 402
#define AR_minus 450000


#define RSA_HIBERNUS_Restore 7949
#define RSA_HIBERNUS_Save 4445
#define RSA_numbercheck 32650	
#define RSA_check 5



#define CRC_HIBERNUS_Restore 1419
#define CRC_HIBERNUS_Save 709 
#define CRC_numbercheck 18290
#define CRC_check 5

#define SENSE_HIBERNUS_Restore 1380
#define SENSE_HIBERNUS_Save 709
#define SENSE_numbercheck 6400
#define SENSE_check 5

#define LOOPTEST_MEMENTOS_Save 661
#define LOOPTEST_MEMENTOS_Restore 1510

#define LOOPTEST_HIBERNUS_Save 3933
#define LOOPTEST_HIBERNUS_Restore 7301

#define LOOPTEST_numbercheck 6400
#define LOOPTEST_check 5

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//static void MX_USART2_UART_Init(void);

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
double counter = 0;
double counter2 = 0;
double counter3 = 0;
double timer = 0;
double timer2 = 0;
double check2 = 0;
double sum1 = 0;
double sum2 = 0;
double check;

double dead=0;
double forward =0;
double restore = 0;
double save = 0;
double checkpoint = 0;


static TIM_HandleTypeDef s_TimerInstance = { 
    .Instance = TIM2
};

static TIM_HandleTypeDef s_Timer2Instance = { 
    .Instance = TIM5
};
void InitializeTimer()
{
    __TIM2_CLK_ENABLE();
    s_TimerInstance.Init.Prescaler = 0;
    s_TimerInstance.Init.CounterMode = TIM_COUNTERMODE_UP;
    s_TimerInstance.Init.Period = 0xFFFFFFFF;
    s_TimerInstance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    s_TimerInstance.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&s_TimerInstance);
    HAL_TIM_Base_Start(&s_TimerInstance);
}

void InitializeTimer2()
{
    __TIM5_CLK_ENABLE();
    s_Timer2Instance.Init.Prescaler = 0;
    s_Timer2Instance.Init.CounterMode = TIM_COUNTERMODE_UP;
    s_Timer2Instance.Init.Period = 0xFFFFFFFF;
    s_Timer2Instance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    s_Timer2Instance.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&s_Timer2Instance);
    HAL_TIM_Base_Start(&s_Timer2Instance);
}




int main(void)
	{
  HAL_Init();
	
  SystemClock_Config();
  InitializeTimer();
	InitializeTimer2();
  MX_GPIO_Init();
  //MX_USART2_UART_Init();
	//HAL_TIM_Base_Init(&s_TimerInstance);
	//timer = __HAL_TIM_GET_COUNTER(&s_TimerInstance);
	
	//HAL_TIM_Base_Init(&s_Timer2Instance);
	//timer = __HAL_TIM_GET_COUNTER(&s_Timer2Instance);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
	
  while (1)
  {
		//DINO
		/*
		if (check2 == 0) {
		
			//counter2=0;
			check = 0;
		
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		
		for (long long int i=0; i<400000; i++);
	
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
 		for (long long int i=0; i<10000000; i++);
		counter3++;
		sum1 = sum1 + timer - DS_minus2;
		sum2 = sum2 + timer2 - DS_minus2;
		
		}
		
		else {
			sum1  = sum1/100;
			sum2 = sum2/100;
			dead = sum2/sum1;
			forward = (sum1 - sum2 - counter3*DS_Restore -DS_numbersaves*DS_Save)/sum1;
			save = DS_numbersaves*DS_Save/sum1;
			restore = counter3*DS_Restore/sum1;
			while (1) {
			
			}
		}
		*/
		
		//HIBERNUS
		
		if (check2 == 0) {
		
			//counter2=0;
			check = 0;
		
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		
		for (long long int i=0; i<400000; i++);
	
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
 		for (long long int i=0; i<10000000; i++);
		counter3++;
		sum1 = sum1 + timer;
		//sum2 = sum2 + timer2 - DS_minus2;
		
		}
		
		else {
			sum1  = sum1/100;
			//sum2 = sum2/100;
			//dead = sum2/sum1;
			forward = (sum1 - (counter3-1)*LOOPTEST_HIBERNUS_Restore -(counter2-1)*LOOPTEST_HIBERNUS_Save)/sum1;
			save = (counter2-1)*LOOPTEST_HIBERNUS_Save/sum1;
			restore = (counter3-1)*LOOPTEST_HIBERNUS_Restore/sum1;
			while (1) {
			
			}
		}
		
		//MEMENTOS
		/*
		if (check2 == 0) {
		
			//counter2=0;
			check = 0;
		
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		
		for (long long int i=0; i<400000; i++);
	
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
 		for (long long int i=0; i<10000000; i++);
		counter3++;
		sum1 = sum1 + timer - 350000;
		sum2 = sum2 + timer2 ;
		
		}
		
		else {
			
			sum1  = sum1/100;
			sum2 = sum2/100;
			dead = (sum1-sum2-(counter2-1)*LOOPTEST_MEMENTOS_Save)/sum1;
			forward = (sum2-(counter3-1)*LOOPTEST_MEMENTOS_Restore-LOOPTEST_numbercheck*LOOPTEST_check)/sum1;
			save = (counter2-1)*LOOPTEST_MEMENTOS_Save/sum1;
			restore = (counter3-1)*LOOPTEST_MEMENTOS_Restore/sum1;
			checkpoint = LOOPTEST_numbercheck*LOOPTEST_check/sum1;
			*/
			/*
			sum1 = sum1/100;
			forward = (sum1- (counter3-1)*SENSE_HIBERNUS_Restore - (counter2-1)*SENSE_HIBERNUS_Save - SENSE_numbercheck*SENSE_check)/sum1;
			save = (counter2-1)*SENSE_HIBERNUS_Save/sum1;
			restore = (counter3-1)*SENSE_HIBERNUS_Restore/sum1;
			checkpoint = SENSE_numbercheck*SENSE_check/sum1;
				*/
				/*
			while (1) {
			
			}
			
		
		}
		*/
  }

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART2 init function */
/*
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

} */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	// Initialise PIN_PC8 with output of 0
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	
	GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	
	GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	
	GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}
void EXTI4_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
void EXTI2_IRQHandler(void)
{
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
	
    // ISR code
}
void EXTI3_IRQHandler(void)
{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
	
    // ISR code
}

void EXTI1_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
	
    // ISR code
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_2)
  {
   counter2++;
		timer = __HAL_TIM_GET_COUNTER(&s_TimerInstance);
		//HAL_TIM_Base_Init(&s_Timer2Instance);
		//timer2 = __HAL_TIM_GET_COUNTER(&s_Timer2Instance);
  }
	else if(GPIO_Pin == GPIO_PIN_3) 
	{
		if (check == 0) {
			//__HAL_TIM_SET_COUNTER(&s_TimerInstance,0);
			
			check = 1;
		}
		else {
			//timer = __HAL_TIM_GET_COUNTER(&s_TimerInstance);
			//timer2 = __HAL_TIM_GET_COUNTER(&s_Timer2Instance);
			//counter++;
		}
	}
	
	else if (GPIO_Pin == GPIO_PIN_1)
	{

		HAL_TIM_Base_Init(&s_TimerInstance);
		HAL_TIM_Base_Init(&s_Timer2Instance);
	}
	else if (GPIO_Pin == GPIO_PIN_4)
	{
		check2 = 1;
	}
	
	else {
		while(1) {
		}
	}

}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
