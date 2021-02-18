#include "stm32l4xx.h"
#include "sys.h"
#include "delay.h"
#include "stm32l4xx_hal_gpio.h"
#include "lcd.h"
#include "usart.h"
#include "includes.h"
#include "esp8266.h"

//UCOSIII���������ȼ��û�������ʹ��
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ���������� OS_IntQTask()   Ĭ�Ϲص�
//���ȼ�1: ʱ�ӽ�������OS_TickTask()
//���ȼ�2: ��ʱ����OS_TmrTask()            Ĭ��Ϊ11
//���ȼ�OS_CFG_PRIO_MAX-2: ͳ������    OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1: ��������    OS_IdleTask()
void LED_Init(void);


//�������ȼ�
#define APP_MAIN_TASK_PRIO              3

//�����ջ��С
#define APP_MAIN_TASK_SIZE              512

//������ƿ�
OS_TCB AppMainTaskTCB;

//�����ջ
CPU_STK APP_MAIN_TASK_STK[APP_MAIN_TASK_SIZE];

//������
void app_main(void *p_arg);

int main()
{
    OS_ERR err;
    CPU_SR_ALLOC();
    HAL_Init();
    SystemClock_Config();
    delay_init(80);
    LED_Init();
    uart_init(115200);
    init_uart2();
    OSInit(&err);                   //��ʼ��UCOSIII
    OS_CRITICAL_ENTER();            //�����ٽ���
//    delay_ms(500);
    OSTaskCreate((OS_TCB *)&AppMainTaskTCB,
                 (CPU_CHAR *) "app_main task",
                 (OS_TASK_PTR ) app_main,
                 (void *)       0,                                                      //���ݸ��������Ĳ���
                 (OS_PRIO)      APP_MAIN_TASK_PRIO,                                     //�������ȼ�
                 (CPU_STK *)    &APP_MAIN_TASK_STK[0],                                  //�����ջ����ַ
                 (CPU_STK_SIZE) APP_MAIN_TASK_SIZE/10,                                  //�����ջ�����λ
                 (CPU_STK_SIZE) APP_MAIN_TASK_SIZE,                                     //
                 (OS_MSG_QTY)   0,                                                      //�����ڲ���Ϣ�����ܹ����ܵ������Ŀ��Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK)      0,                                                      //��ʹ��ʱ��Ƭ��תʱ��ʱ�䣬Ƭ����Ϊ0ʱΪĬ�ϳ���
                 (void *)       0,                                                      //�û�����Ĵ洢��
                 (OS_OPT)       OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP,
                 (OS_ERR *)     &err);
      OS_CRITICAL_EXIT();                                                               //�˳��ٽ���
      OSStart(&err);                                                                    //����UCOSIII
    while(1)
    {
        
    }    
}

void app_main(void *p_arg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    p_arg =p_arg;
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



