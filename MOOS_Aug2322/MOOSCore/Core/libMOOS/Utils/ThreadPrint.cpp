/**
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
**/




/*
 * ThreadPrint.cpp
 *
 *  Created on: Dec 31, 2011
 *      Author: pnewman
 */

#include <iostream>
#include <iomanip>
#include "MOOS/libMOOS/Utils/MOOSLock.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"

namespace MOOS {


#define VENUE std::cout

class ThreadPrint::Impl
{
public:
	explicit Impl(std::ostream & os): _outstream(os), _Enable(true) {}
	std::ostream & _outstream;
    static CMOOSLock _Lock;
    bool _Enable;
private:
	Impl();
	Impl(const Impl&);
	Impl& operator = (const Impl&);
};

CMOOSLock ThreadPrint::Impl::_Lock;

ThreadPrint::ThreadPrint(std::ostream & os) : _Impl(new Impl(os))
{
}

ThreadPrint::~ThreadPrint() {
	// TODO Auto-generated destructor stub
	delete _Impl;
}

void ThreadPrint::PrintStatus(bool bStatus,const std::string & sMessage)
{
	if(!_Impl->_Enable)
		return;

	_Impl->_Lock.Lock();

	if(bStatus)
		_Impl->_outstream<<MOOS::ConsoleColours::Green()<<"[OK] ";
	else
		_Impl->_outstream<<MOOS::ConsoleColours::Red()<<"[!]  ";

	_Impl->_outstream<<sMessage<<std::endl;

	_Impl->_outstream<<MOOS::ConsoleColours::reset();

	_Impl->_Lock.UnLock();


}


void ThreadPrint::SimplyPrintTimeAndMessage(const std::string & sMessage,color_t color )
{
	if(!_Impl->_Enable)
		return;

	_Impl->_Lock.Lock();

	switch(color)
	{
		case RED:
			_Impl->_outstream<<MOOS::ConsoleColours::red();
			break;
		case YELLOW:
			_Impl->_outstream<<MOOS::ConsoleColours::yellow();
			break;
		case GREEN:
			_Impl->_outstream<<MOOS::ConsoleColours::green();
			break;
		case MAGENTA:
			_Impl->_outstream<<MOOS::ConsoleColours::magenta();
			break;
		case CYAN:
			_Impl->_outstream<<MOOS::ConsoleColours::cyan();
			break;
		case NONE:
			_Impl->_outstream<<MOOS::ConsoleColours::reset();
			break;

	}

	_Impl->_outstream.setf(std::ios::fixed);
	_Impl->_outstream<<std::setprecision(4)<<MOOS::Time()<<"   "<<sMessage<<"\n";
	_Impl->_outstream<<MOOS::ConsoleColours::reset();

	_Impl->_Lock.UnLock();

}


void ThreadPrint::Print(const std::string & sMessage, const std::string & sPrompt,color_t color, bool bAppendNewLine )
{

	if(!_Impl->_Enable)
		return;


	_Impl->_Lock.Lock();


#ifdef _WIN32
    DWORD Me = GetCurrentThreadId();
#else
    pthread_t Me =  pthread_self();
#endif
    _Impl->_outstream.setf(std::ios::fixed);

    _Impl->_outstream<<std::setprecision(4)<<MOOS::Time()<<" "<<MOOS::ConsoleColours::yellow()<<std::left<< std::setw(15) << std::setfill(' ')<<Me<<"- ";

	switch(color)
	{
		case RED:
			_Impl->_outstream<<MOOS::ConsoleColours::red();
			break;
		case YELLOW:
			_Impl->_outstream<<MOOS::ConsoleColours::yellow();
			break;
		case GREEN:
			_Impl->_outstream<<MOOS::ConsoleColours::green();
			break;
		case MAGENTA:
			_Impl->_outstream<<MOOS::ConsoleColours::magenta();
			break;
		case CYAN:
			_Impl->_outstream<<MOOS::ConsoleColours::cyan();
			break;
		case NONE:
			_Impl->_outstream<<MOOS::ConsoleColours::reset();
			break;

	}

	_Impl->_outstream<< std::setw(25)<<sPrompt<<" "<<sMessage;

	if(bAppendNewLine)
		_Impl->_outstream<<std::endl;

	_Impl->_outstream<<MOOS::ConsoleColours::reset();

	_Impl->_Lock.UnLock();
}

void ThreadPrint::Enable()
{
	_Impl->_Enable = true;
}


void ThreadPrint::Disable()
{
	_Impl->_Enable = false;
}


}
