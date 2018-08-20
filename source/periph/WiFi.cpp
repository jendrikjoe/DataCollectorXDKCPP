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
#include "BCDS_NetworkConfig.h"
#include "simplelink.h"
#include "string.h"
#include "stdio.h"
}
#include "Interrupt.h"

namespace periph {

WiFi::WiFi(const char* ssid, const char* passphrase):
		session_handle(session_ptr){
	for(int i=0; i<SSID_BUFFER_LEN; i++) {
		this->ssid[i] = ssid[i];
	}
	for(int i=0; i<PASS_BUFFER_LEN; i++) {
		this->passphrase[i] = passphrase[i];
	}

	WlanConnect_Init();
	this->connectToWifi();
	// NetworkConfig_SetIpDhcp(0);
	this->fetchIP();
	PAL_initialize();
	PAL_socketMonitorInit();
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
    	config_set_target();
        config_set_connect_data();
        config_set_event_handler();
        connectMQTT();
    } else {
    	printf("Initialize Failed\n\r");
    }

}

void WiFi::connectToWifi() {
	Retcode_T retStatusConnect;
	retStatusConnect = (Retcode_T) WlanConnect_WPA((WlanConnect_SSID_T) ssid,
			(WlanConnect_PassPhrase_T) passphrase, 0);
	if (retStatusConnect == RETCODE_OK) {
		printf("Connected successfully.\n\r");
	} else {
	// Connection failed
	}
}

void WiFi::fetchIP() {
	Retcode_T retStatusSetIp;
	retStatusSetIp = NetworkConfig_SetIpDhcp(0);
	if (retStatusSetIp == RETCODE_OK) {
		NetworkConfig_IpSettings_T myIpGet;
		Retcode_T retStatusGetIp;
		retStatusGetIp = NetworkConfig_GetIpSettings(&myIpGet);
		if (retStatusGetIp == RETCODE_OK) {
			ip[0] = NetworkConfig_Ipv4Byte(myIpGet.ipV4, 3);
			ip[1] = NetworkConfig_Ipv4Byte(myIpGet.ipV4, 2);
			ip[2] = NetworkConfig_Ipv4Byte(myIpGet.ipV4, 1);
			ip[3] = NetworkConfig_Ipv4Byte(myIpGet.ipV4, 0);
			printf("The static IP was retrieved : %u.%u.%u.%u \n\r",
				(unsigned char) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 3)),
				(unsigned char) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 2)),
				(unsigned char) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 1)),
				(unsigned char) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 0)));
		} else {
			// Get IP settings failed
		}
	} else {
		// Setting Ip over DHCP failed
	}
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
	// subscribing and publishing can now be done
   // subscribe();
   // publish();
		break;
	case MQTT_CONNECTION_ERROR:
		handle_connection(eventData->connect);
		break;
	/*case MQTT_INCOMING_PUBLISH:
		handle_incoming_publish(eventData->publish);
		break;*/
	case MQTT_SUBSCRIPTION_ACKNOWLEDGED:
		printf("Subscription Successful\n\r");
		break;
	case MQTT_PUBLISHED_DATA:
		data_published = 1;
		printf("Publish Successful\n\r");
	break;
	default:
		printf("Unhandled MQTT Event: %d\n\r", event);
		break;
	}
	return RC_OK;
}

void WiFi::handle_connection(MqttConnectionEstablishedEvent_T connectionData) {
	int rc_connect = (int) connectionData.connectReturnCode;
	printf("Connection Event:\n\r\tServer Return Code: %d (0 for success)\n\r",
			(int) rc_connect);
}

/*void WiFi::handle_incoming_publish(MqttPublishData_T publishData) {
	int topic_length = publishData.topic.length + 1;
	int data_length = publishData.length + 1;
	char published_topic_buffer[topic_length];
	char published_data_buffer[data_length];
	snprintf(published_topic_buffer, topic_length, publishData.topic.start);
	snprintf(published_data_buffer, data_length, (char *) publishData.payload);
	printf("Incoming Published Message:\n\r\tTopic: %s\n\r" + "\tPayload: %s\n\r",
			published_topic_buffer, published_data_buffer);
}*/

void WiFi::config_set_target() {
	static char mqtt_broker[64];
	const char *mqtt_broker_format = "mqtt://%s:%d";
	char server_ip_buffer[13];
	Ip_Address_T ip;
	PAL_getIpaddress((uint8_t *) MQTT_BROKER_HOST, &ip);
	Ip_convertAddrToString(&ip, server_ip_buffer);
	sprintf(mqtt_broker, mqtt_broker_format, server_ip_buffer, MQTT_BROKER_PORT);
	SupportedUrl_fromString(mqtt_broker, (uint16_t) strlen(mqtt_broker), &session_ptr->target);
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
	int writerPoint = 0;
	writerPoint += sprintf(this->topic_buffer + writerPoint, this->device_name);
	writerPoint += sprintf(this->topic_buffer + writerPoint, topic);
	sprintf(this->message_buffer, message);
	static StringDescr_T pub_topic_descr;
	StringDescr_wrap(&pub_topic_descr, this->topic_buffer);
	Mqtt_publish(session_ptr, pub_topic_descr, this->message_buffer,
			strlen(this->message_buffer), MQTT_QOS_AT_MOST_ONE, false);
}

WiFi::~WiFi() {
	// TODO Auto-generated destructor stub
}
















} /* namespace periph */
