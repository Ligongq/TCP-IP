#include "wifi.h"
#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"

// 发送数据
void send_at_command(const char* cmd) {
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);
}

// 初始化ESP8266
void esp8266_init() {
    send_at_command("AT\r\n");             // 测试通信
		HAL_Delay(1000);
    send_at_command("AT+RESTORE\r\n");     // 重置模块
			HAL_Delay(3000);
    send_at_command("AT+CWMODE=2\r\n");    // 设置AP模式
			HAL_Delay(1000);
}

// 配置AP
void setup_ap(const char* ssid, const char* password, int channel, int ecn) {
    char cmd[40];
    sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n", ssid, password, channel, ecn);
    send_at_command(cmd);
		HAL_Delay(1000);
}

// 启用多连接和启动TCP服务器
void start_tcp_server(int port) {
    send_at_command("AT+CIPMUX=1\r\n");           // 启用多连接
		HAL_Delay(1000);
    char cmd[30];
    sprintf(cmd, "AT+CIPSERVER=1,%d\r\n", port);
    send_at_command(cmd);
		HAL_Delay(1000);
}

void send_sensor_data(int link_id, float humidity, float temperature, uint32_t weight) 
{
    char data[30];
    // 格式化要发送的数据
    sprintf(data, "%.1fTEM%.1fHUM%04dWEI", humidity, temperature, weight);
    char cmd[30];
    // 准备发送数据长度命令
    sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", link_id, strlen(data));
    send_at_command(cmd);
    HAL_Delay(200);
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), HAL_MAX_DELAY);
}

