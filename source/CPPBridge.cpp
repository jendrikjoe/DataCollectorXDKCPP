/*
 * CPPBridge.cpp
 *
 *  Created on: May 18, 2018
 *      Author: jendrik
 */
#define BCDS_FEATURE_BSP_MIC_AKU340
#include "CPPBridge.h"
#include "periph/LED.h"
#include "periph/Button.h"
#include "periph/BMA280.h"
#include "periph/AKU340.h"
//#include "alloc.h"
#include "periph/Interrupt.h"
//#include "periph/SDCard.h"
#include "periph/WiFi.h"
#include "DataWriter.h"
#include "Keys.h"

#ifdef  __cplusplus
extern "C" {
#endif
	#include "FreeRTOS.h"
	#include "stdio.h"
	#include "BCDS_CmdProcessor.h"
	#include "BSP_BoardType.h"
	#include "BCDS_Accelerometer.h"
	#include "timers.h"
	#include "XdkSensorHandle.h"
	#include "simplelink.h"
 	#include "BCDS_WlanConnect.h"
#ifdef  __cplusplus
}
#endif

periph::LED red = periph::LED(periph::LED::RED);
periph::LED orange = periph::LED(periph::LED::ORANGE);
periph::LED yellow = periph::LED(periph::LED::YELLOW);
periph::InterruptHandler<periph::LED> *hand;
//periph::Button button = periph::Button(periph::Button::BUTTON1, &periph::Interrupt::Button1Handler);
periph::BMA280 *acc;
periph::AKU340 *mic;
//periph::SDCard *sd;
periph::WiFi *wifi;
io::DataWriter *accWriter;
io::DataWriter *micWriter;
char accName[100];



void readMic(void *pvParameters) {
    TickType_t xLastWakeTimeSensorRead;
    xLastWakeTimeSensorRead = xTaskGetTickCount();
    const TickType_t xSensorReadFrequency = 1/portTICK_PERIOD_MS;
	while(1) {
    	vTaskDelayUntil(&xLastWakeTimeSensorRead, xSensorReadFrequency);
        xLastWakeTimeSensorRead = xTaskGetTickCount();
		periph::Interrupt::NewMicData();
	}
}


//void flushSD(void *pvParameters) {
//    TickType_t xLastWakeTimeSensorRead;
//    xLastWakeTimeSensorRead = xTaskGetTickCount();
//    const TickType_t xSensorReadFrequency = 20/portTICK_PERIOD_MS;
//	while(1) {
//		vTaskDelayUntil(&xLastWakeTimeSensorRead, xSensorReadFrequency);
//		yellow.on();
//		sd->flushBuffer();
//		yellow.off();
//	}
//}


void flushWifi(void *pvParameters) {
	printf("Flush WiFi started\n");
    TickType_t xLastWakeTimeSensorRead;
    xLastWakeTimeSensorRead = xTaskGetTickCount();
    const TickType_t xSensorReadFrequency = 10/portTICK_PERIOD_MS;
	while(1) {
		vTaskDelayUntil(&xLastWakeTimeSensorRead, xSensorReadFrequency);
		yellow.on();
		wifi->flushBuffer();
		yellow.off();
	}
}

void bridge(void * CmdProcessorHandle) {
	BCDS_UNUSED(CmdProcessorHandle);
	printf("In bridge\r\n");
	orange.on();
	hand = new periph::InterruptHandler<periph::LED>(&red, &periph::LED::toggle, periph::Interrupt::FAULT);
	printf("Interrupt build\r\n");
	//printf("Try Building SD Card\n");
	//sd = new periph::SDCard(&orange);
	wifi = new periph::WiFi(WIFI_SSID, WIFI_PW);
	printf("Building Mac File\n");

    Retcode_T retval = WlanConnect_Init();
	_u8 macAddressVal[SL_MAC_ADDR_LEN];
	_u8 macAddressLen = SL_MAC_ADDR_LEN;
	printf("Fetch Mac Address\n");
	long int macAddress = sl_NetCfgGet(SL_MAC_ADDRESS_GET, NULL, &macAddressLen, (_u8 *)macAddressVal);
	//sd->buildFile("MAC.txt");
	char buffer[30] = {0};
	for(int i=0; i<SL_MAC_ADDR_LEN; i++) {
		sprintf(buffer+i*3, "%02x-", macAddressVal[i]);
	}
	printf("Write Mac Address\n");
	//sd->writeToFile("MAC.txt", buffer);
	printf("Try Building Mic\n");
	mic = new periph::AKU340();
	acc = new periph::BMA280(periph::BMA280::RANGE_2G, periph::BMA280::BW_125HZ);
	//sd->fillBuffer(periph::Sensor::ACC, "timestamp,BMA280_X,BMA280_Y,BMA280_Z\n");
	//sd->fillBuffer(periph::Sensor::MIC, "timestamp,Voltage\n");
	printf("Try Building DataWriter\n");
	accWriter = new io::DataWriter(acc, wifi, periph::Interrupt::BMA280_NEW_DATA, true);
	micWriter = new io::DataWriter(mic, wifi, periph::Interrupt::AKU340_NEW_DATA, true);
	printf("Try Building tesks\n");
    const char *pcTextForTask1 = "Mic Task";
    TaskHandle_t sensorTaskHandle = NULL;
    if (xTaskCreate(readMic, (const char * const) pcTextForTask1,
                        configMINIMAL_STACK_SIZE, NULL, 2, &sensorTaskHandle) != pdTRUE) {
        	printf("Could not create task %s\n", pcTextForTask1);
        	periph::Interrupt::Fault();
        	assert(0);
        }
    const char *pcTextForTask2 = "Wifi Task";
    TaskHandle_t sdTaskHandle = NULL;
    if (xTaskCreate(flushWifi, (const char * const) pcTextForTask2,
                    configMINIMAL_STACK_SIZE, NULL, 1, &sdTaskHandle) != pdTRUE) {
    	printf("Could not create task %s\n", pcTextForTask2);
    	periph::Interrupt::Fault();
    	assert(0);
    }
    acc->initCallback();
    orange.off();
}

