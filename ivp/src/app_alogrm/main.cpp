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
#include "FiltHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("alogfilt");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  alogrm input.alog [SWITCHES] [VAR] [VAR] [output.alog]" << endl;
    cout << "  Notes: The second alog is the output file. Otherwise  " << endl;
    cout << "         argument ordering is irrelevent.               " << endl;
    cout << "         An input file is mandatory.                    " << endl;
    cout << "         VAR* will pattern-match on VAR                 " << endl;
    cout << "  Switches:                                             " << endl;
    cout << "    -nostr will remove lines with string data values    " << endl;
    cout << "    -nonum will remove lines with double data values    " << endl;
    cout << "    -clean will remove lines that have a timestamp that " << endl;
    cout << "           is non-numerical or lines w/ no 4th column   " << endl;
    cout << "    -times will rewrite all timestamps to be time since " << endl;
    cout << "           logger started and LOGSTART is set to zero.  " << endl;
    return(0);
  }

  vector<string> keys;

  string alogfile_in;
  string alogfile_out;
  string bad_switch;

  bool nostrings = false;
  bool nonumbers = false;
  bool timeshift = false;
  bool rm_bad_lines = false;

  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".alog")) {
      if(alogfile_in == "")
	alogfile_in = sarg;
      else 
	alogfile_out = sarg;
    }
    else if(strContains(sarg, "-nonum"))
      nonumbers = true;
    else if(strContains(sarg, "-nostr"))
      nostrings = true;
    else if(strContains(sarg, "-clean"))
      rm_bad_lines = true;
    else if(strContains(sarg, "-times"))
      timeshift = true;
    else if(sarg.at(0) == '-')
      bad_switch = sarg;
    else
      keys.push_back(sarg);
  }
 
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }

  if(bad_switch != "") {
    cout << "Bad switch [" << bad_switch << "] - exiting" << endl;
    exit(0);
  }

  FiltHandler handler;

  int ksize = keys.size();
  for(int i=0; i<ksize; i++) 
    handler.setParam("newkey", keys[i]);
  
  if(nonumbers)
    handler.setParam("nonumbers", "true");
  
  if(rm_bad_lines)
    handler.setParam("clean", "true");

  if(nostrings)
    handler.setParam("nostrings", "true");

  if(timeshift)
    handler.setParam("timeshift", "true");


  handler.handle(alogfile_in, alogfile_out);
}






