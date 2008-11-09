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

  string server     = "localhost";
  bool   server_set = false;
  int    port       = 9000;
  bool   port_set   = false;

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
      string left = stripBlankEnds(biteString(sarg, '='));
      string right = stripBlankEnds(sarg);
      if(left == "server") {
	server = right;
	server_set = true;
      }
      else if((left == "port") && (isNumber(right))) {
	port = atoi(right.c_str());
	port_set = true;
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

  if(!mission_file_provided) {
    char buff[1000];
    cout << "Enter IP address:  [localhost] ";
    fgets(buff, 999, stdin);
    if(buff[0] != '\n') {
      server = buff;    
      server_set = true;
    }
    cout << "Enter Port number: [9000] ";
    fgets(buff, 999, stdin);
    if(buff[0] != '\n') {
      port = atoi(buff); 
      port_set = true;
    }
  }
  
  PokeDB Poker(server, port);

  if(mission_file_provided)
    Poker.setConfigureCommsLocally(true);

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
