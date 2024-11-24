/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
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
int fputc(int ch, FILE *f){
	HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint64_t WriteFlashData[3] = {0x1111222233334444,0x5555666677778888,0x9999AAAABBBBCCCC};//数据

uint32_t addr = 0x08003800;//page 7
uint32_t		Page = 7;

void PrintFlashTest(uint32_t L,uint32_t Page);
void WriteFlashTest(uint32_t L,uint64_t Data[],uint32_t Page,uint32_t addr);

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    printf("=== First Write and Read ===\n");
	WriteFlashTest(3,WriteFlashData,Page,addr);
	PrintFlashTest(3,addr);//WriteFlashData[3] = {0x1111222233334444,0x5555666677778888,0x9999AAAABBBBCCCC};//数据
    printf("\n=== Modify Data and Rewrite ===\n");
	WriteFlashData[0]=0x9999;
	WriteFlashData[1]=0x8888;
	WriteFlashData[2]=0x7777;
	WriteFlashTest(3,WriteFlashData,Page,addr);
	PrintFlashTest(3,addr);
  /* USER CODE END 2 */

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* FLASH 写入程序 */
void WriteFlashTest(uint32_t L, uint64_t Data[], uint32_t Page, uint32_t addr)
{
    uint32_t i = 0;
    uint32_t PageError = 0;

    // 1/4 解锁 FLASH
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        printf("Error: Failed to unlock FLASH.\n");
        return;
    }

    // 2/4 初始化擦除结构体并擦除指定页
    FLASH_EraseInitTypeDef FlashSet;
    FlashSet.TypeErase = FLASH_TYPEERASE_PAGES; // 擦除类型：页擦除
    FlashSet.Page = Page;                       // 页索引
    FlashSet.NbPages = 1;                       // 一次擦除 1 页

    if (HAL_FLASHEx_Erase(&FlashSet, &PageError) != HAL_OK)
    {
        printf("Error: Failed to erase FLASH. PageError: %d\n", PageError);
        HAL_FLASH_Lock();
        return;
    }
    printf("FLASH Page %u erased successfully.\n", Page);

    // 3/4 将数据写入指定地址
    for (i = 0; i < L; i++)
    {
        if (HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, addr + 8 * i, Data[i]) != HAL_OK)
        {
            printf("Error: FLASH program failed at address 0x%08X.\n", addr + 8 * i);
            HAL_FLASH_Lock();
            return;
        }
    }
    printf("FLASH programming completed successfully.\n");

    // 4/4 锁住 FLASH
    HAL_FLASH_Lock();
}


/* FLASH 读取并打印程序 */
void PrintFlashTest(uint32_t L, uint32_t addr)
{
    uint32_t i = 0;
    uint64_t data = 0;

    printf("Reading FLASH data from address 0x%08X:\n", addr);

    for (i = 0; i < L; i++)
    {
        data = *(__IO uint64_t *)(addr + i * 8); // 按 64 位读取数据
				printf("Address: 0x%08X, Data: 0x%016llX\n", addr + i * 8, data);

    }

    printf("FLASH read completed.\n");
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
