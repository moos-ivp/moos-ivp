/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: June 22nd, 2013                                      */
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
#include "SortHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogsort", "gpl");
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
  
  bool check_only = false;
  if(scanArgs(argc, argv, "-c", "--check", "-check"))
    check_only = true;
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  alogsort in.alog [out.alog] [OPTIONS]                    " << endl;
    cout << "                                                           " << endl;
    cout << "Synopsis:                                                  " << endl;
    cout << "  Create a new MOOS .alog file by sorting the alog entries " << endl;
    cout << "  based on their timestamps.                               " << endl;
    cout << "                                                           " << endl;
    cout << "Standard Arguments:                                        " << endl;
    cout << "  in.alog  - The input logfile.                            " << endl;
    cout << "  out.alog - The newly generated output logfile. If no     " << endl;
    cout << "             file provided, output goes to stdout.         " << endl;
    cout << "                                                           " << endl;
    cout << "Options:                                                   " << endl;
    cout << "  -h,--help     Displays this help message                 " << endl;
    cout << "  -v,--version  Displays the current release version       " << endl;
    cout << "  -f,--force    Force overwrite of existing file           " << endl;
    cout << "  -c,--check    Just check the ordering with no sorting    " << endl;
    cout << "  -q,--quiet    Verbose report suppressed at conclusion    " << endl;
    cout << "                                                           " << endl;
    cout << "See also:                                                  " << endl;
    cout << "  aloggrep, alogscan, alogrm, alogclip, alogview           " << endl;
    cout << endl;
    return(0);
  }

  string alogfile_in;
  string alogfile_out;
  string cache_size = "1000";

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strEnds(argi, ".alog")) {
      if(alogfile_in == "")
	alogfile_in = argi;
      else 
	alogfile_out = argi;
    }
    if(strBegins(argi, "--cache="))
      cache_size = argi.substr(8);
  }
 
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }
  else if(verbose)
    cout << "Processing on file : " << alogfile_in << endl;
  
  SortHandler handler;
  handler.setFileOverWrite(file_overwrite);

  if(cache_size != "") {
    unsigned int csize = (unsigned int)(atof(cache_size.c_str()));
    handler.setCacheSize(csize);
  }

  if(check_only) {
    bool in_order = handler.handleCheck(alogfile_in);
    if(in_order)
      cout << "All alog file entries are in order" << endl;
    else
      cout << "Out-of-order state detected." << endl;
  }
  else {
    handler.handleSort(alogfile_in, alogfile_out);
    handler.printReport();
  }

  cout << "Sorter Cache size: " << handler.getCacheSize() << endl;
}











