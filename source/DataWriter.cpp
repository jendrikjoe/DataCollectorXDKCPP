/*
 * DataWriter.cpp
 *
 *  Created on: May 29, 2018
 *      Author: root
 */

#include "DataWriter.h"

namespace io {

DataWriter::DataWriter(periph::Sensor *sensor, periph::SDCard *sd,
		periph::Interrupt::INTERRUPT interrupt, bool writeTime) :
		sensor(sensor), sd(sd), writeTime(writeTime) {
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
	writerPoint += sprintf(buffer + writerPoint, "\n");
	sd->fillBuffer(sensor->getSensorType(), buffer);

}

DataWriter::~DataWriter() {
	// TODO Auto-generated destructor stub
}

} /* namespace io */
