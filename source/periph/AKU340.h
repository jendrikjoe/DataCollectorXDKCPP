/*
 * AKU340.h
 *
 *  Created on: May 30, 2018
 *      Author: root
 */

#ifndef SOURCE_PERIPH_AKU340_H_
#define SOURCE_PERIPH_AKU340_H_

#include "Sensor.h"

namespace periph {

class AKU340 : public Sensor {

public:
	AKU340();
	virtual ~AKU340();
	int32_t getData();
	bool getUnitValues(SensorValues &vals) override;

protected:
	void init() override;
};
}

#endif /* SOURCE_PERIPH_AKU340_H_ */
