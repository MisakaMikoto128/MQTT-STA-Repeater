/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "SuperSCP.h"
/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;

            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;

            break;

        case FLASH_SIZE_16M_MAP_512_512:
            rf_cal_sec = 512 - 5;

            break;
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;

            break;

        case FLASH_SIZE_32M_MAP_512_512:
            rf_cal_sec = 1024 - 5;

            break;
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;

            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;

            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;

            break;
        default:
            rf_cal_sec = 0;

            break;
    }

    return rf_cal_sec;
}



void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{
}


/*---------------USER BEGIN-----------------*/

#include "SuperSCP.h"
#include <stdio.h>
#include <string.h>
#include "mMQTT.h"


#define BUFFER_SIZE 1024
__implemented byte __send_data_super_scp[BUFFER_SIZE];
__implemented byte __recv_data_super_scp[BUFFER_SIZE];

__implemented void SuperSCPRecvCallback(byte *data, int len)
{
	int i=0;
	if(MQTT_INIT_OK==true){
		os_printf("OK\r\n");
		//test mqtt publish to android client
		//MQTT_Publish(client_ptr, PUBLISH_TOPIC, "{\"1\":\"1\"}", strlen("{\"1\":\"1\"}"), 0, 0);
		MQTT_Publish(client_ptr, PUBLISH_TOPIC, data, len, 0, 0);
	}else{
		os_printf("Busy\r\n");
	}


}
__implemented byte SuperSCP_putchar(byte c)
{
    return c;
}

__implemented void SuperSCPErrorCallback(SuperSCPERROR error_code){
	os_printf("The sent message was too long. The maximum payload was 1023 bytes.\r\n");
}
//
//////������ʱ����Ҫ��ʱ̫�ã�
////void ICACHE_FLASH_ATTR delay_ms(int time)
////{
////	for(;time>0;time--)
////	{os_delay_us(1000);}
////}
//
//#define ESP8266_AP_SSID "HUAWEI-5G"
//#define ESP8266_AP_PASS "64456445"
//void ICACHE_FLASH_ATTR
//ESP8266_STA_Init()
//{
//	struct station_config STA_config;	//AP�����ṹ��
//	struct ip_info ST_ESP8266_IP;
//
//
//	wifi_set_opmode(0x01);//����ΪSTAģʽ�����浽Flash�������ں˵��ò�����������ִ�еģ����Բ����ں�������ѭ���������޷�����AP_config��
//
//	//����STAģʽ�µ�IP��ַ��ESP8266Ĭ�Ͽ���DHCP Client������WIFIʱ���Զ�����IP��ַ��
//	//wifi_station_dhcpc_stop();					//�ر�DHCP Client
//	//IP4_DDR(&ST_ESP8266_IP.ip,192,168,8,88);		//����IP��ַ
//	//IP4_DDR(&ST_ESP8266_IP.netmask,255,255,255,0);//������������
//	//IP4_DDR(&ST_ESP8266_IP.gw,192,168,4,1);		//�������ص�ַ
//	//wifi_set_ip_info(STATION_IF,&ST_ESP8266_IP);	//����STAģʽ�µ�IP��ַ
//
//	//�ṹ�帳ֵ�����񼯱�ʶ��/������Ҫ����Ϊ�ַ�����ʽ,����STA����
//	os_memset(&STA_config, 0,sizeof(struct  station_config));//STA�����ṹ�����0
//	os_strcpy(STA_config.ssid,ESP8266_AP_SSID);		//����WiFi��
//	os_strcpy(STA_config.password,ESP8266_AP_PASS);	//����WiFi����
//
//
//	wifi_station_set_config(&STA_config);//����STA-AP�����ұ��浽Flash�����ں˵��ò�����������ִ�еģ����Բ����ں�������ѭ���������޷�����AP_config��
//
//	//��API������user_init��ʼ���е���
//	//���user_init�е���wifi_station_set_config()�Ļ�,�ں˻��Զ���ESP8266����WIFI,�����ٵ���wifi_station_connect();
//	//wifi_station_connect();//ESP8266����WiFi
//
//}
//

//OS_Timer_1���붨��λȫ�ֱ�������ΪES8266�ں�Ҫʹ��
os_timer_t OS_Timer_1; //���������ʱ��


//���������ʱ���ص�����
void ICACHE_FLASH_ATTR
OS_Timer_1_cb(void)
{
	os_printf("OS_Timer_1_cb\r\n");
//	if(client_ptr!=NULL){
//		MQTT_Publish(client_ptr, PUBLISH_TOPIC, "{\"1\":\"1\"}", strlen("{\"1\":\"1\"}"), 0, 0);
//	}
}


//����2 = 1�ظ�/0ִֻ��һ�λص�����
void ICACHE_FLASH_ATTR
OS_Timer_1_Init(u32 time_ms,u8 time_repetitive)
{
	//�رն�ʱ����һ��Ҫ�ر�
	os_timer_disarm(&OS_Timer_1);

	//����1 = Ҫ���õĶ�ʱ��������2 = �ص����� ������3 = �ص���������
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb,NULL);

	//ʹ��(����)ms��ʱ��
	//����1 = Ҫʹ�ܵĶ�ʱ��������2 = ��ʱʱ�� ������3 = 1�ظ�/0ִֻ��һ�λص�����
	os_timer_arm(&OS_Timer_1,time_ms,time_repetitive);

}
#include "mMQTT.h"

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_init(void)
{
	uart_init(115200,115200);//����0��1�����ʳ�ʼ��
//uart_recvTask
	os_printf("\r\n----------------------------------\r\n");
	os_printf("SDK	version: %s\n",	system_get_sdk_version());//���ڴ�ӡSDK�汾
	uart0_sendStr("\r\nHello World\r\n");
	os_printf("\r\n----------------------------------\r\n");
	SuperSCP_default_init();
//	ESP8266_STA_Init();
//	OS_Timer_1_Init(1000,1);

	MQTT_Init();

}



