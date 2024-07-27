/*
 * PeriodicEvent.cpp
 *
 *  Created on: Apr 1, 2013
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/ThreadPriority.h"


#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/PeriodicEvent.h"
#include <iostream>
#include <iomanip>



namespace MOOS
{
	class PeriodicEvent::Impl
	{
	public:

		Impl()
		{
			pfn_ = DefaultCallback;
			pParamCaller_= NULL;
			Period_ = 1.0;

		}

		static bool DefaultCallback(double TimeNow,double TimeLastRun,double TimeScheduled, void * pParamCaller)
		{
      UNUSED_PARAMETER(pParamCaller);

			std::cout.setf(std::ios::fixed);

			std::cout<<std::setprecision(4);

			std::cout<<"Timer Callback \n";
			std::cout<<"  TimeNow       "<<TimeNow<<"\n";
			std::cout<<"  TimeScheduled "<<TimeScheduled<<"\n";
			std::cout<<"  TimeLastRun   "<<TimeLastRun<<"\n";

			return true;
		}

		static bool PeriodicEventDispatch(void * pParam)
		{
			PeriodicEvent::Impl* pMe = static_cast<PeriodicEvent::Impl*> (pParam);
			return pMe->DoWork();
		}

		void SetCallback(bool (*pfn)(double TimeNow,double TimeLastRun,double TimeScheduled, void * pParamCaller), void * pCallerParam)
		{
      UNUSED_PARAMETER(pCallerParam);
			pfn_ = pfn;
			//pParamCaller_ = pCallerParam;

		}

		bool SetPeriod(double PeriodSeconds)
		{
			if(PeriodSeconds<0)
				return false;
			Period_ = PeriodSeconds;

			return true;
		}

		bool Stop()
		{
			Thread_.Stop();
			return true;
		}


		bool Start()
		{
			if(Thread_.IsThreadRunning())
				return false;
			else
			{
				Thread_.Initialise(PeriodicEventDispatch,this);
				return Thread_.Start();
			}
		}


		bool DoWork()
		{

			double TimeLast=MOOS::Time();

			MOOS::BoostThisThread();

			while(!Thread_.IsQuitRequested())
			{
				double TimeScheduled=TimeLast+Period_;

				int MillisecondsSleep = static_cast<int>( ( TimeScheduled-MOOS::Time() )*1000.0 )  ;
				if(MillisecondsSleep>0)
				{
					MOOS::Pause(MillisecondsSleep);
				}

				double TimeNow = MOOS::Time();

				if(!(*pfn_)(MOOS::Time(), TimeLast,TimeScheduled,pParamCaller_))
				{
					break;
				}

				TimeLast = TimeNow;



			}

			return true;
		}


		CMOOSThread Thread_;
		bool (*pfn_)(double TimeNow,double TimeLastRun,double TimeScheduled, void* pParam);
		void * pParamCaller_;
		double Period_;

	};

	void PeriodicEvent::SetCallback(bool (*pfn)(double Now,double LastRun,double Scheduled, void * pParamCaller), void * pCallerParam)
	{
		Impl_->SetCallback(pfn,pCallerParam);
	}

	PeriodicEvent::PeriodicEvent(): Impl_(new PeriodicEvent::Impl  )
	{

	}

	bool PeriodicEvent::SetPeriod(double PeriodSeconds)
	{
		return Impl_->SetPeriod(PeriodSeconds);
	}

	bool PeriodicEvent::Start()
	{
		return Impl_->Start();
	}

	bool PeriodicEvent::Stop()
	{
		return Impl_->Stop();
	}


}
