#ifndef __DHT11_H__
#define __DHT11_H__
 
/* Private includes ----------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "stdio.h"
#include "stm32f1xx.h"

/* Private define ------------------------------------------------------------*/
#define DHT11_PIN_SET   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET)                                            //  设置GPIO为高
#define DHT11_PIN_RESET HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET)                                          //  设置GPIO为低
#define DHT11_READ_IO   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)                                                          //  DHT11 GPIO定义
 

 extern	float humidity, temperature;
/* Private function prototypes -----------------------------------------------*/
void DHT11(void);
void DHT11_START(void);
unsigned char DHT11_READ_BIT(void);
unsigned char DHT11_READ_BYTE(void);
unsigned char DHT11_READ_DATA(float *humidity, float *temperature);
unsigned char DHT11_Check(void);
static void DHT11_GPIO_MODE_SET(uint8_t mode);

void Coarse_delay_us(uint32_t us);
    
#endif

