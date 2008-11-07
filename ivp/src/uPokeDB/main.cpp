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

int main(int argc ,char * argv[])
{
  const char *sMissionFile = "Mission.moos";
  const char *sMOOSName    = "uPokeDB";

  bool help_requested = false;

  vector<string> varname;
  vector<string> varvalue;
  vector<string> vartype;

  for(int i=0; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".moos"))
      sMissionFile = argv[i];
    else if(strContains(sarg, "-h"))
      help_requested = true;
    else if(strContains(sarg, ":=")) {
      vector<string> svector = parseString(sarg, ":=");
      if(svector.size() != 2)
	help_requested = true;
      else {
	varname.push_back(stripBlankEnds(svector[0]));
	varvalue.push_back(svector[1]);
	vartype.push_back("string");
      }
    }
    else if(strContains(sarg, "=")) {
      string left = biteString(sarg, '=');
      varname.push_back(stripBlankEnds(left));
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

  string server = "localhost";
  int    port   = 9000;

  if(!strcmp(sMissionFile, "Mission.moos")) {
    char buff[1000];
    cout << "Enter IP address:  [localhost] ";
    fgets(buff, 999, stdin);
    if(buff[0] != '\n') 
      server = buff;    
    cout << "Enter Port number: [9000] ";
    fgets(buff, 999, stdin);
    if(buff[0] != '\n') 
      port = atoi(buff);    
  }


  if(help_requested) {
    MOOSTrace("uPokeDB: Usage:\n\n");
    MOOSTrace("  PokeDB [foo.moos] <varname=value> <varname=value>\n\n");
    return(0);
  }

  PokeDB Poker(server, port);
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
