/*
 * BMM150.h
 *
 *  Created on: May 29, 2018
 *      Author: jendrik
 */

#ifndef SOURCE_BMG160_H_
#define SOURCE_BMG160_H_

#include "Sensor.h"
extern "C" {
	#include "BCDS_Gyroscope.h"
}

namespace periph {

class BMG160 : public Sensor {
public:
	enum DATARATE {

	};
	enum RANGE {
	    RANGE_125S = GYROSCOPE_BMG160_RANGE_125s, /**< set to 0.0625 deg/s in 125 deg/s range */
		RANGE_250S = GYROSCOPE_BMG160_RANGE_250s, /**< set to 0.125 deg/s in 250 deg/s range */
		RANGE_500S = GYROSCOPE_BMG160_RANGE_500s, /**< set to 0.25 deg/s in 500 deg/s range */
		RANGE_1000S = GYROSCOPE_BMG160_RANGE_1000s,/**< set to 0.5 deg/s in 1000 deg/s range */
		RANGE_2000S = GYROSCOPE_BMG160_RANGE_2000s, /**< set to 1 deg/s in 2000 deg/s range */
	};
	enum BW {
	    BW_12HZ = GYROSCOPE_BMG160_BANDWIDTH_12HZ, /**< sets bandwidth to LowPass 12 HZ */
		BW_23HZ = GYROSCOPE_BMG160_BANDWIDTH_23HZ, /**< sets bandwidth to LowPass 23 HZ */
		BW_32HZ = GYROSCOPE_BMG160_BANDWIDTH_32HZ, /**< sets bandwidth to LowPass 32 HZ */
		BW_47HZ = GYROSCOPE_BMG160_BANDWIDTH_47HZ, /**< sets bandwidth to LowPass 47 HZ */
		BW_64HZ = GYROSCOPE_BMG160_BANDWIDTH_64HZ, /**< sets bandwidth to LowPass 64 HZ */
		BW_116HZ = GYROSCOPE_BMG160_BANDWIDTH_116HZ, /**< sets bandwidth to LowPass 116 HZ */
		BW_230HZ = GYROSCOPE_BMG160_BANDWIDTH_230HZ, /**< sets bandwidth to LowPass 230 HZ */
		BW_523HZ = GYROSCOPE_BMG160_BANDWIDTH_523HZ, /**< sets bandwidth to LowPass 523  HZ */
	};

	BMG160(RANGE range = RANGE_250S, BW bw = BW_523HZ);
	void initCallback();
	bool getUnitValues(SensorValues &values) override;
	bool getRawValues(SensorValues &values);
	virtual ~BMG160();

protected:
	void init() override;
	Gyroscope_HandlePtr_T handle;
	RANGE range;
	BW bw;
};
}

#endif /* SOURCE_BMG160_H_ */
