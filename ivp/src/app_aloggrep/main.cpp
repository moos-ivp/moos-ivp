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

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  GrepHandler handler;

  vector<string> keys;
  bool make_end_report = true;
  string alogfile_in;
  string alogfile_out;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogavg", "gpl");
      return(0);
    }
    else if((argi == "--no_comments") || (argi == "-nc"))
      handler.setCommentsRetained(false);
    else if((argi == "--keep_badlines") || (argi == "-kb"))
      handler.setBadLinesRetained(false);
    else if((argi == "--no_report") || (argi == "-nr"))
      make_end_report = false;
    else if((argi == "--gap_len") || (argi == "-gl"))
      handler.setGapLinesRetained(true);
    else if((argi == "--appcast") || (argi == "-ac"))
      handler.setAppCastRetained(true);
    else if((argi == "--sort") || (argi == "-s"))
      handler.setSortEntries(true);
    else if((argi == "--duplicates") || (argi == "-d"))
      handler.setRemoveDuplicates(true);
    else if((argi == "--csw") || (argi == "-csw"))
      handler.setColSep(' ');
    else if((argi == "--csc") || (argi == "-csc"))
      handler.setColSep(',');
    else if((argi == "--cso") || (argi == "-cso"))
      handler.setColSep(':');
    else if((argi == "--css") || (argi == "-css"))
      handler.setColSep(';');
    else if((argi == "--sd") || (argi == "-sd")) {
      handler.setSortEntries(true);
      handler.setRemoveDuplicates(true);
    }
    else if(argi == "--final") {
      handler.setFinalEntryOnly(true);
      make_end_report  = false;
    }   
    else if((argi == "--finalx") || (argi == "-x")) {
      handler.setFinalEntryOnly(true);
      handler.setFinalValueOnly(true);
      make_end_report  = false;
    }   
    else if((argi == "--finalz") || (argi == "-z")) {
      handler.setFinalEntryOnly(true);
      handler.setFinalTimeOnly(true);
      make_end_report  = false;
    }   
    else if((argi == "--values_only") || (argi == "-vo")) {
      make_end_report  = false;
      handler.setValuesOnly(true);
      handler.setCommentsRetained(false);
    }
    else if((argi == "--times_values_only") || (argi == "-tvo")) {
      make_end_report  = false;
      handler.setValuesOnly(true);
      handler.setTimesOnly(true);
      handler.setCommentsRetained(false);
    }
    else if((argi == "--quiet") || (argi == "-q")) {
      handler.setCommentsRetained(false);
      make_end_report  = false;
    }
    else if((argi == "--force") || (argi == "-force") || (argi == "-f")) 
      handler.setFileOverWrite(true);
    else if(strEnds(argi, ".alog") || strEnds(argi, ".klog")) {
      if(alogfile_in == "")
	alogfile_in = argi;
      else 
	alogfile_out = argi;
    }
    else
      handler.addKey(argi);
  }
 
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(1);
  }
  else if(make_end_report)
    cout << "aloggrep - Processing on file: " << alogfile_in << endl;
  
  bool handled = handler.handle(alogfile_in, alogfile_out);
  if(!handled)
    return(1);

  if(make_end_report)
    handler.printReport();

  return(0);
}




//------------------------------------------------------------
// Procedure: showHelpAndExit()  

void showHelpAndExit()
{
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
  cout << "  -s,--sort         Sort the log entries                   " << endl;
  cout << "  -d,--duplicates   Remove Duplicate entries               " << endl;
  cout << "  -sd,--sd          Remove Duplicate AND sort              " << endl;
  cout << "                                                           " << endl;
  cout << "  --final           Output only final matching line        " << endl;
  cout << "  -x,--finalx       Output only final matching line's val  " << endl;
  cout << "  -z,--finalz       Output only final matching line's time " << endl;
  cout << "                                                           " << endl;
  cout << "  --values_only     Output only value part of each line    " << endl;
  cout << "  -vo                                                      " << endl;
  cout << "                                                           " << endl;
  cout << "  --times_vals_only Output time also in values_only mode   " << endl;
  cout << "  -tvo                                                     " << endl;
  cout << "                                                           " << endl;
  cout << "  --keep_badlines   Do not discard lines that don't begin  " << endl;
  cout << "  -kb               with a timestamp or comment character. " << endl;
  cout << "                                                           " << endl;
  cout << "Further Notes:                                             " << endl;
  cout << "  (1) The second alog is the output file. Otherwise the    " << endl;
  cout << "      order of arguments is irrelevant.                    " << endl;
  cout << "  (2) VAR* matches any MOOS variable starting with VAR     " << endl;
  cout << "  (3) The --sort and --duplicates options address an issue " << endl;
  cout << "      with pLogger in that some entries are out of order   " << endl;
  cout << "      and some entries are logged twice.                   " << endl;
  cout << "  (4) If the output file name is vname.alog, will attempt  " << endl;
  cout << "      to replace with vname_STR.alog where STR is the      " << endl;
  cout << "      detected community taken from DB_TIME source.        " << endl;
  cout << "  (5) See also: alogscan, alogrm, alogclip, alogsplit,     " << endl;
  cout << "      alogview, alogsort                                   " << endl;
  exit(0);
}

  
