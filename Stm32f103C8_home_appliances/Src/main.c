/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BH1750.h"
#include "SSD1306.h"
#include "fonts.h"
#include "DS1307.h"
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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
osThreadId joystickTaskHandle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */

void StartJoystickTask(void const * argument);
int ConvertFromInt(char* str, int a);
int ConvertFromFloat(char* str, float a);
int ConvertFromIntForTime(char* str, int a);
void Delay_us (uint16_t nTime);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	
	enum menu{	
		LUX,
		HUMIDITY,
	};
	
	enum menu_do{
		NOT_DO,
		DO,
	};
	
	enum level{
		Menu,
		Component,
	};
	
	
	uint8_t tx_buff[10] = {'H','E','L','L','\n'};
	uint8_t rx_buff[10];
	uint16_t adcBuffer[2];
	time tDS1307;
	
	int menu_select, menu_level, is_clicked;
	
	SemaphoreHandle_t xSemaphore;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_UART_Receive_DMA(&huart1, rx_buff, 10);
	ssd1306_Init();
	BH1750_Init();

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer, 2);

	menu_select = LUX;
	menu_level = Menu;
	is_clicked = 0;
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	/* Create a mutex type semaphore. */
	 xSemaphore = xSemaphoreCreateMutex();

	 if( xSemaphore == NULL )
	 {
			 Error_Handler();
	 }
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
	
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(joystickTask, StartJoystickTask, osPriorityNormal, 0, 128);
  joystickTaskHandle = osThreadCreate(osThread(joystickTask), NULL);
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = (36000000 / 10000) - 1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
	if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_MultiProcessor_Init(&huart1, 0, UART_WAKEUPMETHOD_IDLELINE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	HAL_UART_Transmit(UartHandle, rx_buff, sizeof(rx_buff), 1000);
	HAL_UART_Receive_DMA(UartHandle, rx_buff, 10);
}

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	if (GPIO_Pin == GPIO_PIN_4)
//	{ 
//		if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))
//			HAL_UART_Transmit(&huart1,(uint8_t *) "Clicked\n", 8, 1000);
//	}
//}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartJoystickTask(void const * argument)
{
	for(;;)
  {
		char str[6];
		int len;
		
		// Call DMA ADC
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer, 2);
		
		//ssd1306_SetCursor(70,23);
		len = ConvertFromInt(str, adcBuffer[0]);
		//ssd1306_WriteString(str ,Font_11x18, Black);
//		HAL_UART_Transmit(&huart1,(uint8_t*) str, len, 1000);
//		HAL_UART_Transmit(&huart1,(uint8_t *) "\n", 1, 1000);
//		len = ConvertFromInt(str, adcBuffer[1]);
		//ssd1306_SetCursor(70,40);
		//ssd1306_WriteString(str ,Font_11x18, Black);
//		HAL_UART_Transmit(&huart1,(uint8_t*) str, len, 1000);
//		HAL_UART_Transmit(&huart1,(uint8_t *) "\n", 1, 1000);
		
		//ssd1306_UpdateScreen();
		
		/* Check X, Y Joystick task */
		if (adcBuffer[0] > 3500) // right
		{
			HAL_UART_Transmit(&huart1,(uint8_t *) "trigger right\n", 8, 1000);
			while (adcBuffer[0] > 2000) HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer, 2);
		}
		else if (adcBuffer[0] < 100) // left
		{
			HAL_UART_Transmit(&huart1,(uint8_t *) "trigger left\n", 8, 1000);
			while (adcBuffer[0] < 1500) HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer, 2);
		}
		else if (adcBuffer[1] > 3500) // down
		{
			if (menu_level == Menu)
			{
				menu_select++;
			}
			HAL_UART_Transmit(&huart1,(uint8_t *) "trigger down\n", 8, 1000);
			while (adcBuffer[1] > 2000) HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer, 2);
		}
		else if (adcBuffer[1] < 500) // up
		{
			if (menu_level == Menu)
			{
				menu_select--;
			}
			HAL_UART_Transmit(&huart1,(uint8_t *) "trigger up\n", 8, 1000);
			while (adcBuffer[1] < 1500) HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer, 2);
		}
		
		if (menu_select > HUMIDITY)
		{
			menu_select = LUX;
		}
		if (menu_select < LUX)
		{
			menu_select = HUMIDITY;
		}
		
		/* Check SW */
		if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))
		{
			HAL_UART_Transmit(&huart1,(uint8_t *) "Clicked\n", 8, 1000);
			is_clicked = 1;
			while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4));
		}
		
		osDelay(1);
	}
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
    
    
    

  /* USER CODE BEGIN 5 */
	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
	osDelay(1000);
	
	tDS1307.out = 1;
	tDS1307.sqwe = 1;
	tDS1307.rs1 = 0;
	tDS1307.rs0 = 0;
	
	tDS1307.day_of_week = 6;
	tDS1307.date = 21;
	tDS1307.month = 9;
	tDS1307.year = 19;

	tDS1307.hours = 2;
	tDS1307.minutes = 12;
	tDS1307.seconds = 10;
					
	//writeTime(&tDS1307);
	osDelay(1000);
	
  /* Infinite loop */
  for(;;)
  {
		char str[6];
		int len;
		
		//if( xSemaphore != NULL )
    {
			// Block 
			//if (xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE)
			{
//				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//				int temp = DHT22_Read (&data);
//				if ( temp == 1 )
//				{
//					/* Use data.temperature and data.humidity */
//					int len = ConvertFromFloat(str, data.temperature);
//					HAL_UART_Transmit(&huart1,(uint8_t*) str, len, 1000);
//					HAL_UART_Transmit(&huart1,(uint8_t*) "oC\n", 3, 1000);
//					ConvertFromFloat(str, data.humidity);
//					HAL_UART_Transmit(&huart1,(uint8_t*) str, sizeof(str), 1000);
//					HAL_UART_Transmit(&huart1,(uint8_t*) "GO\n", 3, 1000);
					
					
				
					/* FILL TIME */
					//tDS1307 = getTime();
					//printTime(&tDS1307);
				
					ssd1306_Fill(White);
					
					ssd1306_SetCursor(1,2);
					len = ConvertFromIntForTime(str, tDS1307.hours);
					ssd1306_WriteString(str ,Font_7x10,Black);
					ssd1306_WriteString(":" ,Font_7x10,Black);
					len = ConvertFromIntForTime(str, tDS1307.minutes);
					ssd1306_WriteString(str ,Font_7x10,Black);
					ssd1306_WriteString(" " ,Font_7x10,Black);
					
					get_Day_Of_Week(str, tDS1307.day_of_week);
					//len = ConvertFromInt(str, tDS1307.day_of_week);
					ssd1306_WriteString(str ,Font_7x10,Black);
					ssd1306_WriteString("," ,Font_7x10,Black);
					len = ConvertFromIntForTime(str, tDS1307.date);
					ssd1306_WriteString(str ,Font_7x10,Black);
					ssd1306_WriteString("-" ,Font_7x10,Black);
					len = ConvertFromIntForTime(str, tDS1307.month);
					ssd1306_WriteString(str ,Font_7x10,Black);
					ssd1306_WriteString("-" ,Font_7x10,Black);
					len = ConvertFromIntForTime(str, tDS1307.year);
					ssd1306_WriteString(str ,Font_7x10,Black);
					
					
					/* Do menu */
					if (menu_level == Menu)
					{
						/* FILL MENU */
						ssd1306_SetCursor(3,20);
						ssd1306_WriteString("View lux" ,Font_7x10, Black);
						ssd1306_SetCursor(3,33);
						ssd1306_WriteString("View humidity" ,Font_7x10, Black);
						switch (menu_select)
						{
							case LUX:
								ssd1306_SetCursor(60,20);
								ssd1306_WriteString("*", Font_7x10, Black);
								break;
							case HUMIDITY:
								ssd1306_SetCursor(100,33);
								ssd1306_WriteString("*", Font_7x10, Black);
								break;
						};
						if (is_clicked == 1) // user just clicked
						{
							menu_level = Component;
							is_clicked = 0;
						}
					}
					else if (menu_level == Component)
					{
						switch (menu_select)
						{
							case LUX:
								ssd1306_SetCursor(2,23);
								ssd1306_DrawLuxIcon(Black);
								
								ssd1306_SetCursor(24,23);
								len = ConvertFromInt(str, BH1750_Read());
								ssd1306_WriteString(str ,Font_11x18,Black);
							
								ssd1306_SetCursor(70,23);
							  ssd1306_WriteString("Back*" ,Font_7x10,Black);
								if (is_clicked == 1)
								{
									menu_level = Menu;
									is_clicked = 0;
								}
								break;
							case HUMIDITY:
								ssd1306_SetCursor(100,33);
								ssd1306_WriteString("*", Font_7x10, Black);
								break;
						};
					}
					
					ssd1306_UpdateScreen();					
					osDelay(1000);
				}				 
//				else 
//				{
//					/* 
//						Error - check response code in data.rcv_response, for more info 
//						attach debugger and examine impulse times in data.bits[40] (dht22.c) 
//					*/
//					int len = ConvertFromInt(str, data.rcv_response);
//					HAL_UART_Transmit(&huart1,(uint8_t*) str, len, 1000);
//				}
				
				// Unlock
				//xSemaphoreGive( xSemaphore );
			}
		
		
//		HAL_GPIO_WritePin(DHT22_GPIO_PORT, DHT22_GPIO_PIN, GPIO_PIN_SET);
//		for (time = 0; time < 100; time++)
//		{
//			Delay_us(10000);
//		}
//		HAL_GPIO_WritePin(DHT22_GPIO_PORT, DHT22_GPIO_PIN, GPIO_PIN_RESET);
//		for (time = 0; time < 100; time++)
//		{
//			Delay_us(10000);
//		}
//		
		}
	//}
  /* USER CODE END 5 */ 
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	while(1){
		
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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
