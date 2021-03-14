#ifndef __MQTT_H
#define __MQTT_H
#include "stm32l4xx.h"
#include "sys.h"

#define     BYTE0(temp)                        *((uint8_t *)(&temp))
#define     BYTE1(temp)                        *((uint8_t *)(&temp)+1)
#define     BYTE2(temp)                        *((uint8_t *)(&temp)+2)
#define     BYTE3(temp)                        *((uint8_t *)(&temp)+3)
/*
*   ����������
//  "ProductKey": "a1HflwuvkIR",
//  "DeviceName": "iot_mq2",
//  "DeviceSecret": "eb59bb2d088a36d1fe44af6a9fd301d9"
*/



extern uint8_t MQTT_Connect_Pack(char *ClientID,char *Username,char *Password);
extern void MQTT_SendHeart(void);
extern u8 MQTT_PublishData_Pack(char *topic ,char *message, u8 qos);

#endif