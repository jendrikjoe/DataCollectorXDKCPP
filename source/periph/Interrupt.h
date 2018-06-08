/*
 * Interrupt.h
 *
 *  Created on: Nov 13, 2015
 *      Author: Jendrik
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_
#define MAX_INTERRUPTS 10

#include <stdint.h>

namespace periph {

class Interrupt {
	public:
		enum INTERRUPT {
			FAULT=0,
			BUTTON1_HANDLER,
			BUTTON2_HANDLER,
			BMA280_NEW_DATA,
			AKU340_NEW_DATA,
		};
		Interrupt();
		static void Register(int interrupt_number, Interrupt *intThisPtr);
		virtual void ISR(void*, unsigned long) = 0;
		static void Fault();
		static void Button1Handler(uint32_t data);
		static void Button2Handler(uint32_t data);
		static void AccNewDataInt();
		static void NewMicData();
		static void callISR(void* param, uint32_t data);
		virtual ~Interrupt();


	private:
		static Interrupt* ISRVectorTable[];

};

} /* namespace inter */

#endif /* INTERRUPT_H_ */
