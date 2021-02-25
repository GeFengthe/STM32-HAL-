#include "stm32l4xx.h"
#include "sys.h"
#include "delay.h"
#include "stm32l4xx_hal_gpio.h"
#include "lcd.h"
#include "usart.h"
#include "esp8266.h"
#include "FreeRTOS.h"
#include "task.h"
void LED_Init(void);
static void appmain(void *parameter);

//������
static TaskHandle_t AppMain_Handle=	NULL;

int main()
{
	BaseType_t xReturn =pdPASS;
//    HAL_Init();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    SystemClock_Config();
    LED_Init();
    uart_init(115200);
//    init_uart2();
	xReturn = xTaskCreate((TaskFunction_t )appmain,						//������ں���
						  (const char *)"appmain",						//��������
						  (const uint16_t)512,							//����ջ��С
						  (void *)NULL,									//������ں�������
						  (UBaseType_t) 1,								//�������ȼ�
						  (TaskHandle_t *)&AppMain_Handle);				//������
	if(pdPASS ==xReturn)
		vTaskStartScheduler();
	else
		return -1;
    while(1)
    {
        
    }    
}



void LED_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    GPIO_InitStructure.Pin=GPIO_PIN_7;
    GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE,&GPIO_InitStructure);
    
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_SET);
}

static void appmain(void *parameter)
{
    
}



