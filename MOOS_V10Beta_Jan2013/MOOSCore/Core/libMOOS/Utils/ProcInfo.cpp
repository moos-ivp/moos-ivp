/*
 * ProcInfo.cpp
 *
 *  Created on: Dec 15, 2012
 *      Author: pnewman
 */
#include <iostream>

#include "MOOS/libMOOS/Thirdparty/PocoBits/ScopedLock.h"
#include "MOOS/libMOOS/Thirdparty/PocoBits/Mutex.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/ProcInfo.h"

#include <sys/resource.h>



namespace MOOS {

const int sample_period_ms =500;

#ifdef _WIN32
#include <Windows.h>

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





class ProcInfo::Impl
{
public:
	Impl()
	{
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

	static bool Dispatch(void * pParam)
	{
		ProcInfo::Impl* pMe = reinterpret_cast<ProcInfo::Impl*> (pParam);
		return pMe->Run();
	}

	bool Run()
	{

#ifdef _WIN32
		FILETIME sysIdleA, sysKernelA, sysUserA,sysIdleB, sysKernelB, sysUserB;
		FILETIME procCreation, procExit,
		FILETIME procKernelA, procUserA, procKernelB, procUserB;

		if (!GetSystemTimes(&sysIdleA, &sysKernelA, &sysUserA) ||
			!GetProcessTimes(GetCurrentProcess(), &procCreationA,
					&procExitA, &procKernelA, &procUserA))		{
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
			ULONGLONG sysUserDiff = subtractTime(sysUserB, SysUserA);
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
		}

#else

		struct rusage uA, uB;
		if(getrusage(0, &uA)!=0)
			return false;

		double tA = MOOSLocalTime();

		cpu_load_ = 0.0;

		while(!Thread_.IsQuitRequested())
		{
			MOOSPause(sample_period_ms);
			double tB = MOOSLocalTime();
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

		    //std::cerr<<"cpu_load_ is "<<cpu_pc<<std::endl;
		}
#endif

		return true;
	}
protected:
	CMOOSThread Thread_;
    Poco::FastMutex _mutex;
    double cpu_load_;



};

bool ProcInfo::GetPercentageCPULoad(double &cpu_load)
{
	return Impl_->GetPercentageCPULoad(cpu_load);
}

ProcInfo::ProcInfo(): Impl_(new ProcInfo::Impl ){
	// TODO Auto-generated constructor stub

}

ProcInfo::~ProcInfo() {
	// TODO Auto-generated destructor stub
}

}
