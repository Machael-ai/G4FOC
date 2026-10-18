/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "current.h"
#include "oled.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
const unsigned char G[12]={0x0F,0x00,0x10,0x80,0x20,0x40,0x22,0x40,0x33,0x80,0x02,0x00};/*"G",39*/

uint8_t taskFlag_1000Hz = false;
uint8_t taskFlag_100Hz = false;
uint8_t taskFlag_10Hz = false;
uint8_t taskFlag_1Hz = false;
uint8_t led_cnt=0;
uint8_t DriverAlign_flag=false;
uint8_t isample_flag=0;

uint8_t tran_index=0;
uint8_t tran_flag=0;

uint16_t adc_cnt=0;

uint16_t factor=0;
uint8_t usart1_rx[usart1_rx_num]={0};
uint8_t rx_cplt=0XFF;
uint16_t ADC_Value_temp[ADC_CHANNELS]={0};
float ADC_Value[ADC_CHANNELS]={0};


extern MT6816 encoder;
uint8_t try_oled=0xCF;
char tail[4]={0x00,0x00,0x80,0x7f};
SVPWM_Result result1; 
SVPWM_Struct FOC1={0};
float pi=3.141593;
float theta_step=0;
PIController cur_lop_iq_pictr;
PIController cur_lop_id_pictr;

char align0[50]={0};
char align1[50]={0};
char align2[50]={0};
int i_DriverAlign=0;



float iq_ref=0.4;
float id_ref=0;
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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
        
        htim1.Instance->CCER |= TIM_CCER_CC4E;  // 启用内部比较事件
        htim1.Instance->CCR4 = 800;           // 设置比较值
        HAL_TIM_Base_Start(&htim1); 
  HAL_Delay(1000);
  HAL_TIM_Base_Start_IT(&htim2);
  initMT6816(&encoder,&hspi1);
  SVPWM_Init(&result1);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
//  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,800);
  HAL_ADCEx_InjectedStart_IT(&hadc1);
      /*使用串口空闲中断+DMA前要先接收一次*/
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1,usart1_rx,usart1_rx_num);
    HAL_UART_DMAResume(&huart1);
    PIController_Init(&cur_lop_iq_pictr,25,83159,15,0); //83159
    PIController_Init(&cur_lop_id_pictr,25,83159,15,0); 
    //DriverAlign(&FOC1);
  //HAL_UART_Receive_IT(&huart1, usart1_rx, usart1_rx_num);
  //HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Value_temp,ADC_CHANNELS);
//  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,249);
//  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,15);
//  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,484);
    OLED_Init();
        
    OLED_DisPlay_On();
       // OLED_DisplayTurn(1);
    //OLED_Refresh();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
                //OLED_ShowNum(0,0,4,3,16);
             
//	   OLED_WR_Byte(0xb0+10,OLED_CMD); //设置行起始地址
//	   OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
//	   OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
//	   for(int n=0;n<12;n++)
//		 OLED_WR_Byte(G[n],OLED_DATA);
//          HAL_Delay(100);
                //OLED_Clear()
	  if(taskFlag_10Hz==true){
               if(led_cnt==5){
                      HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
                      led_cnt=0;
                }		                
                led_cnt+=1;
                HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Value_temp,ADC_CHANNELS);
                taskFlag_10Hz=false;               
	  }
          if(taskFlag_1Hz==true){
                if(tran_flag==1){
                        openloop_analysis(usart1_rx,&FOC1);
			tran_flag=0;
		}
                OLED_ShowString(0,0,(uint8_t*)"Phase Sequence:",12);
                sprintf(align0,"%d",FOC1.ch_a);
                sprintf(align1,"%d",FOC1.ch_b);
                sprintf(align2,"%d",FOC1.ch_c);
                OLED_ShowString(90,0,(uint8_t*)align0,12);
                OLED_ShowString(100,0,(uint8_t*)align1,12);
                OLED_ShowString(110,0,(uint8_t*)align2,12);
                OLED_ShowString(0,10,(uint8_t*)"Location:",12); 
                OLED_ShowNum(55,10,encoder.angle_f,3,12);
                OLED_Refresh();
                taskFlag_1Hz=false;
          }
          if(taskFlag_100Hz==true){
                //Transmit_4float(FOC1.i_sample[0],FOC1.i_sample[1],FOC1.i_sample[2],FOC1.iq,&huart1);
                //Transmit_4float(FOC1.iq,iq_ref,FOC1.vq,iq_ref-FOC1.iq,&huart1);
                taskFlag_100Hz=false;
          } 
          if(taskFlag_1000Hz==true){
                Transmit_4float(FOC1.iq,iq_ref,FOC1.vq,iq_ref-FOC1.iq,&huart1);
                  taskFlag_1000Hz=false;
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV5;
  RCC_OscInitStruct.PLL.PLLN = 68;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Timer2 Interrupt
��ʱ���жϣ�1��LED��˸
		   2������������
		   3��ADC����
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
    if (htim->Instance == TIM2)
    {         
      updateMT6816(&encoder); 
      //HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Value_temp,ADC_CHANNELS);
    }
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ADC�ص�
	1������ֵת��
	2��SVPWM�������
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

*/

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc == &hadc1) {
            ADC_Value[0]=HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1)*VREF/4095.0F;
            ADC_Value[1]=HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2)*VREF/4095.0F;
            ADC_Value[2]=HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3)*VREF/4095.0F;
            if(adc_cnt<200){
                current_zero(&FOC1);
                adc_cnt++;
            }
            else if(adc_cnt==200){
                isample_flag=1; 
                   HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
                   //HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
                   HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
                   //HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_2);
                   HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
                   //HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_3);
//                   HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
                adc_cnt+=1;
                set_pwm(&FOC1,htim1,htim1.Init.Period/2,0,0);//此时设置占空比，才能在下一次进入回调时读取准确的相序学习的电流值
                return;
            }
            if(isample_flag==1){
                    DriverAlign(&FOC1);
                   
            }
            else if(isample_flag==2 && DriverAlign_flag==true){
                current_analysis(ADC_Value,&FOC1);
                open_loop(&FOC1);
                //Current_loop(&FOC1,id_ref,iq_ref);
            }

    }
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if(hadc->Instance == ADC1){
		HAL_TIM_Base_Stop_IT(&htim2);
                ADC_Value[3]=ADC_Value_temp[3]*VREF/4095.0F;
                ADC_Value[4]=ADC_Value_temp[4]*VREF/4095.0F;
//		for(uint8_t i=0;i<ADC_CHANNELS;i++){
//			ADC_Value[i]=ADC_Value_temp[i]*VREF/4095.0F;
//		}
//                if(DriverAlign_flag){
//                        //current_analysis(ADC_Value,&FOC1);
//                        //open_loop(&FOC1);
//                        //Current_loop(&FOC1,id_ref,iq_ref);
//                }
		HAL_TIM_Base_Start_IT(&htim2);
		//HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Value_temp,ADC_CHANNELS);
	}
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
���ڽ����ж�
�����ǶȲ�����vq��
		{0X01 ; factor[high] ; factor[low] ; encoder.angle_f ; FOC1.vq ; 0XAF}
			01 00 64 02 0B AF//HEX����
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
void Transmit_float(float data,uint8_t acc,UART_HandleTypeDef *huart){
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%f\n", data); 
    HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer),HAL_MAX_DELAY);
}
void Transmit_4float(float data1,float data2,float data3,float data4,UART_HandleTypeDef *huart){
    char buffer[32*3];
    snprintf(buffer, sizeof(buffer), "%f,%f,%f,%f\n", data1,data2,data3,data4); 
    HAL_UART_Transmit_DMA(huart, (uint8_t*)buffer, strlen(buffer));
}


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
