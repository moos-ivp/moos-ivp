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




#include "MOOS/libMOOS/App/MOOSApp.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"


void PrintHelpAndExit()
{
    MOOSTrace("\n\nThis test tests the behaviour and response of a DB/App pair when the application\n"
    		"publishes in every iteration the variable it is subscribing to. Behaviour in this case\n"
    		"is governed (when using Asynchronous comms clients) by the MaximumAppTick parameter. \n");

    MOOSTrace("--data_size)                  : data size in bytes\n");

    exit(0);


}
class TestApp : public CMOOSApp
{
public:

	void OnPrintHelpAndExit()
	{
		PrintHelpAndExit();
	}
	bool OnProcessCommandLine()
	{
		int data_size = 100;
		m_CommandLineParser.GetVariable("--data_size", data_size);
		data_.resize(data_size);

		return true;
	}

  bool OnNewMail(MOOSMSG_LIST & /*List*/)
	{
		static double dfRate = 0;
		static double dfLastTime = 0.0;
		static double dfLastPrintTime = 0.0;

		double dfAlpha = 0.6;

		dfRate  = dfAlpha*dfRate+ (1.0-dfAlpha)/(MOOS::Time()-dfLastTime);

		if(MOOS::Time()-dfLastPrintTime>1.0)
		{
			dfLastPrintTime = MOOS::Time();
			std::cout<<MOOSFormat(" OnNewMail Running at %.1f Hz\n",dfRate);
		}

		dfLastTime = MOOS::Time();

		return true;
	}
	bool Iterate()
	{
		double dfAlpha = 0.6;
		static double dfRate = 0;
		static double dfLastTime = 0.0;
		static double dfLastPrintTime = 0.0;
		static bool crazy_state = false;

		dfRate  = dfAlpha*dfRate+ (1.0-dfAlpha)/(MOOS::Time()-dfLastTime);

		if(MOOS::Time()-dfLastPrintTime>1.0)
		{
			dfLastPrintTime = MOOS::Time();
			std::cout<<MOOSFormat(" Iterate Running at %.1f Hz\n",dfRate);
		}

		//every 5 seconds, in five seconds bursts publish what we want to
		//receive ourselves
		if(int(MOOSTime()-GetAppStartTime()) % 10 > 5)
		{
			if(!crazy_state)
			{
				std::cout<<"starting to post \"X\" from iterate...\n";
				crazy_state = true;
			}
			Notify("X",data_,MOOS::Time());
		}
		else
		{
			if(crazy_state)
			{
				std::cout<<"stopping sending \"X\" in iterate...\n";
				crazy_state = false;
			}
		}

		dfLastTime = MOOS::Time();
		return true;
	}
	bool OnStartUp()
	{
		switch(iterate_mode_)
		{
		case 0:SetIterateMode(REGULAR_ITERATE_AND_MAIL); break;
		case 1:SetIterateMode(COMMS_DRIVEN_ITERATE_AND_MAIL); break;
		case 2:SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL); break;
		default: SetIterateMode(REGULAR_ITERATE_AND_MAIL); break;
		}
		SetAppFreq(request_app_freq_,request_max_freq_);
		return true;
	}

	bool OnConnectToServer()
	{
		Register("X",0.0);
		return true;
	}

protected:
	double request_max_freq_;
	double request_app_freq_;
	int iterate_mode_;
	std::vector<unsigned char > data_;
};

int main(int argc, char * argv[])
{

    //here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);
	//mission file could be first free parameter
	std::string mission_file = P.GetFreeParameter(0, "Mission.moos");
	//mission name can be the  second free parameter
	std::string app_name = P.GetFreeParameter(1, "DBAppTest");

	TestApp A;
	A.Run(app_name,mission_file,argc,argv);
}
