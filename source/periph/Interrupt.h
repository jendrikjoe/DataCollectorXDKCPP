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
#ifdef  __cplusplus
extern "C" {
#endif
	//#include "Serval_Mqtt.h"
	#include "Serval_Types.h"
#ifdef  __cplusplus
}
#endif

namespace periph {

class Interrupt {
	public:
		enum INTERRUPT {
			FAULT=0,
			BUTTON1_HANDLER,
			BUTTON2_HANDLER,
			BMA280_NEW_DATA,
			AKU340_NEW_DATA,
			MQTT,
		};
		Interrupt();
		static void Register(int interrupt_number, Interrupt *intThisPtr);
		virtual void ISR(void*, unsigned long) = 0;
		virtual void ISR(MqttSession_T* session, MqttEvent_t event,
			const MqttEventData_t* eventData) = 0;
		static void Fault();
		static void Button1Handler(uint32_t data);
		static void Button2Handler(uint32_t data);
		static void AccNewDataInt();
		static void NewMicData();
		static retcode_t MQTTCallback(MqttSession_T* session, MqttEvent_t event,
				const MqttEventData_t* eventData);
		static void callISR(void* param, uint32_t data);
		virtual ~Interrupt();


	private:
		static Interrupt* ISRVectorTable[];

};

} /* namespace inter */

#endif /* INTERRUPT_H_ */
