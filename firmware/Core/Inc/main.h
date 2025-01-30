/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// In your main.h or a config file
#define REGION_US915  // or your region
#define USE_MODEM_LORA
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BMP280_nCS_Pin GPIO_PIN_4
#define BMP280_nCS_GPIO_Port GPIOA
#define SX1276_DIO0_Pin GPIO_PIN_0
#define SX1276_DIO0_GPIO_Port GPIOB
#define SX1276_DIO1_Pin GPIO_PIN_1
#define SX1276_DIO1_GPIO_Port GPIOB
#define SX1276_nCS_Pin GPIO_PIN_8
#define SX1276_nCS_GPIO_Port GPIOA
#define SX1276_DIO2_Pin GPIO_PIN_11
#define SX1276_DIO2_GPIO_Port GPIOA
#define SX1276_DIO3_Pin GPIO_PIN_12
#define SX1276_DIO3_GPIO_Port GPIOA
#define LED_PIN_Pin GPIO_PIN_15
#define LED_PIN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
