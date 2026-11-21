/*
 * MOOSCommClient.hxx
 *
 *  Created on: Aug 24, 2013
 *      Author: pnewman
 */

#ifndef MOOSCOMMCLIENT_HXX_
#define MOOSCOMMCLIENT_HXX_

#include "MOOS/libMOOS/Utils/MOOSScopedLock.h"

template <class T>
bool CMOOSCommClient::AddMessageRouteToActiveQueue(const std::string & sQueueName,
				const std::string & sMsgName,
				T* Instance,bool (T::*memfunc)(CMOOSMsg &) )
{
	if(!HasActiveQueue(sQueueName))
		AddActiveQueue(sQueueName,Instance,memfunc);

	return AddMessageRouteToActiveQueue(sQueueName,sMsgName);

}

template <class T>
bool CMOOSCommClient::AddActiveQueue(const std::string & sQueueName,
		T* Instance,bool (T::*memfunc)(CMOOSMsg &)  )
{
	MOOS::ScopedLock L(ActiveQueuesLock_);

	std::map<std::string,MOOS::ActiveMailQueue*>::iterator w =  ActiveQueueMap_.find(sQueueName);
	if(w==ActiveQueueMap_.end())
	{
		//we need to create a new queue
		MOOS::ActiveMailQueue* pQ = new MOOS::ActiveMailQueue(sQueueName);
		ActiveQueueMap_[sQueueName] = pQ;
		pQ->SetCallback(Instance,memfunc);
		pQ->Start();
		return true;
	}
	else
	{
		std::cerr<<"warning active queue "<<sQueueName<<" already exists\n";
		return false;
	}

}


template <class T>
bool CMOOSCommClient::AddWildcardActiveQueue(const std::string & sQueueName,
				const std::string & sPattern,
	    		T* Instance,
	    		bool (T::*memfunc)(CMOOSMsg &)  )
{

	if(!AddActiveQueue(sQueueName,Instance,memfunc))
		return false;

	MOOS::ScopedLock L(ActiveQueuesLock_);

	WildcardQueuePatterns_[sQueueName]=sPattern;

	//now we had better see if the wildcard queue is interested
	//in any messages we have already seen
	std::set< std::string>::iterator q;

	for(q=WildcardCheckSet_.begin();q!=WildcardCheckSet_.end();++q)
	{
		if(MOOSWildCmp(sPattern,*q))
		{
			Msg2ActiveQueueName_[*q].insert(sQueueName);
		}
	}


	return true;

}


#endif /* MOOSCOMMCLIENT_HXX_ */
