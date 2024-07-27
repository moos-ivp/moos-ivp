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
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
/*
 * ServerAudit.h
 *
 *  Created on: Oct 31, 2012
 *      Author: pnewman
 */

#ifndef SERVERAUDIT_H_
#define SERVERAUDIT_H_

/*
 *
 */

#include <string>
#include <map>
namespace MOOS {
#define DEFAULT_AUDIT_PORT 9090
class ServerAudit {
public:
	ServerAudit();
	virtual ~ServerAudit();
  bool AddStatistic(const std::string & sClient, unsigned int nBytes, unsigned int nMessages, double dfTime, bool bIncoming);
	bool Run(const std::string & destination_host = "localhost", unsigned int port = DEFAULT_AUDIT_PORT);
	bool Remove(const std::string & sClient);
	bool SetQuiet(bool bQuiet);
	bool AddTimingStatistic(const std::string & sClient,
                           double dfTransmitTime,
                           double dfReceiveTime);

    /**fill in a string which tells us all about client timing statistics.
     * @param sSummary has format clientname=a:b:c:d,.....
     * @param a recent latency in ms
     * @param b max latency in ms
     * @param c min latency in ms
     * @param d moving average latency
     */

	bool GetTimingStatisticSummary(std::string & sSummary);



	class Impl;
protected:
	Impl* Impl_;
};

}

#endif /* SERVERAUDIT_H_ */
