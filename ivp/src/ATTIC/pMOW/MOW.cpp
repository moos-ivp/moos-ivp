#include "MOW.h"

#include <sstream>
#include <iomanip>


#define DEFAULT_NAV_PREFIX "GPS"
#define DEFAULT_DRIFT_PREFIX "DRIFT"
#define DEFAULT_XY_DRIFT_RATE 0.005
#define TOO_LONG_A_TIME 1.0
#define DEFAULT_RESET_FLAG "RESTART_HELM"
#define DEFAULT_PING_NAME "SIM_LBL_TOF"
#define DEFAULT_SV 1498.0
#define DEFAULT_TAT 0.125
#define NUM_ACOUSTIC_CHANNELS 16



CMOW::CMOW()
{
    m_dfSV = DEFAULT_SV;

    //we have a bank of acoutsic solvers
    m_PingSolvers.resize(NUM_ACOUSTIC_CHANNELS);

    //default Turnaround times
    m_TATs.resize(NUM_ACOUSTIC_CHANNELS,DEFAULT_TAT);


}

bool CMOW::Iterate()
{
    DoDrift();

    PublishFreshMOOSVariables();

    return true;
}

bool CMOW::OnNewMail(MOOSMSG_LIST &NewMail)
{
    //catch all
    UpdateMOOSVariables(NewMail);
    

    //specialised
    double dfNow = HPMOOSTime();

    CMOOSMsg Msg;

    //look for reset...
    if(m_Comms.PeekMail(NewMail,m_sResetFlag,Msg))
    {
        if(!Msg.IsSkewed(dfNow))
        {
            MOOSDebugWrite("Reseting Drift to zero");
            OnReset();
        }
    }

    //look for ping data
    while(m_Comms.PeekMail(NewMail,m_sPingName,Msg,true))
    {
        if(!Msg.IsSkewed(dfNow))
        {
            OnPing(Msg.GetString());
        }
    }

    return true;
}



bool CMOW::OnConnectToServer()
{
    if(!m_sResetFlag.empty())
        m_Comms.Register(m_sResetFlag,0);

    if(!m_sPingName.empty())
        m_Comms.Register(m_sPingName,0);


    RegisterMOOSVariables();
    return true;
}

bool CMOW::OnStartUp()
{
    m_dfErrX = 0;
    m_dfErrY = 0;

    m_sNavPrefix = DEFAULT_NAV_PREFIX;

    //how do we suck in position info
    if(!m_MissionReader.GetConfigurationParam("NavPrefix",m_sNavPrefix))
        MOOSTrace("No NavPrefix specified : assuming %s\n",m_sNavPrefix.c_str());

    AddMOOSVariable("X",m_sNavPrefix+"_X","",0.1);
    AddMOOSVariable("Y",m_sNavPrefix+"_Y","",0.1);
    AddMOOSVariable("YAW",m_sNavPrefix+"_YAW","",0.1);

    //add the target solution variable
    AddMOOSVariable("TARGETSOLUTION","","MOW_TARGET",0);


    //what name should drifted data be published under
    m_sDriftPrefix = DEFAULT_DRIFT_PREFIX;

    if(!m_MissionReader.GetConfigurationParam("DriftPrefix",m_sDriftPrefix))
        MOOSTrace("No DriftPrefix specified : assuming %s\n",m_sDriftPrefix.c_str());

    AddMOOSVariable("XDrift","",m_sDriftPrefix+"_X",0);
    AddMOOSVariable("YDrift","",m_sDriftPrefix+"_Y",0);
    AddMOOSVariable("YAWDrift","",m_sDriftPrefix+"_YAW",0);

    //what are the drift parameters in m/s r.m.s
    m_dfXYDriftRate = DEFAULT_XY_DRIFT_RATE;
    if(!m_MissionReader.GetConfigurationParam("XYDriftRate",m_dfXYDriftRate))
        MOOSTrace("No XYDriftRate specified : assuming %f\n",m_dfXYDriftRate);


    //what flag should be watched to cause a reset ?
    m_sResetFlag = DEFAULT_RESET_FLAG;
    if(!m_MissionReader.GetConfigurationParam("ResetFlag",m_sResetFlag))
        MOOSTrace("No ResetFlag specified : assuming %s\n",m_sResetFlag.c_str());
    //Register for it    
    m_Comms.Register(m_sResetFlag,0);


    m_sPingName = DEFAULT_PING_NAME;
    if(!m_MissionReader.GetConfigurationParam("PingName",m_sPingName))
        MOOSTrace("No ResetFlag specified : assuming %s\n",m_sResetFlag.c_str());
    //Register for it    
    m_Comms.Register(m_sPingName,0);

    //what sound spped to use
    if(!m_MissionReader.GetConfigurationParam("SV",m_dfSV))
        MOOSTrace("No ResetFlag specified : assuming %f\n",m_dfSV);


    RegisterMOOSVariables();
    return true;
}


bool CMOW::DoDrift()
{
    double dfNow = HPMOOSTime(); 
    double dfdT = dfNow-m_dfLastRunTime;
    
    //only add on sensible time differences
    //this avoids start up problems
    if(fabs(dfdT)>TOO_LONG_A_TIME)
        return false;

    m_dfErrX += dfdT*m_dfXYDriftRate*MOOSWhiteNoise(1);
    m_dfErrY += dfdT*m_dfXYDriftRate*MOOSWhiteNoise(1);

    if(GetMOOSVar("X")->IsFresh())
    {
        double dfX = GetMOOSVar("X")->GetDoubleVal();        
        SetMOOSVar("XDrift",dfX+m_dfErrX,dfNow);

        //add it to the ping solvers..
        for(int i = 0;i<m_PingSolvers.size();i++)
        {
            m_PingSolvers[i].AddX(dfX+m_dfErrX,dfNow);
        }
    }
    
    if(GetMOOSVar("Y")->IsFresh())
    {
        double dfY = GetMOOSVar("Y")->GetDoubleVal();
        SetMOOSVar("YDrift",dfY+m_dfErrY,dfNow);

        //add it to the ping solver..
        //add it to the ping solvers..
        for(int i = 0;i<m_PingSolvers.size();i++)
        {
            m_PingSolvers[i].AddY(dfY+m_dfErrY,dfNow);
        }
    }

    return true;
}


bool CMOW::OnReset()
{
   m_dfErrX = 0;
   m_dfErrY = 0;

   m_PingSolvers.clear();
   m_PingSolvers.resize(NUM_ACOUSTIC_CHANNELS);

   return true;
}


bool CMOW::OnPing(std::string sStr)
{
    //TOF = 0.1234,Ch[5] = 0.123
    double dfTOF,dfT;

    if(!MOOSValFromString(dfT,sStr,"Tx"))
        return MOOSFail("No \"TOF\" field in %s %s",sStr.c_str(),MOOSHERE);

    std::string sCpy = sStr;
    MOOSChomp(sCpy,"Ch[");
    int nChannel = atoi(sCpy.c_str());
    if(nChannel<=0)
        return MOOSFail("No Ch[xxx] in Ping String %s",MOOSHERE);

    if(nChannel>m_TATs.size()-1)
        return MOOSFail("No Ch[%d] understood %s",nChannel,MOOSHERE);

    if(!MOOSValFromString(dfTOF,sStr,MOOSFormat("Ch[%d]",nChannel)))
        return MOOSFail("No \"Ch[xxx]\" field in %s %s",sStr.c_str(),MOOSHERE);

    double dfR = 0.5*(dfTOF-m_TATs[nChannel])*m_dfSV;

    //finally we add it but only if it is temporaly bracketed by NAV data
    m_PingSolvers[nChannel].AddPing(dfR,dfT);

    return SolvePings(nChannel);

}

bool CMOW::SolvePings(int nChannel)
{
    std::list< CPings2Solution::Solution > Solutions;
    if(m_PingSolvers[nChannel].Solve(Solutions))
    {
        std::list< CPings2Solution::Solution >::iterator q;
        MOOSTrace("%d unique solutions :\n",Solutions.size());
        std::stringstream ss;

        //here we format a results string
        if(Solutions.size()>0)
        {
            ss<<"Channel="<<nChannel<<",";
            ss<<"Target=[2x1]{"<<Solutions.begin()->m_Location.first<<',';
            ss<<Solutions.begin()->m_Location.second<<"},";
         
            int n2Send = Solutions.size()>30?30:Solutions.size();
            ss<<"Scores=[3x"<<n2Send<<"]{";
            int n = 0;
            for(q = Solutions.begin();q!= Solutions.end()&&n<n2Send; q++,n++)
            {
                CPings2Solution::XY_LOCATION XY = q->m_Location;
                double dfX = XY.first;
                double dfY = XY.second;
                double dfScore = q->m_dfSSE;;
                
                //MOOSTrace("\t[X,Y] = {%f %f} Score = %f\n", dfX,dfY,dfScore);
                
                ss<<std::setprecision(4)<<dfX<<",";
                ss<<std::setprecision(4)<<dfY<<",";
                ss<<std::setprecision(4)<<dfScore<<",";
                
            }
            ss<<"}";
            
//            MOOSTrace(ss.str());

            SetMOOSVar("TARGETSOLUTION",ss.str(),MOOSTime());

        }

    }
    return true;
}
