/*
 * mMQTT.h
 *
 *  Created on: 2022��1��24��
 *      Author: YUANLIN
 */

#ifndef APP_USER_MMQTT_H_
#define APP_USER_MMQTT_H_
#include "mqtt.h"
void MQTT_Init();
extern MQTT_Client* client_ptr;
extern bool MQTT_INIT_OK;
#endif /* APP_USER_MMQTT_H_ */
