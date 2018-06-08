/*
 * alloc.h
 *
 *  Created on: May 25, 2018
 *      Author: jendrik
 */

#ifndef SOURCE_ALLOC_H_
#define SOURCE_ALLOC_H_

#include <cstdlib>

void * operator new(std::size_t n)
{
  void * const p = std::malloc(n);
  // handle p == 0
  return p;
}

void operator delete(void * p) // or delete(void *, std::size_t)
{
  std::free(p);
}

#endif /* SOURCE_ALLOC_H_ */
