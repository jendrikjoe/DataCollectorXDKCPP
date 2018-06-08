/*
 * CPPBridge.h
 *
 *  Created on: May 18, 2018
 *      Author: root
 */

#ifndef SOURCE_CPPBRIDGE_H_
#define SOURCE_CPPBRIDGE_H_

#ifdef __cplusplus
extern "C" {
#endif
	#include <stdint.h>
    void bridge(void * CmdProcessorHandle);
    void readMic(void *pvParameters);
    void flushSD(void *pvParameters);
   // other functions here
#ifdef __cplusplus
}
#endif
#endif /* SOURCE_CPPBRIDGE_H_ */
