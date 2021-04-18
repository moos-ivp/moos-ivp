/*
 * ktm.cpp
 *
 *  Created on: Feb 6, 2014
 *      Author: pnewman
 */
#if !defined(_WIN32)
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#include <string>
#include <cstring>
#include <stdexcept>

#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"

#include "MOOS/libMOOS/Comms/MulticastNode.h"
#include "MOOS/libMOOS/Comms/SuicidalSleeper.h"



bool SetupSocket(const std::string & sAddress, int Port);
bool WaitForSocket(int fd, int TimeoutSeconds);

//such a simple program lets be old skool - file scope vars;
std::string phrase = MOOS::SuicidalSleeper::GetDefaultPassPhrase();
std::string multicast_address = MOOS::SuicidalSleeper::GetDefaultMulticastAddress();
int multicast_port = MOOS::SuicidalSleeper::GetDefaultMulticastPort();
std::string name_pattern = "*";
unsigned char hops = 0;


void PrintHelpAndExit()
{
    std::cerr<<"Kill The MOOS (ktm)\n";
    std::cerr<<"--channel=<address>    address to issue kill on \n";
    std::cerr<<"--port=<int>           port to issue kill on \n";
    std::cerr<<"--phrase=<string>      pass phrase which Suicide listeners are keyed to\n";
    std::cerr<<"--all                  pass to network, it is a massacre\n";
    std::cerr<<"--query                find out who would jump\n";
    std::cerr<<"--name=<string>        only apply if matches pattern\n";



    std::cerr<<"example    \n";
    std::cerr<<"    ./ktm  --phrase=\"die now\""<<"\n";

    exit(0);

}

int main(int argc, char *argv[])
{

    MOOS::CommandLineParser P(argc,argv);

    if(P.GetFlag("--help","-h"))
        PrintHelpAndExit();

    P.GetVariable("--channel",multicast_address);
    P.GetVariable("--port",multicast_port);
    P.GetVariable("--phrase",phrase);
    P.GetVariable("--name",name_pattern);

    bool bQuery = P.GetFlag("--query");

    if(P.GetFlag("--all"))
    {
        std::cerr<<MOOS::ConsoleColours::Red()<<"THIS WILL KILL ALL APPS WITHIN 1 HOP...[N/y]\n";
        std::cerr<<MOOS::ConsoleColours::reset();
        char answer;
        std::cin>>answer;
        if(answer!='y')
            return 0;

        hops=1;
    }

    MOOS::MulticastNode FDUPA;
    FDUPA.Configure(multicast_address,multicast_port,hops);
    FDUPA.Run(true,true);



    std::string action =  bQuery ? "?" : "K";

    if(bQuery)
    {
        std::cerr<<"searching for suicidal sleepers on "<<multicast_address<<":"<< multicast_port<<"\n";
    }
    else
    {
      //std::cerr<<"sending suicide instruction to "<<multicast_address<<":"<< multicast_port<<"\n";
      // Change by mikerb Apr 09, 2021
      std::cerr<<"sending self-exit instruction to "<<multicast_address<<":"<< multicast_port<<"\n";
    }

    std::string command = action+":"+phrase+":"+name_pattern;

    FDUPA.Write(command.c_str());

    std::string sReply;
    while(FDUPA.Read(sReply,1000))
    {
        if(sReply==command)
            continue;

        std::cout<<(bQuery?MOOS::ConsoleColours::Yellow(): MOOS::ConsoleColours::Red())<<sReply;

    }
    std::cout<<MOOS::ConsoleColours::reset();

    return 0;


}

