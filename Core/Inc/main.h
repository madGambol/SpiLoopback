/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define INDICATOR_LED_Pin GPIO_PIN_0
#define INDICATOR_LED_GPIO_Port GPIOA
#define MASTER_NSS_Pin GPIO_PIN_4
#define MASTER_NSS_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define DEBUG_TX_Pin GPIO_PIN_4
#define DEBUG_TX_GPIO_Port GPIOC
#define DEBUG_RX_Pin GPIO_PIN_5
#define DEBUG_RX_GPIO_Port GPIOC
#define SLAVE_NSS_Pin GPIO_PIN_15
#define SLAVE_NSS_GPIO_Port GPIOA
#define SLAVE_SCK_Pin GPIO_PIN_10
#define SLAVE_SCK_GPIO_Port GPIOC
#define SLAVE_MISO_Pin GPIO_PIN_11
#define SLAVE_MISO_GPIO_Port GPIOC
#define SLAVE_MOSI_Pin GPIO_PIN_12
#define SLAVE_MOSI_GPIO_Port GPIOC
#define MASTER_SCK_Pin GPIO_PIN_3
#define MASTER_SCK_GPIO_Port GPIOB
#define MASTER_MISO_Pin GPIO_PIN_4
#define MASTER_MISO_GPIO_Port GPIOB
#define MASTER_MOSI_Pin GPIO_PIN_5
#define MASTER_MOSI_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
