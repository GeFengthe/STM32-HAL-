#include "esp8266.h"
/*
WIFI :HUAWEI-2303
����: zyj15251884308

*/
#define WIFISTA_SSID                    "HUAWEI-2303"               //wifi����
#define WIFISTA_PASSWORD                "zyj15251884308"            //��������
#define WIFISTA_PORTNUM                 "8080"                      //���Ӷ˿ں�
#define WIFISTA_ENCRYPTION              "wpawpa2_aes"               //wpa/wpa2 aes ���ܷ�ʽ


uint16_t USART2_RX_STA =0;              //���ս�����־
static uint8_t USART2_TX_BUF[1024];     //����BUFF
uint8_t USART2_RX_BUF[1024];

const char sta[]="AT+CWMODE=1";         //STA ģʽ
const char ap[] ="AT+CWMODE=2";         //AP ģʽ
const char sta_ap[] ="AT+CWMODE=3";     //STA_APģʽ

//typedef struct wifi{
//    const u8 * wifista_ssid,
//    const u8 * wifista_password,
//    const u8 * wifista_portnum,
//}wifi_sta;
    
UART_HandleTypeDef         UART2_Hander;


void init_uart2(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = UART2_TX_PIN;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = UART2_RX_PIN;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    UART2_Hander.Instance =USART2;
    UART2_Hander.Init.BaudRate = 115200;
    UART2_Hander.Init.Parity =UART_PARITY_NONE;
    UART2_Hander.Init.WordLength = UART_WORDLENGTH_8B;
    UART2_Hander.Init.StopBits = UART_STOPBITS_1;
    UART2_Hander.Init.Mode = UART_MODE_TX_RX;
    UART2_Hander.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      //��Ӳ������               
    HAL_UART_Init(&UART2_Hander);
    
    __HAL_UART_CLEAR_FLAG(&UART2_Hander,UART_FLAG_RXNE);
    __HAL_UART_ENABLE_IT(&UART2_Hander,USART_IT_RXNE);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_SetPriority(USART2_IRQn,2,0);
    init_TIM7(1000,7999);                                       //100ms �ж�
    USART2_RX_STA =0;
    TIM7->CR1 &=~(1<<0);                                        //�رն�ʱ��7
}


//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	uint16_t i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
		while((USART2->ISR&0x40)==0);			//ѭ������,ֱ���������   
		USART2->TDR=USART2_TX_BUF[j];  
	} 
}

//ͨ��͸�����������������
void esp_8266_SendData(u8 *p,u32 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		while((USART2->ISR&0x40)==0);
        USART2->TDR =p[i];
	}
}

void USART2_IRQHandler(void)
{
    uint8_t res;
    if(__HAL_UART_GET_IT(&UART2_Hander,UART_IT_RXNE) !=RESET)       //���յ�����
    {
        res =USART2->RDR;
        if((USART2_RX_STA &(1<<15))==0)
        {
            if(USART2_RX_STA <1024)
            {
                TIM7->CNT =0;                                           //���������
                if(USART2_RX_STA==0)
                {
                    TIM7->CR1 |=1<<0;                                   //ʹ�ܶ�ʱ��7
                }
                USART2_RX_BUF[USART2_RX_STA++]=res;
            }else
            {
                USART2_RX_STA|=1<<15;
            }
        }
        __HAL_UART_CLEAR_IT(&UART2_Hander,UART_IT_RXNE);
    }
}

//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
uint8_t * atk_8266_check_cmd(uint8_t *src)
{
    char *strx =0;
    if(USART2_RX_STA &0x8000)
    {
        USART2_RX_BUF[USART2_RX_STA &0x7FFF] =0;            //����ַ����Ľ�����
        strx =strstr((const char *)USART2_RX_BUF,(const char *)src);       //����Ӧ��
    }
    return (uint8_t *)strx;
}

/*
CMD :ָ��
ack��Ӧ�� ��Ӧ����NULL
waittime����ʱʱ�� ��λ10ms
*/

uint8_t atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
    u8 res =0;
    USART2_RX_STA = 0;
    u2_printf("%s\r\n",cmd);                                //��������
    if(ack && waittime)
    {
        while(--waittime)
        {
            delay_ms(10);
            if(USART2_RX_STA &0x8000)                       //���յ��ڴ���Ӧ����
            {
                if(atk_8266_check_cmd((u8 *)ack))
                {
                    printf("ack:%s\r\n",(u8 *)ack);
                    break;
                }
                printf("ack is no wait----\r\n");
                USART2_RX_STA =0;
            }
        }
        if(waittime ==0)
            res =1;
    }
    return res;
}


/*
ESP8266ģʽѡ����
mode :ģʽѡ�� AP STA   AP+STA
*/
u8 esp_8266_mode(char *mode)
{
    u8 ret =0;
    u8 *p;
    p =(u8 *)mode;
    if(atk_8266_send_cmd(p,(u8 *)"OK",60))
    {
        ret =1;
    }
    if(atk_8266_send_cmd((u8*)"AT+RST",(u8*)"OK",60))               //������Ч
    {
        ret =1;
    }
    return ret;
}

//esp8266 ����wifi����
/*ssid :wifi����
pass_word:����
*/
u8 esp_8266_connect_wifi(char *ssid,char *pass_word)
{
    u8 ret =0;
    u8 *p;
    //����wifi
    sprintf((char *)p,"AT+CWJAP=\"%s\",\"%s\"",ssid,pass_word);
    if(atk_8266_send_cmd(p,(u8*)"OK",70))
    {
        ret =1;
    }
    //�رջ���
    if(atk_8266_send_cmd((u8 *)"ATE0",(u8 *)"OK",60))
    {
        ret =1;
    }
    return ret;
    
}
/*
*���ӷ�����
*tpye��TCP����UDP
*addr��ip��ַ
*port: �˿ں�
*/
u8 esp_8266_connect_server(char *type,char *addr,u16 port)
{
    u8 ret =0;
    u8 *p;
    sprintf((char *)p,"AT+CIPSTART=\"%s\",\"%s\",%d",type,addr,port);
	if(atk_8266_send_cmd(p,(u8*)"OK",100))
	{
		ret =1;
	}
	return ret;
}
//��͸��ģʽ
u8 esp_8266_passthrough(void)
{
	u8 ret=0;
	u8 *p;
	//����Ϊ͸��ģʽ
	if(atk_8266_send_cmd((u8 *)"AT+CIPMODE=1",(u8 *)"OK",70))
	{
		ret =1;
	}
	if(atk_8266_send_cmd((u8 *)"AT+CIPSEND",(u8 *)"OK",60))
	{
		ret =1;
	}
	return ret;
}



////����wifi
//u8 esp_8266_sta_con(char *ssid,char *pass_word)
//{
//    u8 ret =0;
//    u8 *p;
//    if(atk_8266_send_cmd((u8*)"AT+CWMODE=1",(u8*)"OK",100))
//    {
//        printf("AT+MODE=ERROR\r\n");
//        return ret;
//    }
//    delay_ms(1000);
//    p=(u8 *)"AT+CWJAP=\"HUAWEI-2303\",\"zyj15251884308\"";
////    sprintf((char *)p,"AT+CWJAP=\"%s\",\"%s\"",ssid,pass_word);
//    ret=atk_8266_send_cmd(p,(u8*)"OK",300);
//    return ret;
//}

//u8 esp_8266_test_client(void)
//{
//    u8 ret =0;
//    u8 *p;
////    sprintf((char *)p,"AT+CIPSTART=\"TCP\",\"192.168.3.39\",8080");
//    p=(u8*)"AT+CIPSTART=\"TCP\",\"192.168.3.39\",8080";
//    atk_8266_send_cmd(p,(u8*)"OK",300);
//    delay_ms(2000);
//    atk_8266_send_cmd((u8*)"AT+CIPSEND=10",(u8*)"OK",300);
//    delay_ms(500);
//    while(1)
//    {
//        u2_printf("%s\r\n","1234567890");
//        delay_ms(2000);
//        atk_8266_send_cmd((u8*)"AT+CIPSEND=10",(u8*)"OK",300);
//        delay_ms(2000);
//    }
//}