/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 2nd, 2015                                        */
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
#include "OpenURL.h"
#include "ReleaseInfo.h"
#include "SplitHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogsplit", "gpl");
    return(0);
  }
  if(scanArgs(argc, argv, "-w", "-web", "--web")) 
    openURLX("https://oceanai.mit.edu/ivpman/apps/alogsplit");
    
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  alogsplit in.alog [OPTIONS]                              " << endl;
    cout << "                                                           " << endl;
    cout << "Synopsis:                                                  " << endl;
    cout << "  Split the given alog file into a directory, within which " << endl;
    cout << "  each MOOS variable is split into its own (klog) file     " << endl;
    cout << "  containing only that variable. The split will also create" << endl;
    cout << "  a summary.klog file with summary information.            " << endl;
    cout << "                                                           " << endl;
    cout << "  Given file.alog, file_alvtmp/ directory will be created. " << endl;
    cout << "  Will not overwrite directory if previously created.      " << endl;
    cout << "  This is essentially the operation done at the outset of  " << endl;
    cout << "  launching the alogview applicaton.                       " << endl;
    cout << "                                                           " << endl;
    cout << "Standard Arguments:                                        " << endl;
    cout << "  in.alog  - The input logfile.                            " << endl;
    cout << "                                                           " << endl;
    cout << "Options:                                                   " << endl;
    cout << "  -h,--help      Displays this help message                " << endl;
    cout << "  -v,--version   Displays the current release version      " << endl;
    cout << "  --verbose      Show output for successful operation      " << endl;
    cout << "  --dir=DIR      Override the default dir with given dir.  " << endl;
    cout << "                                                           " << endl;
    cout << "  --max_fptrs=N  Set max number of OS file pointers allowed" << endl;
    cout << "                 to be open during splitting. Default 125. " << endl;
    cout << "                                                           " << endl;
    cout << "  --web,-w   Open browser to:                              " << endl;
    cout << "             https://oceanai.mit.edu/ivpman/apps/alogiter  " << endl;
    cout << "                                                           " << endl;
    cout << "Further Notes:                                             " << endl;
    cout << "  (1) The order of arguments is irrelevant.                " << endl;
    cout << "  (2) See also: alogscan, alogrm, aloggrep, alogclip,      " << endl;
    cout << "      alogview " << endl;
    cout << endl;
    return(0);
  }

  string alogfile_in;
  string given_dir;
  string max_fptrs;
  
  bool verbose = false;
  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strEnds(sarg, ".alog")) {
      if(alogfile_in == "")
	alogfile_in = sarg;
      else {
	cout << "Only one alog file allowed." << endl;
	exit(1);
      }
    }
    else if(sarg == "--verbose")
      verbose = true;
    else if(strBegins(sarg, "--max_fptrs="))
      max_fptrs = sarg.substr(12);
    else if(strBegins(sarg, "--dir=")) 
      given_dir = sarg.substr(6);
  }
  
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(1);
  }
  
  SplitHandler handler(alogfile_in);
  handler.setVerbose(verbose);
  handler.setDirectory(given_dir);

  if(isNumber(max_fptrs)) {
    int int_max_fptrs = atoi(max_fptrs.c_str());
    handler.setMaxFilePtrCache((unsigned int)(int_max_fptrs));
  }
  
  bool handled = handler.handle();
  if(!handled)
    return(1);

  return(0);
}


