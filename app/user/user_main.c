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
//////毫秒延时（不要延时太久）
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
//	struct station_config STA_config;	//AP参数结构体
//	struct ip_info ST_ESP8266_IP;
//
//
//	wifi_set_opmode(0x01);//设置为STA模式并保存到Flash，【由内核调用并且在任务中执行的，所以不能在函数中死循环，否则无法设置AP_config】
//
//	//设置STA模式下的IP地址【ESP8266默认开启DHCP Client，接入WIFI时会自动分配IP地址】
//	//wifi_station_dhcpc_stop();					//关闭DHCP Client
//	//IP4_DDR(&ST_ESP8266_IP.ip,192,168,8,88);		//配置IP地址
//	//IP4_DDR(&ST_ESP8266_IP.netmask,255,255,255,0);//配置子网掩码
//	//IP4_DDR(&ST_ESP8266_IP.gw,192,168,4,1);		//配置网关地址
//	//wifi_set_ip_info(STATION_IF,&ST_ESP8266_IP);	//设置STA模式下的IP地址
//
//	//结构体赋值，服务集标识符/密码需要设置为字符串形式,配置STA参数
//	os_memset(&STA_config, 0,sizeof(struct  station_config));//STA参数结构体等于0
//	os_strcpy(STA_config.ssid,ESP8266_AP_SSID);		//设置WiFi名
//	os_strcpy(STA_config.password,ESP8266_AP_PASS);	//设置WiFi密码
//
//
//	wifi_station_set_config(&STA_config);//设置STA-AP，并且保存到Flash【由内核调用并且在任务中执行的，所以不能在函数中死循环，否则无法设置AP_config】
//
//	//此API不能再user_init初始化中调用
//	//如果user_init中调用wifi_station_set_config()的话,内核会自动将ESP8266接入WIFI,无需再调用wifi_station_connect();
//	//wifi_station_connect();//ESP8266连接WiFi
//
//}
//

//OS_Timer_1必须定义位全局变量，因为ES8266内核要使用
os_timer_t OS_Timer_1; //定义软件定时器


//定义软件定时器回调函数
void ICACHE_FLASH_ATTR
OS_Timer_1_cb(void)
{
	os_printf("OS_Timer_1_cb\r\n");
//	if(client_ptr!=NULL){
//		MQTT_Publish(client_ptr, PUBLISH_TOPIC, "{\"1\":\"1\"}", strlen("{\"1\":\"1\"}"), 0, 0);
//	}
}


//参数2 = 1重复/0只执行一次回调函数
void ICACHE_FLASH_ATTR
OS_Timer_1_Init(u32 time_ms,u8 time_repetitive)
{
	//关闭定时器，一定要关闭
	os_timer_disarm(&OS_Timer_1);

	//参数1 = 要设置的定时器，参数2 = 回调函数 ，参数3 = 回调函数参数
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb,NULL);

	//使能(启动)ms定时器
	//参数1 = 要使能的定时器，参数2 = 定时时间 ，参数3 = 1重复/0只执行一次回调函数
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
	uart_init(115200,115200);//串口0，1波特率初始化
//uart_recvTask
	os_printf("\r\n----------------------------------\r\n");
	os_printf("SDK	version: %s\n",	system_get_sdk_version());//串口打印SDK版本
	uart0_sendStr("\r\nHello World\r\n");
	os_printf("\r\n----------------------------------\r\n");
	SuperSCP_default_init();
//	ESP8266_STA_Init();
//	OS_Timer_1_Init(1000,1);

	MQTT_Init();

}



