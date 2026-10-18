/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FOC.h"
#include "MT6816.h"
#include "string.h"
#include "stdio.h"
#include "FOC.h"
#include "math.h"
#include <stdbool.h>
#include "def.h"

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
void Transmit_float(float data,uint8_t acc,UART_HandleTypeDef *huart);
void Transmit_4float(float data1,float data2,float data3,float data4,UART_HandleTypeDef *huart);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MT6816_CS_Pin GPIO_PIN_4
#define MT6816_CS_GPIO_Port GPIOA
#define OLED_RES_Pin GPIO_PIN_12
#define OLED_RES_GPIO_Port GPIOB
#define VBUS_Pin GPIO_PIN_14
#define VBUS_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_11
#define OLED_DC_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_4
#define OLED_CS_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define usart1_rx_num 6
#define ADC_CHANNELS 5
#define VREF 3.3f

extern uint8_t DriverAlign_flag;
extern uint8_t tran_flag;
extern uint8_t usart1_rx[usart1_rx_num];

extern float theta_step;
extern uint16_t factor;
extern float pi;
extern SVPWM_Struct FOC1;
extern SVPWM_Result result1; 
extern float ADC_Value[ADC_CHANNELS];
extern PIController cur_lop_iq_pictr;
extern PIController cur_lop_id_pictr;
extern uint16_t ADC_Value_temp[ADC_CHANNELS];
extern int i_DriverAlign;
extern uint8_t isample_flag;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
