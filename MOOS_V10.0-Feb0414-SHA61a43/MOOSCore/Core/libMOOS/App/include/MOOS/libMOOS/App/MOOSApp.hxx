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



#endif /* MOOSAPP_HXX_ */
