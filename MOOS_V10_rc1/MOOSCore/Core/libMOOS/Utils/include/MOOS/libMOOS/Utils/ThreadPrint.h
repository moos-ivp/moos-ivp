///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
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

	void SimplyPrintTimeAndMessage(const std::string & sMessage , color_t color = NONE);



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
