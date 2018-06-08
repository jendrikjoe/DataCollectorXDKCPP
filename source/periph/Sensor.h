/*
 * Sensor.h
 *
 *  Created on: May 29, 2018
 *      Author: root
 */

#ifndef SOURCE_SENSOR_H_
#define SOURCE_SENSOR_H_

#include <stdint.h>
namespace periph {

struct SensorValues{
	int32_t values[3];
	uint32_t usedVals;
};
class Sensor {

public:
	enum SENSOR {
		ACC=0,
		MIC=1,
		GYRO=2,
		NONE=-1
	};
	Sensor(SENSOR sensor);
	SENSOR getSensorType() {return sensor;};
	virtual bool getUnitValues(SensorValues &vals) = 0;
	virtual ~Sensor();
protected:
	virtual void init() = 0;
	SENSOR sensor;
};
}

#endif /* SOURCE_SENSOR_H_ */
