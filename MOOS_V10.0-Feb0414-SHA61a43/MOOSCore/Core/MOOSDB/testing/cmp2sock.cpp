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
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////


#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/Comms/XPCTcpSocket.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"

#include "MOOS/libMOOS/Utils/ThreadPrint.h"

#include <fstream>




void PrintHelpAndExit()
{
	std::cout<<"This program simply send/receives a hundred or so bytes over a vanilla tcp/ip socket. "
			"It is provided as a baseline for MOOS communications performance. MOOS will never be faster than this. "
			"All, transit times (Tx->Rx) or latencies, are logged to file and a message is written to "
			"stdout if a threshold is exceeded\n\n";
	std::cout<<"Usage:\n";
	std::cout<<"	./cmp2sock [options]\n";
	std::cout<<"Options:\n";
	std::cout<<"	--send                be a sender of data\n";
	std::cout<<"	--receive             be a receiver of data\n";
	std::cout<<"	--host                host to send to (default localhost)\n";
	std::cout<<"	--port                port to send to or receive on (default 9007)\n";
	std::cout<<"	--threshold=<float>   latency (s) above which print alert\n";
	std::cout<<"	--log_file=string     file to log to (default cmp2sock.txt)\n";
	std::cout<<"Example:\n";
	std::cout<<"    ./cmp2sock --send\n";
	std::cout<<"    ./cmp2sock --receive --threshold=0.01\n";


	exit(0);
}

struct Msg
{
	double time;
	double val;
	unsigned char space[100];
};

int main(int argc, char * argv[])
{


	MOOS::CommandLineParser P(argc,argv);

	if(P.GetFlag("--help"))
		PrintHelpAndExit();

	std::string sHost = "localhost";
	int nPort = 9007;

	P.GetVariable("--host",sHost);
	P.GetVariable("--port",nPort);

	double dfThreshold = 0.0;
	P.GetVariable("--threshold",dfThreshold);

	std::string sLogFile = "cmp2sock.txt";
	P.GetVariable("--log_file",sLogFile);


	if(P.GetFlag("--send"))
	{
		XPCTcpSocket* pSocket=NULL;
		while(1)
		{
			try
			{
				//try to connect
				pSocket =  new XPCTcpSocket((long)nPort);
				pSocket->vConnect(sHost.c_str());
				break;
			}
			catch(XPCException & e)
			{
				std::cerr<<e.sGetException()<<"\n";
			}
			delete pSocket;
			MOOSPause(1000);
		}

		Msg M;
		M.val = 0.0;
		while(1)
		{
			M.time=MOOSLocalTime();
			pSocket->iSendMessage(&M,sizeof(M));
			MOOSPause(10);
		}
	}

	if(P.GetFlag("--receive"))
	{
		std::ofstream f(sLogFile.c_str());
		MOOS::ThreadPrint gPrinter(f);

		XPCTcpSocket* pListenSocket = new XPCTcpSocket((long)nPort);

		try
		{
			pListenSocket->vSetReuseAddr(1);
			pListenSocket->vBindSocket();
		}
		catch(XPCException & e)
		{
			std::cerr<<e.sGetException()<<"\n";
			return -1;
		}


		while(1)
		{
			Msg M;
			XPCTcpSocket* pSocket=NULL;
			double dfT0 = MOOSLocalTime();
			try
			{
				pListenSocket->vListen(5);
				pSocket = pListenSocket->Accept();

				while(1)
				{
					if(pSocket->iRecieveMessage(&M,sizeof(M))==sizeof(M))
					{
						double dfT = MOOSLocalTime()-M.time;
						gPrinter.SimplyPrintTimeAndMessage(MOOSFormat("%f",dfT));
						if(dfT>dfThreshold)
							std::cerr<<MOOSLocalTime()-dfT0<<" : "<<dfT<<" s lag\n";
					}
				}

			}
			catch(XPCException & e)
			{
				std::cerr<<e.sGetException()<<"\n";
				if(pSocket!=NULL)
				    delete pSocket;
			}

		}
	}


}
