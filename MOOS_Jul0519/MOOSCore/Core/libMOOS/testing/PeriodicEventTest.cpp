/*
 * PeriodicEventTest.cpp
 * a simple test for the MOOS::PeriodicEvent class.
 *  Created on: Apr 1, 2013
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Utils/PeriodicEvent.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include <iostream>
#include <iomanip>
#include <cmath>



double dfMean = 0;
double dfVar = 0;
unsigned int N = 0;

bool f(double TimeNow,double TimeLastRun,double TimeScheduled, void * /*pParamCaller*/)
{
	//lets wait a while
	MOOSPause(2);

	//some stats
	double dfE =  TimeNow-TimeScheduled;
	N=N+1;
	dfMean = dfMean*(N-1)/N+(dfE)/N;

	if(N>1)
		dfVar = dfE/(N-1)+(N-1)/N*dfVar;

	std::cout.setf(std::ios::fixed);
	std::cout<<std::setprecision(4);
	std::cout<<"Timer Callback \n";
	std::cout<<"  TimeNow       "<<TimeNow<<"\n";
	std::cout<<"  TimeScheduled "<<TimeScheduled<<"\n";
	std::cout<<"  TimeLastRun   "<<TimeLastRun<<"\n";
	std::cout<<"  Error         "<<(TimeNow-TimeScheduled)*1000.0<<"ms\n";
	std::cout<<"  Mean Error    "<<dfMean*1000.0<<"ms\n";
	std::cout<<"  Std  Error    "<<std::sqrt(dfVar)*1000.0<<"ms\n";


	return true;
}

int main()
{
	MOOS::PeriodicEvent P;

	//100Hz
	P.SetPeriod(0.01);

	//install callback
	P.SetCallback(f,NULL);

	//start
	P.Start();

	while(N<1000)
	{
		MOOSPause(100); //do nought
	}

	//stop
	P.Stop();
}
