// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/multi/MOOSBlink/CMOOSBlink.cpp,v 1.2 2007/01/31 20:13:48 anrp Exp $
// (c) 2004

// CMOOSBlink.cpp: implementation of the CMOOSBlink class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CMOOSBlink.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdexcept>

#include "tokenize.h"

using namespace std;

#define MOOSBRIDGE_PORT 31163

CMOOSBlink::CMOOSBlink()
{
	// constructor
	pthread_mutex_init(&lm, NULL);
	pthread_mutex_init(&wm, NULL);

	srand(time(NULL));
}

CMOOSBlink::~CMOOSBlink()
{
	// destructor
}

bool CMOOSBlink::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin(); p != NewMail.end(); p++) {
		L_w();
		out.push_back(*p);
		U_w();
	}

	NewMail.clear();
	
	return true;
}

bool CMOOSBlink::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet

	if(m_MissionReader.GetConfigurationParam("BroadcastAddr", broadcastaddr)) {
		// good
		MOOSTrace("Using configured broadcast addr of " + broadcastaddr);
	} else {
		MOOSTrace("Using default broadcast addr of 255.255.255.255");
		broadcastaddr = "255.255.255.255";
	}

	if(!m_MissionReader.GetValue("Community", communityname)) {
		throw runtime_error("No community set (?) exiting");
	}

	STRING_LIST sl;
	if(!m_MissionReader.GetConfiguration(m_MissionReader.GetAppName(), sl)) {
		throw runtime_error("No configuration block for "+ m_MissionReader.GetAppName());
	}

	for(STRING_LIST::iterator it = sl.begin(); it != sl.end(); it++) {
		vector<string> vs = explode(*it, "=");
		if(strcasecmp(vs[0].c_str(), "share") == 0) {
			if(vs.size() < 2) {
				printf("Broken config line\n");
			}
			vector<string> ps = explode(vs[1], ",");
			if(ps.size() >= 3) {
				printf("Source variable %s, destination variable %s, destination community %s\n", ps[1].c_str(), ps[2].c_str(), ps[0].c_str());

				if(ps.size() == 4) {
					cfg[ps[1]] = config_var_t(ps[0], ps[2], atof(ps[3].c_str()));
					printf("Min time is %lf\n", atof(ps[3].c_str()));
				} else {
					cfg[ps[1]] = config_var_t(ps[0], ps[2], 0);
				}
				
				m_Comms.Register(ps[1], 0);
			}
		}
	}
	
	// now..!
	
	return true;
}

bool CMOOSBlink::Iterate()
{
	L_l();
	while(in.size()) {
		CMOOSMsg cm = in.front();
		in.pop_front();
		U_l();

		m_Comms.Post(cm);

		L_l();
	}
	U_l();
	recent.clear_old(10);
}

bool CMOOSBlink::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...

	pthread_create(&listen_thread, NULL, &listen_thread_proc_tramp, this);
	pthread_create(&write_thread,  NULL, &write_thread_proc_tramp,  this);	

	return true;
}

void CMOOSBlink::write_thread_p()
{
	int sockfd, nb, yes = 1;
	struct sockaddr_in their_addr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(MOOSBRIDGE_PORT);
	struct hostent *h = gethostbyname(broadcastaddr.c_str());
	their_addr.sin_addr = *((struct in_addr *)h->h_addr);

	memset(&(their_addr.sin_zero), 0, 8);

	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int));
	
	// now loop...
	while(1) {
		L_w();
		while(out.size()) {
			CMOOSMsg m = out.front();
			out.pop_front();
			U_w();
			
			int size = 500;
			uint8_t *buf = (uint8_t *)malloc(size);
			uint32_t sequence = rand();
			memset(buf, 0, size);

			buf[0] = 'M';
			buf[1] = 'L';

			config_t::iterator it;
			if((it = cfg.find(m.m_sKey)) == cfg.end()) {
				printf("Error reverse mapping key?\n");
				goto finished;
			}

			if(it->second.lasttx + it->second.min_time < dtime()) {
				it->second.lasttx = dtime();
			} else {
				goto finished;
			}
			
			memcpy(buf+2, &sequence, 4);
			strncpy((char *)buf+2+4, m.m_sOriginatingCommunity.c_str(), 16);
			strncpy((char *)buf+2+4+16, it->second.todb.c_str(), 16);
			strncpy((char *)buf+2+4+16+16, it->second.tovar.c_str(), 32);

			if(m.IsDouble()) {
				buf[2+4+16+16+32] = 'D';
				snprintf((char *)buf+2+4+16+16+32+1, size - (2+4+16+16+32+1+1), "%09.9lf", m.m_dfVal);
			} else if(m.IsString()) {
				buf[2+4+16+16+32] = 'S';
				strncpy((char *)buf+2+4+16+16+32+1, m.m_sVal.c_str(), size - (2+4+16+16+32+1));
			} else {
				printf("MOOSMsg with no type\n");
				goto finished;
			}
			
			recent.add(sequence);
			nb = sendto(sockfd, buf, size, 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr));
			finished:
			free(buf);

			L_w();
		}
		U_w();

		sleep(1);
	}
}

void CMOOSBlink::listen_thread_p(void)
{
	// start up sockets
	// read
	int r_sfd, r_yes = 1, r_addr_len, r_nb;
	struct sockaddr_in r_my_addr, r_their_addr;

	r_sfd = socket(AF_INET, SOCK_DGRAM, 0);

	r_my_addr.sin_family = AF_INET;
	r_my_addr.sin_port = htons(MOOSBRIDGE_PORT);
	r_my_addr.sin_addr.s_addr = INADDR_ANY;

	memset(&(r_my_addr.sin_zero), 0, 8);

	setsockopt(r_sfd, SOL_SOCKET, SO_BROADCAST, &r_yes, sizeof(int));
	bind(r_sfd, (struct sockaddr *)&r_my_addr, sizeof(struct sockaddr));
	setsockopt(r_sfd, SOL_SOCKET, SO_BROADCAST, &r_yes, sizeof(int));

	r_addr_len = sizeof(struct sockaddr);

	while(1) {
		char buf[2048];
		memset(buf, 0, 2048);
		r_nb = recvfrom(r_sfd, buf, 2048, 0, (struct sockaddr *)&r_their_addr, (socklen_t *)&r_addr_len);

		// now poke at buf...
		if(buf[0] == 'M' && buf[1] == 'L') {
			uint32_t seq;
			memcpy(&seq, buf+2, 4);

			if(recent.has(seq)) continue;
			recent.add(seq);

			char cname[17];
			strncpy(cname, buf+2+4, 16);
			cname[16] = 0;

			char ocname[17];
			strncpy(ocname, buf+2+4+16, 16);
			ocname[16] = 0;

			// var update
			char var[33];
			strncpy(var, buf+2+4+16+16, 32);
			var[32] = 0;

			fprintf(stderr, "Received variable packet, for com %s, from com %s, containing %s\n", ocname, cname, var);
				
			if(string(ocname) != communityname && string(ocname) != "global" && string(ocname) != "GLOBAL") continue;

			if(buf[2+4+16+16+32] == 'D') {
				// num...
				double tmp;

				sscanf(buf+2+4+16+16+32+1, "%09lf", &tmp);
				
				CMOOSMsg m(MOOS_NOTIFY, var, tmp, -1);
				m.m_sOriginatingCommunity = cname;
				L_l();
				in.push_back(m);
				U_l();
			} else if(buf[2+4+16+16+32] == 'S') {
				CMOOSMsg m(MOOS_NOTIFY, var, buf+2+4+16+16+32+1, -1);
				m.m_sOriginatingCommunity = cname;
				L_l();
				in.push_back(m);
				U_l();
			}
		}
	}
}
