
///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of 
//   Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and 
//   the University of Oxford 2003-2013 
//   
//   email: pnewman@robots.ox.ac.uk. 
//              
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////





/*
 * ActiveMailQueue.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Comms/ActiveMailQueue.h"
#include <iostream>

namespace MOOS {


bool dispatch(void * pParam)
{
	ActiveMailQueue* pMe = static_cast<ActiveMailQueue*> (pParam);
	return pMe->DoWork();
}

ActiveMailQueue::ActiveMailQueue(const std::string & Name) : Name_(Name)
{
	// TODO Auto-generated constructor stub
	pClassMemberFunctionCallback_ = NULL;
	pfn_ = NULL;


}

ActiveMailQueue::~ActiveMailQueue() {
	// TODO Auto-generated destructor stub
	Stop();
}

std::string ActiveMailQueue::GetName()
{
	return Name_;
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
			{
				//now there are two ways to register a callback
				//you can install a MOOS::MsgFunctor
				//which lets you point to a memebr function of
				//another class!
				if(pClassMemberFunctionCallback_)
				{
                    if(!(*pClassMemberFunctionCallback_)(M))
					{
						std::cerr<<"ActiveMailQueue::DoWork() user callback returns false\n";
					}
				}

				//or you can have an old style cfunction
				if(pfn_)
                {
                    if(!(*pfn_)(M,caller_param_))
					{
						std::cerr<<"ActiveMailQueue::DoWork() user callback returns false\n";
					}

				}
				break;
			}
		}

	}
	return true;
}


bool ActiveMailQueue::IsRunning()
{
    return thread_.IsThreadRunning();
}

void ActiveMailQueue::SetCallback(bool (*pfn)(CMOOSMsg &M, void * pParam), void * pCallerParam)
{
	pClassMemberFunctionCallback_ = NULL;

	pfn_=pfn;
	caller_param_ = pCallerParam;

	//std::cerr<<"set pfn to "<<std::hex<<(void *)pfn_<<"\n";
}


}
