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




#include "MOOS/libMOOS/Comms/MOOSAsyncCommClient.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"
#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"


int count=0;
CMOOSLock L;

MOOS::ThreadPrint gPrinter(std::cerr);

bool func(CMOOSMsg & M, void *pParam)
{
	MOOS::DeliberatelyNotUsed(pParam);
	MOOS::DeliberatelyNotUsed(M);
	//gPrinter.SimplyPrintTimeAndMessage(M.GetAsString());
	L.Lock();
		count++;
	L.UnLock();
	return true;
}


bool on_connect(void * pParam)
{
	CMOOSCommClient * pC = static_cast<CMOOSCommClient*> (pParam);
	pC->Register("X");
	return true;
}

int main(int argc, char * argv[])
{
    //here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);

	MOOS::MOOSAsyncCommClient A,B;
	//CMOOSCommClient A,B;
	A.SetQuiet(true);
	B.SetQuiet(true);

	A.SetOnConnectCallBack(on_connect, &A);
	B.SetOnConnectCallBack(on_connect, &B);

	A.Run("localhost",9000,"A");
	B.Run("localhost",9000,"B");

	A.AddMessageRouteToActiveQueue("CBA","X",func,NULL);
	B.AddMessageRouteToActiveQueue("CBB","X",func,NULL);


	while(1)
	{

		std::cerr<<"1)closing all\n";
		A.Close();
		B.Close();
		std::cerr<<"2)restarting all\n";

		A.Run("localhost",9000,"C");
		B.Run("localhost",9000,"D");

		while(!A.IsConnected())
			MOOSPause(10);

		while(!B.IsConnected())
			MOOSPause(10);

		std::cerr<<"3)installing callbacks\n";
		A.AddMessageRouteToActiveQueue("CBA","X",func,NULL);
		B.AddMessageRouteToActiveQueue("CBB","X",func,NULL);


		//wait here because "X" is still in the DB so we will be told
		//of its existence the minute we connect...
		MOOSPause(100); //callback will have fired by now..

		//reset count
		count = 0;

		std::cerr<<"4)starting rapid fire notifications...\n";
		int n = 30;
		for(int i = 0;i<n;i++)
		{
			//send data from everyone to everyone
			//4 hundred notifications and two interested parties
			A.Notify("X",i);
			B.Notify("X",i);
		}

		//this should be fast no need to wait long
		MOOSPause(100);

		//by this point we shoudl have received n*4 messages
		std::cerr<<"6)starting rapid fire notifications...\n";
		std::cerr<<"7)testing\n\treceived "<<count<< " and expected "<<n*4<<":"<<(count==n*4?"PASS":"FAIL")<<"\n\n\n";

	}

}
