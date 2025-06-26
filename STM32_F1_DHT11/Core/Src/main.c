
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dht11.h"
#include "stdio.h"
#include "string.h"
#include "wifi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint32_t weight_first = 0;//首次质量
uint32_t weight_real = 0;//每次读取的实际值
uint32_t weight_print = 0; 
uint32_t get_hx711_ad(void);//读取AD值函数

void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
char rxtran;
#define BUFFER_SIZE 30  // 根据需要可以调整缓冲区大小
uint8_t buffer[BUFFER_SIZE];
uint16_t buffer_index = 0;
char data[40];

uint16_t tim_cnt=0;
uint8_t tim_count=1;
uint8_t send_flag=0;
int value=1;
uint32_t weight=0;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int ch, FILE *p);
uint32_t ReadCount(void);
uint32_t WeightReal(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{


  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	printf("all ok \r\n");
	esp8266_init();
  setup_ap("ATK66", "12345678", 5, 3);
  start_tcp_server(8086);
	HAL_UART_Receive_IT(&huart2,(uint8_t *)&rxtran, 1);
//	HAL_TIM_Base_Start_IT(&htim2);
	HAL_Delay(1000);
	weight_first = ReadCount();//首次读取重量进行去皮操作
  printf("weight_first=%d\n",weight_first);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		weight_print = WeightReal();//实际质量计算
		DHT11();
		HAL_Delay(value*1000);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

	uint32_t ReadCount(void)
	{
		uint32_t Count = 0;
		uint8_t i = 0;
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1));//判断AD转换器是否准备好（DOUT应为低电平）
		for(i = 0; i <24; i++)//24
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			Count = Count << 1;		
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1))
			{
					Count ++;
			}
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);			
		}
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		Count = Count ^ 0x800000;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		return Count;
	}
	uint32_t WeightReal(void)
	{
		 weight_real = ReadCount();//实际质量计算
			if(weight_first>=weight_real)
			{
				weight_real = weight_first-weight_real;
				weight_real = weight_real*100/37074;
				//printf("重量为：%dg\n",weight_real);
			}
			else
			{
				weight_real = weight_real-weight_first;
				weight_real = weight_real*100/37074;
				//printf("重量为：-%dg\n",weight_real);
			}
			return weight_real;
	}

int fputc(int ch, FILE *p)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,HAL_MAX_DELAY);
	return ch;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   if (huart->Instance == USART2) {
        // Continue receiving data
        HAL_UART_Receive_IT(&huart2, (uint8_t *)&rxtran, 1); 

        // Store received data into buffer
        buffer[buffer_index++] = rxtran;

        // Check if the received character is a newline or if the buffer is full
        if (rxtran == '\n' || buffer_index == BUFFER_SIZE) {
            buffer[buffer_index] = '\0';  // Null-terminate for string operations

            printf("%s\r\n", buffer);  // Print the entire buffer content

            // Parse the specific format of data looking for a number after a colon
            char *ptr = strchr((char *)buffer, ':');
            if (ptr) {
                int extracted_value;
                if (sscanf(ptr + 1, "%d", &value) == 1) {
                    printf("Received value: %d\r\n", value);
                }
            }

            buffer_index = 0;  // Reset buffer index
        }
    }
//    if (huart->Instance == USART2) {
//        // 将接收到的数据存储到buffer中
//        buffer[buffer_index++] = rxtran;// 假设每次只接收一个字节
//         HAL_UART_Receive_IT(&huart2, (uint8_t *)&rxtran, 1); // 继续接收下一个字节

//        // 如果接收到换行符或者缓冲区满，发送数据
//        if (buffer[buffer_index-1] == '\n' || buffer_index == BUFFER_SIZE) {
//            HAL_UART_Transmit(&huart1, buffer, buffer_index, HAL_MAX_DELAY);  // 通过USART2发送数据
//            buffer_index = 0;  // 重置缓冲区索引
//        }
//    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//10ms进一次
{
	tim_cnt++;
	if(tim_cnt==(100*value))//1s
	{
		
		
		tim_cnt=0;
	}
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
