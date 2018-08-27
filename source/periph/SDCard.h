/*
 * SDCard.h
 *
 *  Created on: May 28, 2018
 *      Author: jendrik
 */

#ifndef SOURCE_SDCARD_H_
#define SOURCE_SDCARD_H_
#define NUMBEROFBUFFERS 2

#include "Sensor.h"
#include "LED.h"
#include "Buffer.h"
extern "C" {
	#include "ff.h"
	#include "stdio.h"
}

namespace periph {


class SDCard {

public:
	SDCard(LED *led);
	void buildFile(const char* filename);
	void fillBuffer(Sensor::SENSOR sensor, const char* content);
	void writeToFile(const char* filename, const char* str);
	void flushBuffer();
	void deleteFile(const char* filename);
	bool fileExists(const char* filename);
	virtual ~SDCard();

private:
	LED *led;
	Buffer* buffer[NUMBEROFBUFFERS];
	char* filenames[NUMBEROFBUFFERS] = {new char[15], new char[15]};
	Buffer* outBuffer;
	Sensor::SENSOR outBufferType;
	volatile bool writeBuffer = false;
};

} // namespace periph

#endif /* SOURCE_SDCARD_H_ */
