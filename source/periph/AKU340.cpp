/*
 * AKU340.cpp
 *
 *  Created on: May 30, 2018
 *      Author: jendrik
 */

#include "AKU340.h"
extern "C" {
#include "BCDS_BSP_Mic_AKU340.h"

}

namespace periph {

AKU340::AKU340() :Sensor(Sensor::MIC){
	BSP_Mic_AKU340_Connect();
	BSP_Mic_AKU340_Enable();
}

int32_t AKU340::getData() {
	return BSP_Mic_AKU340_Sense();
}

bool AKU340::getUnitValues(SensorValues &vals) {
	vals.values[0] = getData();
	vals.usedVals = 1;
	return true;
}


void AKU340::init() {
}

AKU340::~AKU340() {
	BSP_Mic_AKU340_Disable();
	BSP_Mic_AKU340_Disconnect();
}

} /* namespace periph */
