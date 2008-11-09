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
#include "GrabHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("aloggrab");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  aloggrab input.alog VAR [VAR] [VAR] [output.alog]" << endl;
    cout << "  (#2 alog is output - otherwise order is irrelevent)" << endl;
    cout << "  (Must provide input file and at least one variable)" << endl;
    cout << "  (VAR* will pattern-match on VAR)" << endl << endl;
    return(0);
  }

  vector<string> keys;

  string alogfile_in;
  string alogfile_out;

  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".alog")) {
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
    exit(0);
  }
  else  
    cout << "Processing on file : " << alogfile_in << endl;

  GrabHandler handler;

  int ksize = keys.size();
  for(int i=0; i<ksize; i++) {
    cout << "Grabbing key: " << keys[i] << endl;
    handler.addKey(keys[i]);
  }

  handler.handle(alogfile_in, alogfile_out);
}






