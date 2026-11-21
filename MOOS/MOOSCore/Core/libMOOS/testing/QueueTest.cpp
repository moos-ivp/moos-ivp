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




/*
 * QueueTest.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: pnewman
 */
#include "MOOS/libMOOS/Comms/MOOSAsyncCommClient.h"
#include "MOOS/libMOOS/Comms/MessageQueueAccumulator.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"
#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/App/MOOSApp.h"


MOOS::ThreadPrint gPrinter(std::cerr);

bool func(CMOOSMsg & M, void * /*pParam*/)
{
	gPrinter.Print(MOOSFormat("in callback for %s",M.GetKey().c_str()));
	return true;
}

bool func_alt(CMOOSMsg & M, void * /*pParam*/)
{
	gPrinter.Print(MOOSFormat("in alternate callback for %s",M.GetKey().c_str()));
	return true;
}

bool func_wildcard(CMOOSMsg & M, void * /*pParam*/)
{
	gPrinter.Print(MOOSFormat("in wildcard callback for %s",M.GetKey().c_str()));
	return true;
}


bool func_wild(CMOOSMsg & M, void * /*pParam*/)
{
	gPrinter.Print(MOOSFormat("in wildcard callback for %s",M.GetKey().c_str()));
	return true;
}


bool on_connect(void * pParam)
{
	CMOOSCommClient * pC = static_cast<CMOOSCommClient*> (pParam);
	return pC->Register("la") && pC->Register("di") && pC->Register("da");
}

void PrintHelpAndExit()
{
	std::cerr<<"quick test for active queues on a comms client\n\n";
	std::cerr<<"    stimulate with umm -p=la,di,da\n\n";
	std::cerr<<"you should see :\n";
	std::cerr<<" a) la appearing in callback \"func\"\n";
	std::cerr<<" b) di appearing in callback \"func\" and \"func_alt\"\n";
	std::cerr<<" c) di appearing in callback \"func_wild\" because it is caught by the wildcard queue\n";
	std::cerr<<" c) la appearing in callback \"class-method-callback\" because it is caught by a class member\n";
	exit(0);

}

class InterestedParty
{
public:
	bool HandleMessageA(CMOOSMsg &M)
	{
		gPrinter.Print(MOOSFormat("in class::HandleMessageA for %s",M.GetKey().c_str()));
		return true;
	}
	bool HandleMessageB(CMOOSMsg &M)
	{
		gPrinter.Print(MOOSFormat("in class::HandleMessageB for %s",M.GetKey().c_str()));
		return true;
	}
	bool HandleMessageC(CMOOSMsg &M)
	{
		gPrinter.Print(MOOSFormat("in class::HandleMessageC for %s",M.GetKey().c_str()));
		return true;
	}
	bool HandleMessageSet(std::vector<CMOOSMsg> & Mvec)
	{
		gPrinter.Print(MOOSFormat("in class::HandleMessageSet"));
		for(unsigned int k = 0;k<Mvec.size();k++)
		{
			gPrinter.Print(MOOSFormat("     %s %10.3f",Mvec[k].GetKey().c_str(), Mvec[k].GetTime() ));
		}
		return true;
	}

};


int main(int argc, char * argv[])
{
	MOOS::MOOSAsyncCommClient C;
	MOOS::CommandLineParser P(argc,argv);

	if(P.GetFlag("-h","--help"))
	{
		PrintHelpAndExit();
	}

	InterestedParty aClass;

	//C.AddMessageRouteToActiveQueue("CallbackA","la",func,NULL);
	//C.AddMessageRouteToActiveQueue("CallbackB","di",func,NULL);
	//C.AddMessageRouteToActiveQueue("CallbackC","di",func_alt,NULL);
	//C.AddMessageRouteToActiveQueue("Wildcard","*", func_wild,NULL);
	//C.AddMessageRouteToActiveQueue("ClassMember","la", &aClass,&InterestedParty::HandleMessageA);
	//C.AddMessageRouteToActiveQueue("ClassMember","di", &aClass,&InterestedParty::HandleMessageB);
	//C.AddWildcardActiveQueue("WCA","*", func_wildcard,NULL);

	//C.PrintMessageToActiveQueueRouting();


	MOOS::MessageQueueAccumulator Acc;

	std::vector<std::string> Names;
	Names.push_back("di");
	Names.push_back("la");

	Acc.Configure(Names);
	C.AddMessageRouteToActiveQueue("Accumulator","di", &Acc,&MOOS::MessageQueueAccumulator::AddMessage);
	C.AddMessageRouteToActiveQueue("Accumulator","la", &Acc,&MOOS::MessageQueueAccumulator::AddMessage);
	Acc.SetCallback(&aClass,&InterestedParty::HandleMessageSet);



	//C.lala();
	C.SetOnConnectCallBack(on_connect, &C);
	C.Run("localhost",9000,"queue_test");


	unsigned int j = 0;
	while(++j)
	{
		if(j%100==0)
		{
			C.AddWildcardActiveQueue("WCB","d*", &aClass,&InterestedParty::HandleMessageC);
			C.PrintMessageToActiveQueueRouting();
		}
		MOOSPause(100);
		continue;
	}




}
