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
 * ActiveMailQueue.h
 *
 *  Created on: Dec 6, 2012
 *      Author: pnewman
 */

#ifndef ACTIVEMAILQUEUE_H_
#define ACTIVEMAILQUEUE_H_
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/SafeList.h"

/*
 *
 */
namespace MOOS {

class ActiveMailQueue {
public:
	ActiveMailQueue(const std::string &sName);
	virtual ~ActiveMailQueue();
	bool Push(const CMOOSMsg & M);
    void SetCallback(bool (*pfn)(CMOOSMsg &M, void * pParamCaller), void * pCallerParam);
    bool DoWork();
    bool Stop();
    bool Start();
    std::string GetName();
protected:
	MOOS::SafeList<CMOOSMsg> queue_;

    /** the user supplied Callback*/
    bool (*pfn_)(CMOOSMsg &M, void* pParam);
    void * caller_param_;

    CMOOSThread thread_;

    //this is a mick name for the Queue
    std::string Name_;

};

}

#endif /* ACTIVEMAILQUEUE_H_ */
