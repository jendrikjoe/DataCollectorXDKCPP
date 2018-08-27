/*
 * WiFi.h
 *
 *  Created on: Aug 15, 2018
 *      Author: root
 */

#ifndef SOURCE_PERIPH_WIFI_H_
#define SOURCE_PERIPH_WIFI_H_


#ifdef  __cplusplus
extern "C" {
#endif
#include "Serval_Mqtt.h"
#include "Serval_Types.h"
#include "BCDS_WlanConnect.h"
#include "BCDS_NetworkConfig.h"
#ifdef  __cplusplus
}
#endif
#include "InterruptHandler.h"
#include "Sensor.h"
#include "Buffer.h"
#include "Keys.h"
#define MQTT_BROKER_PORT 1883
#define SSID_BUFFER_LEN 20
#define PASS_BUFFER_LEN 20
#define MSG_BUFFER_LEN 1200
#define TOPIC_BUFFER_LEN 100
#define NUMBEROFBUFFERS 2
extern "C" {
	static MqttSession_T session;
	static MqttSession_T *session_ptr = &session;
}
namespace periph {



class WiFi {
public:
	WiFi(const char* ssid, const char* passphrase);
	virtual ~WiFi();
	void publish(const char* topic, const char* message);
	void publish(Sensor::SENSOR sensor, const char* message);
	void flushBuffer();
	void fillBuffer(Sensor::SENSOR sensor, const char *content);
private:
	void connectToWifi();
	void fetchIP();
	retcode_t initMQTT();
	retcode_t event_handler(MqttSession_T* session, MqttEvent_t event,
			const MqttEventData_t* eventData);
	static void handle_connection(MqttConnectionEstablishedEvent_T connectionData);
	//void handle_incoming_publish(MqttPublishData_T publishData);
	void config_set_connect_data();
	InterruptHandler<WiFi> *hand;
	void config_set_target();
	void config_set_event_handler();
	retcode_t connectMQTT();
	MqttSession_T* session_handle;
	volatile bool data_published = 0;
	unsigned char ip[4] = {0,0,0,0};
	char* message_buffer = new char[MSG_BUFFER_LEN];
	char topic_buffer[TOPIC_BUFFER_LEN] = {0};
	char ssid[SSID_BUFFER_LEN] = {0};
	char passphrase[PASS_BUFFER_LEN] = {0};
	char device_name[30] = {0};


	Buffer* buffer[NUMBEROFBUFFERS];
	char* topicnames[NUMBEROFBUFFERS] = {"/acc", "/mic"};
	Buffer* outBuffer;
	Sensor::SENSOR outBufferType;
	volatile bool writeBuffer = false;

};
}

#endif /* SOURCE_PERIPH_WIFI_H_ */
