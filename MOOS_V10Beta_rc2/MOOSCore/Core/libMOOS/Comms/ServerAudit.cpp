/**
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
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/




/*
 * ServerAudit.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Comms/ServerAudit.h"
#include "MOOS/libMOOS/Comms/XPCUdpSocket.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace MOOS {



struct ClientAudit
{
	long long unsigned int total_received_;
	long long unsigned int total_sent_;
	long long unsigned int recently_received_;
	long long unsigned int recently_sent_;
	long long unsigned int max_size_received_;
	long long unsigned int max_size_sent_;
	long long unsigned int min_size_received_;
	long long unsigned int min_size_sent_;
	int recent_packets_received_;
	int recent_packets_sent_;
	int recent_messages_sent_;
	int recent_messages_received_;

	void ClearAll()
	{
		total_received_ = 0;
		total_sent_ = 0;
		recently_received_= 0;
		recently_sent_ = 0;
		max_size_received_ = 0;
		max_size_sent_ = 0;
		min_size_received_ = 0;
		min_size_sent_ = 0;
		recent_packets_received_ = 0;
		recent_packets_sent_ = 0;
		recent_messages_received_=0;
		recent_messages_sent_=0;

	}
	void ClearRecents()
	{
		recently_received_= 0;
		recently_sent_ = 0;
		recent_packets_received_ = 0;
		recent_packets_sent_ = 0;
		recent_messages_received_=0;
		recent_messages_sent_=0;
	}

};

bool AuditDispatch(void * pParam);

class ServerAudit::Impl
{
public:
	Impl()
	{
		thread_.Initialise(AuditDispatch,this);
	}

	~Impl()
	{
		thread_.Stop();
	}
	bool Run(const std::string & destination_host, unsigned int port)
	{
		destination_host_ = destination_host;
		destination_port_ = port;

		std::cout<<MOOS::ConsoleColours::Yellow()<<"network performance data published on "<<destination_host_<<":"<<destination_port_<<"\n";
		std::cout<<"listen with \"nc -u -lk "<<destination_port_<<"\"\n";

		return thread_.Start();
	}

	bool Work()
	{

		XPCUdpSocket output(0);

		while(!thread_.IsQuitRequested())
		{
			//we shall run once a second
			MOOSPause(1000);

			std::stringstream ss;

			long long unsigned int total_in = 0;
			long long unsigned int total_out = 0;
			long long unsigned int total_packets_in = 0;
			long long unsigned int total_packets_out = 0;
			long long unsigned int total_messages_in = 0;
			long long unsigned int total_messages_out = 0;

			lock_.Lock();
			{

				ss<<std::endl<<std::setw(12)<<"client name"<<std::setw(10)
					<<"pkts in"<<std::setw(10)
					<<"pkts out"<<std::setw(10)
					<<"msgs in"<<std::setw(10)
					<<"msgs out"<<std::setw(10)
					<<"B/s in"<<std::setw(10)
					<<"B/s out\n";

				std::map<std::string,ClientAudit>::iterator q;
				for(q=Audits_.begin(); q!=Audits_.end();q++)
				{
					ss<<std::setw(10)<<q->first;
					ss<<std::setw(10)<<q->second.recent_packets_received_;
					ss<<std::setw(10)<<q->second.recent_packets_sent_;
					ss<<std::setw(10)<<q->second.recent_messages_received_;
					ss<<std::setw(10)<<q->second.recent_messages_sent_;
					ss<<std::setw(10)<<q->second.recently_received_;
					ss<<std::setw(10)<<q->second.recently_sent_;
					ss<<std::endl;

					total_in+=q->second.recently_received_;
					total_out+=q->second.recently_sent_;
					total_packets_in+=q->second.recent_packets_received_;
					total_packets_out+=q->second.recent_packets_sent_;
					total_messages_in+=q->second.recent_messages_received_;
					total_messages_out+=q->second.recent_messages_sent_;

					q->second.ClearRecents();
				}

				//print totals
				ss<<std::setw(10)<<"total";
				ss<<std::setw(10)<<total_packets_in;
				ss<<std::setw(10)<<total_packets_out;
				ss<<std::setw(10)<<total_messages_in;
				ss<<std::setw(10)<<total_messages_out;
				ss<<std::setw(10)<<total_in;
				ss<<std::setw(10)<<total_out;
				ss<<std::endl;



			}
			lock_.UnLock();

			std::string s = ss.str();

			try
			{
				unsigned int sent=0;
				while(sent<s.size())
				{
					//note we send in multiple chunks to support some versions of netcat
					//which appear to only listne to 1024 chunks!
					std::string little_bit = s.substr(sent,1024);
					sent+= output.iSendMessageTo((void*)little_bit.data(),
							little_bit.size(),
							destination_port_,
							destination_host_);
				}

			}
			catch(const XPCException & e)
			{
				//this is bad news! can't send audits
				std::cerr<<"failed to send audit - won't try again\n";
				return false;
			}
		}

		return true;
	}

	bool Remove(const std::string & sClient)
	{
		lock_.Lock();
		std::map<std::string,ClientAudit>::iterator q = Audits_.find(sClient);
		if(q!=Audits_.end())
			Audits_.erase(q);
		lock_.UnLock();
		return true;
	}

	bool AddStatistic(const std::string sClient, unsigned int nBytes, unsigned int nMessages, double dfTime, bool bIncoming)
	{
		lock_.Lock();
		ClientAudit & rA = Audits_[sClient];
		if(bIncoming)
		{
			rA.recently_received_+=nBytes;
			rA.total_received_+=nBytes;
			rA.max_size_received_=std::max<unsigned long long>(rA.max_size_received_,nBytes);
			rA.min_size_received_=std::min<unsigned long long>(rA.min_size_received_,nBytes);
			rA.recent_packets_received_+=1;
			rA.recent_messages_received_+=nMessages;


		}
		else
		{
			rA.recently_sent_+=nBytes;
			rA.total_sent_+=nBytes;
			rA.max_size_sent_=std::max<unsigned long long>(rA.max_size_received_,nBytes);
			rA.min_size_sent_=std::min<unsigned long long>(rA.min_size_received_,nBytes);
			rA.recent_packets_sent_+=1;
			rA.recent_messages_sent_+=nMessages;


		}
		lock_.UnLock();

		return true;
	}


	CMOOSThread thread_;
	CMOOSLock lock_;

	std::string destination_host_;
	unsigned int destination_port_;

	std::map<std::string,ClientAudit> Audits_;


};

bool AuditDispatch(void * pParam)
{
	MOOS::ServerAudit::Impl * pMe = (MOOS::ServerAudit::Impl *)pParam;
	return pMe->Work();
}


ServerAudit::ServerAudit():Impl_(new ServerAudit::Impl)
{
	// TODO Auto-generated constructor stub

}

ServerAudit::~ServerAudit() {
	// TODO Auto-generated destructor stub
	delete Impl_;
}

bool ServerAudit::Run(const std::string & destination_host, unsigned int port)
{
	return Impl_->Run(destination_host,port);
}

bool ServerAudit::Remove(const std::string & sClient)
{
	return Impl_->Remove(sClient);
}


bool ServerAudit::AddStatistic(const std::string sClient, unsigned int nBytes,unsigned int nMessages, double dfTime, bool bIncoming)
{

	return Impl_->AddStatistic(sClient,nBytes,nMessages,dfTime,bIncoming);
}


}
