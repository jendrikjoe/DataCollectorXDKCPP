/*
 * DataWriter.h
 *
 *  Created on: May 29, 2018
 *      Author: jendrik
 */

#ifndef SOURCE_DATAWRITER_H_
#define SOURCE_DATAWRITER_H_

#include "periph/SDCard.h"
#include "periph/Sensor.h"
#include "periph/InterruptHandler.h"
#include "periph/Interrupt.h"
#include "FreeRTOS.h"
#include "timers.h"

namespace io {
class DataWriter {
public:
	DataWriter(periph::Sensor *sensor, periph::SDCard *sd,
			periph::Interrupt::INTERRUPT interrupt, bool writeTime);
	void newSensorInterrupt(void*, unsigned long data);
	virtual ~DataWriter();

private:
	bool writeTime;
	periph::Sensor *sensor;
	periph::SDCard *sd;
	periph::InterruptHandler<DataWriter> *hand;
};
}

#endif /* SOURCE_DATAWRITER_H_ */
