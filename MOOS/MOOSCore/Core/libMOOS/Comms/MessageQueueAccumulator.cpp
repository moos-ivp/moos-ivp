/*
 * MessageQueueAccumulator.cpp
 *
 *  Created on: Sep 8, 2013
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Comms/MessageQueueAccumulator.h"
#include <iostream>


namespace MOOS {

MessageQueueAccumulator::MessageQueueAccumulator() {
	max_stored_messages_ = 10;
	pClassMemberFunctionCallback_ = 0;
}

MessageQueueAccumulator::~MessageQueueAccumulator() {
	// TODO Auto-generated destructor stub
}


void MessageQueueAccumulator::Configure(std::vector<std::string > MsgNames)
{
	for(unsigned int k = 0;k<MsgNames.size();k++)
	{
		msg2queue_[MsgNames[k]] = k;
		std::deque<CMOOSMsg> q;
		store_.push_back(q);
	}
}

bool MessageQueueAccumulator::AddMessage(CMOOSMsg & M)
{

	if(msg2queue_.find(M.GetKey())==msg2queue_.end())
	{
		//we should never have been sent this message
		std::cerr<<"MessageQueueAccumulator never asked for "<<M.GetKey()<<"\n";
		return false;
	}

	unsigned int qnum = msg2queue_[M.GetKey()];
	if(qnum>=store_.size())
	{
		std::cerr<<M.GetKey()<<" maps to queue "<<qnum<<" which is out of range\n";
		return false;
	}

	store_[qnum].push_front(M);
	if(store_[qnum].size()>max_stored_messages_)
		store_[qnum].pop_back();

	//now see if we have a full house...
	std::map<std::string, unsigned int>::iterator q;

	bool bFullHouse = true;
	for(q=msg2queue_.begin();q!=msg2queue_.end();++q)
	{
		std::cerr<<" queue "<<q->first<<" has "<<store_[q->second].size()<<"stored\n";
		if(store_[q->second].empty())
			bFullHouse&=false; //no nothing to do
	}
	if(!bFullHouse)
		return true;

	//if we get here then fire our call back!
	std::vector<CMOOSMsg> mv(store_.size());
	for(unsigned int k = 0;k<store_.size();k++)
	{
		mv[k]=store_[k].front();
		store_[k].pop_front();
	}

	if(!pClassMemberFunctionCallback_)
		return false;

	return (*pClassMemberFunctionCallback_)(mv);


}


}
