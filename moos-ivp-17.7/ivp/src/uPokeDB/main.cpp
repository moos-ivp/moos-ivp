/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 9th 2008                                         */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
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

  string server_host = "";  // localhost
  int    server_port = 0;   // 9000
  
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

    else if(strBegins(argi, "--host="))       // recommended to user
      server_host = argi.substr(7);
    else if(strBegins(argi, "host="))
      server_host = argi.substr(5);
    else if(strBegins(argi, "serverhost="))
      server_host = argi.substr(11);
    else if(strBegins(argi, "server_host="))
      server_host = argi.substr(12);

    else if(strBegins(argi, "--port="))      // recommended to user
      server_port = atoi(argi.substr(7).c_str());
    else if(strBegins(argi, "port="))
      server_port = atoi(argi.substr(5).c_str());
    else if(strBegins(argi, "serverport="))
      server_port = atoi(argi.substr(11).c_str());
    else if(strBegins(argi, "server_port="))
      server_port = atoi(argi.substr(12).c_str());

    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strContains(argi, ":=")) {
      vector<string> svector = parseString(argi, ":=");
      if(svector.size() != 2)
	showHelpAndExit();
      else {
	varname.push_back(stripBlankEnds(svector[0]));
	varvalue.push_back(stripBlankEnds(svector[1]));
	vartype.push_back("string!");
      }
    }
    else if(strContains(argi, "=")) {
      string left  = biteStringX(argi, '=');
      string right = argi;
      varname.push_back(left);
      if(isNumber(right)) {
	varvalue.push_back(right);
	vartype.push_back("double");
      }
      else {
	varvalue.push_back(right);
	vartype.push_back("string");
      }
    }
  }

  // If the mission file is not provided, we prompt the user if the 
  // server_host or server_port information is not on command line.
  if(mission_file == "") {
    char buff[1000];
    // If server_host info was not on the command line, prompt here.
    if(server_host == "") {
      server_host = "localhost";
      cout << "Enter IP address:  [localhost] ";
      if(fgets(buff, 999, stdin) != NULL) {
	if(buff[0] != '\n') {
	  server_host     = buff;    
	}
      }
    }
    // If server_port info was not on the command line, prompt here.
    if(server_port == 0) {
      cout << "Enter Port number: [9000] ";
      server_port = 9000;
      if(fgets(buff, 999, stdin) != NULL) {
	if(buff[0] != '\n') {
	  server_port     = atoi(buff); 
	}
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
  
  Poker.Run("uPokeDB", mission_file.c_str(), argc, argv);

  return(0);
}









