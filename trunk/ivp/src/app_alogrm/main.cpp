/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 3rd, 2008                                       */
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

#include <string>
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "FiltHandler.h"

using namespace std;

void help_message();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for help information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    help_message();
    return(0);
  }

  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogrm", "gpl");
    return(0);
  }

  bool verbose = true;
  if(scanArgs(argc, argv, "-q", "--quiet", "-quiet"))
    verbose = false;

  FiltHandler handler;  
  if(scanArgs(argc, argv, "-nostr", "--nostr"))
    handler.setParam("nostrings", "true");
  if(scanArgs(argc, argv, "-nonum", "--nonum"))
    handler.setParam("nonumbers", "true");
  if(scanArgs(argc, argv, "-nostr", "--nostr"))
    handler.setParam("nostrings", "true");
  if(scanArgs(argc, argv, "-clean", "--clean"))
    handler.setParam("clean", "true");
  if(scanArgs(argc, argv, "-f", "-force", "--force"))
    handler.setParam("file_overwrite", "true");

  string alogfile_in;
  string alogfile_out;
  string bad_switch;

  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".alog")) {
      if(alogfile_in == "")
	alogfile_in = sarg;
      else 
	alogfile_out = sarg;
    }
    else if(sarg.at(0) != '-')
      handler.setParam("newkey", sarg);
  }
 
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }

  if(bad_switch != "") {
    cout << "Bad switch [" << bad_switch << "] - exiting" << endl;
    exit(0);
  }

  bool handled = handler.handle(alogfile_in, alogfile_out);
  if(handled && verbose)
    handler.printReport();
}


//-------------------------------------------------------------
// Procedure: help_message

void help_message()
{
  cout << "Usage: " << endl;
  cout << "  alogrm in.alog [VAR] [SRC] [out.alog] [OPTIONS]           " << endl;
  cout << "                                                            " << endl;
  cout << "Synopsis:                                                   " << endl;
  cout << "  Remove the entries matching the given MOOS variables or   " << endl;
  cout << "  sources from given .alog file and generate a new .alog    " << endl;
  cout << "  file.                                                     " << endl;
  cout << "                                                            " << endl;
  cout << "Standard Arguments:                                         " << endl;
  cout << "  in.alog  - The input logfile.                             " << endl;
  cout << "  out.alog - The newly generated output logfile. If no      " << endl;
  cout << "             file provided, output goes to stdout.          " << endl;
  cout << "  VAR      - The name of a MOOS variable                    " << endl;
  cout << "  SRC      - The name of a MOOS process (source)            " << endl;
  cout << "                                                            " << endl;
  cout << "Options:                                                    " << endl;
  cout << "  -h,--help     Displays this help message                  " << endl;
  cout << "  -v,--version  Displays the current release version        " << endl;
  cout << "  -f,--force    Force overwrite of existing file            " << endl;
  cout << "  -q,--quiet    Verbose report suppressed at conclusion     " << endl;
  cout << "  --nostr       Remove lines with string data values        " << endl;
  cout << "  --nonum       Remove lines with double data values        " << endl;
  cout << "  --clean       Remove lines that have a timestamp that is  " << endl;
  cout << "                non-numerical or lines w/ no 4th column     " << endl;
  //cout << "  --times       Rewrite all timestamps to be time since   " << endl;
  //cout << "                logger started and LOGSTART is set to zero" << endl;
  cout << "                                                            " << endl;
  cout << "Further Notes:                                              " << endl;
  cout << "  (1) The second alog is the output file. Otherwise the     " << endl;
  cout << "      order of arguments is irrelevant.                     " << endl;
  cout << "  (2) VAR* matches any MOOS variable starting with VAR      " << endl;
  cout << "  (3) See also: alogscan, aloggrep, alogclip, alogview      " << endl;
  cout << endl;
}
  









