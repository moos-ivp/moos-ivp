#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/App/MOOSApp.h"



class MonitorMOOSQoS :  public CMOOSApp
{
public:
    MonitorMOOSQoS()
    {
        m_Comms.SetQuiet(true);
        SetQuiet(true);
    }
    bool OnConnectToServer()
    {
        m_Comms.EnableCommsStatusMonitoring(true);
        return true;
    }

    bool Iterate()
    {
        std::list<MOOS::ClientCommsStatus>  Statuses;
        std::list<MOOS::ClientCommsStatus>::iterator q;
        m_Comms.GetClientCommsStatuses(Statuses);

        for(q=Statuses.begin();q!= Statuses.end();++q)
        {
            std::vector<std::string>::iterator p;
            for(p=Wildcard_names_.begin();p!= Wildcard_names_.end();++p)
            {
                if(MOOSWildCmp(*p,q->name_))
                {
                    q->Write(std::cerr);
                }
            }
        }
        return true;

    }

    void OnPrintHelpAndExit()
    {
        MOOSTrace("  Print out QoS and comms information for community");
        MOOSTrace("\n\noptions:\n");
        MOOSTrace("  --pattern=<string>     : globbing pattern for client name\n");
        MOOSTrace("\n\nexample:\n");
        MOOSTrace("  ./mqos --pattern='foo*'\n");
        exit(0);
    }

    bool OnProcessCommandLine()
    {
        std::string t;
        if(m_CommandLineParser.GetVariable("--pattern",t))
            Wildcard_names_=MOOS::StringListToVector(t,",");
        else
            Wildcard_names_.push_back("*");
        return true;
    }
    bool OnStartUp()
    {
        SetAppFreq(1.0);
        return true;
    }

protected:
    std::vector<std::string> Wildcard_names_;
};


int main(int argc , char* argv[])
{
    //here we do some command line parsing...
    MOOS::CommandLineParser P(argc,argv);

    //we may want many instances run from command line so lets guess
    //a random name. This is just to stop users having to specify
    //--moos_name at the command line lots and lots...
    std::stringstream ss;
    srand ( static_cast<unsigned int>( time(NULL)) );
    ss<<"mqos-"<< rand() %1024;
    std::string default_name = ss.str();
    std::string app_name = P.GetFreeParameter(1, default_name);



    MonitorMOOSQoS mtm;
    mtm.Run(app_name,argc,argv);
}
