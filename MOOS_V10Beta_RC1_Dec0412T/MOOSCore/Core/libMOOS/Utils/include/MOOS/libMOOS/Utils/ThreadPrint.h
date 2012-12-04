/*
 * ThreadPrint.h
 *
 *  Created on: Dec 31, 2011
 *      Author: pnewman
 */

#ifndef THREADPRINT_H_
#define THREADPRINT_H_

/*
 *
 */
#include <iostream>
#include <string>

namespace MOOS {

class ThreadPrint {
public:
	ThreadPrint(std::ostream & ofs);
	virtual ~ThreadPrint();

	enum color_t
	{
		RED,
		GREEN,
		MAGENTA,
		CYAN,
		YELLOW,
		NONE,
	};

	/**
	 * Print a formatted message to std::cerr in a threadsafe way.Prints a coloured message to std err in a way
	 * which is guaranteed to be thread safe (ie atomic). Message will be printed with thread ID first, then
	 * prompt the message, then std::endl
	 * @param sMessage - what is to be printed
	 * @param sPrompt - optional a prompt
	 * @param color - optional colour
	 */
	void Print(const std::string & sMessage, const std::string & sPrompt="", color_t color = NONE, bool bAppendNewLine = true );

	void PrintStatus(bool bStatus,const std::string & sMessage);



	/**
	 * Turn off printing
	 */
	void Disable();

	/**
	 * Turn on printing
	 */
	void Enable();


private:
	class Impl;
	Impl* _Impl;
};

}

#endif /* THREADPRINT_H_ */
