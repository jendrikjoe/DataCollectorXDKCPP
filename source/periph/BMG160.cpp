/*
 * BMM150.cpp
 *
 *  Created on: May 29, 2018
 *      Author: jendrik
 */

#include "BMG160.h"
#include "Interrupt.h"
extern "C" {
	#include "stdio.h"
	#include <assert.h>
	#include "XdkSensorHandle.h"
	#include "BCDS_SensorErrorType.h"
}

namespace periph {

BMG160::BMG160(RANGE range, BW bw) : Sensor(SENSOR::GYRO),
	handle(xdkGyroscope_BMG160_Handle), range(range), bw(bw){
	init();
}

void BMG160::init() {
	Retcode_T accelReturnValue = (Retcode_T) RETCODE_FAILURE;
	accelReturnValue = Gyroscope_init(handle);
	if (RETCODE_OK == accelReturnValue) {
		accelReturnValue = Gyroscope_setRange(
				handle, (Gyroscope_Range_E) range);
		if (RETCODE_OK != accelReturnValue) {
	    	periph::Interrupt::Fault();
			printf("Could not set BMG160 range");
		}
		accelReturnValue = Gyroscope_setBandwidth(
				handle, (Gyroscope_Bandwidth_E) bw);

		if (RETCODE_OK != accelReturnValue) {
	    	periph::Interrupt::Fault();
			printf("Could not set BMG160 bandwith");
		}
	}
	else {
    	periph::Interrupt::Fault();
		printf("BMG160 initialization failed\r\n");
	}
	printf("BMG160 ready to use\r\n");
}

void BMG160::initCallback() {
	//TODO Init callback here.
	printf("Gyro Callback is not yet implemented\n");
	periph::Interrupt::Fault();
	assert(0);
}

bool BMG160::getUnitValues(SensorValues &values) {

	Retcode_T advancedApiRetValue = (Retcode_T) RETCODE_FAILURE;
	Gyroscope_XyzData_T getGyroDataUnit = { INT32_C(0), INT32_C(0), INT32_C(0) };
	advancedApiRetValue = Gyroscope_readXyzDegreeValue(
	            handle, &getGyroDataUnit);
	if (RETCODE_OK != advancedApiRetValue) {
		printf("Gyroscope_readXyzDegreeValue FAILED\r\n");
		return false;
	}
	values.values[0] = getGyroDataUnit.xAxisData;
	values.values[1] = getGyroDataUnit.yAxisData;
	values.values[2] = getGyroDataUnit.zAxisData;
	values.usedVals = 3;
	return true;
}

bool BMG160::getRawValues(SensorValues &values) {
	Retcode_T advancedApiRetValue = (Retcode_T) RETCODE_FAILURE;
	Gyroscope_XyzData_T getGyroData = { INT32_C(0), INT32_C(0), INT32_C(0) };
	advancedApiRetValue = Gyroscope_readXyzValue(
	            handle, &getGyroData);
	if (RETCODE_OK != advancedApiRetValue) {
		printf("Gyroscope_readXyzValue FAILED\r\n");
		return false;
	}
	values.values[0] = getGyroData.xAxisData;
	values.values[1] = getGyroData.yAxisData;
	values.values[2] = getGyroData.zAxisData;
	values.usedVals = 3;
	return true;
}

BMG160::~BMG160() {
    Retcode_T returnValue = RETCODE_FAILURE;
    returnValue = Gyroscope_deInit(handle);
    if (RETCODE_OK == returnValue) {
        printf("Accelerometer Deinit Success\r\n");
    }
    else {
        printf("Accelerometer Deinit Failed\r\n");
    }
}

} /* namespace periph */
