#include "aht10.h"


void IIC_Delay(void)
{
    delay_us(100);
}
/**
 * @brief       IIC��ʼ������
 * 
*/
void IIC_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*  
            SCL -   PD6         SDA -   PC1
    */
   GPIO_InitStruct.Pin =GPIO_PIN_1;
   GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_PP;               //�������
   GPIO_InitStruct.Pull =GPIO_PULLUP;
   GPIO_InitStruct.Speed =GPIO_SPEED_FAST;
   HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);

   GPIO_InitStruct.Pin =GPIO_PIN_6;
   GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull =GPIO_PULLUP;
   GPIO_InitStruct.Speed =GPIO_SPEED_FAST;
   HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);

   IIC_SDA(1);
   IIC_SCL(1);
}

/**
 * @brief   ����IIC��ʼ�ź�
*/

void IIC_Start(void)
{
    SDA_OUT();              //sda�����
    IIC_SDA(1);
    IIC_SCL(1);
    IIC_Delay();
    IIC_SDA(0);                 //start:��CLK��Ϊ�ߵ�ƽ��������������IIC��ʼ�ź�
    IIC_Delay();
    IIC_SCL(0);                 //ǯסI2C���ߣ�׼�����ͻ��������
}

/**
 * @brief ����IICֹͣ�ź�
 * 
*/
void IIC_Stop(void)
{
    SDA_OUT();          //sda�����
    IIC_SDA(0);
    IIC_SCL(1);
    IIC_Delay();
    IIC_SDA(1);         //STOP ��ʱ���ź�Ϊ�ߣ������ߴӵͱ��
    IIC_Delay();
    IIC_SCL(0);         //����I2Cֹͣ�ź�
}

/**
 * @brief �ȴ�Ӧ���źŵ���
 * 
 * @return u8  1,����Ӧ��ʧ��
 *             0,����Ӧ��ɹ�
*/
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    SDA_IN();
    IIC_SDA(1);
    IIC_Delay();
    IIC_SCL(1);
    IIC_Delay();
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL(0);                 //ʱ�����0
    return 0;
}

/**
 * @brief ����ACKӦ��
*/
void IIC_Ack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(0);
    IIC_Delay();
    IIC_SCL(1);
    IIC_Delay();
    IIC_SCL(0);
}
/**
 * @brief ������ACKӦ��
 * 
*/
void IIC_NAck(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(1);
    IIC_Delay();
    IIC_SCL(1);
    IIC_Delay();
    IIC_SCL(0);
}

/**
 * @brief IIC����һ���ֽ�
 * 
 * @param txd ��Ҫ���͵�����
 * 
*/
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL(0);             //����ʱ�� ��ʼ���ݴ���
    for(t=0;t<8;t++)
    {
        IIC_SDA((txd & 0x80) >>7);       //����7λ   
        txd <<=1;                       //����1λ
        IIC_SCL(1);
        IIC_Delay();
        IIC_SCL(0);
        IIC_Delay();
    }
}

/**
 * @brief	��1���ֽ�����
 *
 * @param   ack		1������ACK		0������nACK
 *
 * @return  u8		���ض�ȡ����
 */
uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t i,receive =0;
    SDA_IN();                           //SDA����Ϊ����
    for(i =0;i<8;i++)
    {
        IIC_SCL(0);
        IIC_Delay();
        IIC_SCL(1);
        receive <<=1;
        if(READ_SDA)
        {
            receive++;
        }
        IIC_Delay();
    }
    if(!ack)
    {
        IIC_NAck();             //����nACK
    }else{
        IIC_Ack();              //����ACK
    }
    return receive;
}

/**
 * @brief	��ATH10д������
 *
 * @param   cmd		����
 * @param   data	Ҫд�������
 * @param   len		д�����ݴ�С
 *
 * @return  u8		0,����,����,�������
 */
uint8_t AHT10_Write_Data(uint8_t cmd,uint8_t *data,uint8_t len)
{
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR << 1) | 0);                       //addr+��дָ��     1��ʾ��ȡ   0��ʾд�� 
    if(IIC_Wait_Ack())                                              //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(cmd);                                             //д�Ĵ�����ַ
    IIC_Wait_Ack();

    for(uint8_t i;i<len;i++)
    {
        IIC_Send_Byte(data[i]);
        IIC_Wait_Ack();
    }
    IIC_Stop();
    return 0;
}
/**
 * @brief	��һ���ֽ�
 *
 * @param   void
 *
 * @return  u8		����������
 */
uint8_t AHT10_ReadOneByte(void)
{
    uint8_t res;
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR <<1) |0x01);                  //����������ַ+������

    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }
    res =IIC_Read_Byte(0);
    IIC_Stop();
    return res;
}
/**
 * @brief	������
 *
 * @param   data	���ݻ���
 * @param   len		�����ݴ�С
 *
 * @return  u8		0,����,����,�������
 */
uint8_t AHT10_Read_Data(uint8_t *data,uint8_t len)
{
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR) <<1|0x01);

    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    for(uint8_t i=0;i< len;i++)
    {
        if(i ==(len -1))
        {
            data[i] =IIC_Read_Byte(0);
        }else{
            data[i] =IIC_Read_Byte(1);
        }
    }
    IIC_Stop();
    return 0;
}

/**
 * @brief	��ȡ�¶�����
 *
 * @param   void
 *
 * @return  float	�¶����ݣ���λ�����϶ȣ�
 */

float AHT10_Read_Temperature(void)
{
    uint8_t res;
    uint8_t cmd[2] ={0,0};
    uint8_t temp[6];
    float cur_temp;

    res =AHT10_Write_Data(AHT10_GET_DATA,cmd,2);
    if(res) return 1;

    res =AHT10_Read_Data(temp,6);

    if(res) return 1;

    cur_temp = ((temp[3] &0xf) <<16 |temp[4]<<8 |temp[5])* 200.0/(1<<20)-50;
    return cur_temp;

}

/**
 * @brief	��ȡʪ������
 *
 * @param   void
 *
 * @return  float	ʪ�����ݣ���λ��%RH��
 */
float AHT10_Read_Humidity(void)
{
    uint8_t res =0;
    uint8_t cmd[2] ={0,0};
    uint8_t humi[6];
    float cur_humi;

    res =AHT10_Write_Data(AHT10_GET_DATA,cmd,2);                            //���Ͷ�ȡ��������

    if(res) return 1;

    res =AHT10_Read_Data(humi,6);                                       //��ȡ����
    if(res) return 1;

    cur_humi =(humi[1] << 12 | humi[2] <<4 |(humi[3] & 0xF0)) * 100.0/(1<<20);

    return cur_humi;
}

/**
 * @brief	ATH10��������ʼ��
 *
 * @param   void
 *
 * @return  u8		0,��ʼ���ɹ�������,ʧ��
 */
uint8_t AHT10_Init(void)
{
    uint8_t res;
    uint8_t temp[5] ={0,0};
    IIC_Init();

    res = AHT10_Write_Data(AHT10_NORMAL_CMD,temp, 2);

    if(res !=0) return 1;

    delay_ms(300);
    temp[0] =0x08;
    temp[1] =0x00;
    res =AHT10_Write_Data(AHT10_CALIBRATION_CMD,temp,2);

    if(res !=0) return 1;
    delay_ms(300);

    return 0;
}

