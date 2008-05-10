/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 9th 2008                                         */
/*          Motivated by Matt Grund's uMOOSPoke App              */
/*****************************************************************/

#include <vector>
#include "PokeDB.h"
#include "MBUtils.h"

using namespace std;

int main(int argc ,char * argv[])
{
  string sMissionFile = "PokeDB.moos";
  
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
      vector<string> svector = parseString(sarg, '=');
      if(svector.size() != 2)
	help_requested = true;
      else {
	varname.push_back(stripBlankEnds(svector[0]));
	if(isNumber(stripBlankEnds(svector[1]))) {
	  varvalue.push_back(stripBlankEnds(svector[1]));
	  vartype.push_back("double");
	}
	else {
	  varvalue.push_back(svector[1]);
	  vartype.push_back("string");
	}
      }
    }
  }

  if(help_requested) {
    MOOSTrace("uPokeDB: Usage:\n\n");
    MOOSTrace("  PokeDB [foo.moos] <varname=value> <varname=value>\n\n");
    return(0);
  }
  else {
    PokeDB Poker;
    int vsize = varname.size();
    for(int j=0; j<vsize; j++) {
      if(vartype[j] == "string")
	Poker.setPokeString(varname[j], varvalue[j]);
      else
	Poker.setPokeDouble(varname[j], atof(varvalue[j].c_str()));
    }
    string cmd = argv[0];
    Poker.Run(cmd.c_str(), sMissionFile.c_str());
  }

  return(0);
}
