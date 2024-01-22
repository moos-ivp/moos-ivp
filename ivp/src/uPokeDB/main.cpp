/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "OpenURL.h"
#include "PokeDB.h"
#include "PokeDB_Info.h"

using namespace std;

int main(int argc ,char * argv[])
{
  string mission_file;

  vector<string> pokes;

  string server_host = "";  // localhost
  int    server_port = 0;   // 9000
  bool   use_cache = false;
  
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

    else if((argi == "-q") || (argi == "--quiet")) {
      std::ofstream file("/dev/null");
      std::streambuf *strm_buffer = std::cout.rdbuf();
      cout.rdbuf(file.rdbuf());
      cout.rdbuf(strm_buffer);
    }
    else if(strBegins(argi, "--host="))       // recommended to user
      server_host = argi.substr(7);
    else if(strBegins(argi, "host="))
      server_host = argi.substr(5);
    else if(strBegins(argi, "serverhost="))
      server_host = argi.substr(11);
    else if(strBegins(argi, "server_host="))
      server_host = argi.substr(12);

    else if((argi == "--cache") || (argi == "-c"))
      use_cache = true;

    else if(strBegins(argi, "--port="))      // recommended to user
      server_port = atoi(argi.substr(7).c_str());
    else if(strBegins(argi, "port="))
      server_port = atoi(argi.substr(5).c_str());
    else if(strBegins(argi, "serverport="))
      server_port = atoi(argi.substr(11).c_str());
    else if(strBegins(argi, "server_port="))
      server_port = atoi(argi.substr(12).c_str());
    else if((argi == "-w") || (argi == "--web") || (argi == "-web"))
      openURLX("https://oceanai.mit.edu/ivpman/apps/uPokeDB");

    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strContains(argi, "="))
      pokes.push_back(argi);
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
  Poker.setUseCache(use_cache);
  
  if(mission_file == "") {
    cout << "Mission File not provided. " << endl;
    cout << "  server_host  = " << server_host << endl;
    cout << "  server_port  = " << server_port << endl;
    Poker.setConfigureCommsLocally(true);
  }
  else
    cout << "Mission File was provided: " << mission_file << endl;
  
  for(unsigned int j=0; j<pokes.size(); j++)
    Poker.setPoke(pokes[j]);
  
  Poker.Run("uPokeDB", mission_file.c_str(), argc, argv);

  return(0);
}










