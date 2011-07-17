#include "VariableHeadingTask.h"

#define SETPOINT_TIMEOUT 5.0

CVariableHeadingTask::CVariableHeadingTask(void)
{
    m_YawDOF.SetDesired(0);
    m_dfThrust = 0;

    m_SetPointTimes["DESIRED_YAW"] = -1.0;
    m_SetPointTimes["DESIRED_THRUST"] = -1.0;

}

CVariableHeadingTask::~CVariableHeadingTask(void)
{
}


bool CVariableHeadingTask::OnNewMail(MOOSMSG_LIST &NewMail)
{

    CMOOSMsg Msg;
    if(PeekMail(NewMail,"VARIABLE_YAW",Msg))
    { 
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_YawDOF.SetDesired(Msg.GetDouble());
            m_SetPointTimes[Msg.GetKey()] = Msg.GetTime();
        }
    }
   

    if(PeekMail(NewMail,"VARIABLE_THRUST",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_dfThrust = Msg.GetDouble();
            m_SetPointTimes[Msg.GetKey()] = Msg.GetTime();
            m_bThrustSet = true;
        }
    }

    return BASE::OnNewMail(NewMail);

}
bool CVariableHeadingTask::Run(CPathAction &DesiredAction)
{
    return BASE::Run(DesiredAction);
}

bool CVariableHeadingTask::GetRegistrations(STRING_LIST &List)
{
    List.push_back("VARIABLE_YAW");
    List.push_back("VARIABLE_THRUST");

    return BASE::GetRegistrations(List);
}
bool CVariableHeadingTask::RegularMailDelivery(double dfTimeNow)
{
    std::map<std::string,double>::iterator q;
    
    for(q = m_SetPointTimes.begin();q!=m_SetPointTimes.end();q++)
    {
        double dfLastMail = q->second;
 
        //case 1: No mail at all...
        if(dfLastMail ==-1 && (dfTimeNow-GetStartTime() > SETPOINT_TIMEOUT))
            return MOOSFail("No %s setpoints received for %f seconds",q->first.c_str(),SETPOINT_TIMEOUT);

        //case 2: Had mail but it has stopped..
        if(dfLastMail>0 && dfTimeNow-dfLastMail>SETPOINT_TIMEOUT)
            return MOOSFail("No %s setpoints received for %f seconds",q->first.c_str(),SETPOINT_TIMEOUT);
    }

    return BASE::RegularMailDelivery(dfTimeNow);
}

