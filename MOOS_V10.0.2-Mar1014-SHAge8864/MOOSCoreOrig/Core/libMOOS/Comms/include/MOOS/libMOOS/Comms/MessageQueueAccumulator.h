/*
 * MessageQueueAccumulator.h
 *
 *  Created on: Sep 8, 2013
 *      Author: pnewman
 */

#ifndef MESSAGEQUEUEACCUMULATOR_H_
#define MESSAGEQUEUEACCUMULATOR_H_

#include "MOOS/libMOOS/Comms/MessageFunction.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include <deque>
#include <map>
#include <vector>
namespace MOOS {

class MessageQueueAccumulator {
public:
	MessageQueueAccumulator();
	virtual ~MessageQueueAccumulator();


	void Configure(std::vector<std::string > MsgNames);

	bool AddMessage(CMOOSMsg & M);

	template <class T>
    void SetCallback(T* Instance,bool (T::*memfunc)(std::vector<CMOOSMsg> &));


private:
	//given by MessageFunction.h
	//this is a pointer to a function in a class which
	//should be called be when a complete message set has been found
	MOOS::MsgFunctor* pClassMemberFunctionCallback_;

	std::map<std::string, unsigned int> msg2queue_;
	typedef std::vector<std::deque< CMOOSMsg > > store_t;
	store_t store_;

	unsigned int max_stored_messages_;
};


template <class T>
   void MessageQueueAccumulator::SetCallback(T* Instance,bool (T::*memfunc)(std::vector<CMOOSMsg> &))
   {
   		pClassMemberFunctionCallback_ = MOOS::BindMsgFunctor<T>(Instance,memfunc);
   }


}

#endif /* MESSAGEQUEUEACCUMULATOR_H_ */
