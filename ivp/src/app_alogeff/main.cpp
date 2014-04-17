/*****************************************************************/
/*    NAME: Kyle Woerner, Michael Benjamin                       */
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
#include <cstring>
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "EffHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
 // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogscan", "gpl");
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage:                                               " << endl;
    cout << "  alogscan file.alog [OPTIONS]                       " << endl;
    cout << "                                                     " << endl;
    cout << "Synopsis:                                            " << endl;
    cout << "  Generate summary files of efficiency and collisions" << endl;
    cout << "  from MOOS .alog files.                             " << endl;
    cout << "                                                     " << endl;
    cout << "Required:                                             " << endl;
    cout << "  output=STRING  Where string is the prefix for     " << endl;
    cout << "                 output files.   " << endl;
    cout << "                    " << endl;
    cout << "Outputs:       " << endl;
    cout << "          STRING.adata for avoiding data.   " << endl;
    cout << "          STRING.tdata for transiting data.   " << endl;
    cout << "          STRING.cdata for collision data.   " << endl;
    return(0);
  }

  bool   reverse_requested  = false;
  bool   app_stat_requested = false;
  string proc_colors        = "true";
  string sort_style         = "bysrc_ascending";

  string alogfile = "";
  for(int i=1; i<argc; i++) {
    string orig = argv[i];
    string sarg = tolower(argv[i]);
    string sort = "";
    if(!strncmp(sarg.c_str(), "--sort=", 7)) {
      biteString(sarg, '=');
      sort = sarg;
    }

    if(strContains(sarg, ".alog"))
      alogfile = orig;
    else if((sarg == "-c") || (sarg == "--chars") || (sort == "chars"))
      sort_style = "bychars_ascending";
    else if((sarg == "-l") || (sarg == "--lines") || (sort == "lines"))
      sort_style = "bylines_ascending";
    else if((sarg == "--start") || (sort == "start"))
      sort_style = "bystarttime_ascending";
    else if((sarg == "--stop") || (sort == "stop"))
      sort_style = "bystoptime_ascending";
    else if((sarg == "--vars") || (sort == "vars") || (sort == "var"))
      sort_style = "byvars_ascending";
    else if((sarg == "--procs") || (sort == "procs") || (sort == "proc"))
      sort_style = "bysrc_ascending";
    else if(sarg == "--appstat")
      app_stat_requested = true;
    else if((sarg == "--nocolors") || (sarg == "-n"))
      proc_colors = "false";
    else if((sarg == "-r") || (sarg == "--reversed"))
      reverse_requested = true;
  }
 
  if(reverse_requested) {
    if(sort_style == "bychars_ascending")
      sort_style = "bychars_descending";
    else if(sort_style == "bylines_ascending")
      sort_style = "bylines_descending";
    else if(sort_style == "bystart_ascending")
      sort_style = "bystart_descending";
    else if(sort_style == "bystop_ascending")
      sort_style = "bystop_descending";
    else if(sort_style == "byvars_ascending")
      sort_style = "byvars_descending";
    else if(sort_style == "bysrc_ascending")
      sort_style = "bysrc_descending";
  }

  cout << "Processing on file : " << alogfile << endl;

  EffHandler handler;
  handler.setParam("sort_style",  sort_style);
  handler.setParam("proc_colors", proc_colors);
  handler.handle(argc, argv); //alogfile);
  
  if(app_stat_requested)
    handler.appStatReport();

}











