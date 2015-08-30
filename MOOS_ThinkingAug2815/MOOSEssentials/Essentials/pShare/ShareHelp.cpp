/*
 * ShareHelp.cpp
 *
 *  Created on: Sep 8, 2012
 *      Author: pnewman
 */

#include <iostream>
#include <cstdlib>
#include "MOOS/libMOOS/Utils/ConsoleColours.h"


#define RED MOOS::ConsoleColours::Red()
#define GREEN MOOS::ConsoleColours::Green()
#define YELLOW MOOS::ConsoleColours::Yellow()
#define NORMAL MOOS::ConsoleColours::reset()

#include "ShareHelp.h"

ShareHelp::ShareHelp() {
	// TODO Auto-generated constructor stub

}

ShareHelp::~ShareHelp() {
	// TODO Auto-generated destructor stub
}
void ShareHelp::PrintConfigurationExample()
{
	std::cout<<RED<<"\n--------------------------------------------\n";
	std::cout<<RED<<"      \"pShare\" Example Configuration      \n";
	std::cout<<RED<<"--------------------------------------------\n\n"<<NORMAL;

	std::cout<<"ProcessConfig =pShare\n"
			"{\n"
			<<YELLOW<<"  //simple forward of X to A on channel 8\n"<<NORMAL<<
			"  output = src_name = X,dest_name=A,route=multicast_8\n\n"

			<<YELLOW<<"  //simple forward of Y to B on a udp address\n"<<NORMAL<<
			"  output = src_name = Y,dest_name = B, route = localhost:9010\n\n"

			<<YELLOW<<"  //simple forward of Z to Z on a named address (no renaming)\n"<<NORMAL<<
			"  output = src_name = Z, route = oceanai.mit.edu:9020\n\n"

			<<YELLOW<<"  //forwarding to a list of places outputs\n"<<NORMAL<<
			"  output = src_name = P,dest_name = H,route=multicast_9 & robots.ox.ac.uk:10200 & 178.4.5.6:9001\n\n"

			<<YELLOW<<"  //setting up an input\n"<<NORMAL<<
			"  input = route = multicast_9\n"
			"  input = route = localhost:9067\n\n"

			<<YELLOW<<"  //setting up lots at once\n"<<NORMAL<<
			"  input = route = localhost:9069 & multicast_9 & multicast_65\n"

           <<YELLOW<<"  //setting up other config options (optional)\n"<<NORMAL<<
            "  multicast_base_port = 9061\n"
            "  multicast_address = 224.1.1.12\n"


			"}\n"<<std::endl;

}

void ShareHelp::PrintHelp()
{
	std::cout<<RED<<"\n--------------------------------------------\n";
	std::cout<<RED<<"      \"pShare\" Help                       \n";
	std::cout<<RED<<"--------------------------------------------\n";

	std::cout<<YELLOW<<"\nGeneral Usage\n\n"<<NORMAL;
	std::cout<<"pShare MissionFile [switches] \n\n"
			"switches:\n"
			"  -o=<outputs>: specify outputs from command line\n"
			"  -i=<inputs> : specify inputs from command line\n"
            "  --verbose   : verbose operation\n"
            "  --multicast_base_port=<uint_16> multicast base port\n"
            "  --multicast_address=<ip-address> multicast address\n";


	std::cout<<YELLOW<<"\nExamples:\n\n"<<NORMAL;

	std::cout<<"a)   \"./pShare\"  (register under default name of pShare, no "
			"configuration all sharing configured online)\n";
	std::cout<<"b)   \"./pShare special.moos\" (register under default name of pShare)\n";
	std::cout<<"c)   \"./pShare special.moos --moos_name=pShare2\" (register under MOOSName of pShare2)\n";
	std::cout<<"d)   \"./pShare --moos_file=special.moos --moos_name=pShare2\" (register under MOOSName of pShare2)\n";

	std::cout<<YELLOW<<"\nSpecifying shares from the command line:\n\n"<<NORMAL<<
			" Outputs are specified as :\n\n"
			"        'src_var->route & route & ....' \n\n"
			" where a \"route\" is a string with any of the following formats\n\n"
			"    dest_var:dest_host:dest_port\n"
			"    dest_host:dest_port\n"
			"    dest_var:multicast_channel\n"
			"    multicast_channel\n\n"
			" This looks more complicated than it is so some examples are helpful\n\n"
			" 1) sharing \"VAR1\" to port 10007 on 18.38.2.158 as udp and call it \"Var2\" \n"
			"  ./pShare  -o='VAR1->Var2:18.38.2.158:10007' \n"
	        " 2) sharing as above but only at 4Hz \n"
	                    "  ./pShare  -o='VAR1->Var2:18.38.2.158:10007@4.0' \n"
			" 3) if you aren't bothered about renaming...\n"
			"  ./pShare  -o='VAR1->oceanai.mit.edu:10007' \n"
			" 4) if you want to use a predefined multicast channel, say 8,...\n"
			"  ./pShare  -o='VAR1->multicast_8' \n"
			" 5) if you want to rename the variable\n"
			"  ./pShare  -o='VAR1->VAR3:multicast_8\n\n"
			" Of course any variable can be sent to many routes. Here we send VAR1 to three places\n"
			" multicast channel 8 as VAR3, multicast channel 2 as VAR1 (no renaming) and a machine\n"
			" called oceanaias \"oranges\":\n\n"
			" 6) ./pShare  -o='VAR1->VAR3:multicast_8 & multicast_2 & oranges:oceanai.mit.edu:10007' \n\n"
			" And finally you can specify many shares at once. Here we share VAR1 and VAR2\n\n"
			" 7) ./pShare  -o='VAR1->multicast_8,VAR2->oranges:oceanai.mit.edu:10007' \n\n";


	std::cout<<YELLOW<<"\nSpecifying wildcard shares:\n\n"<<NORMAL<<
			" MOOS Supports wildcard subscriptions and this reflected in pShare. It is possible\n"
			" to replace src_var with a var_pattern:app_pattern tuple. Only * and ? wildcarding\n"
			" is supported. The app_pattern can be ommitted and a * is assumed. Again some examples\n"
			" are helpful.\n\n"
			" 7) sharing anything from an app called MyApp to a port on ocean AI, no renaming and\n"
			"    multicast 8:\n"
			"   ./pShare -o='*:MyApp->oceanai.mit.edu:1007 & multicast_8' \n"
			" 8) sharing any variable ending _WARNING from any three letter-named app beginning with ST at 10Hz max \n"
			"   ./pShare -o='*_WARNING:ST?->localhost:8009@10.0\n\n"
			" Renaming on wildcard shares is of course hard to specify (you don't know what is going to\n"
			" fit the bill. So in the case of wildcarding the dest_name field, if specified is used as\n"
			" a prefix:\n\n"
			" 9) sharing any variable <V> beginning with X e from an app called GPS as shared_<V>\n"
			"   ./pShare -o='V*:GPS->shared_:localhost:8009\n\n ";


}

void ShareHelp::PrintInterface()
{
	std::cout<<RED<<"\n--------------------------------------------\n";
	std::cout<<RED<<"      \"pShare\" Interface Description    \n";
	std::cout<<RED<<"--------------------------------------------\n";

	std::cout<<GREEN<<"\nSynopsis:\n\n"<<NORMAL;
	std::cout<<
			"Transfers data between a connected MOOSDB and udp based connections.\n"
			"Best way to understand this is via an example. A client could be publishing\n"
			"variable X to the DB, pShare can subscribe to that variable, rename it as\n"
			"Y and push send it via UDP to any number of specified address.\n"
			"The addresses need not be simple UDP destinations, pShare also supports \n"
			"multicasting. There are 256 predefined multicast channels available which can \n"
			"be reference with the short hand multicast_N, where N<256. Forwarding data\n"
			"to a multicast channel allows one send to be received by any number of receivers.\n\n"
			"Of course pShare can also receive data on udp / multicast channel and forward it to\n"
			"the local, connected MOOSDB. This is done using the input directive. \n"
			"pShare supports dynamic sharing configuration by subscribing to PSHARE_CMD\n"
			"or <AppName>_CMD if running under and alias.\n";

	std::cout<<GREEN<<"\nSubscriptions:\n\n"<<NORMAL;
	std::cout<<"  a) <AppName>_CMD\n\n";
	std::cout<<YELLOW<<"PSHARE_CMD\n"<<NORMAL;
	std::cout<<"This variable can be used to dynamically configure sharing at run time\n"
			"It has the following format:\n";
	std::cout<<"     cmd=<IO directive>,<details>\n";
	std::cout<<"where directive is either \"output\" or \"input\" for example \n";
	std::cout<<"     cmd = output,src_name = P,dest_name = H,route=multicast_9,frequency=10.0 \n";




	std::cout<<GREEN<<"\nPublishes:\n\n"<<NORMAL;
	std::cout<<"  a) <AppName>_INPUT_SUMMARY\n";
	std::cout<<"  b) <AppName>_OUTPUT_SUMMARY\n\n";


	std::cout<<YELLOW<<"PSHARE_OUTPUT_SUMMARY\n"<<NORMAL;
	std::cout<<"This variable describes the forwarding or sharing currently being undertaken by pShare.\n";
	std::cout<<"\nIt has the following format:\n ";
	std::cout<<"  Output = src_name->route1 & route 2, src_name->route1 & route 2....\n";
	std::cout<<"where a route is a colon delimited tuple\n ";
	std::cout<<"  dest_name:host_name:port:protocol \n";
	std::cout<<"example:\n";
	std::cout<<"  \"Output = X->Y:165.45.3.61:9000& Z:165.45.3.61.2000:multicast_8,K->K:192.168.66.12:3000\"\n";
	std::cout<<"\n\n";
	std::cout<<YELLOW<<"PSHARE_INPUT_SUMMARY\n"<<NORMAL;
	std::cout<<"This variable describes channels and ports on which pShare receives data.\n";
	std::cout<<"\nIt has the following format:\n ";
	std::cout<<"  Input = hostname:port:protocol,hostname:port:protocol...\n";
	std::cout<<"example:\n";
	std::cout<<"  \"input = localhost:9001 , 221.1.1.18:multicast_18\"\n";
	std::cout<<"\n\n";

}
