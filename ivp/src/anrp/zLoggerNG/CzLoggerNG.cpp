// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/logger/zLoggerNG/CzLoggerNG.cpp,v 1.3 2006/04/25 16:31:42 anrp Exp $
// (c) 2005

// CzLoggerNG.cpp: implementation of the CzLoggerNG class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CzLoggerNG.h"
#include <signal.h>
#include <stdexcept>
#include "esc_spc.h"
#include "dtime.h"
#include "tokenize.h"

CzLoggerNG *__sh_ptr;

CzLoggerNG::CzLoggerNG()
{
	// constructor
	__sh_ptr = this;
	__sh_val = 0;
	pthread_mutex_init(&__sh_mutex, NULL);
	pthread_mutex_init(&mtw_m, NULL);
	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigint_handler);
	dbStartTime = -1;
	firstHit = false;
	lastFlush = 0;
}

CzLoggerNG::~CzLoggerNG()
{
	// destructor
}

bool CzLoggerNG::OnNewMail(MOOSMSG_LIST &NewMail)
{
	mtw_lock();
	NewMail.reverse();
	messages_to_write.splice(messages_to_write.end(), NewMail);
	mtw_unlock();

	NewMail.clear();

	return true;
}

void CzLoggerNG::WriteOutMessage(CMOOSMsg &msg)
{
	if(message_times[msg.m_sKey] == msg.m_dfTime || msg.m_dfTime == -1) {
		return; // uninteresting
	} else {
		message_times[msg.m_sKey] = msg.m_dfTime;
	}

	char tmp[65536];
	snprintf(tmp, 1024, "N\nV%s\nS%s\nW%f\n", esc_spc(msg.m_sKey).c_str(), esc_spc(msg.m_sSrc).c_str(), msg.m_dfTime);
	wrth.AppendWrite(string(tmp));

	switch(msg.m_cDataType) {
	case MOOS_STRING:
		snprintf(tmp, 65535, "TS\nC%s\n", esc_spc(msg.m_sVal).c_str());
		break;
	case MOOS_DOUBLE:
		snprintf(tmp, 65535, "TD\nC%lf\n", msg.m_dfVal);
		break;
	default:
		snprintf(tmp, 65535, "TU\nC\n");
		break;
	}

	wrth.AppendWrite(string(tmp));
}
	

bool CzLoggerNG::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet
	
	MOOSTrace("\n");
	
	if(!m_MissionReader.GetConfigurationParam("LogPath", logPath)) {
		MOOSTrace("!>> Warning: no log path, using ``.\'\'\n");
		logPath = ".";
	}
	
	if(!m_MissionReader.GetConfigurationParam("LogName", logBaseName)) {
		printf("!>> Warning: no log name, using log_%%DATE%%_%%TIME%%.zlog\n");         
		logBaseName = "log_%DATE%_%TIME%.zlog";
	}
	
	lastLogReset = dtime();
	OpenNewLogFile();

	return true;
}

bool CzLoggerNG::Iterate()
{
	// happens AppTick times per second
	MOOSMSG_LIST ml;
	if(m_Comms.ServerRequest("VAR_SUMMARY", ml, 2, false)) {
		MOOSMSG_LIST::iterator p;
		for(p=ml.begin(); p!= ml.end(); p++) {

			vector<string> vars = explode(p->m_sVal, ",");

			for(vector<string>::iterator it = vars.begin(); it != vars.end(); it++) {
				if(registered_vars.find(*it) == registered_vars.end()) {
					registered_vars[*it] = true;
					m_Comms.Register(*it, 0);
				}
			}
		}
	}

	mtw_lock();
	messages_to_write.splice(messages_to_write.end(), ml);
	mtw_unlock();

	struct timeval tv; struct timezone tz;
	gettimeofday(&tv, &tz);
	
	mtw_lock();
	for(MOOSMSG_LIST::iterator p = messages_to_write.begin(); p != messages_to_write.end(); p++) {
                if(!(*p).IsSkewed(MOOSTime()) && (*p).m_sKey == "LOGGER_RESTART" && (*p).m_dfTime > lastLogReset) {
                        OpenNewLogFile();
			lastLogReset = (*p).m_dfTime;
                }
        }

        if(firstHit) {
		char tmp[1000];
		snprintf(tmp, 1000, "zLogger 1 %f %i.%06i\n", dbStartTime, tv.tv_sec, tv.tv_usec);
		wrth.AppendWrite(string(tmp));
		snprintf(tmp, 1000, "H%i.%06i\n", tv.tv_sec, tv.tv_usec);
		wrth.AppendWrite(string(tmp));
		firstHit = false;
	}
	
	WriteOutMessageList(messages_to_write);
	messages_to_write.clear();

	mtw_unlock();

	if(lastFlush + 30.0 < dtime()) {
		lastFlush = dtime();
		printf("Flushing\n");
		wrth.Flush();
	}


	if(__sh_val) { // signal received. time to go
		throw runtime_error("Time to exit!");
	}

	return true;
}

void CzLoggerNG::WriteOutMessageList(MOOSMSG_LIST ml)
{
	for(MOOSMSG_LIST::iterator p = ml.begin(); p != ml.end(); p++) {
		WriteOutMessage(*p);
	}
}

void CzLoggerNG::OpenNewLogFile()
{
	string n = inp_logname_to_str(logBaseName);

	wrth.OpenNewFile(n);

	firstHit = true;
}

bool CzLoggerNG::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	// do initial scrape
	MOOSMSG_LIST ml;
	if(m_Comms.ServerRequest("ALL", ml)) {
		MOOSMSG_LIST::iterator p;
		for(p=ml.begin(); p!= ml.end(); p++) {
			if(registered_vars.find(p->m_sKey) == registered_vars.end()) {
				registered_vars[p->m_sKey] = true;
				if(p->m_sKey != "DB_TIME") {
					m_Comms.Register(p->m_sKey, 0);
				}
			}
		}
	}

	mtw_lock();
	// causes dupes
	//messages_to_write.splice(messages_to_write.end(), ml);
	mtw_unlock();
	
	return true;
}

static void f_and_r(string &s1, const string s2, const string s3)
{
	while(s1.find(s2, 0) != string::npos) {
		unsigned int loc = s1.find(s2);
		s1.erase(loc, s2.length());
		s1.insert(loc, s3);
	}
}

string CzLoggerNG::inp_logname_to_str(string input)
{
	char ldate[100], ltime[100], lunixts[100];
	time_t lt; struct tm ltm;
	string d, t, u;
	string output;

	time(&lt); gmtime_r(&lt, &ltm);

	strftime(ldate, 100, "%Y-%m-%d", &ltm);
	strftime(ltime, 100, "%H%M%S", &ltm);
	strftime(lunixts, 100, "%s", &ltm);

	d = ldate; t = ltime; u = lunixts;

	output = logPath + "/" + input;

	f_and_r(output, "%DATE%", d);
	f_and_r(output, "%TIME%", t);
	f_and_r(output, "%UNIXTS%", u);

	while(output.find("%MOOSVar(") != string::npos) {
		unsigned int loc = output.find("%MOOSVar(");
		output.erase(loc, string("%MOOSVar(").length());
		unsigned int loc2 = output.find(")%");
		string nVar = output.substr(loc, loc2 - loc);
		output.erase(loc, nVar.length() + 2);

		string nVal;
		if(!m_MissionReader.GetValue(nVar.c_str(), nVal)){
			MOOSTrace("!!> Non-existant variable %s!\n", nVar.c_str());
			throw;
		} else {
			output.insert(loc, nVal);
		}
	}

	return output;
}

void CzLoggerNG::sigint_handler(int a)
{
	pthread_mutex_lock(&(__sh_ptr->__sh_mutex));
	if(__sh_ptr->__sh_val == 1) {
		pthread_mutex_unlock(&(__sh_ptr->__sh_mutex));
		return;
	}
	__sh_ptr->__sh_val = 1;
	MOOSTrace("!>> Caught signal %i (%s)\n", a, a == 2? "interrupt" : a == 15? "terminate" : "unknown");
	
	pthread_mutex_unlock(&(__sh_ptr->__sh_mutex));
}
