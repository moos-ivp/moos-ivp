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
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/




/*
 * ProcInfo.cpp
 *
 *  Created on: Dec 15, 2012
 *      Author: pnewman
 */
#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <iomanip>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif

#include "MOOS/libMOOS/Thirdparty/PocoBits/ScopedLock.h"
#include "MOOS/libMOOS/Thirdparty/PocoBits/Mutex.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/MemInfo.h"

#include "MOOS/libMOOS/Utils/ProcInfo.h"






#ifndef _WIN32
#include <sys/resource.h>
#else

ULONGLONG subtractTime(const FILETIME &a, const FILETIME &b)
{
    LARGE_INTEGER la, lb;
    la.LowPart = a.dwLowDateTime;
    la.HighPart = a.dwHighDateTime;
    lb.LowPart = b.dwLowDateTime;
    lb.HighPart = b.dwHighDateTime;

    return la.QuadPart - lb.QuadPart;
}

#endif


namespace MOOS {

const int sample_period_ms =500;

class ProcInfo::Impl
{
public:
	Impl()
	{
	    //now do memory usage
	    resident_memory_ = GetCurrentMemoryUsage();
	    max_memory_ = GetPeakMemoryUsage();

		Thread_.Initialise(Dispatch, this);
		Thread_.Start();
	}
	virtual ~Impl()
	{
		Thread_.Stop();
	}

	bool GetPercentageCPULoad(double & cpu_load)
	{
	    Poco::FastMutex::ScopedLock Lock(_mutex);
	    cpu_load = cpu_load_;
	    return true;
	}

	bool GetMemoryUsage(size_t & current,size_t & maximum)
	{
	    Poco::FastMutex::ScopedLock Lock(_mutex);
	    current = resident_memory_;
	    maximum = max_memory_;
	    return true;
	}


	static bool Dispatch(void * pParam)
	{
		ProcInfo::Impl* pMe = reinterpret_cast<ProcInfo::Impl*> (pParam);
		return pMe->Run();
	}

	bool Run()
	{
	    //now do memory usage
	    resident_memory_ = GetCurrentMemoryUsage();
	    max_memory_ = GetPeakMemoryUsage();


#ifdef _WIN32
		FILETIME sysIdleA, sysKernelA, sysUserA,sysIdleB, sysKernelB, sysUserB;
		FILETIME procCreation, procExit;
		FILETIME procKernelA, procUserA, procKernelB, procUserB;

		if (!GetSystemTimes(&sysIdleA, &sysKernelA, &sysUserA) ||
			!GetProcessTimes(GetCurrentProcess(), &procCreation,
					&procExit, &procKernelA, &procUserA))		{
			return false;
		}

		while(!Thread_.IsQuitRequested())
		{
			MOOSPause(sample_period_ms);

			if (!GetSystemTimes(&sysIdleB, &sysKernelB, &sysUserB) ||
					!GetProcessTimes(GetCurrentProcess(), &procCreation,
							&procExit, &procKernelB, &procUserB)){
				return false;
			}

			ULONGLONG sysKernelDiff = subtractTime(sysKernelB, sysKernelA);
			ULONGLONG sysUserDiff = subtractTime(sysUserB, sysUserA);
			ULONGLONG procKernelDiff = subtractTime(procKernelB, procKernelA);
			ULONGLONG procUserDiff = subtractTime(procUserB, procUserA);
			ULONGLONG sysTotal = sysKernelDiff + sysUserDiff;
			ULONGLONG procTotal = procKernelDiff + procUserDiff;

			{
				Poco::FastMutex::ScopedLock Lock(_mutex);
				cpu_load_ = (double)(100.0 * procTotal)/sysTotal;
			}

			sysIdleA = sysIdleB;
			sysKernelA = sysKernelB;
			procUserA = procUserB;
			procKernelA = procKernelB;


		    //now do memory usage
		    resident_memory_ = GetCurrentMemoryUsage();
		    max_memory_ = GetPeakMemoryUsage();

		}

#else

		struct rusage uA, uB;
		if(getrusage(0, &uA)!=0)
			return false;

		double tA = MOOSLocalTime();

		cpu_load_ = 0.0;

		while(!Thread_.IsQuitRequested())
		{

			MOOSPause(sample_period_ms,false);
			double tB = MOOSLocalTime(false);
			double tAB = tB-tA;

		    if(getrusage(0, &uB)!=0)
		    	return false;

		    {
			    Poco::FastMutex::ScopedLock Lock(_mutex);

				cpu_load_ = 100.0*(
						(uB.ru_utime.tv_sec-uA.ru_utime.tv_sec)+
						(uB.ru_utime.tv_usec-uA.ru_utime.tv_usec)/1000000.0+
						(uB.ru_stime.tv_sec-uA.ru_stime.tv_sec)+
						(uB.ru_stime.tv_usec-uA.ru_stime.tv_usec)/1000000.0
						)/tAB;
		    }
		    uA = uB;
		    tA = tB;

		    //now do memory usage
		    resident_memory_ = GetCurrentMemoryUsage();
		    max_memory_ = GetPeakMemoryUsage();

		}
#endif

		return true;
	}
protected:
	CMOOSThread Thread_;
    Poco::FastMutex _mutex;
    double cpu_load_;
    size_t resident_memory_;
    size_t max_memory_;



};

bool ProcInfo::GetPercentageCPULoad(double &cpu_load)
{
	return Impl_->GetPercentageCPULoad(cpu_load);
}

bool ProcInfo::GetMemoryUsage(size_t & current,size_t & maximum)
{
	return Impl_->GetMemoryUsage(current,maximum);
}

int ProcInfo::GetPid()
{

#ifdef _WIN32
        return (int)GetCurrentProcessId();
#else
        return (int)getpid();
#endif

}


ProcInfo::ProcInfo(): Impl_(new ProcInfo::Impl ){
	// TODO Auto-generated constructor stub

}

ProcInfo::~ProcInfo() {
	// TODO Auto-generated destructor stub
}

}
