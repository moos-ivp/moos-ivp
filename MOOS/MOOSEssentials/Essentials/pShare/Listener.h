/*
 * MulticastListener.h
 *
 *  Created on: Aug 27, 2012
 *      Author: pnewman
 */

#ifndef MULTICASTLISTENER_H_
#define MULTICASTLISTENER_H_

/*
 *
 */
#include "MOOS/libMOOS/Utils/SafeList.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include "MOOS/libMOOS/Utils/IPV4Address.h"

namespace MOOS {

class Listener {
public:

	Listener(SafeList<CMOOSMsg> & queue, const MOOS::IPV4Address & address,bool multicast, const std::vector<std::string> & white_list);
	virtual ~Listener();
	bool Run();
	std::string host(){return address_.host();};
	unsigned int port(){return address_.port();};
	bool multicast(){return multicast_;};
protected:
	bool ListenLoop();
	bool IsWanted(const CMOOSMsg & msg);
	CMOOSThread thread_;
	SafeList<CMOOSMsg > & queue_;

	IPV4Address address_;

	bool multicast_;

	std::vector<std::string> white_list_;

public:
	static bool dispatch(void * pParam)
	{
		Listener* pMe = (Listener*)pParam;
		return pMe->ListenLoop();
	}


};

}

#endif /* MULTICASTLISTENER_H_ */
