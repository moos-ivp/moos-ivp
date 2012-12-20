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
	ActiveMailQueue();
	virtual ~ActiveMailQueue();
	bool Push(const CMOOSMsg & M);
    void SetCallback(bool (*pfn)(CMOOSMsg &M, void * pParamCaller), void * pCallerParam);
    bool DoWork();
    bool Stop();
    bool Start();
protected:
	MOOS::SafeList<CMOOSMsg> queue_;

    /** the user supplied Callback*/
    bool (*pfn_)(CMOOSMsg &M, void* pParam);
    void * caller_param_;

    CMOOSThread thread_;

};

}

#endif /* ACTIVEMAILQUEUE_H_ */
