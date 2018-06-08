/*
 * Interrupt.cpp
 *
 *  Created on: Nov 13, 2015
 *      Author: Jendrik
 */

#include "Interrupt.h"
#include <assert.h>
#include "FreeRTOS.h"
#include "timers.h"
extern "C" {
#include <stdio.h>
#include "BCDS_CmdProcessor.h"
}

namespace periph {

Interrupt* Interrupt::ISRVectorTable[MAX_INTERRUPTS];

Interrupt::Interrupt() {
}

void Interrupt::Register(int interrupt_number, Interrupt* intThisPtr)
{
	assert(interrupt_number < MAX_INTERRUPTS);
    ISRVectorTable[interrupt_number] = intThisPtr;
}

void Interrupt::Fault() {
	ISRVectorTable[FAULT]->ISR(NULL, 0);
}

void Interrupt::Button1Handler(uint32_t data) {
	ISRVectorTable[BUTTON1_HANDLER]->ISR(NULL, (unsigned long)data);
}

void Interrupt::Button2Handler(uint32_t data) {
	ISRVectorTable[BUTTON2_HANDLER]->ISR(NULL, (unsigned long)data);
}

void Interrupt::AccNewDataInt() {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if(xTimerPendFunctionCallFromISR(&callISR, NULL, BMA280_NEW_DATA, &xHigherPriorityTaskWoken) == pdPASS) {
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else {
    	Fault();
		printf("Interrupt did not work\n");
	}
	//ISRVectorTable[BMA280_NEW_DATA]->ISR((unsigned long)0);
}

void Interrupt::NewMicData() {
	ISRVectorTable[AKU340_NEW_DATA]->ISR(NULL, 0);
}

void Interrupt::callISR(void* param, uint32_t data) {
	ISRVectorTable[data]->ISR(param, data);
}

Interrupt::~Interrupt() {
	// TODO Auto-generated destructor stub
}

} /* namespace inter */



