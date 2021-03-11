#include "stm32l4xx.h"
#include "sys.h"
#include "delay.h"
#include "stm32l4xx_hal_gpio.h"
#include "lcd.h"
#include "usart.h"
#include "esp8266.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"             //ʹ���ź���Ҫ������ͷ�ļ�
#include "mqtt.h"


#define LED_R_Pin               GPIO_PIN_7
#define LED_G_Pin               GPIO_PIN_8
#define LED_B_Pin               GPIO_PIN_9

#define LED_Port                GPIOE

#define LED_R_ON                HAL_GPIO_WritePin(LED_Port,LED_R_Pin,GPIO_PIN_RESET)
#define LED_R_OFF               HAL_GPIO_WritePin(LED_Port,LED_R_Pin,GPIO_PIN_SET)


//�����Ʒ�������Ϣ
#define MQTT_BROKERADDRESS              "a16NWgAwqsz.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_PORTNUM                    1883
#define MQTT_CLIENTID                   "00001|securemode=3,signmethod=hmacsha1|"
#define MQTT_USARNAME                   "Device2&a16NWgAwqsz"
#define MQTT_PASSWORD                   "630CF42D5F5CF44F51C8889800702BD0076E813E"          //

void LED_Init(void);
//������ں���
static void appmain(void *parameter);
static void IOT_Esp8266(void *parameter);
static void IOT_Contral(void *parameter);





//������
static TaskHandle_t AppMain_Handle=	NULL;
static TaskHandle_t ESP8266_Handle= NULL;
static TaskHandle_t IOTContral_Handle =NULL;

//�ź���
SemaphoreHandle_t esp8266_Semaphore;                                    //ESP8266�ź���

int main()
{
	BaseType_t xReturn =pdPASS;
//    HAL_Init();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    SystemClock_Config();
    LED_Init();
    init_uart2();
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

//�Ƴ�ʼ������
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
    BaseType_t xReturn =pdPASS;
    taskENTER_CRITICAL();                                               //�����ٽ���
    esp8266_Semaphore =xSemaphoreCreateBinary();                        //�����ź���
    if(esp8266_Semaphore ==NULL)
    {
        printf("Semaphore error\r\n");
    }
    xReturn = xTaskCreate((TaskHandle_t )IOT_Esp8266,
                          (const char *)"ESP8266",
                          (const uint16_t)512*5,
                          (void *)NULL,
                          (UBaseType_t)3,
                          (TaskHandle_t *)&ESP8266_Handle);

    if(pdPASS !=xReturn)
    {
        return ;
    }
    xReturn = xTaskCreate((TaskHandle_t )IOT_Contral,
                          (const char *)"Contral",
                          (const uint16_t)512,
                          (void *)NULL,
                          (UBaseType_t)2,
                          (TaskHandle_t *)&IOTContral_Handle);
    if(pdPASS !=xReturn)
    {
        return ;
    }
    vTaskDelete(AppMain_Handle);
    taskEXIT_CRITICAL();
     
}

void IOT_Esp8266(void *parameter)
{
    uint32_t test =0x12345678;
    while(1)
    {
        printf("0x%x\r\n",BYTE0(test));
        printf("0x%x\r\n",BYTE1(test));
        printf("0x%x\r\n",BYTE2(test));
        printf("0x%x\r\n",BYTE3(test));
        delay_ms(59000);

    }

}

void IOT_Contral(void *parameter)
{
    static u8 cnt =0;
    while(1)
    {
        LED_R_ON;
        delay_ms(1000);
        LED_R_OFF;
        printf("IOT LED IS RUNNING\r\n");
        cnt ++;
        if(cnt ==5)
        {
            xSemaphoreGive(esp8266_Semaphore);
            printf("is give esp8266 semphor\r\n");
            cnt =0;
        }
        vTaskDelay(1000);   
    }

}
