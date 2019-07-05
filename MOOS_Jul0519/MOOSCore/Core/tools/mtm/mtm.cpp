#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/App/MOOSApp.h"


#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>

unsigned int GetScreenWidth()
{
   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);
   return ws.ws_col;
}
#else
unsigned int GetScreenWidth()
{
    return 80;
}
#endif



class MonitorTheMOOS :  public CMOOSApp
{
    bool OnConnectToServer()
    {
        return Register("DB_VARSUMMARY",0.0);
    }
    bool OnSummary(CMOOSMsg & M)
    {
        std::string sAll = M.GetString();
        while(!sAll.empty())
        {
            std::string sLine = MOOSChomp(sAll,"\n");
            std::string sVar = MOOSChomp(sLine," ");

            if(known_.find(sVar)==known_.end())
            {
                std::cout<<MOOS::ConsoleColours::Magenta();
            }
            else if (known_[sVar]!=sLine)
            {
                std::cout<<MOOS::ConsoleColours::Green();
            }
            else if(live_only_)
            {
                continue;
            }
            std::string output =sVar+" "+sLine;

            if(output.size()>GetScreenWidth()-3)
                output=output.substr(0,GetScreenWidth()-3)+"...";

            std::cout<<output<<std::endl<<MOOS::ConsoleColours::reset();

            known_[sVar]=sLine;

        }
        std::cout<<std::endl;
        return true;
    }

    void OnPrintHelpAndExit()
    {
        std::cout<<"  --live_only                 : only display updating variables\n";
    }

    bool OnStartUp()
    {
        live_only_=m_CommandLineParser.GetFlag("--live_only");
        SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);

        AddActiveQueue("summary",this,&MonitorTheMOOS::OnSummary);
        return AddMessageRouteToActiveQueue("summary","DB_VARSUMMARY");
    }

protected:
    std::map<std::string,std::string> known_;
    bool live_only_;

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
    ss<<"mtm-"<< rand() %1024;
    std::string default_name = ss.str();
    std::string app_name = P.GetFreeParameter(1, default_name);



    MonitorTheMOOS mtm;
    mtm.Run(app_name,argc,argv);
}
