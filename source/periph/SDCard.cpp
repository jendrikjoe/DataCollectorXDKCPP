/*
 * SDCard.cpp
 *
 *  Created on: May 28, 2018
 *      Author: jendrik
 */

#include "SDCard.h"
#include "Interrupt.h"
extern "C" {
#include "FreeRTOS.h"
#include "timers.h"
#include "stdio.h"
#include "semphr.h"
#include <assert.h>
}
#include "BCDS_SDCard_Driver.h"

#define DEFAULT_LOGICAL_DRIVE ""
#define DRIVE_ZERO UINT8_C(0)
#define FORCE_MOUNT UINT8_C(1)
#define FIRST_LOCATION UINT8_C(0)

namespace periph {

static xSemaphoreHandle sdSemaphore = xSemaphoreCreateMutex();

static FIL fileObject;

SDCard::SDCard(LED *led) : led(led) {
	Retcode_T retVal = RETCODE_FAILURE;
	FRESULT FileSystemResult = FR_OK;
	static FATFS FatFileSystemObject;
	outBufferType = Sensor::NONE;
	for(int i=0; i<NUMBEROFBUFFERS; i++) {
		buffer[i] = new Buffer();
	}
	outBuffer = new Buffer();
	retVal = SDCardDriver_Initialize();
	if(retVal == RETCODE_OK) printf("SD Card initialized\n");
	else printf("SD Card initialize failed: %lu\n", retVal);
	if(SDCARD_INSERTED == SDCardDriver_GetDetectStatus()) {
		printf("Disk initialized %d\n", SDCardDriver_GetDiskStatus(DRIVE_ZERO) == SDCARD_DISK_INITIALIZED);
		retVal = SDCardDriver_DiskInitialize(DRIVE_ZERO);
		if (RETCODE_OK == retVal){
			printf("SD Card Disk initialize succeeded \n\r");
			FileSystemResult = f_mount(&FatFileSystemObject, DEFAULT_LOGICAL_DRIVE, FORCE_MOUNT);
			if(FR_OK!= FileSystemResult) printf("Mounting SD card failed \n\r");
		} else {
			printf("SD Card Disk initialize failed %lu \n\r", retVal);
	    	periph::Interrupt::Fault();
			assert(0);
		}
	} else {
		printf("No SD-Card!\n");
    	periph::Interrupt::Fault();
		assert(0);
	}
	for(int i=0; i<100; i++) {
		sprintf(filenames[Sensor::ACC], "Acc%02d.csv", i);
		if(!fileExists(filenames[Sensor::ACC])) {
			buildFile(filenames[Sensor::ACC]);
			break;
		}
	}
	for(int i=0; i<100; i++) {
		sprintf(filenames[1], "Mic%02d.csv", i);
		if(!fileExists(filenames[1])) {
			buildFile(filenames[1]);
			break;
		}
	}
}

void SDCard::buildFile(const char* filename) {
	if (RETCODE_OK == fileExists(filename)){
		deleteFile(filename);
	}
	if(FR_OK == f_open(&fileObject, filename, FA_CREATE_NEW))
		printf("File %s was created successfully \n\r", filename);
	else printf("Could not creat file %s \n\r", filename);
	FRESULT fileSystemResult = f_close(&fileObject);

}

void SDCard::deleteFile(const char* filename) {
	f_unlink(filename);
}

bool SDCard::fileExists(const char* filename) {
	if(FR_OK == f_stat(filename, NULL)) {
		printf("File %s found on SD card. \n\r", filename);
		return true;
	}
	printf("File %s does not exist. \n\r", filename);
	return false;
}

void SDCard::fillBuffer(Sensor::SENSOR sensor, const char *content) {
	uint16_t contLen = 0;
	if (buffer[sensor]->length >= BUFFERSIZE * 9 / 10) {
		led->on();
		if (!writeBuffer && xSemaphoreTake(sdSemaphore, portTickType(1)) == pdTRUE) {
			led->off();
			writeBuffer = true;
			Buffer* temp = outBuffer;
			outBuffer = buffer[sensor];
			buffer[sensor] = temp;
			outBufferType = sensor;
			if (xSemaphoreGive(sdSemaphore) != pdTRUE) {
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
	if(buffer[sensor]->length > BUFFERSIZE/5 and !writeBuffer) {
		if(xSemaphoreTake(sdSemaphore, portTickType(1)) == pdTRUE) {
			writeBuffer = true;
			Buffer* temp = outBuffer;
			outBuffer = buffer[sensor];
			buffer[sensor] = temp;
			outBufferType = sensor;
			if(xSemaphoreGive(sdSemaphore) != pdTRUE) {
		    	periph::Interrupt::Fault();
				assert(0);
			}
		}
	}
}

void SDCard::writeToFile(const char* filename, const char* str) {
	FRESULT fileSystemResult;
	// Temporary buffer for write file
	uint16_t fileSize;
	fileSystemResult = f_open(&fileObject, filename, FA_OPEN_EXISTING | FA_WRITE);
	if ((fileSystemResult != FR_OK)) printf("Could not open file %s.\n", filename);
	fileSystemResult = f_lseek(&fileObject, f_size(&fileObject));
	if ((fileSystemResult != FR_OK)) printf("Could not find end of file.\n");
	fileSize = strlen(str);
	//printf("Plan to write %d bytes\n", fileSize);
	int bytesWritten = f_printf(&fileObject, str);
	//printf("Wrote %d bytes, len: %u\n", bytesWritten, fileSize);
	if ((fileSystemResult != FR_OK))
		printf(" Error: Cannot write to file %s \n\r", filename);
	fileSystemResult = f_sync(&fileObject);
	if ((fileSystemResult != FR_OK))
		printf(" Error: Cannot sync file %s \n\r", filename);
	fileSystemResult = f_close(&fileObject);
	if ((fileSystemResult != FR_OK))
		printf(" Error: Cannot close file %s \n\r", filename);
}

void SDCard::flushBuffer() {
	if(!writeBuffer) return;
	if(xSemaphoreTake(sdSemaphore, portTickType(10000)) == pdTRUE) {
		// Temporary buffer for write file
		uint16_t fileSize;
		fileSize = outBuffer->length;
		//printf("File Size %d bytes\n", fileSize);
		writeToFile(filenames[outBufferType], outBuffer->data);
		for(int i=0; i<outBuffer->length; i++) outBuffer->data[i] = 0;
		outBuffer->length = 0;
		if(xSemaphoreGive(sdSemaphore) != pdTRUE) {
			printf("Could not free semaphore\n");
	    	periph::Interrupt::Fault();
			assert(0);
		}
		writeBuffer = false;
	}

}


SDCard::~SDCard() {
	// TODO Auto-generated destructor stub
}

} /* namespace periph */
