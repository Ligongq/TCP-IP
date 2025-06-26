#include "wifi.h"
#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"

// ��������
void send_at_command(const char* cmd) {
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);
}

// ��ʼ��ESP8266
void esp8266_init() {
    send_at_command("AT\r\n");             // ����ͨ��
		HAL_Delay(1000);
    send_at_command("AT+RESTORE\r\n");     // ����ģ��
			HAL_Delay(3000);
    send_at_command("AT+CWMODE=2\r\n");    // ����APģʽ
			HAL_Delay(1000);
}

// ����AP
void setup_ap(const char* ssid, const char* password, int channel, int ecn) {
    char cmd[40];
    sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n", ssid, password, channel, ecn);
    send_at_command(cmd);
		HAL_Delay(1000);
}

// ���ö����Ӻ�����TCP������
void start_tcp_server(int port) {
    send_at_command("AT+CIPMUX=1\r\n");           // ���ö�����
		HAL_Delay(1000);
    char cmd[30];
    sprintf(cmd, "AT+CIPSERVER=1,%d\r\n", port);
    send_at_command(cmd);
		HAL_Delay(1000);
}

void send_sensor_data(int link_id, float humidity, float temperature, uint32_t weight) 
{
    char data[30];
    // ��ʽ��Ҫ���͵�����
    sprintf(data, "%.1fTEM%.1fHUM%04dWEI", humidity, temperature, weight);
    char cmd[30];
    // ׼���������ݳ�������
    sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", link_id, strlen(data));
    send_at_command(cmd);
    HAL_Delay(200);
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), HAL_MAX_DELAY);
}

