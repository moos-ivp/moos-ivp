/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 9th 2008                                         */
/*          Motivated by Matt Grund's uMOOSPoke App              */
/*****************************************************************/

#include <cstring>
#include <vector>
#include "PokeDB.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: display_usage

void display_usage()
{
  cout << "uPokeDB: Usage: " << endl;
  cout << "  PokeDB [foo.moos] [server=val] [port=val] <var=value> <var=value>" << endl;
}

//-----------------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("uPokeDB");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  // Look for a request for help or usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    display_usage();
    return(0);
  }

  const char *sMissionFile = "Mission.moos";
  const char *sMOOSName    = "uPokeDB";

  vector<string> varname;
  vector<string> varvalue;
  vector<string> vartype;

  string server_host     = "localhost";
  bool   server_host_set = false;
  int    server_port     = 9000;
  bool   server_port_set = false;

  for(int i=0; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".moos"))
      sMissionFile = argv[i];
    else if(strContains(sarg, ":=")) {
      vector<string> svector = parseString(sarg, ":=");
      if(svector.size() != 2) {
	display_usage();
	return(0);
      }
      else {
	varname.push_back(stripBlankEnds(svector[0]));
	varvalue.push_back(svector[1]);
	vartype.push_back("string");
      }
    }

    else if(strContains(sarg, "=")) {
      string left  = stripBlankEnds(biteString(sarg, '='));
      string right = stripBlankEnds(sarg);
      string lleft = tolower(left);
      if((lleft == "server_host") || (lleft == "serverhost")) {
	server_host     = right;
	server_host_set = true;
      }
      else if((lleft == "server_port") || (lleft=="serverport")) {
	if(isNumber(right)) {
	  server_port     = atoi(right.c_str());
	  server_port_set = true;
	}
      }
      else {
	varname.push_back(left);
	if(isNumber(stripBlankEnds(sarg))) {
	  varvalue.push_back(stripBlankEnds(sarg));
	  vartype.push_back("double");
	}
	else {
	  varvalue.push_back(sarg);
	  vartype.push_back("string");
	}
      }
    }
  }

  bool mission_file_provided = false;
  if(strcmp(sMissionFile, "Mission.moos"))
    mission_file_provided = true;

  // If the mission file is not provided, we prompt the user if the 
  // server_host or server_port information is not on command line.
  if(!mission_file_provided) {
    char buff[1000];
    // If server_host info was not on the command line, prompt here.
    if(!server_host_set) {
      cout << "Enter IP address:  [localhost] ";
      fgets(buff, 999, stdin);
      if(buff[0] != '\n') {
	server_host     = buff;    
	server_host_set = true;
      }
    }
    // If server_port info was not on the command line, prompt here.
    if(!server_port_set) {
      cout << "Enter Port number: [9000] ";
      fgets(buff, 999, stdin);
      if(buff[0] != '\n') {
	server_port     = atoi(buff); 
	server_port_set = true;
      }
    }
  }
  
  PokeDB Poker(server_host, server_port);

  if(mission_file_provided == false) {
    cout << "Mission File not provided. " << endl;
    cout << "  server_host  = " << server_host << endl;
    cout << "  server_port  = " << server_port << endl;
    Poker.setConfigureCommsLocally(true);
  }
  else {
    cout << "Mission File was provided: " << sMissionFile << endl;
  }

  int vsize = varname.size();
  for(int j=0; j<vsize; j++) {
    if(vartype[j] == "string")
      Poker.setPokeString(varname[j], varvalue[j]);
    else
      Poker.setPokeDouble(varname[j], atof(varvalue[j].c_str()));
  }
  

  Poker.Run(sMOOSName, sMissionFile);

  return(0);
}
