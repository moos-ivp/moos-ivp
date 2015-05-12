/*
 * SuicidalSleeper.cpp
 *
 *  Created on: Feb 4, 2014
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


#include "MOOS/libMOOS/Comms/SuicidalSleeper.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/Utils/IPV4Address.h"
#include "MOOS/libMOOS/Utils/ProcInfo.h"
#include "MOOS/libMOOS/Comms/MulticastNode.h"


namespace MOOS
{

bool SuicidalSleeper::_dispatch_(void * pParam)
{
    SuicidalSleeper* pMe = (SuicidalSleeper*)pParam;
    return pMe->Work();
}


std::string SuicidalSleeper::GetDefaultPassPhrase()
{
    return "elks have been know to waltz";
}

std::string SuicidalSleeper::GetDefaultMulticastAddress()
{
    return "224.1.1.3";
}
int SuicidalSleeper::GetDefaultMulticastPort()
{
    return 4000;
}


SuicidalSleeper::~SuicidalSleeper()
{}

SuicidalSleeper::SuicidalSleeper()
{
    multicast_group_IP_address_ = GetDefaultMulticastAddress();
    multicast_port_  = GetDefaultMulticastPort();
    pass_phrase_=GetDefaultPassPhrase();
    last_rights_callback_ = NULL;
    count_down_seconds_ = 3;
}

bool SuicidalSleeper::SetPassPhrase(const std::string & sPassPhrase)
{
    pass_phrase_=sPassPhrase;
    return true;
}
bool SuicidalSleeper::SetChannel(const std::string & sAddress)
{
    multicast_group_IP_address_ = sAddress;
    return true;
}


bool SuicidalSleeper::SetPort(int nPort)
{
    multicast_port_=nPort;
    return true;
}

bool SuicidalSleeper::SetName(const std::string & name)
{
    name_= name;
    return true;
}


std::string SuicidalSleeper::GetPassPhrase()
{
    return pass_phrase_;
}
std::string SuicidalSleeper::GetChannel()
{
    return multicast_group_IP_address_;
}
int SuicidalSleeper::GetPort()
{
    return multicast_port_;
}



bool SuicidalSleeper::Run()
{
    thread_.Initialise(_dispatch_, this);
    return thread_.Start();
}

bool DoCountDown(void * pParam)
{
    unsigned int * pk = (unsigned int *)pParam;
    unsigned int k = *pk;
    while(k)
    {
        std::cerr<<MOOS::ConsoleColours::Red();
        std::cerr<<"    "<<k<<" SECONDS TO LIVE\r";
        std::cerr<<MOOS::ConsoleColours::reset();
        MOOSPause(1000);
        k--;
    }

    //BANG!!!
    exit(0);

    return false;
}

bool SuicidalSleeper::Work()
{
    MOOS::MulticastNode MCN;
    MCN.Configure(multicast_group_IP_address_,multicast_port_);
    MCN.Run(true,true);
    while(!thread_.IsQuitRequested())
    {
        std::string msg;
        if(MCN.Read(msg,500))
        {

            std::string Action=MOOSChomp(msg,":");
            std::string Phrase=MOOSChomp(msg,":");
            std::string AppPattern = MOOSChomp(msg,"\n");

            if(Phrase!=pass_phrase_ || !MOOSWildCmp(AppPattern,name_))
                continue;

            std::string sHeader = MOOSFormat("%-20s pid %-7d on %s",
                                             name_.c_str(),
                                             MOOS::ProcInfo::GetPid(),
                                             MOOS::IPV4Address::GetIPAddress().c_str());

            if(Action=="?")
            {
                std::string response = MOOSFormat("%s would die\n",sHeader.c_str());
                MCN.Write(response);
            }
            else if(Action=="K")
            {
                std::cerr<<MOOS::ConsoleColours::Red()<<"\n ** Received suicide instruction **\n";

                std::string response = MOOSFormat("%s is committing suicide\n",sHeader.c_str());
                MCN.Write(response);

                CMOOSThread count_down;
                count_down.Initialise(DoCountDown,&count_down_seconds_);
                count_down.Start();
                if(last_rights_callback_)
                {
                    std::string user_message;
                    if((*last_rights_callback_)(user_message))
                    {
                        std::cerr<<"last rights message is "<<user_message<<"\n";
                        MCN.Write(MOOSFormat("%s last words are \"%s\"\n",
                                             sHeader.c_str(),
                                             user_message.c_str()));
                    }
                }

                count_down.Stop();
                exit(0);
            }
        }
    }




    return true;
}


};//namespace MOOS




