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
#include "GrepHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("aloggrep", "gpl");
    return(0);
  }
  
  bool comments_retained = true;
  if(scanArgs(argc, argv, "--no_comments", "-nc"))
    comments_retained = false;
  
  bool badlines_retained = false;
  if(scanArgs(argc, argv, "--keep_badlines", "-kb"))
    badlines_retained = true;
  
  bool make_end_report = true;
  if(scanArgs(argc, argv, "--no_report", "-nr"))
    make_end_report = false;
  
  bool gaplines_retained = true;
  if(scanArgs(argc, argv, "--gap_len", "-gl"))
    gaplines_retained = false;
  
  bool appcast_retained = true;
  if(scanArgs(argc, argv, "--appcast", "-ac"))
    appcast_retained = false;
  
  if(scanArgs(argc, argv, "--quiet", "-q")) {
    comments_retained = false;
    make_end_report = false;
  }
    
  
  bool file_overwrite = false;
  if(scanArgs(argc, argv, "-f", "--force", "-force"))
    file_overwrite = true;
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  aloggrep in.alog [VAR] [SRC] [out.alog] [OPTIONS]        " << endl;
    cout << "                                                           " << endl;
    cout << "Synopsis:                                                  " << endl;
    cout << "  Create a new MOOS .alog file by retaining only the       " << endl;
    cout << "  given MOOS variables or sources, named on the command    " << endl;
    cout << "  line, from a given .alog file.                           " << endl;
    cout << "                                                           " << endl;
    cout << "Standard Arguments:                                        " << endl;
    cout << "  in.alog  - The input logfile.                            " << endl;
    cout << "  out.alog - The newly generated output logfile. If no     " << endl;
    cout << "             file provided, output goes to stdout.         " << endl;
    cout << "  VAR      - The name of a MOOS variable                   " << endl;
    cout << "  SRC      - The name of a MOOS process (source)           " << endl;
    cout << "                                                           " << endl;
    cout << "Options:                                                   " << endl;
    cout << "  -h,--help         Displays this help message             " << endl;
    cout << "  -v,--version      Displays the current release version   " << endl;
    cout << "  -f,--force        Force overwrite of existing file       " << endl;
    cout << "  -q,--quiet        Supress summary report, header comments" << endl;
    cout << "  -nc,--no_comments Supress comment (header) lines         " << endl;
    cout << "  -nr,--no_report   Supress summary report                 " << endl;
    cout << "  -gl,--no_gaplen   Supress vars ending in _GAP or _LEN    " << endl;
    cout << "  -ac,--no_appcast  Supress APPCAST lines                  " << endl;
    cout << "                                                           " << endl;
    cout << "  --keep_badlines   Do not disscard lines that don't begin " << endl;
    cout << "  -kb               with a timestamp or comment character. " << endl;
    cout << "                                                           " << endl;
    cout << "Further Notes:                                             " << endl;
    cout << "  (1) The second alog is the output file. Otherwise the    " << endl;
    cout << "      order of arguments is irrelevant.                    " << endl;
    cout << "  (2) VAR* matches any MOOS variable starting with VAR     " << endl;
    cout << "  (3) See also: alogscan, alogrm, alogclip, alogsplit,     " << endl;
    cout << "      alogview                                             " << endl;
    cout << "  (4) If the output file name is vname.alog, will attempt  " << endl;
    cout << "      to replace with vname_STR.alog where STR is the      " << endl;
    cout << "      detected community taken from DB_TIME source.        " << endl;
    cout << endl;
    return(0);
  }

  vector<string> keys;
  string alogfile_in;
  string alogfile_out;

  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strEnds(sarg, ".alog") || strEnds(sarg, ".klog")) {
      if(alogfile_in == "")
	alogfile_in = sarg;
      else 
	alogfile_out = sarg;
    }
    else
      keys.push_back(sarg);
  }
 
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(1);
  }
  else if(make_end_report)
    cout << "aloggrep - Processing on file: " << alogfile_in << endl;
  
  GrepHandler handler;
  handler.setFileOverWrite(file_overwrite);
  handler.setCommentsRetained(comments_retained);
  handler.setBadLinesRetained(badlines_retained);
  handler.setGapLinesRetained(gaplines_retained);
  handler.setAppCastRetained(appcast_retained);

  int ksize = keys.size();
  for(int i=0; i<ksize; i++)
    handler.addKey(keys[i]);

  bool handled = handler.handle(alogfile_in, alogfile_out);
  if(!handled)
    exit(1);

  if(handled && make_end_report)
    handler.printReport();
}










