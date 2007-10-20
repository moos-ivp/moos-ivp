// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/logger/zLoggerNG/CzLoggerNG.h,v 1.1 2006/04/10 04:43:15 anrp Exp $
// (c) 2005 
// CzLoggerNG.h: interface for the CzLoggerNG class.
////////////////////////////////////////////////

#ifndef __CzLoggerNG_h__
#define __CzLoggerNG_h__

#include "MOOSLib.h"
#include <map>
#include <string>
#include "WriterThread.h"

using namespace std;

class CzLoggerNG : public CMOOSApp
{
public:
	CzLoggerNG();
	virtual ~CzLoggerNG();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	map<string,bool> registered_vars;
	map<string,double> message_times;

	// signal hander stuff
	pthread_mutex_t __sh_mutex;
	int __sh_val;
	static void sigint_handler(int);
	
	string inp_logname_to_str(string);
	string logPath;
	string logBaseName;
	double dbStartTime;

	void OpenNewLogFile();

	WriterThread wrth;

	void WriteOutMessage(CMOOSMsg &);
	void WriteOutMessageList(MOOSMSG_LIST);
	
	pthread_mutex_t mtw_m;
	void mtw_lock() { pthread_mutex_lock(&mtw_m); }
	void mtw_unlock() { pthread_mutex_unlock(&mtw_m); }
	MOOSMSG_LIST messages_to_write;
	bool firstHit;
	double lastLogReset;

	double lastFlush;
};

#endif /* __CzLoggerNG_h__ */
