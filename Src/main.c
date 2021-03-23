/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_hd44780.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
	uint8_t hour;
	uint8_t minute;
	uint8_t duration;
} Schedule;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MODE_DISPLAY_CLCOK		0x00
#define MODE_SET_CLOCK			0x01
#define SET_HOUR				0x00
#define SET_MINUTE				0x01
#define SET_SECONDS				0x02
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define BTN_PRESSED(GPIOx, GPIO_Pin) ({\
	uint8_t result = 0;\
	if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 0) { \
		while(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 0);\
		Delay(100);\
		result = 1;\
	}\
	result;\
})
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim16;

/* USER CODE BEGIN PV */
char cTime[12];
RTC_TimeTypeDef tTime = {0};
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};
uint8_t mode = 0;
uint8_t subMode = 0;
uint8_t onCounter = 0;
Schedule schedules[] = {{7, 0, 120}, {17, 0, 60}};
uint8_t scheduleIdx = -1;
uint8_t scheduleCount = 2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM14_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */
extern void TM_HD44780_Init(uint8_t cols, uint8_t rows);
void DisplayTime(RTC_TimeTypeDef sTime);
void setNextSchedule();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance == TIM14) {
		if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK ||
				HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
		DisplayTime(sTime);
	}
	if(htim->Instance == TIM16) {
		if(onCounter <= 0) {
			HAL_GPIO_WritePin(PN2222_GPIO_Port, PN2222_Pin, 0);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
			HAL_TIM_Base_Stop_IT(&htim16);
		}
		else {
			onCounter--;
		}
	}
}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	HAL_GPIO_WritePin(PN2222_GPIO_Port, PN2222_Pin, 1);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
	onCounter = schedules[scheduleIdx].duration;
	HAL_TIM_Base_Start_IT(&htim16);
	setNextSchedule();
}
void setNextSchedule() {
	scheduleIdx = (scheduleIdx < scheduleCount) ? scheduleIdx + 1 : 0;
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
	RTC_AlarmTypeDef sAlarm = {0};
	sAlarm.AlarmTime.Hours = schedules[scheduleIdx].hour;
	sAlarm.AlarmTime.Minutes = schedules[scheduleIdx].minute;
	sAlarm.AlarmTime.Seconds = 0;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 1;
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}
}
void DisplayTime(RTC_TimeTypeDef sTime) {
	sprintf(cTime, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
	TM_HD44780_Puts(0, 1, cTime);
}
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
  MX_RTC_Init();
  MX_TIM14_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  TM_HD44780_Init(16, 2);
  TM_HD44780_Puts(0, 0, "Hello Booboo");
  HAL_TIM_Base_Start_IT(&htim14);
  setNextSchedule();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if(BTN_PRESSED(BTN_SET_GPIO_Port, BTN_SET_Pin)) {
		if(mode == MODE_DISPLAY_CLCOK) {
			mode = MODE_SET_CLOCK;
			subMode = SET_HOUR;
			HAL_TIM_Base_Stop(&htim14);
			tTime = sTime;
		}
		else {
			if(mode == MODE_SET_CLOCK && subMode < SET_SECONDS) {
				subMode++;
			}
			else {
				mode = MODE_DISPLAY_CLCOK;
				subMode = -1;
				if (HAL_RTC_SetTime(&hrtc, &tTime, RTC_FORMAT_BIN) != HAL_OK)
				{
					Error_Handler();
				}
				sTime = tTime;
				HAL_TIM_Base_Start_IT(&htim14);
			}
		}
		switch(subMode) {
		case SET_HOUR: { TM_HD44780_Puts(0, 0, "Set Hour    "); break; }
		case SET_MINUTE: { TM_HD44780_Puts(0, 0, "Set Minutes "); break; }
		case SET_SECONDS: { TM_HD44780_Puts(0, 0, "Set Seconds "); break; }
		default: { TM_HD44780_Puts(0, 0, "Hello Booboo"); }
		}
	}
	if(BTN_PRESSED(BTN_UP_GPIO_Port, BTN_UP_Pin)) {
		switch (subMode) {
		case SET_HOUR: {
			if((hrtc.Init.HourFormat == RTC_HOURFORMAT_24 && tTime.Hours < 23U) ||
				(hrtc.Init.HourFormat == RTC_HOURFORMAT_12 && tTime.Hours < 11U)) {
				tTime.Hours += 1;
			}
			break;
		}
		case SET_MINUTE: { if(tTime.Minutes < 59) tTime.Minutes += 1; break; }
		case SET_SECONDS: {	if(tTime.Seconds < 59) tTime.Seconds += 1; break; }
		}
		DisplayTime(tTime);
	}
	if(BTN_PRESSED(BTN_DOWN_GPIO_Port, BTN_DOWN_Pin)) {
		switch (subMode) {
		case SET_HOUR: { if(tTime.Hours > 0) tTime.Hours -= 1; break; }
		case SET_MINUTE: { if(tTime.Minutes > 0) tTime.Minutes -= 1; break; }
		case SET_SECONDS: { if(tTime.Seconds > 0) tTime.Seconds -= 1; break; }
		}
		DisplayTime(tTime);
	}
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV32;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 1953;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 32000;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 1000;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 32000;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 1000;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_Pin|LCD_D7_Pin|LCD_D6_Pin|LCD_D5_Pin
                          |LCD_D4_Pin|LCD_EN_Pin|LCD_RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PN2222_GPIO_Port, PN2222_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BTN_DOWN_Pin BTN_SET_Pin BTN_UP_Pin */
  GPIO_InitStruct.Pin = BTN_DOWN_Pin|BTN_SET_Pin|BTN_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin LCD_D7_Pin LCD_D6_Pin LCD_D5_Pin
                           LCD_D4_Pin LCD_EN_Pin LCD_RS_Pin */
  GPIO_InitStruct.Pin = LED_Pin|LCD_D7_Pin|LCD_D6_Pin|LCD_D5_Pin
                          |LCD_D4_Pin|LCD_EN_Pin|LCD_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PN2222_Pin */
  GPIO_InitStruct.Pin = PN2222_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(PN2222_GPIO_Port, &GPIO_InitStruct);

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
