/*
 * WiFi.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: root
 */

#include "WiFi.h"

extern "C" {
#include "Serval_Mqtt.h"
#include "PAL_initialize_ih.h"
#include "PAL_socketMonitor_ih.h"
#include "simplelink.h"
#include "string.h"
#include "stdio.h"
#include "semphr.h"
}
#include "Interrupt.h"

namespace periph {

static xSemaphoreHandle wifiSemaphore = xSemaphoreCreateMutex();

void myDhcpIpCallbackFunc(NetworkConfig_IpStatus_T returnStatus) {
	printf("Callback Called DHCP\n\r");
	if ( returnStatus == NETWORKCONFIG_IPV4_ACQUIRED) {
		printf("Callback Function : IP was acquired using DHCP\n\r");
	} else {
		printf("Callback Function : IP acquiring failed DHCP\n\r");
    }
}
retcode_t mqttEventHandler(MqttSession_T* session, MqttEvent_t event,
				const MqttEventData_t* eventData) {
	periph::Interrupt::MQTTCallback(session, event, eventData);
	return RC_OK;
}



WiFi::WiFi(const char* ssid, const char* passphrase):
		session_handle(session_ptr){
	for(int i=0; i<SSID_BUFFER_LEN; i++) {
		this->ssid[i] = ssid[i];
	}
	for(int i=0; i<PASS_BUFFER_LEN; i++) {
		this->passphrase[i] = passphrase[i];
	}
	outBufferType = Sensor::NONE;
	for(int i=0; i<NUMBEROFBUFFERS; i++) {
		buffer[i] = new Buffer();
	}
	outBuffer = new Buffer();

	WlanConnect_Init();
	NetworkConfig_SetIpDhcp(myDhcpIpCallbackFunc);
	this->connectToWifi();
	PAL_initialize();
	PAL_socketMonitorInit();
	this->fetchIP();
	_u8 macAddressVal[SL_MAC_ADDR_LEN];
	_u8 macAddressLen = SL_MAC_ADDR_LEN;

	printf("Fetch Mac Address\n");
	long int macAddress = sl_NetCfgGet(SL_MAC_ADDRESS_GET, NULL, &macAddressLen, (_u8 *)macAddressVal);

	for(int i=0; i<SL_MAC_ADDR_LEN; i++) {
		sprintf(device_name+i*3, "%02x-", macAddressVal[i]);
	}

	hand = new periph::InterruptHandler<WiFi>(this, &WiFi::event_handler, Interrupt::MQTT);
    retcode_t rc = RC_MQTT_NOT_CONNECTED;

    rc = this->initMQTT();
    if (rc == RC_OK){
    	printf("Initialized MQTT\n\r");
    	config_set_target();
        config_set_connect_data();
        printf("Set connection Data\n\r");
        config_set_event_handler();
        printf("Confgured Event Handler\n\r");
        connectMQTT();
    } else {
    	printf("Initialize Failed\n\r");
    	periph::Interrupt::Fault();
    }
    while(!data_published) {
		TickType_t xLastWakeTimeSensorRead;
		xLastWakeTimeSensorRead = xTaskGetTickCount();
		const TickType_t xSensorReadFrequency = 2000 / portTICK_PERIOD_MS;
		vTaskDelayUntil(&xLastWakeTimeSensorRead, xSensorReadFrequency);
    	printf("Waiting for server to reply\n\r");

    }

}

void WiFi::connectToWifi() {
	printf("Connect to WiFi\n");
	Retcode_T retStatusConnect;
	retStatusConnect = (Retcode_T) WlanConnect_WPA((WlanConnect_SSID_T) ssid,
			(WlanConnect_PassPhrase_T) passphrase, 0);
	if (retStatusConnect == RETCODE_OK) {
		printf("Connected successfully.\n\r");
	} else {
		printf("Connecting failed.\n\r");
    	periph::Interrupt::Fault();
	}
	printf("Done Connecting %d.\n\r", retStatusConnect == RETCODE_OK);
}

void WiFi::fetchIP() {
	Retcode_T retStatusSetIp;
	printf("Waiting for IP....\n");
	NetworkConfig_IpSettings_T myIpGet;
	Retcode_T retStatusGetIp;
	do {
		TickType_t xLastWakeTimeSensorRead;
		xLastWakeTimeSensorRead = xTaskGetTickCount();
		const TickType_t xSensorReadFrequency = 500 / portTICK_PERIOD_MS;
		vTaskDelayUntil(&xLastWakeTimeSensorRead, xSensorReadFrequency);
		retStatusGetIp = NetworkConfig_GetIpSettings(&myIpGet);
		ip[0] = NetworkConfig_Ipv4Byte(myIpGet.ipV4, 3);
		ip[1] = NetworkConfig_Ipv4Byte(myIpGet.ipV4, 2);
		ip[2] = NetworkConfig_Ipv4Byte(myIpGet.ipV4, 1);
		ip[3] = NetworkConfig_Ipv4Byte(myIpGet.ipV4, 0);
		printf("The dynamic IP was retrieved : %u.%u.%u.%u \n\r",
				(unsigned char) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 3)),
				(unsigned char) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 2)),
				(unsigned char) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 1)),
				(unsigned char) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 0)));
	} while (!myIpGet.ipV4);// Get IP settings failed
}

retcode_t WiFi::initMQTT(){
	retcode_t rc_initialize = Mqtt_initialize();
	if (rc_initialize == RC_OK) {
		session_ptr = &session;
		Mqtt_initializeInternalSession(session_ptr);
	}
	return rc_initialize;
}

retcode_t WiFi::event_handler(MqttSession_T* session, MqttEvent_t event,
		const MqttEventData_t* eventData) {
	BCDS_UNUSED(session);
	switch (event) {
	case MQTT_CONNECTION_ESTABLISHED:
		handle_connection(eventData->connect);
		data_published = 1;
		break;
	case MQTT_CONNECTION_ERROR:
		handle_connection(eventData->connect);
    	periph::Interrupt::Fault();
		break;

//	case MQTT_INCOMING_PUBLISH:
//		handle_incoming_publish(eventData->publish);
//		break;
	case MQTT_SUBSCRIPTION_ACKNOWLEDGED:
		printf("Subscription Successful\n\r");
		break;
	case MQTT_PUBLISH_SEND_FAILED:
		printf("Send Failed\n\r");
		data_published = 1;
		break;
	case MQTT_PUBLISHED_DATA:
		data_published = 1;
	break;
	default:
		printf("Unhandled MQTT Event: %d\n\r", event);
    	periph::Interrupt::Fault();
		break;
	}
	return RC_OK;
}

void WiFi::handle_connection(MqttConnectionEstablishedEvent_T connectionData) {
	int rc_connect = (int) connectionData.connectReturnCode;
	printf("Connection Event:\n\r\tServer Return Code: %d (0 for success)\n\r",
			(int) rc_connect);
}

//void WiFi::handle_incoming_publish(MqttPublishData_T publishData) {
//	int topic_length = publishData.topic.length + 1;
//	int data_length = publishData.length + 1;
//	char published_topic_buffer[topic_length];
//	char published_data_buffer[data_length];
//	snprintf(published_topic_buffer, topic_length, publishData.topic.start);
//	snprintf(published_data_buffer, data_length, (char *) publishData.payload);
//	printf("Incoming Published Message:\n\r\tTopic: %s\n\r" + "\tPayload: %s\n\r",
//			published_topic_buffer, published_data_buffer);
//}

void WiFi::config_set_target() {
	static char mqtt_broker[64];
	const char *mqtt_broker_format = "mqtt://%s:%d";
	char server_ip_buffer[16] = MQTT_HOST;
	sprintf(mqtt_broker, mqtt_broker_format, server_ip_buffer, MQTT_BROKER_PORT);
	printf("Configured broker %s\n\r", mqtt_broker);
	SupportedUrl_fromString(mqtt_broker, (uint16_t) strlen(mqtt_broker), &session_ptr->target);
	printf("Configured broker %s\n\r", mqtt_broker);
}


void WiFi::config_set_connect_data() {
	session_ptr->MQTTVersion = 3;
	session_ptr->keepAliveInterval = 100;
	session_ptr->cleanSession = true;
	session_ptr->will.haveWill = false;
	StringDescr_T device_name_descr;
	StringDescr_wrap(&device_name_descr, this->device_name);
	session_ptr->clientID=device_name_descr;
}

void WiFi::config_set_event_handler() {
	session_ptr->onMqttEvent = &periph::Interrupt::MQTTCallback;
}

retcode_t WiFi::connectMQTT() {
	retcode_t rc = RC_INVALID_STATUS;
	rc = Mqtt_connect(session_ptr);
	if (rc != RC_OK) {
		printf("Could not connect, error 0x%04x\n", rc);
	}
	return rc;
}

void WiFi::publish(const char* topic, const char* message) {
	if(data_published) {
		data_published = 0;
		int writerPoint = 0;
		writerPoint += sprintf(this->topic_buffer + writerPoint, this->device_name);
		writerPoint += sprintf(this->topic_buffer + writerPoint, topic);
		sprintf(this->message_buffer, message);
		static StringDescr_T pub_topic_descr;
		StringDescr_wrap(&pub_topic_descr, this->topic_buffer);
		Mqtt_publish(session_ptr, pub_topic_descr, this->message_buffer,
				strlen(this->message_buffer), MQTT_QOS_AT_MOST_ONE, false);
	}
}

void WiFi::publish(Sensor::SENSOR sensor, const char* message) {
	publish(topicnames[sensor], message);
}

WiFi::~WiFi() {
	// TODO Auto-generated destructor stub
}

void WiFi::fillBuffer(Sensor::SENSOR sensor, const char *content) {
	uint16_t contLen = 0;
	if (buffer[sensor]->length >= BUFFERSIZE * 9 / 10) {
		if (!writeBuffer && xSemaphoreTake(wifiSemaphore, 0) == pdTRUE) {
			writeBuffer = true;
			Buffer* temp = outBuffer;
			outBuffer = buffer[sensor];
			buffer[sensor] = temp;
			outBufferType = sensor;
			if (xSemaphoreGive(wifiSemaphore) != pdTRUE) {
				periph::Interrupt::Fault();
				assert(0);
			}
		}
		else return;
	}
	while(content[contLen] != 0) {
		buffer[sensor]->data[buffer[sensor]->length+contLen] = content[contLen];
		contLen++;
	}
	buffer[sensor]->length += contLen;
	buffer[sensor]->length = buffer[sensor]->length >= BUFFERSIZE ? BUFFERSIZE-1 : buffer[sensor]->length;
	if(buffer[sensor]->length > 4*BUFFERSIZE/5 and !writeBuffer) {
		if(xSemaphoreTake(wifiSemaphore, 0) == pdTRUE) {
			writeBuffer = true;
			Buffer* temp = outBuffer;
			outBuffer = buffer[sensor];
			buffer[sensor] = temp;
			outBufferType = sensor;
			if(xSemaphoreGive(wifiSemaphore) != pdTRUE) {
		    	periph::Interrupt::Fault();
				assert(0);
			}
		}
	}
}

void WiFi::flushBuffer() {
	if(!writeBuffer) return;
	if(xSemaphoreTake(wifiSemaphore, portTickType(10000)) == pdTRUE) {
		uint16_t fileSize;
		fileSize = outBuffer->length;
		publish(topicnames[outBufferType], outBuffer->data);
		for(int i=0; i<outBuffer->length; i++) outBuffer->data[i] = 0;
		outBuffer->length = 0;
		if(xSemaphoreGive(wifiSemaphore) != pdTRUE) {
			printf("Could not free semaphore\n");
	    	periph::Interrupt::Fault();
			assert(0);
		}
		writeBuffer = false;
	}

}




} /* namespace periph */
