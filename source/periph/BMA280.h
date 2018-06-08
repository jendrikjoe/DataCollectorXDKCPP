/*
 * BMA280.h
 *
 *  Created on: May 25, 2018
 *      Author: root
 */

#ifndef SOURCE_BMA280_H_
#define SOURCE_BMA280_H_

#include <stdint.h>
#include "Sensor.h"

extern "C" {

	#include "BCDS_Accelerometer.h"
}

namespace periph {

struct AccValues {
		int32_t x;
		int32_t y;
		int32_t z;
	};

class BMA280 : public Sensor {

public:
	enum RANGE {
		RANGE_2G = ACCELEROMETER_BMA280_RANGE_2G,/**< ID to represent bma280 range to +/- 2G mode */
		RANGE_4G = ACCELEROMETER_BMA280_RANGE_4G,/**< ID to represent bma280 range to +/- 4G mode */
		RANGE_8G = ACCELEROMETER_BMA280_RANGE_8G,/**< ID to represent bma280 range to +/- 8G mode */
		RANGE_16G = ACCELEROMETER_BMA280_RANGE_16G,/**< ID to represent bma280 range to +/- 16G mode */
	};
	enum BW {
	    BW_7_81HZ = ACCELEROMETER_BMA280_BANDWIDTH_7_81HZ, /**< ID to represent bma280 accel sensor bandwidth 7.81  HZ */
		BW_15_63HZ = ACCELEROMETER_BMA280_BANDWIDTH_15_63HZ, /**< ID to represent bma280 accel sensor bandwidth 15.63  HZ */
		BW_31_25HZ = ACCELEROMETER_BMA280_BANDWIDTH_31_25HZ, /**< ID to represent bma280 accel sensor bandwidth 31.25  HZ */
		BW_63_5HZ = ACCELEROMETER_BMA280_BANDWIDTH_62_50HZ, /**< ID to represent bma280 accel sensor bandwidth 62.50  HZ */
		BW_125HZ = ACCELEROMETER_BMA280_BANDWIDTH_125HZ, /**< ID to represent bma280 accel sensor bandwidth 125  HZ */
		BW_250HZ = ACCELEROMETER_BMA280_BANDWIDTH_250HZ, /**< ID to represent bma280 accel sensor bandwidth 250  HZ */
		BW_500HZ = ACCELEROMETER_BMA280_BANDWIDTH_500HZ, /**< ID to represent bma280 accel sensor bandwidth 500  HZ */
	};
	BMA280(RANGE range = RANGE_16G, BW bw = BW_500HZ);
	void initCallback();
	bool getUnitValues(SensorValues &values) override;
	bool getRawValues(SensorValues &values);
	virtual ~BMA280();

protected:
	void init() override;
	Accelerometer_HandlePtr_T handle;
	RANGE range;
	BW bw;
};
}

#endif /* SOURCE_BMA280_H_ */
