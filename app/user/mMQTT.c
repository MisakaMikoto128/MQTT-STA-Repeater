/*
 * mMQTT.cpp
 *
 *  Created on: 2022��1��24��
 *      Author: YUANLIN
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//															//																		//
// ���̣�	MQTT_JX											//	ע���ڡ�esp_mqtt_proj���������޸�									//
//															//																		//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0			//	�٣���ӡ���ʵ��ע�͡�������˵�ˣ�˵���˶����ᣡ����				//
//															//																		//
// ���ܣ�	�٣�����MQTT��ز���							//	�ڣ��޸ġ�MQTT�������顿config.h -> device_id/mqtt_host/mqtt_pass	//
//															//																		//
//			�ڣ���MQTT����ˣ�������������(TCP)				//	�ۣ��޸ġ�MQTT_CLIENT_ID�궨�塿mqtt_config.h -> MQTT_CLIENT_ID		//
//															//																		//
//			�ۣ�����/���͡�CONNECT�����ģ�����MQTT�����	//	�ܣ��޸ġ�PROTOCOL_NAMEv31�꡿mqtt_config.h -> PROTOCOL_NAMEv311	//
//															//																		//
//			�ܣ���������"SW_LED"							//	�ݣ��޸ġ��������ĵķ��ͼ����mqtt.c ->	[mqtt_timer]����			//
//															//																		//
//			�ݣ�������"SW_LED"����"ESP8266_Online"			//	�ޣ��޸ġ�SNTP���������á�user_main.c -> [sntpfn]����				//
//															//																		//
//			�ޣ����ݽ��յ�"SW_LED"�������Ϣ������LED����	//	�ߣ�ע�͡��������á�user_main.c -> [user_init]����					//
//															//																		//
//			�ߣ�ÿ��һ���ӣ���MQTT����˷��͡�������		//	�ࣺ��ӡ�MQTT��Ϣ����LED��/��user_main.c -> [mqttDataCb]����		//
//															//																		//
//	�汾��	V1.1											//																		//
//															//																		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "mMQTT.h"
#include "ets_sys.h"
#include "driver/uart.h"
#include "osapi.h"
#include "mqtt.h"
#include "wifi.h"
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"
#include "sntp.h"
//==============================

// ���Ͷ���
//=================================
typedef unsigned long 		u32_t;
//=================================


// ȫ�ֱ���
//============================================================================
MQTT_Client mqttClient;			// MQTT�ͻ���_�ṹ�塾�˱����ǳ���Ҫ��

static ETSTimer sntp_timer;		// SNTP��ʱ��

bool MQTT_INIT_OK = false;
//============================================================================


// SNTP��ʱ��������ȡ��ǰ����ʱ��
//============================================================================
void sntpfn()
{
    u32_t ts = 0;

    ts = sntp_get_current_timestamp();		// ��ȡ��ǰ��ƫ��ʱ��

    os_printf("current time : %s\n", sntp_get_real_time(ts));	// ��ȡ��ʵʱ��

    if (ts == 0)		// ����ʱ���ȡʧ��
    {
        os_printf("did not get a valid time from sntp server\n");
    }
    else //(ts != 0)	// ����ʱ���ȡ�ɹ�
    {
            os_timer_disarm(&sntp_timer);	// �ر�SNTP��ʱ��

            MQTT_Connect(&mqttClient);		// ��ʼMQTT����
    }
}
//============================================================================


// WIFI����״̬�ı䣺���� = wifiStatus
//============================================================================
void wifiConnectCb(uint8_t status)
{
	// �ɹ���ȡ��IP��ַ
	//---------------------------------------------------------------------
    if(status == STATION_GOT_IP)
    {
    	ip_addr_t * addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));

    	// �ڹٷ����̵Ļ����ϣ�����2�����÷�����
    	//---------------------------------------------------------------
    	sntp_setservername(0, "us.pool.ntp.org");	// ������_0��������
    	sntp_setservername(1, "ntp.sjtu.edu.cn");	// ������_1��������

    	ipaddr_aton("210.72.145.44", addr);	// ���ʮ���� => 32λ������
    	sntp_setserver(2, addr);					// ������_2��IP��ַ��
    	os_free(addr);								// �ͷ�addr

    	sntp_init();	// SNTP��ʼ��


        // ����SNTP��ʱ��[sntp_timer]
        //-----------------------------------------------------------
        os_timer_disarm(&sntp_timer);
        os_timer_setfn(&sntp_timer, (os_timer_func_t *)sntpfn, NULL);
        os_timer_arm(&sntp_timer, 1000, 1);		// 1s��ʱ
    }

    // IP��ַ��ȡʧ��
	//----------------------------------------------------------------
    else
    {
          MQTT_Disconnect(&mqttClient);	// WIFI���ӳ���TCP�Ͽ�����
    }
}
//============================================================================
MQTT_Client* client_ptr = NULL;

// MQTT�ѳɹ����ӣ�ESP8266���͡�CONNECT���������յ���CONNACK��
//============================================================================
void mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;	// ��ȡmqttClientָ��
    client_ptr = client;
    INFO("MQTT: Connected\r\n");

    // ������2����������� / ����3������Qos��
    //-----------------------------------------------------------------
	MQTT_Subscribe(client, PUBLISH_TOPIC, 0);	// ��������"SW_LED"��QoS=0
//	MQTT_Subscribe(client, "SW_LED", 1);
//	MQTT_Subscribe(client, "SW_LED", 2);

	// ������2�������� / ����3��������Ϣ����Ч�غ� / ����4����Ч�غɳ��� / ����5������Qos / ����6��Retain��
	//-----------------------------------------------------------------------------------------------------------------------------------------
	MQTT_Publish(client, PUBLISH_TOPIC, "ESP8266_Online", strlen("ESP8266_Online"), 0, 0);	// ������"SW_LED"����"ESP8266_Online"��Qos=0��retain=0
//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 1, 0);
//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 2, 0);

	os_printf("MQTT Init OK\r\n");
	MQTT_INIT_OK = true;
}
//============================================================================

// MQTT�ɹ��Ͽ�����
//============================================================================
void mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Disconnected\r\n");
}
//============================================================================

// MQTT�ɹ�������Ϣ
//============================================================================
void mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Published\r\n");
}
//============================================================================

// ������MQTT��[PUBLISH]���ݡ�����		������1������ / ����2�����ⳤ�� / ����3����Ч�غ� / ����4����Ч�غɳ��ȡ�
//===============================================================================================================
void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len+1);		// ���롾���⡿�ռ�
    char *dataBuf  = (char*)os_zalloc(data_len+1);		// ���롾��Ч�غɡ��ռ�


    MQTT_Client* client = (MQTT_Client*)args;	// ��ȡMQTT_Clientָ��


    os_memcpy(topicBuf, topic, topic_len);	// ��������
    topicBuf[topic_len] = 0;				// �����'\0'

    os_memcpy(dataBuf, data, data_len);		// ������Ч�غ�
    dataBuf[data_len] = 0;					// �����'\0'

    INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);	// ���ڴ�ӡ�����⡿����Ч�غɡ�
    INFO("Topic_len = %d, Data_len = %d\r\n", topic_len, data_len);	// ���ڴ�ӡ�����ⳤ�ȡ�����Ч�غɳ��ȡ�


// ����С�¡����
//########################################################################################
    // ���ݽ��յ���������/��Ч�غɣ�����LED����/��
    //-----------------------------------------------------------------------------------
    if( os_strcmp(topicBuf,"SW_LED") == 0 )			// ���� == "SW_LED"
    {
    	if( os_strcmp(dataBuf,"LED_ON") == 0 )		// ��Ч�غ� == "LED_ON"
    	{
    		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);		// LED��
    	}

    	else if( os_strcmp(dataBuf,"LED_OFF") == 0 )	// ��Ч�غ� == "LED_OFF"
    	{
    		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);			// LED��
    	}
    }
//########################################################################################


    os_free(topicBuf);	// �ͷš����⡿�ռ�
    os_free(dataBuf);	// �ͷš���Ч�غɡ��ռ�
}
//===============================================================================================================


void MQTT_Init(){

	os_delay_us(60000);


	//����С�¡����
	//###########################################################################
	    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4�����	#
		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// LED��ʼ��	#
	//###########################################################################


	    CFG_Load();	// ����/����ϵͳ������WIFI������MQTT������


	    // �������Ӳ�����ֵ�������������mqtt_test_jx.mqtt.iot.gz.baidubce.com�����������Ӷ˿ڡ�1883������ȫ���͡�0��NO_TLS��
		//-------------------------------------------------------------------------------------------------------------------
		MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);

		// MQTT���Ӳ�����ֵ���ͻ��˱�ʶ����..����MQTT�û�����..����MQTT��Կ��..������������ʱ����120s��������Ự��1��clean_session��
		//----------------------------------------------------------------------------------------------------------------------------
		MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);

		// ������������(����ƶ�û�ж�Ӧ���������⣬��MQTT���ӻᱻ�ܾ�)
		//--------------------------------------------------------------
	//	MQTT_InitLWT(&mqttClient, "Will", "ESP8266_offline", 0, 0);


		// ����MQTT��غ���
		//--------------------------------------------------------------------------------------------------
		MQTT_OnConnected(&mqttClient, mqttConnectedCb);			// ���á�MQTT�ɹ����ӡ���������һ�ֵ��÷�ʽ
		MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);	// ���á�MQTT�ɹ��Ͽ�����������һ�ֵ��÷�ʽ
		MQTT_OnPublished(&mqttClient, mqttPublishedCb);			// ���á�MQTT�ɹ���������������һ�ֵ��÷�ʽ
		MQTT_OnData(&mqttClient, mqttDataCb);					// ���á�����MQTT���ݡ���������һ�ֵ��÷�ʽ


		// ����WIFI��SSID[..]��PASSWORD[..]��WIFI���ӳɹ�����[wifiConnectCb]
		//--------------------------------------------------------------------------
		WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);



		INFO("\r\nSystem started ...\r\n");

}
