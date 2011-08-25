/*
 *  AGVHeadingSpeedTask.cpp
 *  MOOS
 *
 *  Created by pnewman on 06/02/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "AGVHeadingSpeedTask.h"
#include <algorithm>
bool CAGVHeadingSpeedTask::SetParam(string sParam, string sVal)
{
    if(MOOSStrCmp(sParam,"PoseSource"))
    {
        m_sPoseSource = sVal;
        return true;
    }
    return BASE::SetParam(sParam,sVal);
};


bool CAGVHeadingSpeedTask::RegularMailDelivery(double dfTimeNow)
{
    return true;
}


class FindNavYaw
{
public:
   bool operator()(CMOOSMsg & M) {return MOOSStrCmp(M.GetKey(),"NAV_YAW");};
};

bool CAGVHeadingSpeedTask::OnNewMail(MOOSMSG_LIST &NewMail)
{
    
    CMOOSMsg Msg;
    if(PeekMail(NewMail,m_sPoseSource,Msg))
    { 
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            std::vector<double> P;
            int nCols,nRows;
            if(MOOSValFromString(P,nRows,nCols, Msg.m_sVal, "Pose"))
            {
                m_YawDOF.SetCurrent(P[2],Msg.GetTime());
            }            
        }
    }

    //this is sneeky - we are going to steal NAV_YAW messages from our parents
    //to stop them using NAV_YAW
    MOOSMSG_LIST::iterator NewEnd =  std::remove_if(NewMail.begin(),NewMail.end(),FindNavYaw());
    NewMail.erase(NewEnd,NewMail.end());

    return BASE::OnNewMail(NewMail);
    
}


bool CAGVHeadingSpeedTask::GetRegistrations(STRING_LIST &List)
{
    List.push_back(m_sPoseSource);
    
    return BASE::GetRegistrations(List);
}
	
