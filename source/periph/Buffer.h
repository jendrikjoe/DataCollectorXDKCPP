/*
 * Buffer.h
 *
 *  Created on: Aug 22, 2018
 *      Author: root
 */

#ifndef SOURCE_PERIPH_BUFFER_H_
#define SOURCE_PERIPH_BUFFER_H_

#define BUFFERSIZE 1000
struct Buffer {
	const char* name;
	char data[BUFFERSIZE] = {0};
	unsigned int length=0;
};

#endif /* SOURCE_PERIPH_BUFFER_H_ */
