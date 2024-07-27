/*
 * MOOSApp.hxx
 *
 *  Created on: Aug 24, 2013
 *      Author: pnewman
 */

#ifndef MOOSAPP_HXX_
#define MOOSAPP_HXX_


template <class T>
bool CMOOSApp::AddActiveQueue(const std::string & sQueueName,
	T* Instance,bool (T::*memfunc)(CMOOSMsg &)  )
{
	return m_Comms.AddActiveQueue(sQueueName,Instance,memfunc);
}

template <class T>
bool CMOOSApp::AddMessageRouteToActiveQueue(const std::string & sQueueName,
				const std::string & sMsgName,
				T* Instance,bool (T::*memfunc)(CMOOSMsg &) )
{


	return m_Comms.AddMessageRouteToActiveQueue(sQueueName,sMsgName,Instance,memfunc);

}

template <class T>
bool CMOOSApp::GetParameterFromCommandLineOrConfigurationFile(std::string sOption, T & var,bool bPrependMinusMinusForCommandLine)
{
    T vF,vC;
    bool bFoundInFile = m_MissionReader.GetConfigurationParam(sOption,vF);

    if(bPrependMinusMinusForCommandLine)
        sOption = "--"+sOption;

    bool bFoundOnCommandLine = m_CommandLineParser.GetVariable(sOption,vC);
    if(bFoundOnCommandLine)
    {
        var=vC;
        return true;
    }
    if(bFoundInFile)
    {
        var=vF;
        return true;
    }
    return false;
}




#endif /* MOOSAPP_HXX_ */
