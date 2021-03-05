#include "mqtt.h"
#include "sys.h"
#include "string.h"
#include "esp8266.h"

char MQTT_ClientID[100];                    //�ͻ���ID
char MQTT_UserName[100];                    //�û���
char MQTT_PassWord[100];                    //��Կ

u8 *mqtt_rxbuf;
u8 *mqtt_txbuf;
u16 mqtt_rxlen;
u16 mqtt_txlen;
u8 _mqtt_rxbuf[256];                        //���ջ�����
u8 _mqtt_txbuf[256];                        //���ͻ�����

typedef enum{
    //����        ֵ           ������������          ����
    
    M_RESERVED1 =0  ,   //      ��ֹ                  ����
    M_CONNECT       ,   //      �ͻ��˵�������         �ͻ����������ӷ���˱���
    M_CONNACK       ,   //      ���������ͻ���         ���ӱ���Ӧ��
    M_PUBLISH       ,   //      ������������          ������Ϣ
    M_PUBACK        ,   //      ������������         QoS 1��Ϣ����ȷ��
    M_PUBREC        ,   //      ������������	    �����յ�����֤������һ����
    M_PUBREL        ,   //      ������������	    �����ͷţ���֤�����ڶ�����
    M_PUBCOMP       ,   //      ������������	    QoS 2��Ϣ������ɣ���֤������������
    M_SUBSCRIBE     ,   //      �ͻ��˵������	    �ͻ��˶�������
    M_SUBACK        ,   //      ����˵��ͻ���	    ����������ȷ��
    M_UNSUBSCRIBE   ,   //      �ͻ��˵������	    �ͻ���ȡ����������
    M_UNSUBACK      ,   //      ����˵��ͻ���	    ȡ�����ı���ȷ��
    M_PINGREQ       ,   //      �ͻ��˵������	    ��������
    M_PINGRESP      ,   //      ����˵��ͻ���	    ������Ӧ
    M_DISCONNECT    ,   //      �ͻ��˵������	    �ͻ��˶Ͽ�����
    M_RESERVED2     ,   //      ��ֹ	                ����
    
}_typdef_mqtt_message;

//���ӳɹ���������Ӧ 20 02 00 00
//�ͻ��������Ͽ����� e0 00

const u8 parket_connetAck[] = {0x20,0x02,0x00,0x00};
const u8 parket_disconnect[] = {0xe0,0x00};
const u8 parket_heart[] = {0xc0,0x00};
const u8 parket_heart_reply[] ={0xc0,0x00};
const u8 parket_subAck[] ={0x90,0x03};

/*
��������: ��ʼ���������������������ĵ�¼����
*/
 
//����
//clientId*deviceName*productKey#
// *�滻ΪDeviceName  #�滻ΪProductKey  ������Կ��DeviceSecret  ���ܷ�ʽ��HmacSHA1  
//PassWord����=clientIdmq2_iotdeviceNamemq2_iotproductKeya1WLC5GuOfx
//hmacsha1������վ��http://encode.chahuo.com/
//���ܵ���Կ��DeviceSecret


#define     BYTE0(temp)                        (*(uint8_t *)(&temp))
#define     BYTE1(temp)                        (*(uint8_t *)(&temp)+1)
#define     BYTE2(temp)                        (*(uint8_t *)(&temp)+2)
#define     BYTE3(temp)                        (*(uint8_t *)(&temp)+3)

//MQTT��������
void MQTT_SendBuf(uint8_t *buf,uint16_t len)
{
    ESP8266_ATSendBuf(buf,len);
}



/*
��������: ��¼������
��������ֵ: 1��ʾ�ɹ� 0��ʾʧ��
*/
u8 MQTT_Connect_Pack(char *ClientID,char *Username,char *Password)
{
    u8 i,j;
    u8 encodedByte;
    int ClientIDlen =strlen(ClientID);
    int Usernamelen =strlen(Username);
    int Passwordlen =strlen(Password);
    int Datalen;
    mqtt_txlen =0;
    //�ɱ䱨ͷ+Payload  ÿ���ֶΰ��������ֽڵĳ��ȱ�ʶ
    Datalen = 10+(ClientIDlen+2)+(Usernamelen+2)+(Passwordlen+2);
    //�̶���ͷ
	//���Ʊ�������
    mqtt_txbuf[mqtt_txlen++]=0x10;
    do{
        encodedByte =Datalen %128;
        Datalen =Datalen/128;
        if(Datalen >0)
            encodedByte =encodedByte |128;                  //�����Ϊ��1��ʾ���������ֽ�
        mqtt_txbuf[mqtt_txlen++] =encodedByte;
    }while(Datalen>0);
    
    //�ɱ䱨ͷ  (�����ĸ��ֶ� Э����(protocol name) Э�鼶��(protocol level) ���ӱ�־(Connect Flags) ��������(keep alive)
    //Э����
    mqtt_txbuf[mqtt_txlen++] =0;
    mqtt_txbuf[mqtt_txlen++] =4;
    mqtt_txbuf[mqtt_txlen++] ='M';
    mqtt_txbuf[mqtt_txlen++] ='Q';
    mqtt_txbuf[mqtt_txlen++] ='T';
    mqtt_txbuf[mqtt_txlen++] ='T';
    
    //Э�鼶�� ����3.1.1�汾 ��Э�鼶���ǵ�ֵ��4(0x04);
    mqtt_txbuf[mqtt_txlen++] =0x04;
    
    //���ӱ�־
    mqtt_txbuf[mqtt_txlen++] =0xc2;
    //�������� keep alive ��һ������Ϊ��λ��ʱ��������ʾΪһ��16λ���� ������ָ�ڿͻ��˷���
    mqtt_txbuf[mqtt_txlen++] =0x00;
    mqtt_txbuf[mqtt_txlen++] =0x64;         //100sһ��������
    
    //�ͻ���ID��Ч�غ��ֶ�
    mqtt_txbuf[mqtt_txlen++] =BYTE1(ClientIDlen);                       // Client ID length MSB
    mqtt_txbuf[mqtt_txlen++] =BYTE0(ClientIDlen);                       // Client ID length LSB
    memcpy(&mqtt_txbuf[mqtt_txlen],ClientID,ClientIDlen);
    mqtt_txlen +=ClientIDlen;
    
    //�û�����Ч�غ��ֶ�
    if(Usernamelen>0)
    {
        mqtt_txbuf[mqtt_txlen++] =BYTE1(Usernamelen);
        mqtt_txbuf[mqtt_txlen++] =BYTE0(Usernamelen);
        memcpy(&mqtt_txbuf[mqtt_txlen],Username,Usernamelen);
        mqtt_txlen +=Usernamelen;
    }
    //������Ч�غ��ֶ�
    if(Passwordlen>0)
    {
        mqtt_txbuf[mqtt_txlen++] =BYTE1(Passwordlen);
        mqtt_txbuf[mqtt_txlen++] =BYTE0(Passwordlen);
        memcpy(&mqtt_txbuf[mqtt_txlen],Password,Passwordlen);
        mqtt_txlen +=Passwordlen;
    }
    uint8_t cnt =2;
    uint8_t wait;
    while(cnt--)
    {
        memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
        MQTT_SendBuf(mqtt_txbuf,mqtt_txlen);
        wait=30;
        while(wait--)
        {
            if(USART2_RX_BUF[0]==parket_connetAck[0] && USART2_RX_BUF[1]==parket_connetAck[1])
            {
                return 1;
            }
            delay_ms(100);
        }
    }
    return 0;   
}

//MQTT�������ݴ������
//topic   ���� 
//message ��Ϣ
//qos     ��Ϣ�ȼ� 
u8 MQTT_PublishData_Pack(char *topic ,char *message, u8 qos)
{
    static u16 id=0;
    int topicLength =strlen(topic);
    int messageLength =strlen(message);
    int Datalen;
}


