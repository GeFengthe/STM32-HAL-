#include "timer.h"
#include "esp8266.h"
TIM_HandleTypeDef TIM7_Handler;

void init_TIM7(uint16_t arr,uint16_t psc)
{
    TIM7_Handler.Instance = TIM7;
    TIM7_Handler.Init.Prescaler =psc;                   //��Ƶϵ��
    TIM7_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; //���ϼ�����
    TIM7_Handler.Init.Period =arr;                      //�Զ�װ��ֵ
    TIM7_Handler.Init.ClockDivision =TIM_CLOCKDIVISION_DIV1;    //ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM7_Handler);
    __HAL_TIM_CLEAR_FLAG(&TIM7_Handler,TIM_EventSource_Update);
    HAL_TIM_Base_Start_IT(&TIM7_Handler);                       //ʹ�ܶ�ʱ��3�Ͷ�ʱ���жϣ�TIM_IT_UPDATE
}
//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance ==TIM7)
    {
        __HAL_RCC_TIM7_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM7_IRQn,3,0);            //�����ж����ȼ�����ռ���ȼ�0�������ȼ�1
        HAL_NVIC_EnableIRQ(TIM7_IRQn);                  //����TIM7�ж�
    }
}

void TIM7_IRQHandler(void)
{
    USART2_RX_STA |=1<<15;                               //��ǽ������
    printf("TIM7 IS HANDLER\r\n");
    __HAL_TIM_CLEAR_FLAG(&TIM7_Handler,TIM_EventSource_Update);     //���TIM7�����жϱ�־
    TIM7->CR1 &=~(1<<0);                                 //�رն�ʱ��7
}