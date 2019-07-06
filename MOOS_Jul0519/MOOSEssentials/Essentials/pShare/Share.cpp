/*
 * Share.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: pnewman
 */
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <map>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <vector>
#include <limits>

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/IPV4Address.h"
//#include "MOOS/libMOOS/Thirdparty/getpot/GetPot"
#include "MOOS/libMOOS/Utils/SafeList.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/Utils/KeyboardCapture.h"

#include "MOOS/libMOOS/App/MOOSApp.h"

#include "Listener.h"
#include "Share.h"
#include "Route.h"
#include "ShareHelp.h"

#define DEFAULT_MULTICAST_GROUP_ADDRESS "224.1.1.11"
#define DEFAULT_MULTICAST_GROUP_PORT 24460
#define MAX_MULTICAST_CHANNELS 256
#define MAX_UDP_SIZE 48*1024

#define RED MOOS::ConsoleColours::Red()
#define GREEN MOOS::ConsoleColours::Green()
#define YELLOW MOOS::ConsoleColours::Yellow()
#define NORMAL MOOS::ConsoleColours::reset()

namespace MOOS {


struct Socket {
	MOOS::IPV4Address address;
	int socket_fd;
	struct sockaddr_in sock_addr;
};

class Share::Impl: public CMOOSApp {
public:
	bool OnNewMail(MOOSMSG_LIST & new_mail);
	bool OnStartUp();
	bool Iterate();
	bool OnConnectToServer();
	bool OnCommandMsg(CMOOSMsg  Msg);
	bool OnProcessCommandLine();
	void OnPrintHelpAndExit();
	void OnPrintInterfaceAndExit();



protected:

	bool ApplyRoutes(CMOOSMsg & msg);

	bool ApplyWildcardRoutes( CMOOSMsg& msg);

	bool AddOutputRoute(MOOS::IPV4Address address, bool multicast = true);

	bool AddInputRoute(MOOS::IPV4Address address, bool multicast = true);

	bool PublishSharingStatus();

	std::vector<std::string>  GetRepeatedConfigurations(const std::string & token);

	bool ProcessIOConfigurationString(std::string  configuration_string,bool is_output);

	bool ProcessShortHandIOConfigurationString(std::string configuration_string, bool is_output);

	bool AddRoute(const std::string & src_name,
				const std::string & dest_name,
				MOOS::IPV4Address address,
				bool multicast,
				double frequency);

	bool  AddMulticastAliasRoute(const std::string & src_name,
					const std::string & dest_name,
					unsigned int channel_num,
					double frequency);

	MOOS::IPV4Address GetAddressFromChannelAlias(unsigned int channel_number) const;

	std::string GetChannelAliasFromMutlicastAddress(const MOOS::IPV4Address & address) const;

	void PrintRoutes();

	void LookForAndHandleUserInput();

	bool PrintSocketMap();

	bool DoRegistrations();

private:
	typedef CMOOSApp BASE;

	//this maps channel number to a multicast socket
	typedef std::map<MOOS::IPV4Address, Socket> SocketMap;
	SocketMap socket_map_;

	//this maps variable name to route
	typedef std::map<std::string, std::list<Route> > RouteMap;
	RouteMap routing_table_;

	typedef std::map<std::pair< std::string,std::string>, std::list<Route> > WildcardRouteMap;
	WildcardRouteMap wildcard_routing_table_;

	//this maps channel number to a listener (with its own thread)
	SafeList<CMOOSMsg > incoming_queue_;
	std::map<MOOS::IPV4Address, Listener*> listeners_;

	//teh address form which we count
	MOOS::IPV4Address base_address_;

	bool verbose_;


};

const std::string trim(const std::string& pString,
                       const std::string& pWhitespace = " \t")
{
    const size_t beginStr = pString.find_first_not_of(pWhitespace);
    if (beginStr == std::string::npos)
    {
        // no content
        return "";
    }

    const size_t endStr = pString.find_last_not_of(pWhitespace);
    const size_t range = endStr - beginStr + 1;

    return pString.substr(beginStr, range);
}

Share::Share() :_Impl(new Impl)
{

}

Share::~Share()
{
}



bool Share::Impl::AddInputRoute(MOOS::IPV4Address address , bool multicast)
{

	if(listeners_.find(address)!=listeners_.end())
	{
		std::cerr<<	"Error ::Listener already"
					" listening on "
				<<address.to_string()<<std::endl;
		return false;
	}


	//OK looking good, make it
	listeners_[address] = new Listener(incoming_queue_,
			address,
			multicast);

	//run it
	return listeners_[address]->Run();

}

std::vector<std::string>  Share::Impl::GetRepeatedConfigurations(const std::string & token)
{
	STRING_LIST params;
	m_MissionReader.GetConfiguration( GetAppName(),params);

	STRING_LIST::iterator q;
	std::vector<std::string> results;
	for(q=params.begin(); q!=params.end();q++)
	{
		std::string tok,val;

		m_MissionReader.GetTokenValPair(*q,tok,val,false);

		if(MOOSStrCmp(tok,token))
		{
			results.push_back(val);
		}
	}
	return results;
}

bool Share::Impl::OnProcessCommandLine()
{
	base_address_.set_host  (DEFAULT_MULTICAST_GROUP_ADDRESS);

	//verbose_ = m_CommandLineParser.GetFlag("--verbose");

    int port = DEFAULT_MULTICAST_GROUP_PORT;
    GetParameterFromCommandLineOrConfigurationFile("multicast_base_port",port);
    if(port<0 || port > std::numeric_limits<uint16_t>::max())
        return MOOSFail("multicast_base_port must be a unsigned 16 bit number");

    base_address_.set_port (port);

    std::string  address = DEFAULT_MULTICAST_GROUP_ADDRESS;
    GetParameterFromCommandLineOrConfigurationFile("multicast_address",address);
    base_address_.set_host (address);



	verbose_ = GetFlagFromCommandLineOrConfigurationFile("verbose");

	//verbose_ = m_CommandLineParser.GetFlag("--verbose");

	verbose_ = GetFlagFromCommandLineOrConfigurationFile("verbose");

	std::string sVar;
	if(m_CommandLineParser.GetVariable("-o",sVar))
	{
		std::vector<std::string> outputs = MOOS::StringListToVector(sVar);


		for(unsigned int i = 0;i<outputs.size();i++)
		{
			ProcessShortHandIOConfigurationString(outputs[i],true);
		}
	}

	if(m_CommandLineParser.GetVariable("-i",sVar))
	{
		std::vector<std::string> inputs = MOOS::StringListToVector(sVar);

		for(unsigned int i = 0;i<inputs.size();i++)
		{
			ProcessShortHandIOConfigurationString(inputs[i],false);
		}
	}


	DoRegistrations();

	return true;
}




bool Share::Impl::OnStartUp()
{
	//return true;
	EnableCommandMessageFiltering(true);

	SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);
	SetAppFreq(40,0);

	try
	{
/*
		AddRoute("  X   ","X",MOOS::IPV4Address("127.0.0.1",9010),false);
		AddRoute("X","sadfsadf",MOOS::IPV4Address("127.0.0.1",9011),false);
		AddRoute("X","long_name",MOOS::IPV4Address("localhost",9012),false);
		AddRoute("X","fly_across",MOOS::IPV4Address("oceanai.mit.edu",9012),false);
		AddMulticastAliasRoute("X","X",0);
		AddRoute("Square","Triangle",MOOS::IPV4Address("127.0.0.1",9010),false);
		AddRoute("Square","sadfsadf",MOOS::IPV4Address("161.8.5.1",9011),false);
		AddMulticastAliasRoute("Horse","Equine",3);
		ProcessIOConfigurationString("src_name =X,dest_name=Z,route=multicast_8",true);
		ProcessIOConfigurationString("src_name =X,dest_name=Z,route=161.4.5.6:9000&multicast_21&localhost:9832",true);
		ProcessIOConfigurationString("route=multicast_21&localhost:9833&multicast_3",false);
		*/


//		std::string multicast_base;
//		if(m_MissionReader.GetValue("multicast_base",multicast_base))
//		{
//			base_address_=IPV4Address(multicast_base);
//		}


		verbose_ = GetFlagFromCommandLineOrConfigurationFile("verbose");

		std::vector<std::string> outputs = GetRepeatedConfigurations("Output");
		for(std::vector<std::string>::iterator q=outputs.begin();
				q!=outputs.end();
				q++)
		{
			if(q->find("route")!=std::string::npos || q->find("Route")!=std::string::npos)
			{
				//this looks like a long hand configuration
				ProcessIOConfigurationString(*q,true);
			}
			else
			{
				//this looks like shorthand..
				ProcessShortHandIOConfigurationString(*q,true);
			}
		}

		std::vector<std::string> inputs = GetRepeatedConfigurations("Input");
		for(std::vector<std::string>::iterator q=inputs.begin();
				q!=inputs.end();
				q++)
		{
			ProcessIOConfigurationString(*q,false);
		}


		PrintRoutes();
	}
	catch(const std::exception & e)
	{
		std::cerr<<RED<<"OnStartUp::exception "<<e.what()<<NORMAL<< std::endl;
	}


	DoRegistrations();


	return true;
}


bool Share::Impl::ProcessShortHandIOConfigurationString(std::string configuration_string, bool is_output)
{

//	std::cout<<"ProcessShortHandIOConfigurationString...\n";


	std::string copy_config = configuration_string;

	if(is_output)
	{
		//X->Y:165.45.3.61:9000@0.1 & Z:multicast_8
		std::string src_name = MOOS::Chomp(configuration_string,"->");
		while(!configuration_string.empty())
		{
			std::string whole_route_description = trim(MOOS::Chomp(configuration_string,"&"));

            //look for a @frequency
			std::string route_description = MOOS::Chomp(whole_route_description,"@");
			std::string sFrequency = whole_route_description;
            if(sFrequency.empty() || !MOOSIsNumeric(sFrequency) )
                sFrequency="0.0";


			std::list<std::string> parts;

            if(route_description.find(":")==std::string ::npos && (route_description.find("multicast_")==std::string::npos))
			{
				std::cerr<<RED<<"error: short hand failed to parse "<<copy_config
													<<" not enough parts in route\n"<<NORMAL;
				return false;
			}
			while(!route_description.empty())
			{
				parts.push_back(MOOS::Chomp(route_description,":"));
			}




			std::string dest_name = src_name;



			if(parts.back().find("multicast_")!=0)
			{

				if(parts.size()>3)
				{
					std::cerr<<RED<<"short hand failed to parse "<<copy_config
									<<" too many parts to the route\n";
					return false;
				}

				if(parts.size()==3)
				{
					dest_name = parts.front();parts.pop_front();
				}
				std::string host_name = parts.front();parts.pop_front();
				std::string host_port = parts.front();parts.pop_front();
				std::string destination = host_name+":"+host_port;

				std::string io;
				MOOSAddValToString(io,"src_name",src_name);
				MOOSAddValToString(io,"dest_name",dest_name);
				MOOSAddValToString(io,"route",destination);
				MOOSAddValToString(io,"frequency",sFrequency);

				try
				{
					ProcessIOConfigurationString(io,true);
				}
				catch(const std::exception & e)
				{
					std::cerr<<RED<<"short hand failed to parse "<<copy_config
							<<" "<<e.what()<<std::endl<<NORMAL;
				}


			}
			else
			{

				if(parts.size()>2)
				{
					std::cerr<<RED<<"short hand failed to parse "<<copy_config
									<<" too many parts to the route\n";
					return false;
				}

				//this is a multicast route
				std::string multicast_channel;
				switch(parts.size())
				{
				case 2:
					//X:multicast_8
					dest_name = parts.front();parts.pop_front();
				case 1:
					//multicast_8
					multicast_channel = parts.front();parts.pop_front();
					break;
				}

				std::string io;
				MOOSAddValToString(io,"src_name",src_name);
				MOOSAddValToString(io,"dest_name",dest_name);
				MOOSAddValToString(io,"route",multicast_channel);
                MOOSAddValToString(io,"frequency",sFrequency);

				try
				{
					ProcessIOConfigurationString(io,true);
				}
				catch(const std::exception & e)
				{
					std::cerr<<RED<<"shorthand failed to parse:\n   "
							<<copy_config<<" \n"
							<<io<<" \n"<<e.what()<<std::endl<<NORMAL;
				}
			}
		}
	}
	else
	{

		std::string io;
		MOOSAddValToString(io,"route", configuration_string);

		//this is an input
		// --input multicast_8 & 192.45.3.4:9000 &
		try
		{
			ProcessIOConfigurationString(io,false);
		}
		catch(const std::exception & e)
		{
			std::cerr<<RED<<"shorthand failed to parse:\n   "
					<<copy_config<<" \n"
					<<io<<" \n"<<e.what()<<std::endl<<NORMAL;
		}

	}

	return true;

}


bool Share::Impl::ProcessIOConfigurationString(std::string  configuration_string, bool is_output )
{
//	std::cout<<"ProcessShortHandIOConfigurationString...\n";


	std::string src_name, dest_name, routes;

	MOOSRemoveChars(configuration_string, " ");

	if (is_output) {
		if (!MOOSValFromString(src_name, configuration_string, "src_name"))
			throw std::runtime_error(
					"ProcessIOConfigurationString \"src_name\" is a required field");

		//default no change in name
		dest_name = src_name;
		MOOSValFromString(dest_name, configuration_string, "dest_name");
	}

	//we do need a route....
	if (!MOOSValFromString(routes, configuration_string, "route"))
		throw std::runtime_error(
				"ProcessIOConfigurationString \"route\" is a required field");

	//are we being asked to delete the route?
	//bDelete=false;
	//MOOSValFromString(bDelete,configuration_string,"delete");

	double frequency = 0.0;
	MOOSValFromString(frequency,configuration_string,"frequency");

	while (!routes.empty()) {
		//look for a space separated list of routes...
		std::string route = MOOSChomp(routes, "&");

		if (route.find("multicast_") == 0)
		{
			//is this a special multicast one?
			std::stringstream ss(std::string(route, 10));
			unsigned int channel_num = 0;
			ss >> channel_num;
			if (!ss) {
				std::cerr << RED << "cannot parse " << route << channel_num
						<< std::endl;
				continue;
			}

			if (is_output)
			{
				if (!AddMulticastAliasRoute(src_name, dest_name,
						channel_num,frequency))
					return false;
			}
			else
			{
				if (!AddInputRoute(GetAddressFromChannelAlias(channel_num),
						true))
					return false;
			}

		}
		else
		{
			MOOS::IPV4Address route_address(route);

			if (is_output)
			{
				if (!AddRoute(src_name, dest_name, route_address, false,frequency))
					return false;
			}
			else
			{
				if (!AddInputRoute(route_address, false))
				{
					return false;
				}
			}
		}

	}

	return true;

}

bool Share::Impl::Iterate()
{
	while(incoming_queue_.Size()!=0)
	{
		CMOOSMsg new_msg;
		if(incoming_queue_.Pull(new_msg))
		{
			//new_msg.Trace();
			if(!m_Comms.IsRegisteredFor(new_msg.GetKey()))
			{
				m_Comms.Post(new_msg,true);
				if(verbose_)
				{
                    std::cout<<std::setprecision(1);
                    std::cout<<MOOS::ConsoleColours::green()<<std::setw(10)<<std::fixed<<(MOOS::Time()-CMOOSApp::GetAppStartTime())
                    <<": "<<MOOS::ConsoleColours::reset();
				    std::cout<<"forwarding share of \""<<new_msg.GetName()<<"\" from "<<new_msg.m_sSrc<<std::endl;
				}


			}
		}
	}

	PublishSharingStatus();
	return true;
}


bool Share::Impl::PublishSharingStatus()
{
	static double last_time = MOOS::Time();
	if(MOOS::Time()-last_time<1.0)
		return true;

	std::stringstream sso;

	RouteMap::iterator q;
	for(q = routing_table_.begin();q!=routing_table_.end();q++)
	{
		if(q!=routing_table_.begin())
			sso<<", ";
		std::list<Route> & routes = q->second;
		sso<<q->first<<"->";
		std::list<Route>::iterator p;
		for(p = routes.begin();p!=routes.end();p++)
		{
			if(p!=routes.begin())
				sso<<" & ";
			Route & route = *p;
			sso<<route.dest_name<<":"<<route.dest_address.to_string();
			if(route.multicast)
			{
				sso<<":multicast_"<<route.dest_address.port()-base_address_.port();
			}
			else
			{
				//sso<<":udp";
			}
		}
	}

	std::stringstream ssi;

	std::map<MOOS::IPV4Address, Listener*>::iterator t;
	for(t = listeners_.begin();t!=listeners_.end();t++)
	{
		if(t!=listeners_.begin())
			ssi<<",";
		ssi<<t->first.to_string();
		if(t->second->multicast())
		{
			ssi<<":multicast_"<<t->second->port()-base_address_.port();
		}
		else
		{
			//ssi<<":udp";
		}
	}


	last_time = MOOS::Time();

	Notify("PSHARE_OUTPUT_SUMMARY",sso.str());
	Notify("PSHARE_INPUT_SUMMARY",ssi.str());


	return true;
}

bool Share::Impl::OnNewMail(MOOSMSG_LIST & new_mail)
{
	MOOSMSG_LIST::iterator q;

	//for all mail
	for(q = new_mail.begin();q != new_mail.end();q++)
	{
		//do we need to forward it
		RouteMap::iterator g = routing_table_.find(q->GetKey());
		try
		{
			if(g != routing_table_.end())
			{
				//yes OK - try to do so
				ApplyRoutes(*q);
			}
			else
			{
				//maybe its a wildcard...
				ApplyWildcardRoutes(*q);
			}
		}
		catch(const std::exception & e)
		{
			std::cerr <<RED<< "Exception thrown: " << e.what() <<NORMAL<< std::endl;
		}
	}

	return true;
}


bool  Share::Impl::AddMulticastAliasRoute(const std::string & src_name,
				const std::string & dest_name,
				unsigned int channel_num,
				double frequency)
{
	MOOS::IPV4Address alias_address = GetAddressFromChannelAlias(channel_num);
	return AddRoute(src_name,dest_name,alias_address,true,frequency);
}

bool  Share::Impl::AddRoute(const std::string & src_name,
				const std::string & dest_name,
				MOOS::IPV4Address address,
				bool multicast,
				double frequency)
{

	SocketMap::iterator mcg = socket_map_.find(address);
	if (mcg == socket_map_.end())
	{
		if(!AddOutputRoute(address,multicast))
			return false;
	}

	std::string trimed_src_name = trim(src_name);
	std::string trimed_dest_name = trim(dest_name);





	if(trimed_src_name.find_last_of("*?:")==std::string::npos)
	{
		Route route;
		route.dest_name = trimed_dest_name;
		route.src_name = trimed_src_name;
		route.dest_address = address;
		route.multicast = multicast;
		route.frequency = frequency;

		std::list<Route> & rlist = routing_table_[trimed_src_name];

		//check we have not already got this exact same route....
		if(find(rlist.begin(), rlist.end(),route)==rlist.end())
		{
			//this is a regular share....
			Register(trimed_src_name, 0.0);

			//add this to our routing table
			rlist.push_back(route);
		}
	}
	else
	{
		//this is a wildcard route
		Route route;
		if(trimed_dest_name.find_last_of("*?")!=std::string::npos)
		{
			//looks like we have no desire to rename
			trimed_dest_name = "";
		}
		route.dest_name = trimed_dest_name;
		route.dest_address = address;
		route.multicast = multicast;

		//this looks like a wildcard share
		std::string var_pattern = MOOS::Chomp(trimed_src_name,":");
		std::string app_pattern = "*";
		if(!trimed_src_name.empty())
			app_pattern = trimed_src_name;


		std::list<Route> & rlist = wildcard_routing_table_[std::make_pair(var_pattern,app_pattern)];

		//check we have not already got this exact same route....
		if(find(rlist.begin(), rlist.end(),route)==rlist.end())
		{
			//do a wildcard registration
			Register(var_pattern,app_pattern,0.0);

			//add this to wildcard routing table
			rlist.push_back(route);
		}
	}

	return true;
}

bool Share::Impl::DoRegistrations()
{
	RouteMap::iterator q;

	for(q=routing_table_.begin();q!=routing_table_.end();q++)
	{
		Register(q->first, 0.0);
	}


	WildcardRouteMap::iterator g;
	for(g=wildcard_routing_table_.begin();g!=wildcard_routing_table_.end();g++)
	{
		Register(g->first.first,g->first.second, 0.0);
	}

	return true;
}

bool Share::Impl::OnConnectToServer()
{
	return DoRegistrations();
}


void Share::Impl::OnPrintHelpAndExit()
{
	ShareHelp::PrintHelp();
	exit(0);
}

void Share::Impl::OnPrintInterfaceAndExit()
{
	ShareHelp::PrintInterface();
	exit(0);
}


bool Share::Impl::OnCommandMsg(CMOOSMsg  Msg)
{
	if(!Msg.IsYoungerThan(GetAppStartTime()))
		return false;

	std::string cmd;
	MOOSValFromString(cmd,Msg.GetString(),"cmd");

	try
	{

		if(MOOSStrCmp("output",cmd))
		{
			if(!ProcessIOConfigurationString(Msg.GetString(),true))
				return false;

			PrintRoutes();
		}
		else if(MOOSStrCmp("input",cmd))
		{
			if(!ProcessIOConfigurationString(Msg.GetString(),false))
				return false;

			PrintRoutes();
		}
		else
		{
			throw std::runtime_error("cmd=X - X was neither \"output\" or \"input\"\n");
		}
	}
	catch(const std::exception & e)
	{
		std::cerr<<RED<<"oops failed to parse dynamic request:\n"
				"    \""<<e.what()<<"\"\n"<<NORMAL;
	}
	return true;
}

bool Share::Impl::PrintSocketMap()
{
	SocketMap::iterator q;
	std::cout<<"socket_map_:\n";
	for(q = socket_map_.begin();q!=socket_map_.end();q++)
	{
		std::cout<<" "<<q->first.to_string() <<" -> "<<q->second.socket_fd<<std::endl;
	}
	return true;
}

void Share::Impl::PrintRoutes()
{
	std::cout<<"---------------------- "<<YELLOW<<"ROUTING INFORMATION"<<NORMAL<<" ------------------------------------"<<std::endl;

	std::cout<<GREEN<<"Standard Routes:\n"<<NORMAL;
	if(routing_table_.empty())
		std::cout<<"   none\n";

	RouteMap::iterator q;
	std::cout<<std::setiosflags(std::ios::left);
	for(q = routing_table_.begin();q!=routing_table_.end();q++)
	{
		std::list<Route> & routes = q->second;
		std::cout<<"  routing for \""<< q->first<<"\""<<std::endl;
		std::list<Route>::iterator p;
		for(p = routes.begin();p!=routes.end();p++)
		{
			Route & route = *p;
			std::cout<<"  --> "<<std::setw(20)<<route.dest_address.to_string()<<" as "<<std::setw(10)<<route.dest_name;
			if(route.multicast)
				std::cout<<" ["<<GetChannelAliasFromMutlicastAddress(route.dest_address)<<"]";
			else
				std::cout<<" [udp]";

			if(route.frequency==0.0)
			    std::cout<<" every notification ";
			else
			    std::cout<<" @ " <<route.frequency <<"Hz";

			std::cout<<std::endl;
		}

	}

	std::cout<<GREEN<<"Wildcard Routes:\n"<<NORMAL;
	if(wildcard_routing_table_.empty())
		std::cout<<"   none\n";

	WildcardRouteMap::iterator w;
	for(w = wildcard_routing_table_.begin();w!=wildcard_routing_table_.end();w++)
	{
		std::list<Route> & routes = w->second;
		std::string var_pattern = w->first.first;
		std::string app_pattern = w->first.second;

		std::cout<<"  routing for variables matching \""<<
				var_pattern <<"\" from sources matching \""<<
				app_pattern<<"\": "<<std::endl;

		std::list<Route>::iterator p;
		for(p = routes.begin();p!=routes.end();p++)
		{
			Route & route = *p;
			std::cout<<"  --> "<<std::setw(20)<<route.dest_address.to_string()
					<<" as ";

			if(std::count(var_pattern.begin(), var_pattern.end(), '*')==1 &&
									route.dest_name=="^")
			{
				std::cout<<std::setw(10)<<" <-wildcard-match-> ";
			}
			else
			{
				std::cout<<std::setw(10)<<route.dest_name+"<src_name>";
			}
			if(route.multicast)
				std::cout<<" ["<<GetChannelAliasFromMutlicastAddress(route.dest_address)<<"]";
			else
				std::cout<<" [udp]";
			std::cout<<std::endl;
		}
	}


	std::cout<<GREEN<<"Listening on:\n"<<NORMAL;
	if(listeners_.empty())
		std::cout<<"   none\n";

	std::map<MOOS::IPV4Address, Listener*>::iterator t;
	for(t = listeners_.begin();t!=listeners_.end();t++)
	{
		std::cout<<"  <-- "<<std::setw(20)<<t->first.to_string();
		if(t->second->multicast())
		{
			unsigned int channel_num = t->second->port()-base_address_.port();
			std::cout<<"[multicast_"<<channel_num<<"]";
		}
		else
		{
			std::cout<<"[udp]";
		}

		std::cout<<std::endl;
	}

	std::cout<<"-------------------------------------------------------------------------------"<<std::endl;

}

bool WildcardMatch(const std::string & var_pattern,
		const std::string & src_app_pattern,
		const std::string & var,
		const std::string & src_app)
{
	return MOOSWildCmp(src_app_pattern,src_app) && MOOSWildCmp(var_pattern,var);
}

bool Share::Impl::ApplyWildcardRoutes( CMOOSMsg& msg)
{
	//maybe it is in our wildcard routing?

	WildcardRouteMap::iterator g;
	for(g=wildcard_routing_table_.begin();g!=wildcard_routing_table_.end();g++)
	{
		std::string var_pattern = g->first.first;
		std::string app_pattern = g->first.second;
		std::list<MOOS::Route> & routes = g->second;

		std::list<MOOS::Route>::iterator h;
		for(h = routes.begin();h!=routes.end();h++)
		{
			Route & route = *h;
			if(WildcardMatch(var_pattern,
					app_pattern,
					msg.GetKey(),
					msg.GetSource()))
			{

				Route new_route = route;
				new_route.src_name = msg.GetKey();

				if(std::count(var_pattern.begin(), var_pattern.end(), '*')==1 &&
						route.dest_name=="^")
				{
					//here we check for a special case if we are presented with a pattern
					//like *_X->^ we will simply forward as the bit that matched * in *_X
					//so concretely A_X will be forwarded as X

					std::string t = msg.m_sKey;
					std::string bit_that_matches;
					if(*var_pattern.begin()=='*')
					{
						//we have *X
						std::string tok = var_pattern.substr(1);
						//we want everything before tok as that matched the wild card...
						bit_that_matches = MOOS::Chomp(t,tok);
					}
					else if(*var_pattern.rbegin()=='*')
					{
						//we have X*
						std::string tok = var_pattern.substr(0,var_pattern.length()-1);
						//we want everything after tok as that matches the wild card...
						MOOS::Chomp(t,tok);
						bit_that_matches = t;
					}
					new_route.dest_name = bit_that_matches;
				}
				else
				{
					//standard thing to do is simply use message name as a suffix
					new_route.dest_name+=msg.GetKey();
				}

				std::cout<<"dynamically creating outgoing route : "<<msg.GetKey()<<"->"<<new_route.dest_name <<" on ";
				if(new_route.multicast)
				{
					std::cout<< GetChannelAliasFromMutlicastAddress(new_route.dest_address)<<"\n";
				}
				else
				{
					std::cout<<new_route.dest_address.to_string()<<"\n";
				}

				routing_table_[msg.GetKey()].push_back(new_route);
				ApplyRoutes(msg);
			}
		}
	}
	return true;
}


bool Share::Impl::ApplyRoutes(CMOOSMsg & msg)
{
	//do we know how to route this? double check
	RouteMap::iterator g = routing_table_.find(msg.GetKey());
	if(g == routing_table_.end())
		throw std::runtime_error("no specified route");

	//we need to find the socket to send via
	std::list<Route> & route_list = g->second;

	double now = MOOS::Time();

	std::list<Route>::iterator q;
	for(q = route_list.begin();q!=route_list.end();q++)
	{
		//process every route
		Route & route = *q;

		if(route.frequency>0.0 && now-route.last_time_sent<(1.0/route.frequency))
		    continue;

		SocketMap::iterator mcg = socket_map_.find(route.dest_address);
		if (mcg == socket_map_.end()) {
			std::stringstream ss;
			ss << "no output socket for "
					<< route.dest_address.to_string() << std::endl;
			PrintSocketMap();
			throw std::runtime_error(ss.str());
		}

		Socket & relevant_socket = mcg->second;


        if(verbose_)
        {
            std::cout<<std::setprecision(1);
            std::cout<<MOOS::ConsoleColours::green()<<std::setw(10)<<std::fixed<<(MOOS::Time()-CMOOSApp::GetAppStartTime())
            <<": "<<MOOS::ConsoleColours::reset();

            std::cout<<"sending \""<<msg.m_sKey<<"\" as \""<<route.dest_name<<"\" to "<<route.dest_address.to_string()<<"\n";
        }

		//rename here...
		msg.m_sKey = route.dest_name;

		//serialise here
		unsigned int msg_buffer_size = msg.GetSizeInBytesWhenSerialised();

		if(msg_buffer_size>MAX_UDP_SIZE)
		{
			std::cerr<<"Message size exceeded payload size of "<<MAX_UDP_SIZE/1024<<" kB - not forwarding\n";
			return false;
		}

		std::vector<unsigned char> buffer(msg_buffer_size);
		if (!msg.Serialize(buffer.data(), msg_buffer_size))
		{
			throw std::runtime_error("failed msg serialisation");
		}

		//send here
		if (sendto(relevant_socket.socket_fd, buffer.data(), buffer.size(), 0,
				(struct sockaddr*) (&relevant_socket.sock_addr),
				sizeof(relevant_socket.sock_addr)) < 0)
		{
			throw std::runtime_error("failed \"sendto\"");
		}


		route.last_time_sent = now;
	}

	return true;

}


MOOS::IPV4Address Share::Impl::GetAddressFromChannelAlias(unsigned int channel_number) const
{
	MOOS::IPV4Address address = base_address_;
	address.set_port(channel_number+address.port());
	return address;
}

std::string Share::Impl::GetChannelAliasFromMutlicastAddress(const MOOS::IPV4Address & address) const
{
	std::stringstream ss;

	int channel = address.port()-base_address_.port();
	if(channel<0)
		throw std::runtime_error("multicast address has port number below base port\n");

	ss<<"multicast_"<<channel;

	return ss.str();
}

bool Share::Impl::AddOutputRoute(MOOS::IPV4Address address, bool multicast)
{

	Socket new_socket;
	new_socket.address=address;

	if ((new_socket.socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	throw std::runtime_error(
			"AddSocketForOutgoingTraffic() failed to open sender socket");

	int reuse = 1;
	if (setsockopt(new_socket.socket_fd, SOL_SOCKET, SO_REUSEADDR,
		&reuse, sizeof(reuse)) == -1)
	throw std::runtime_error("failed to set resuse socket option");
/*
	if (setsockopt(new_socket.socket_fd, SOL_SOCKET, SO_REUSEPORT,
		&reuse, sizeof(reuse)) == -1)
	throw std::runtime_error("failed to set resuse port option");
*/
	int send_buffer_size = 4 * 64 * 1024;
	if (setsockopt(new_socket.socket_fd,
			SOL_SOCKET, SO_SNDBUF,
			&send_buffer_size,
			sizeof(send_buffer_size)) == -1)
	{
		throw std::runtime_error("failed to set size of socket send buffer");
	}

	if(multicast)
	{
		char enable_loop_back = 1;
		if (setsockopt(new_socket.socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP,
			&enable_loop_back, sizeof(enable_loop_back)) == -1)
			throw std::runtime_error("failed to disable loop back");

		char num_hops = 1;
		if (setsockopt(new_socket.socket_fd, IPPROTO_IP, IP_MULTICAST_TTL,
			&num_hops, sizeof(num_hops)) == -1)
			throw std::runtime_error("failed to set ttl hops");
	}


	memset(&new_socket.sock_addr, 0, sizeof (new_socket.sock_addr));
	new_socket.sock_addr.sin_family = AF_INET;

	std::string dotted_ip = MOOS::IPV4Address::GetNumericAddress(new_socket.address.host());

	if(inet_aton(dotted_ip.c_str(), &new_socket.sock_addr.sin_addr)==0)
	{
		throw std::runtime_error("failed to intepret "
				+dotted_ip+" as an ip address");
	}

	//new_socket.sock_addr.sin_addr.s_addr = inet_addr(new_socket.address.ip_num.c_str());
	new_socket.sock_addr.sin_port = htons(new_socket.address.port());

	//finally add it to our collection of sockets
	socket_map_[address] = new_socket;

	return true;
}




int Share::Run(int argc,char * argv[])
{

    //here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);
	//mission file could be first free parameter
	std::string mission_file = P.GetFreeParameter(0, "Mission.moos");
	//mission name can be the  second free parameter
	std::string app_name = P.GetFreeParameter(1, "pShare");


	try
	{
		_Impl->Run(app_name,mission_file,argc,argv);
	}
	catch(const std::exception & e)
	{
		std::cerr<<"oops: "<<e.what();
	}
	return 0;
}



}//end of MOOS namespace..
