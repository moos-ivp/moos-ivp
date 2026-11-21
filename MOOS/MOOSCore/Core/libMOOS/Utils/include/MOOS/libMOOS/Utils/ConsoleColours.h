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
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
/* ConsoleColours.h
 *
 *  Created on: Sep 23, 2011
 *      Author: pnewman
 */

#ifndef CONSOLECOLOURS_H_
#define CONSOLECOLOURS_H_

#ifdef _WIN32
#define _WINSOCKAPI_
    //#include <winsock2.h>
    #include "windows.h"
    #include "winbase.h"
    #include "winnt.h"
#endif

namespace MOOS
{

/** ability to change console text
 *     //use as follows std::cerr<<MOOS::ConsoleColours()<<red();
 *
 **/
//! class for changing console text color
struct ConsoleColours
{
#ifndef _WIN32


    static const char* red() {return control("\x1b[31m");};
    static const char* Red() {return control("\x1b[1;31m");};

    static const char* green() {return control("\x1b[32m");};
    static const char* Green() {return control("\x1b[1;32m");};

    static const char* yellow() {return control("\x1b[33m");};
    static const char* Yellow() {return control("\x1b[1;33m");};

    static const char* blue() {return control("\x1b[34m");};
    static const char* Blue() {return control("\x1b[1;34m");};

    static const char* magenta() {return control("\x1b[35m");};
    static const char* Magenta() {return control("\x1b[1;35m");};

    static const char* cyan() {return control("\x1b[36m");};
    static const char* Cyan() {return control("\x1b[1;36m");};

    static const char* reset() {return control("\x1b[0m");};
#else
	
	static const char* red() {return " \010";};
    static const char* Red() {return " \010";};

    static const char* green() 
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x2);
		return " \010";
	};
    static const char* Green() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),10);
		return " \010";
	};

    static const char* yellow() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
		return " \010";};

    static const char* Yellow() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
		return " \010";};

    static const char* blue() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),1);
		return " \010";};

    static const char* Blue() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),9);
		return " \010";};

    static const char* magenta() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),13);
		return " \010";};
    static const char* Magenta() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),5);
		return " \010";};

    static const char* cyan() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),11);
		return " \010";};
    static const char* Cyan() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),3);
		return " \010";};

    static const char* reset() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),15);
		return " \010";};
#endif

    static void Enable(bool bEnable)
    {
    	disable_color_ = ! bEnable;
    }
private:
    static const char* control(const char * s)
	{
		if(!disable_color_)
			return s;
		else
			return " \010";
	}

    static bool disable_color_;
};

}
#endif
