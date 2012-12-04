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
	Impl(std::ostream & os): _outstream(os){}
	std::ostream & _outstream;
    static CMOOSLock _Lock;
    bool _Enable;
};

CMOOSLock ThreadPrint::Impl::_Lock;

ThreadPrint::ThreadPrint(std::ostream & os) : _Impl(new Impl(os))
{
	// TODO Auto-generated constructor stub
	_Impl->_Enable = true;

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
    VENUE.setf(std::ios::fixed);

    VENUE<<std::setprecision(4)<<MOOS::Time()<<" "<<MOOS::ConsoleColours::yellow()<<std::left<< std::setw(15) << std::setfill(' ')<<Me<<"- ";

	switch(color)
	{
		case RED:
			VENUE<<MOOS::ConsoleColours::red();
			break;
		case YELLOW:
			VENUE<<MOOS::ConsoleColours::yellow();
			break;
		case GREEN:
			VENUE<<MOOS::ConsoleColours::green();
			break;
		case MAGENTA:
			VENUE<<MOOS::ConsoleColours::magenta();
			break;
		case CYAN:
			VENUE<<MOOS::ConsoleColours::cyan();
			break;
		case NONE:
			VENUE<<MOOS::ConsoleColours::reset();
			break;

	}

	VENUE<< std::setw(25)<<sPrompt<<" "<<sMessage;

	if(bAppendNewLine)
		VENUE<<std::endl;

	VENUE<<MOOS::ConsoleColours::reset();

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
