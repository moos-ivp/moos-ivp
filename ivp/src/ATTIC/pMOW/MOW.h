#ifdef _WIN32
	#pragma warning(disable : 4786)
#endif


#ifndef MOWH
#define MOWH

#include <MOOSLIB/MOOSApp.h>
#include "Pings2Solution.h"
#include <vector>
class CMOW : public CMOOSApp
{

public:
    CMOW();

    bool Iterate();
    bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();	
    bool DoDrift();
    bool OnReset();
    bool OnPing(std::string sStr);


    std::string m_sNavPrefix;
    std::string m_sDriftPrefix;
    std::string m_sResetFlag;
    std::string m_sPingName;
    double m_dfXYDriftRate;
    double m_dfErrX;
    double m_dfErrY;
    double m_dfSV;
    std::vector<double> m_TATs;

    std::vector<CPings2Solution> m_PingSolvers;

protected:
	bool SolvePings(int nChannel);
};

#endif
