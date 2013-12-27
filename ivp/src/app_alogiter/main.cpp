/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Dec 22nd, 2013                                       */
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
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "IterHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogiter", "gpl");
    return(0);
  }
  
  bool verbose = true;
  if(scanArgs(argc, argv, "--verbose", "-verbose"))
    verbose = true;
  if(scanArgs(argc, argv, "--quiet", "-quiet", "-q"))
    verbose = false;
  
  bool file_overwrite = false;
  if(scanArgs(argc, argv, "-f", "--force", "-force"))
    file_overwrite = true;
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  alogiter in.alog [VAR] [SRC] [out.alog] [OPTIONS]        " << endl;
    cout << "                                                           " << endl;
    cout << "Synopsis:                                                  " << endl;
    cout << "  Analyze the ITER_GAP and ITER_LEN information provided by" << endl;
    cout << "  all applications recorded in the given alog file.        " << endl;
    cout << "  Create a new MOOS .alog file by retaining only the       " << endl;
    cout << "                                                           " << endl;
    cout << "Standard Arguments:                                        " << endl;
    cout << "  file.alog - The input logfile.                           " << endl;
    cout << "                                                           " << endl;
    cout << "Options:                                                   " << endl;
    cout << "  -h,--help     Displays this help message                 " << endl;
    cout << "  -v,--version  Displays the current release version       " << endl;
    cout << "  -q,--quiet    Verbose report suppressed at conclusion    " << endl;
    cout << "                                                           " << endl;
    cout << "Further Notes:                                             " << endl;
    cout << "  (1) See also: alogscan, alogrm, alogclip, alogview       " << endl;
    cout << endl;
    return(0);
  }

  vector<string> keys;
  string alogfile_in;

  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".alog"))
      alogfile_in = sarg;
  }
 
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }
  else if(verbose)
    cout << "Processing on file : " << alogfile_in << endl;
  
  IterHandler handler;
  bool handled = handler.handle(alogfile_in);
  
  if(handled && verbose)
    handler.printReport();
}



