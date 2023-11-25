/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "OpenURL.h"
#include "ReleaseInfo.h"
#include "GrepHandler.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  GrepHandler handler;

  for(int i=1; i<argc; i++) {
    bool handled = true;
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogrep", "gpl");
      return(0);
    }
    else if((argi == "--n o_comments") || (argi == "-nc"))
      handler.setCommentsRetained(false);
    else if((argi == "--keep_bad") || (argi == "-kb"))
      handler.setBadLinesRetained(false);
    else if((argi == "--no_report") || (argi == "-nr"))
      handler.setMakeReport(false);
    else if((argi == "--gap_len") || (argi == "-gl"))
      handler.setGapLinesRetained(true);
    else if((argi == "--appcast") || (argi == "-ac"))
      handler.setAppCastRetained(true);
    else if((argi == "--sort") || (argi == "-s"))
      handler.setSortEntries(true);
    else if((argi == "--duplicates") || (argi == "-d"))
      handler.setRemoveDups(true);
    else if((argi == "--sd") || (argi == "-sd")) {
      handler.setSortEntries(true);
      handler.setRemoveDups(true);
    }

    else if(argi == "--kk")
      handler.setKeepKey(true);
    else if((argi == "--csw") || (argi == "-csw"))
      handler.setColSep(' ');
    else if((argi == "--csc") || (argi == "-csc"))
      handler.setColSep(',');
    else if((argi == "--cso") || (argi == "-cso"))
      handler.setColSep(':');
    else if((argi == "--css") || (argi == "-css"))
      handler.setColSep(';');

    else if((argi == "--v") || (argi == "-vo")) 
      handler.setFormat("val");
    else if((argi == "--s") || (argi == "-so")) 
      handler.setFormat("src");
    else if((argi == "--tv") || (argi == "-tvo"))
      handler.setFormat("time:val");
    else if(argi == "--tvv")
      handler.setFormat("time:var:val");

    else if(strBegins(argi, "--subpat=")) {
      string patterns = argi.substr(9);
      vector<string> svector = parseString(patterns, ':');
      for(unsigned int i=0; i<svector.size(); i++)
	handler.addSubPattern(svector[i]);
    }
    else if(strBegins(argi, "--format=")) 
      handled = handler.setFormat(argi.substr(9));
    else if(argi == "--final") 
      handler.setFinalOnly(true);
    else if(argi == "--first") 
      handler.setFirstOnly(true);


    else if((argi == "--quiet") || (argi == "-q")) {
      handler.setCommentsRetained(false);
      handler.setMakeReport(false);
    }
    else if((argi == "--force") || (argi == "-force") || (argi == "-f")) 
      handler.setFileOverWrite(true);
    else if(strEnds(argi, ".alog") || strEnds(argi, ".klog")) 
      handled = handler.setALogFile(argi);
    else if((argi == "-w") || (argi == "--web") || (argi == "-web"))
      openURLX("https://oceanai.mit.edu/ivpman/apps/aloggrep");
    else if(strBegins(argi, "-"))
      handled = false;
    else
      handler.addKey(argi);

    if(!handled) {
      cout << "Exiting due to bad arg: " << argi << endl;
      return(1);
    }
  }

  bool handled = handler.handle();
  if(!handled)
    return(1);

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
  cout << "  aloggrep is typically used in one of three ways:         " << endl;
  cout << "  (1) Create a new filtered alog file                      " << endl;
  cout << "  (2) Create a csv file for plotting                       " << endl;
  cout << "  (3) A quick-look command-line utility                    " << endl;
  cout << "                                                           " << endl;
  cout << "  In (1) a new alog file is provided as a target and some  " << endl;
  cout << "  set of variables to retain are specified. The new alog   " << endl;
  cout << "  file retains syntactic structure of an alog file, but all" << endl;
  cout << "  lines that don't match the grep pattern are removed.     " << endl;
  cout << "  In addition to variables to be retained, the user may    " << endl;
  cout << "  specifiy a MOOSApp source, and all variables published   " << endl;
  cout << "  by that source will be retained.                         " << endl;
  cout << "                                                           " << endl;
  cout << "  In (2) some subset of columns will be retained for one   " << endl;
  cout << "  or more specified variables. The columns are separated by" << endl;
  cout << "  a comma, or another chosen column separator.             " << endl;
  cout << "                                                           " << endl;
  cout << "  In (3), the user omits the second/target alog file and   " << endl;
  cout << "  output instead just goes to the terminal to enable a     " << endl;
  cout << "  quick look at log file contents                          " << endl;
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
  cout << "  -gl,--no_gaplen   Retain vars ending in _GAP or _LEN     " << endl;
  cout << "  -kb,--keep_bad    Don't discard lines that don't begin   " << endl;
  cout << "                    with a timestamp or comment character. " << endl;
  cout << "                                                           " << endl;
  cout << "  -s,--sort         Sort the log entries                   " << endl;
  cout << "  -d,--duplicates   Remove Duplicate entries               " << endl;
  cout << "  -sd,--sd          Remove Duplicate AND sort              " << endl;
  cout << "                                                           " << endl;
  cout << "  --web,-w   Open browser to:                              " << endl;
  cout << "             https://oceanai.mit.edu/ivpman/apps/aloggrep  " << endl;
  cout << "                                                           " << endl;
  cout << "Content/format Options:                                    " << endl;
  cout << "  --format=val or --v                                      " << endl;
  cout << "    Output only the value column                           " << endl;
  cout << "                                                           " << endl;
  cout << "  --format=time:val or --tv                                " << endl;
  cout << "    Output only the time and value columns                 " << endl;
  cout << "                                                           " << endl;
  cout << "  --format=time:var:val or --tvv                           " << endl;
  cout << "    Output only time, variable, and value columns          " << endl;
  cout << "                                                           " << endl;
  cout << "  --format=time:var:src                                    " << endl;
  cout << "    Output only time, variable, and source columns         " << endl;
  cout << "                                                           " << endl;
  cout << "  --first           Output only first matching line        " << endl;
  cout << "  --final           Output only final matching line        " << endl;
  cout << "  --csw,-csw        Columns separated with white space     " << endl;
  cout << "  --csc,-csc        Columns separated with a comma         " << endl;
  cout << "  --cso,-cso        Columns separated with a colon         " << endl;
  cout << "  --csc,-csc        Columns separated with a semi-colon    " << endl;
  cout << "                    (Default separator is white space )    " << endl;
  cout << "                                                           " << endl;
  cout << "  --subpat=pattern[:pattern:pattern]                       " << endl;
  cout << "    For postings with components with comma-separated      " << endl;
  cout << "    param=value components, one or more components can be  " << endl;
  cout << "    isolated. Typically used with --v, --format=val        " << endl;
  cout << "                                                           " << endl;
  cout << "  --keep-key, --kk                                         " << endl;
  cout << "    When --subpat is used, this option will keep the key   " << endl;
  cout << "    used in the sub-pattern. For example if the value of   " << endl;
  cout << "    the posting is \"x=23, y=42, z=7\" and --subpat=y:z    " << endl;
  cout << "    the result would be \"42 7\". With --kk it would be    " << endl;
  cout << "    \"y=42, z=7\"                                          " << endl;
  cout << "                                                           " << endl;
  cout << "Further Notes:                                             " << endl;
  cout << "  (1) The second alog is the output file. Otherwise the    " << endl;
  cout << "      order of arguments is irrelevant.                    " << endl;
  cout << "  (2) VAR* matches any MOOS variable starting with VAR     " << endl;
  cout << "  (3) The --sort and --duplicates options address an issue " << endl;
  cout << "      with pLogger in that some entries are out of order   " << endl;
  cout << "      and some entries are logged twice.                   " << endl;
  cout << "  (4) If output file name contains the string \"vname\",   " << endl;
  cout << "      e.g., foobar_vname.alog, aloggrep will attempt to    " << endl;
  cout << "      replace with vname pattern with the detected vehicle " << endl;
  cout << "      neme found in input alog, by searching for DB_TIME   " << endl;
  cout << "      entries and counting on the conventin that the source" << endl;
  cout << "      for these postings is ~MOOSDB_alpha.                 " << endl;
  cout << "                                                           " << endl;
  cout << "Examples:                                                  " << endl;
  cout << " $ aloggrep DEPLOY RETURN file.alog newfile.alog           " << endl;
  cout << " $ aloggrep file.alog ENCOUNTER_CPA --tv -csw results.dat  " << endl;
  cout << " $ aloggrep --sd file.alog new_file.alog                   " << endl;
  cout << " $ aloggrep file.alog NODE_REPORT_LOCAL --subpat=index     " << endl;
  cout << "                                                           " << endl;
  cout << "                                                           " << endl;
  cout << "See Also:                                                  " << endl;
  cout << "  (5) alogscan, alogrm, alogclip, alogsplit, alogavg,      " << endl;
  cout << "      alogview, alogsort.                                  " << endl;
  exit(0);
}

  

