/*
 * BMA280.cpp
 *
 *  Created on: May 25, 2018
 *      Author: jendrik
 */

#include "BMA280.h"
#include "Interrupt.h"
extern "C" {
	#include "stdio.h"
	#include "XdkSensorHandle.h"
	#include "BCDS_SensorErrorType.h"
}

namespace periph {

BMA280::BMA280(RANGE range, BW bw) : Sensor(SENSOR::ACC),
	handle(xdkAccelerometers_BMA280_Handle), range(range), bw(bw){
	init();
}

void BMA280::init() {
	Retcode_T accelReturnValue = (Retcode_T) RETCODE_FAILURE;
	accelReturnValue = Accelerometer_init(handle);
	if (RETCODE_OK == accelReturnValue) {
		accelReturnValue = Accelerometer_setRange(
				handle, (Accelerometer_Range_E) range);
		if (RETCODE_OK != accelReturnValue) {
	    	periph::Interrupt::Fault();
			printf("Could not set BMA280 range");
		}
		accelReturnValue = Accelerometer_setBandwidth(
				handle, (Accelerometer_Bandwidth_T) bw);

		if (RETCODE_OK != accelReturnValue) {
	    	periph::Interrupt::Fault();
			printf("Could not set BMA280 bandwith");
		}
	}
	else {
    	periph::Interrupt::Fault();
		printf("BMA280 initialization failed\r\n");
	}
	printf("BMA280 ready to use\r\n");
}

void BMA280::initCallback() {
	printf("Try setting Interrupt\n");
	Retcode_T accelReturnValue = BCDS_getSensorErrorCode(
			Accelerometer_regRealTimeCallback(handle,
					ACCELEROMETER_BMA280_INTERRUPT_CHANNEL1, &Interrupt::AccNewDataInt));
	printf("Did Callback call\n");
	if (RETCODE_OK != accelReturnValue) {
    	periph::Interrupt::Fault();
		printf("Could not set BMA280 Callback\n");
	}
	accelReturnValue = BCDS_getSensorErrorCode(
			Accelerometer_configInterrupt(handle,
					ACCELEROMETER_BMA280_INTERRUPT_CHANNEL1,
					ACCELEROMETER_BMA280_NEW_DATA_INTERRUPT, NULL));
	printf("Did config Int call\n");
	if (RETCODE_OK != accelReturnValue) {
    	periph::Interrupt::Fault();
		printf("Could not set BMA280 Interrupt\n");
	}
}

bool BMA280::getUnitValues(SensorValues &values) {

	Retcode_T advancedApiRetValue = (Retcode_T) RETCODE_FAILURE;
	Accelerometer_XyzData_T getAccelDataUnit = { INT32_C(0), INT32_C(0), INT32_C(0) };
	advancedApiRetValue = Accelerometer_readXyzGValue(
	            handle, &getAccelDataUnit);
	if (RETCODE_OK != advancedApiRetValue) {
		printf("accelerometerReadXyzGValue FAILED\r\n");
		return false;
	}
	values.values[0] = getAccelDataUnit.xAxisData;
	values.values[1] = getAccelDataUnit.yAxisData;
	values.values[2] = getAccelDataUnit.zAxisData;
	values.usedVals = 3;
	return true;
}

bool BMA280::getRawValues(SensorValues &values) {
	Retcode_T advancedApiRetValue = (Retcode_T) RETCODE_FAILURE;
	Accelerometer_XyzData_T getAccelDataRaw = { INT32_C(0), INT32_C(0), INT32_C(0) };
	advancedApiRetValue = Accelerometer_readXyzLsbValue(
	            handle, &getAccelDataRaw);
	if (RETCODE_OK != advancedApiRetValue) {
		printf("accelerometerReadXyzLsbValue FAILED\r\n");
		return false;
	}
	values.values[0] = getAccelDataRaw.xAxisData;
	values.values[1] = getAccelDataRaw.yAxisData;
	values.values[2] = getAccelDataRaw.zAxisData;
	values.usedVals = 3;
	return true;
}

BMA280::~BMA280() {
    Retcode_T returnValue = RETCODE_FAILURE;
    returnValue = Accelerometer_deInit(handle);
    if (RETCODE_OK == returnValue) {
        printf("Accelerometer Deinit Success\r\n");
    }
    else {
        printf("Accelerometer Deinit Failed\r\n");
    }
}

} /* namespace periph */
