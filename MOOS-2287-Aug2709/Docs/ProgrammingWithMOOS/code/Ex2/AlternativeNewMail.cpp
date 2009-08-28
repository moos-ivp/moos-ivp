/** an alternative OnNewMail using PeekMail and checking for stale
    messages */
bool CSimpleApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    double dfNow = MOOSTime();
    if(m_Comms.PeekMail(NewMail,"VehicleStatus",Msg,false,true))
    {
    if(!Msg.IsSkewed(dfNow))
    {
        OnVehicleStatus(Msg);
    }
    }
    if(m_Comms.PeekMail(NewMail,"Heading",Msg,false,true))
    {
    if(!Msg.IsSkewed(dfNow))
    {
        OnHeading(Msg);
    }
    }

    return true;
}
