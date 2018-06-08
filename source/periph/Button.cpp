/*
 * Button.cpp
 *
 *  Created on: May 25, 2018
 *      Author: jendrik
 */

#include "Button.h"
#include "Interrupt.h"

extern "C" {
	#include "BSP_BoardType.h"
	#include "BCDS_BSP_Button.h"
	#include "BCDS_CmdProcessor.h"
	CmdProcessor_T *AppCmdProcessor;
}

namespace periph {

Button::Button(Buttons button, void (*intFunction)(uint32_t)) {
    Retcode_T returnVal = RETCODE_OK;
    returnVal = BSP_Button_Connect();
    if(button == BUTTON1) this->button = BSP_XDK_BUTTON_1;
    else this->button = BSP_XDK_BUTTON_2;
    returnVal = BSP_Button_Enable(this->button, intFunction);
}

Button::~Button() {
	// TODO Auto-generated destructor stub
}

} /* namespace periph */
