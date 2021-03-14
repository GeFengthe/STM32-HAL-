#ifndef __ATH10_H
#define __ATH10_H
#include "stm32l4xx.h"
#include "delay.h"

//IO��������
#define SDA_IN()            {GPIOC->MODER &=~(3<<(1*2));GPIOC->MODER |=0<<(1*2);}               //PC1����ģʽ
#define SDA_OUT()           {GPIOC->MODER &=~(3<<(1*2));GPIOC->MODER |=1<<(1*2);}               //PC1���ģʽ

//IO����
#define IIC_SCL(n)          (n?HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET))
#define IIC_SDA(n)          (n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET))
#define READ_SDA            HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1)                  //����SDA

#define AHT10_IIC_ADDR              0x38                //AHT10 IIC��ַ

#define AHT10_CALIBRATION_CMD           0xE1                //У׼����(�ϵ�ֻ��Ҫ����һ��)
#define AHT10_NORMAL_CMD                0xA8                //��������ģʽ
#define AHT10_GET_DATA                  0xAC                //��ȡ��������

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(uint8_t ack);

extern uint8_t AHT10_Init(void);
extern float AHT10_Read_Humidity(void);
extern float AHT10_Read_Temperature(void);

#endif