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
#include "MOOS/libMOOS/App/MOOSApp.h"
#include <ctime>

namespace {
std::vector<std::string> vars;
std::ofstream of;
MOOS::ThreadPrint gPrinter(of);
bool bLog=false;
double dfStartTime=0;
}

void PrintHelpAndExit()
{
	std::cerr<<"\nA program which captures times of dispatch and arrival of named messages to file.\n\n";

	std::cerr<<"\nUsage\n";
	std::cerr<<"./capture [options] vars\n";

	std::cerr<<"\nOptions\n";
	std::cerr<<"--moos_host=[localhost]\n";
	std::cerr<<"--moos_port=[9000]\n";
    std::cerr<<"--log\n";
    std::cerr<<"--log_file=[capture.txt]\n";
    std::cerr<<"--moos_boost\n";

	std::cerr<<"\nexamples\n";
	std::cerr<<"   ./capture  --log --log_file=logX.txt  X \n";
	std::cerr<<"   ./capture  --log --log_file=logX.txt --moos_boost  X \n";
	exit(0);
}


std::string GetTimeStampString()
{
    struct tm *Now;
    time_t aclock;
    time( &aclock );

    Now = localtime( &aclock );
    //change suggested by toby schneider April 2009
    //Now = gmtime( &aclock );
    char sTmp[1000];

    // Print local time as a string

    //14_5_1993_____9_30
    sprintf(sTmp, "_%d_%d_%d_____%.2d_%.2d.%2d",
        Now->tm_mday,
        Now->tm_mon+1,
        Now->tm_year+1900,
        Now->tm_hour,
        Now->tm_min,
        Now->tm_sec);


    return std::string(sTmp);

}

void LogMessage(CMOOSMsg & M)
{
	double dfDelay = MOOSLocalTime()-M.GetTime();
	if(dfDelay>30e-3)
	{
		std::cerr<<GetTimeStampString()<<" "<<MOOSTime()-dfStartTime<<" ouch! delay is "<<dfDelay<<"\n";
	}
	if(bLog)
	{
		std::stringstream ss;
		ss.setf(std::ios::fixed);
		ss<<std::setprecision(4)<<M.GetTime()<<" ";
		ss<<std::setprecision(4)<<MOOSLocalTime()<<" ";
		ss<<M.GetKey();
		gPrinter.SimplyPrintTimeAndMessage(ss.str());
	}

}

bool func(CMOOSMsg & M, void * /*pParam*/)
{
	LogMessage(M);
	return true;
}

bool on_mail(void * pParam)
{
	CMOOSCommClient * pC = static_cast<CMOOSCommClient*> (pParam);
	MOOSMSG_LIST Mail;
	pC->Fetch(Mail);

	MOOSMSG_LIST::iterator q;
  for(q=Mail.begin();q!=Mail.end();++q)
	{
		LogMessage(*q);
	}

	return true;
}


bool on_connect(void * pParam)
{
	CMOOSCommClient * pC = static_cast<CMOOSCommClient*> (pParam);
  for(unsigned int k = 0;k<vars.size();++k)
		pC->Register(vars[k]);

	return true;
}


int main(int argc, char * argv[])
{

	dfStartTime = MOOSLocalTime();
	MOOS::MOOSAsyncCommClient C;
	MOOS::CommandLineParser P(argc,argv);
	P.GetFreeParameters(vars);

	if(P.GetFlag("--help"))
		PrintHelpAndExit();

	std::string sHost = "localhost";
	P.GetVariable("--moos_host",sHost);

	unsigned int port = 9000;
	P.GetVariable("--moos_port",port);

	bool bBoostIO = P.GetFlag("-b","--moos_boost");
	C.BoostIOPriority(bBoostIO);

	bLog = P.GetFlag("--log");

	if(bLog)
	{
		std::string sLogFile = "capture.txt";
		P.GetVariable("--log_file",sLogFile);
		of.open(sLogFile.c_str());

		if(of)
			std::cerr<<"logging to "<<sLogFile<<"\n";
	}

	for(unsigned int k = 0;k<vars.size();k++)
	{
		std::cout<<"installing callback for "<<vars[k]<<"\n";
		//C.AddMessageCallback(vars[k]+"_CB",vars[k],func,NULL);
	}

	C.SetOnMailCallBack(on_mail,&C);
	C.SetOnConnectCallBack(on_connect, &C);
	C.Run(sHost,port,"capture_test");

	for(;;)
	{
		MOOSPause(10000);
	}

}
