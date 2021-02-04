#include "mqtt.h"
#include "sys.h"
#include "string.h"

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


/*
��������: ��¼������
��������ֵ: 0��ʾ�ɹ� 1��ʾʧ��
*/
u8 MQTT_Connect(char *ClientID,char *Username,char *Password)
{
    u8 i,j;
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
}

