/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 9th 2008                                         */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <cstring>
#include <vector>
#include "MBUtils.h"
#include "PokeDB.h"
#include "PokeDB_Info.h"

using namespace std;

int main(int argc ,char * argv[])
{
  string mission_file;

  vector<string> varname;
  vector<string> varvalue;
  vector<string> vartype;

  string server_host     = "localhost";
  bool   server_host_set = false;
  int    server_port     = 9000;
  bool   server_port_set = false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      showExampleConfigAndExit();
    else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strContains(argi, ":=")) {
      vector<string> svector = parseString(argi, ":=");
      if(svector.size() != 2)
	showHelpAndExit();
      else {
	varname.push_back(stripBlankEnds(svector[0]));
	varvalue.push_back(svector[1]);
	vartype.push_back("string!");
      }
    }
    else if(strContains(argi, "=")) {
      string left  = biteStringX(argi, '=');
      string right = argi;
      string lleft = tolower(left);
      if((lleft == "server_host") || (lleft == "serverhost") ||
	 (lleft == "host") || (left == "server")) {
	server_host     = right;
	server_host_set = true;
      }
      else if((lleft == "server_port") || (lleft=="serverport") ||
	      (left == "port")) {
	if(isNumber(right)) {
	  server_port     = atoi(right.c_str());
	  server_port_set = true;
	}
      }
      else {
	varname.push_back(left);
	if(isNumber(argi)) {
	  varvalue.push_back(argi);
	  vartype.push_back("double");
	}
	else {
	  varvalue.push_back(argi);
	  vartype.push_back("string");
	}
      }
    }
  }

  // If the mission file is not provided, we prompt the user if the 
  // server_host or server_port information is not on command line.
  if(mission_file == "") {
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
  
  if(mission_file == "") {
    cout << "Mission File not provided. " << endl;
    cout << "  server_host  = " << server_host << endl;
    cout << "  server_port  = " << server_port << endl;
    Poker.setConfigureCommsLocally(true);
  }
  else
    cout << "Mission File was provided: " << mission_file << endl;
  
  unsigned int j, vsize = varname.size();
  for(j=0; j<vsize; j++) {
    if((vartype[j] == "double") || 
       ((varvalue[j] == "@MOOSTIME") && (vartype[j] != "string!")))
      Poker.setPokeDouble(varname[j], varvalue[j]);
    else
      Poker.setPokeString(varname[j], varvalue[j]);
  }
  
  Poker.Run("uPokeDB", mission_file.c_str());

  return(0);
}


