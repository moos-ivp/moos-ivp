/*
 * QueueTest.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: pnewman
 */
#include "MOOS/libMOOS/Comms/MOOSAsyncCommClient.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"
#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/App/MOOSApp.h"


MOOS::ThreadPrint gPrinter(std::cerr);

bool func(CMOOSMsg & M, void *pParam)
{
	gPrinter.Print(MOOSFormat("in callback for %s",M.GetKey().c_str()));
	return true;
}

bool on_connect(void * pParam)
{
	CMOOSCommClient * pC = static_cast<CMOOSCommClient*> (pParam);
	return pC->Register("la") && pC->Register("di");
}

class T : public CMOOSApp
{
	bool OnMessage(CMOOSMsg & M)
	{
		gPrinter.Print("handling "+M.GetKey());
		return true;
	}
	bool OnStartUp()
	{
		return AddMessageCallback("la");
	}
	bool OnConnectToServer()
	{
		return Register("la");
	}
};

int main(int argc, char * argv[])
{
	MOOS::MOOSAsyncCommClient C;
	MOOS::CommandLineParser P(argc,argv);


	C.AddMessageCallback("la",func,NULL);
	C.AddMessageCallback("di",func,NULL);
	C.SetOnConnectCallBack(on_connect, &C);
	C.Run("localhost",9000,"queue_test");


	T theApp;

	theApp.Run("queue_test_app",argc,argv);

}
