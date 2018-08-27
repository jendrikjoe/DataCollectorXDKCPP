/*
 * DataWriter.cpp
 *
 *  Created on: May 29, 2018
 *      Author: jendrik
 */

#include "DataWriter.h"

namespace io {

DataWriter::DataWriter(periph::Sensor *sensor, periph::SDCard *sd,
		periph::Interrupt::INTERRUPT interrupt, bool writeTime) :
		sensor(sensor), sd(sd), writeTime(writeTime), useWifi(false), wifi(0) {
	hand = new periph::InterruptHandler<DataWriter>(this, &DataWriter::newSensorInterrupt,
			interrupt);
}

DataWriter::DataWriter(periph::Sensor *sensor, periph::WiFi *wifi,
		periph::Interrupt::INTERRUPT interrupt, bool writeTime) :
		sensor(sensor), wifi(wifi), writeTime(writeTime), useWifi(true), sd(0) {
	hand = new periph::InterruptHandler<DataWriter>(this, &DataWriter::newSensorInterrupt,
			interrupt);
}

void DataWriter::newSensorInterrupt(void*, unsigned long int) {

	periph::SensorValues values;
	if(!sensor->getUnitValues(values)) {
		return;
	}
	char buffer[50];
	uint32_t writerPoint = 0;
	TickType_t xLastWakeTimeSensorRead = xTaskGetTickCount();
	if(writeTime) writerPoint += sprintf(buffer, "%ld", xLastWakeTimeSensorRead*portTICK_PERIOD_MS);
	for(uint32_t i=0; i<values.usedVals; i++) {
		if(!writeTime && i == 0)
			writerPoint += sprintf(buffer + writerPoint, "%ld", values.values[i]);
		else
			writerPoint += sprintf(buffer + writerPoint, ",%ld", values.values[i]);
	}
	if (!useWifi){
		writerPoint += sprintf(buffer + writerPoint, "\n");
		sd->fillBuffer(sensor->getSensorType(), buffer);
	} else {
		writerPoint += sprintf(buffer + writerPoint, ";");
//		wifi->publish(sensor->getSensorType(), buffer);
		wifi->fillBuffer(sensor->getSensorType(), buffer);
	}
}

DataWriter::~DataWriter() {
	// TODO Auto-generated destructor stub
}

} /* namespace io */
