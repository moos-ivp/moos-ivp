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

#include "MOOS/libMOOS/Comms/MOOSAsyncCommClient.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"
#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/App/MOOSApp.h"


MOOS::ThreadPrint gPrinter(std::cerr);

bool func(CMOOSMsg & M, void * /*pParam*/)
{
	gPrinter.Print(MOOSFormat("in callback for %s",M.GetKey().c_str()));
	return true;
}


bool on_connect(void * pParam)
{
	CMOOSCommClient * pC = static_cast<CMOOSCommClient*> (pParam);
	return pC->Register("X") && pC->Register("Y");
}

int main(int argc, char * argv[])
{
	MOOS::MOOSAsyncCommClient C;
	MOOS::CommandLineParser P(argc,argv);


	C.AddMessageRouteToActiveQueue("CallbackA","X",func,NULL);
	C.AddMessageRouteToActiveQueue("CallbackB","Y",func,NULL);
	C.SetOnConnectCallBack(on_connect, &C);
	C.Run("localhost",9000,"reg_test");

	std::cerr<<"now run \"umm -p=X@2,Y@2\n\"";

	while(1)
	{
		std::cerr<<"waiting 5 seconds\n";
		MOOSPause(5000);
		std::cerr<<"unregistering for everything\n";
		C.UnRegister("*","*");
		std::cerr<<"waiting 5 seconds\n";
		MOOSPause(5000);
		std::cerr<<"registering for \"X\" and \"Y\"\n";
		C.Register("X");
		C.Register("Y");
        MOOSPause(5000);
        C.Register("Y",1.0);
        MOOSPause(5000);
    }



}
