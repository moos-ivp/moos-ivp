/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: mainEchoVar.cpp                                      */
/*    DATE: July 22nd 2006                                       */
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

#include <string>
#include "EchoVar.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "ColorParse.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("pEchoVar", "gpl");
    return(0);
  }

  int    i;
  string mission_file = "";
  string run_command  = argv[0];
  bool   help_requested  = false;

  for(i=1; i<argc; i++) {
    string argi = argv[i];
    if(strEnds(argi, ".moos"))
      mission_file = argv[i];
    else if(strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if((argi == "--help")||(argi=="-h"))
      help_requested = true;
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
  }
  
  if((mission_file == "") || help_requested) {
    cout << "Usage: pEchoVar file.moos [OPTIONS]                       " << endl;
    cout << "                                                          " << endl;
    cout << "Options:                                                  " << endl;
    cout << "  --alias=<ProcessName>                                   " << endl;
    cout << "      Launch pEchoVar with the given process name         " << endl;
    cout << "      rather than pEchoVar.                               " << endl;
    cout << "  --help, -h                                              " << endl;
    cout << "      Display this help message.                          " << endl;
    cout << "  --version,-v                                            " << endl;
    cout << "      Display the release version of pEchoVar.            " << endl;
    return(0);
  }

  cout << termColor("green");
  cout << "pEchoVar running as: " << run_command << endl;
  cout << termColor() << endl;

  EchoVar transponder;
  
  transponder.Run(run_command.c_str(), mission_file.c_str());

  return(0);
}



