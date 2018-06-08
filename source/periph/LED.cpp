/*
 * LED.cpp
 *
 *  Created on: May 18, 2018
 *      Author: jendrik
 */

#include "LED.h"

extern "C" {
	#include "BCDS_BSP_LED.h"
	#include "BSP_BoardType.h"
	#include "BCDS_BSP_LED.h"
	#include "stdio.h"
}

namespace periph {

LED::LED(Color color) {
	if (color == RED) led = (uint32_t) BSP_XDK_LED_R;
	else if (color == ORANGE) led = (uint32_t) BSP_XDK_LED_O;
	else if (color == YELLOW) led = (uint32_t) BSP_XDK_LED_Y;
	BSP_LED_Connect();
	Retcode_T returnVal = BSP_LED_Enable(led);
	if (RETCODE_OK == returnVal) {
		printf("LED Initialization succeed without error %u \n", (unsigned int) returnVal);
	}
}

Retcode_T LED::on() {
	return BSP_LED_Switch(led, (uint32_t) BSP_LED_COMMAND_ON);
}

Retcode_T LED::off() {
	return BSP_LED_Switch(led, (uint32_t) BSP_LED_COMMAND_OFF);
}

void LED::toggle(void* d, unsigned long data) {
	switch (data) {
		case 0:
				this->on();
				break;
		case BSP_XDK_BUTTON_PRESS:
			this->off();
			break;
		case BSP_XDK_BUTTON_RELEASE:
			this->on();
			break;
	}
}

LED::~LED() {
	// TODO Auto-generated destructor stub
}

} /* namespace periph */
