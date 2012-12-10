/*
 * ActiveMailQueue.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Comms/ActiveMailQueue.h"

namespace MOOS {


bool dispatch(void * pParam)
{
	ActiveMailQueue* pMe = static_cast<ActiveMailQueue*> (pParam);
	return pMe->DoWork();
}

ActiveMailQueue::ActiveMailQueue()
{
	// TODO Auto-generated constructor stub

}

ActiveMailQueue::~ActiveMailQueue() {
	// TODO Auto-generated destructor stub
	Stop();
}

bool ActiveMailQueue::Start()
{
	thread_.Initialise(dispatch,this);
	return thread_.Start();
}

bool ActiveMailQueue::Stop()
{
	CMOOSMsg M(MOOS_TERMINATE_CONNECTION,"","");
	Push(M);
	return thread_.Stop();
}


bool ActiveMailQueue::Push(const CMOOSMsg & M)
{
	queue_.Push(M);
	return true;
}

bool ActiveMailQueue::DoWork()
{
	while(!thread_.IsQuitRequested())
	{
		CMOOSMsg M;
		while(queue_.IsEmpty())
		{
			queue_.WaitForPush(1000);
		}
		queue_.Pull(M);

		switch(M.GetType())
		{
			case MOOS_TERMINATE_CONNECTION:
				return true;
			case MOOS_NOTIFY:
				(*pfn_)(M,caller_param_);
				break;
		}

	}
	return true;
}

void ActiveMailQueue::SetCallback(bool (*pfn)(CMOOSMsg &M, void * pParam), void * pCallerParam)
{
	pfn_=pfn;
	caller_param_ = pCallerParam;
}


}
