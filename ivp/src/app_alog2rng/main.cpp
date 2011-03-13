/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: June 3rd, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "Alog2Range.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("alog2rng");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  alog2rng file.alog  [OPTIONS]        " << endl;
    cout << "                                                           " << endl;
    cout << "Synopsis:                                                  " << endl;
    cout << "  Perform one of several optional eports based on          " << endl;
    cout << "  helm output logged in the given .alog file.              " << endl;
    cout << "                                                           " << endl;
    cout << "Options:                                                   " << endl;
    cout << "  -h,--help     Displays this help message.                " << endl;
    cout << "  -v --version  Displays the current release version.      " << endl;
    cout << "  --verbose     Turns on verbose output.                   " << endl;
    cout << "                                                           " << endl;
    cout << "  --delim=val   Determines the report format.              " << endl;
    cout << "                (PADDED, tab, or space)                    " << endl;
    cout << "  --report=val  Determines how columns are delimited.      " << endl;
    cout << "                (TABLE, flat)                              " << endl;
    cout << "  --nohdr       Turns off the header line in the report.   " << endl;
    cout << "  --nav=bool    Include NAV_* in reports                   " << endl;
    cout << "                (TRUE, false)                              " << endl;
    cout << "                                                           " << endl;
    cout << "Further Notes:                                             " << endl;
    cout << "  (1) The order of arguments is irrelevent.                " << endl;
    cout << "  (2) Only the first specified .alog file is reported on.  " << endl;
    cout << endl;
    return(0);
  }

  string alogfile;
  string delim;
  string report;
  bool   nohdr    = false;
  bool   verbose  = false;
  bool   post_nav = true;

  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strEnds(sarg, ".alog") && (alogfile == ""))
      alogfile = sarg;
    else if(strBegins(sarg, "--delim="))
      delim  = sarg.substr(8);
    else if(strBegins(sarg, "--report="))
      report  = sarg.substr(9);
    else if(strBegins(sarg, "--nav=")) {
      string bool_str  = tolower(sarg.substr(6));
      post_nav = (bool_str == "true");
    }
    else if(sarg == "--nohdr")
      nohdr = true;
    else if(sarg == "--verbose")
      verbose = true;
    else {
      cout << termColor("red");
      cout << "Unknown cmd line arg: [" << sarg << "]. Exiting." << endl;
      cout << termColor();
      return(0);
    }
  }
 
  if(alogfile == "") {
    cout << termColor("red");
    cout << "No alog file given - exiting" << endl;
    cout << termColor();
    exit(0);
  }

  Alog2Range converter;
  converter.setPostNav(post_nav);
  converter.setVerbose(verbose);

  if(report != "") {
    bool ok = converter.setReportType(report);
    if(!ok) {
      cout << termColor("red");
      cout << "Unknown report type requested: [" << report << "]" << endl;
      cout << termColor();
      return(0);
    }
  }
  
  if(nohdr)
    converter.setHeader(false);


  if(delim != "") {
    bool ok = converter.setDelim(delim);
    if(!ok) {
      cout << termColor("red");
      cout << "Unknown delimeter requested: [" << delim << "]. ";
      cout << "Exiting." << termColor() << endl;
      return(0);
    }
  }
  
  if(verbose) {
    cout << termColor("blue");
    cout << "Processing alog file : " << alogfile << endl;
    cout << termColor();
  }  

  bool handled = converter.handle(alogfile);
  if(handled)
    converter.printReport();
}
