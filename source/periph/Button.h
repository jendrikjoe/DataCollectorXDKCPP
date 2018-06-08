/*
 * Button.h
 *
 *  Created on: May 25, 2018
 *      Author: jendrik
 */

#ifndef SOURCE_BUTTON_H_
#define SOURCE_BUTTON_H_

#include <stdint.h>
namespace periph {
class Button {

public:
	enum Buttons {
		BUTTON1,
		BUTTON2
	};
	Button(Buttons button, void (*intFunction)(uint32_t));
	virtual ~Button();

private:
	uint32_t button;
};
}
#endif /* SOURCE_BUTTON_H_ */
