/*
 * InterruptHandler.h
 *
 *  Created on: Nov 14, 2015
 *      Author: Jendrik
 */

#ifndef INTERRUPTHANDLER_H_
#define INTERRUPTHANDLER_H_

#include "Interrupt.h"
#include <stdint.h>
extern "C" {
	#include <stdio.h>
	#include "Serval_Mqtt.h"
}

namespace periph {

template <class T> class InterruptHandler: public Interrupt {
	public:
		InterruptHandler(T* owner, void (T::*fct)(void*, unsigned long), INTERRUPT interrupt);
		InterruptHandler(T* owner, retcode_t (T::*fct)(MqttSession_T* session, MqttEvent_t event,
				const MqttEventData_t* eventData), INTERRUPT interrupt);
		virtual ~InterruptHandler();
	private:
		T* owner;
		void ISR(void*, unsigned long) override;
		void ISR(MqttSession_T* session, MqttEvent_t event,
				const MqttEventData_t* eventData) override;
		void (T::*run)(void*, unsigned long);
		retcode_t (T::*runMQTT)(MqttSession_T* session, MqttEvent_t event,
				const MqttEventData_t* eventData);
		INTERRUPT interrupt;
};

} /* namespace periph */

template<class T>
periph::InterruptHandler<T>::InterruptHandler(T* owner, void (T::*fct)(void*, unsigned long), INTERRUPT interrupt) :
	Interrupt(), owner(owner), interrupt(interrupt) {
		printf("Register func %d\r\n", interrupt);
		this->run = fct;
		printf("Fck set\r\n");
		Interrupt::Register(interrupt, this);
		printf("Ran Registered\r\n");
}

template<class T>
periph::InterruptHandler<T>::InterruptHandler(T* owner, retcode_t (T::*fct)(MqttSession_T* session, MqttEvent_t event,
		const MqttEventData_t* eventData), INTERRUPT interrupt) :
	Interrupt(), owner(owner), interrupt(interrupt) {
		printf("Register func %d\r\n", interrupt);
		this->runMQTT = fct;
		printf("Fck set\r\n");
		Interrupt::Register(interrupt, this);
		printf("Ran Registered\r\n");
}

template<class T>
inline periph::InterruptHandler<T>::~InterruptHandler() {
}

template<class T>
inline void periph::InterruptHandler<T>::ISR(void* param, unsigned long data) {
	(owner->*run)(param, data);
}

template<class T>
inline void periph::InterruptHandler<T>::ISR(MqttSession_T* session, MqttEvent_t event,
		const MqttEventData_t* eventData) {
	(owner->*runMQTT)(session, event, eventData);
}


#endif /* INTERRUPTHANDLER_H_ */
