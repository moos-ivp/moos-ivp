
#include <MOOSLIB/MOOSApp.h>

class CB:public CMOOSApp
{
public:
    CB(){m_Spinner = "-/|\\";m_nTick = 0;}
    bool Run(const std::string & sName,
        const std::string & sMissionFile,
        const std::string & sPublish,
        const std::string & sWhat)
    {
        m_sPublish = sPublish;
        m_sWhat = sWhat;
        MOOSTrace("This is Example B:\n");
        
        MOOSTrace("The additional parameters are %s and %s\n",m_sPublish.c_str(),m_sWhat.c_str());
        return CMOOSApp::Run(sName.c_str(),sMissionFile.c_str());
    }
  
    bool Iterate()
    {
        MOOSTrace("Publishing \"%s\" with value \"%s\"",m_sPublish.c_str(),m_sWhat.c_str());
        m_Comms.Notify(m_sPublish,m_sWhat);
        MOOSTrace(" ( Iterate [%.8d] : %c)\r",m_nTick,m_Spinner[m_nTick++%3]);
        return true;
    }
protected:
    std::string m_Spinner,m_sPublish,m_sWhat;
    int m_nTick;
};

int main(int argc,char *argv[])
{
    const char * sMissionFile = "Mission.moos";
    const char * sMOOSName = "pTestAppB";
    const char * sPublish = "NotSet" ;
    const char * sWhat = "not set";
    switch(argc)
    {
    case 5:
        sWhat = argv[4];
    case 4:
        sPublish = argv[3];
    case 3:
        sMOOSName = argv[2];
    case 2:
        sMissionFile = argv[1];
    case 1:
        break;
    default:
        for(int i = 3;i<argc;i++)
        {
            MOOSTrace("arg[%d] = %s\n",i,argv[i]);
        }
    }
    CB B;
    B.Run(sMOOSName,sMissionFile,sPublish,sWhat);
}

