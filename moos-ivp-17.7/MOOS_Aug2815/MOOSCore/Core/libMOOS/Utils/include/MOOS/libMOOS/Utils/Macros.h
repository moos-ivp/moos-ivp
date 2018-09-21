/*
 * Macros.h
 *
 *  Created on: Aug 25, 2013
 *      Author: pnewman
 */

#ifndef MOOSMACROS_H_
#define MOOSMACROS_H_

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif


#endif /* MOOSMACROS_H_ */
