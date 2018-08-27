/*
 * LED.h
 *
 *  Created on: May 18, 2018
 *      Author: jendrik
 */

#ifndef SOURCE_LED_H_
#define SOURCE_LED_H_
extern "C" {
	#include "BCDS_Retcode.h"

}

#include <stdint.h>
#include "InterruptHandler.h"

namespace periph {
class LED {
public:
	enum Color {
		RED,
		ORANGE,
		YELLOW
	};
	LED(Color color);
	Retcode_T on();
	Retcode_T off();
	void toggle(void*, unsigned long);
	virtual ~LED();

private:
	uint32_t led;
};
} /* namespace periph */
#endif /* SOURCE_LED_H_ */
