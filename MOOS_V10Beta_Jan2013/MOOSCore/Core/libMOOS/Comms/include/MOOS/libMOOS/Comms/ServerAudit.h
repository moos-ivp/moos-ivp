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
	bool AddStatistic(const std::string sClient, unsigned int nBytes, unsigned int nMessages, double dfTime, bool bIncoming);
	bool Run(const std::string & destination_host = "localhost", unsigned int port = DEFAULT_AUDIT_PORT);
	bool Remove(const std::string & sClient);

	class Impl;
protected:
	Impl* Impl_;
};

}

#endif /* SERVERAUDIT_H_ */
