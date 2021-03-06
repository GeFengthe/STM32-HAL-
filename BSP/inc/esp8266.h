#ifndef __ESP8266_H
#define __ESP8266_H
#include "stm32l4xx.h"
#include "stm32l4xx_hal_gpio.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "sys.h"
#include "delay.h"
#include "timer.h"
/*PA2 USART2_TX
  PA3 USART2_RX
  PE1 RST
  PE2 IO1


*/
#define   UART2_TX_PIN              GPIO_PIN_2

#define   UART2_RX_PIN              GPIO_PIN_3
void init_uart2(void);

extern uint16_t USART2_RX_STA;   //接收完成标志

u8 esp_8266_mode(char *mode);
u8 esp_8266_connect_wifi(char *ssid,char *pass_word);
u8 esp_8266_connect_server(char *type,char *addr,u16 port);
u8 esp_8266_passthrough(void);
void esp_8266_SendData(u8 *p,u32 len);

//uint8_t atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);  
//u8 esp_8266_sta_con(char *ssid,char *pass_word);
//u8 esp_8266_test_client(void);

#endif