



#include "MOOS/libMOOS/Comms/MOOSCommClient.h"

CMOOSCommClient theMOOS;


bool OnConnectToServer(void * pParam)
{
    return true;
}

void PrintHelp()
{
    MOOSTrace("uPoke Help:\n");
    MOOSTrace("\n general use :\n\t\"uPoke host portnumber msgName msgContent\"");
    MOOSTrace("\n use assumed MOOSDB settings (localhost 9000):\n\t\"uPoke -d msgName msgContent\"");
    MOOSTrace("\n\n\texamples:\n");
    MOOSTrace("\t   uPoke robots.ox.ac.uk 9010 BeSmart how=think,when=now\n");
    MOOSTrace("\t   uPoke robots.ox.ac.uk 9010 WorryLevel 0.99\n");
    MOOSTrace("\t   uPoke -d WorryLevel 0.99\n");

}

bool Send(CMOOSMsg  Msg)
{
    while(!theMOOS.IsConnected())
        MOOSPause(10);


    if(theMOOS.Post(Msg))
    {
        theMOOS.Register(Msg.GetKey(),0);

        MOOSMSG_LIST M;
        while(!theMOOS.Fetch(M))
        {
            MOOSPause(10);
        }
    }

    return true;
}

int main(int argc, char * argv[])
{
    //before we do anything....
    theMOOS.SetQuiet(true);
    theMOOS.SetOnConnectCallBack(OnConnectToServer,NULL);



    std::string sServer = "localhost";
    int nPort = 9000;
    std::string sName;

    std::vector<std::string> Arguments;

    for (unsigned int i = 1; i < argc; i++)
    {
        Arguments.push_back(argv[i]);
    }


    //uPoke -d msg content
    //uPoke localhost 9000 msg contents

    unsigned int k = 0;
    if (Arguments.size() == 3)
    {
        if (Arguments[k++] != "-d")
        {
            PrintHelp();
            exit(1);
        }

    }
    else if (Arguments.size() == 4)
    {

        sServer = Arguments[k++];

        if (!MOOSIsNumeric(Arguments[k]))
            return MOOSFail("second argument (port number) is not numeric");

        nPort = atoi(Arguments[k++].c_str());
    }
    else
    {
        PrintHelp();
        exit(1);
    }


    theMOOS.Run(sServer.c_str(), nPort, theMOOS.GetLocalIPAddress().c_str(),40);

    sName = Arguments[k++];

    bool bOK;
    if (MOOSIsNumeric(Arguments[k]))
    {
        bOK = Send(CMOOSMsg(MOOS_NOTIFY, sName, atof(Arguments[k].c_str()),
                MOOSTime()));
    }
    else
    {
        bOK =Send(CMOOSMsg(MOOS_NOTIFY, sName, Arguments[k].c_str(), MOOSTime()));
    }

    return bOK ? 0: 1;

}


