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
#ifndef WIN32
	#include <pthread.h>
#endif
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <cerrno>

namespace MOOS
{
bool BoostThisThread()
{
#ifdef WIN32
	std::cerr<<"MOOS::BoostThisThread is not supported in WIN32 (yet)\n";
	return false;
#else
	try
	{
		int policy = SCHED_OTHER;
		struct sched_param param;
		if(pthread_getschedparam(pthread_self(), &policy, &param)!=0)
		{
			throw std::runtime_error("MOOS::BoostThisThread() failed to get this thread's scheduling details");
		}
		//std::cout<<"default priority"<< param.sched_priority<<"\n";

		int max_priority;
		if((max_priority = sched_get_priority_max(policy))==-1)
		{
			throw std::runtime_error("MOOS::BoostThisThread() failed to get this thread's max priority");
		}
		//std::cout<<"max priority"<< param.sched_priority<<"\n";

		//we might already be at max priority
		if(param.sched_priority==max_priority)
		{
		    //nothing to be done...
		    throw std::runtime_error("MOOS::BoostThisThread() max priority reached");
		}

		param.sched_priority+=1;

		if(pthread_setschedparam(pthread_self(), policy, &param)!=0)
		{
			throw std::runtime_error("MOOS::BoostThisThread() failed to increase this thread's  priority");
		}

		//std::cerr<<"boosted IO of thread "<<pthread_self()<<"\n";
	}
	catch(const std::runtime_error & e)
	{
		std::cerr<<e.what()<<" "<<strerror(errno)<<"\n";
		return false;
	}
	return true;

#endif

}

bool GetThisThreadsPriority(int & Priority, int & MaxAllowed)
{
#ifdef WIN32
	Priority;
	MaxAllowed;
	std::cerr<<"MOOS::GetThisThreadsPriority is not supported in WIN32 (yet)\n";
	return false;
#else
	int policy = SCHED_OTHER;
	struct sched_param param;
	int max_priority;

	try
	{
		if(pthread_getschedparam(pthread_self(), &policy, &param)!=0)
		{
			throw std::runtime_error("MOOS::BoostThisThread() failed to get this thread's scheduling details");
		}

		if((max_priority = sched_get_priority_max(policy))==-1)
		{
			throw std::runtime_error("MOOS::BoostThisThread() failed to get this thread's max priority");
		}

	}
	catch(const std::runtime_error & e)
	{
		std::cerr<<e.what()<<" "<<strerror(errno)<<"\n";
		return false;
	}

	Priority = param.sched_priority;
	MaxAllowed = max_priority ;

	return true;
#endif

}


}
