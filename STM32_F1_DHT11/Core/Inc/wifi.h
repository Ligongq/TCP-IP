#ifndef __WIFI_H__
#define __WIFI_H__

#include "main.h"
#include "gpio.h"
#include "stdio.h"
#include "stm32f1xx.h"
void send_at_command(const char* cmd);
void esp8266_init();
void setup_ap(const char* ssid, const char* password, int channel, int ecn);
void start_tcp_server(int port);
void send_sensor_data(int link_id, float humidity, float temperature,uint32_t weight) ;



#endif
