#include "stm32l4xx.h"
#include "sys.h"
#include "delay.h"
#include "stm32l4xx_hal_gpio.h"
#include "lcd.h"
#include "usart.h"
#include "includes.h"
#include "esp8266.h"
void LED_Init(void);

int main()
{
    HAL_Init();
    SystemClock_Config();
    delay_init(80);
    LED_Init();
    uart_init(115200);
    init_uart2();
    delay_ms(500);
//    esp_8266_sta_con("HUAWEI-2303","zyj15251884308");
    esp_8266_test_client();
//    atk_8266_send_cmd("AT","OK",100);
//    printf("--this is hal\r\n");
//    LCD_Init();
//    printf("--this is now hal\r\n");
//    LCD_ShowChar(120,120,'A',32);
//    LCD_ShowChar(0,121,'b',32);
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
    
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET);
}



