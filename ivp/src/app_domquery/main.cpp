/*****************************************************************/
/*    NAME: Michael Benjamin, H. Schmidt, J. Leonard             */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: March 10th, _._2009                                    */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "DomQuery.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("domquery");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: domquery filename.dom" << endl;
    return(0);
  }

  DomQuery dom_query;
  string   dom_file;
  string   req_pieces;

  for(int i=1; i<argc; i++) {
    string arg = argv[i];
    if(strContains(arg, ".dom"))
      dom_file = arg;
    else if((arg == "-pieces") && (i+1 < argc))
      req_pieces = argv[i+1];    
  }

  dom_query.setInFile(dom_file);
  dom_query.buildDomain();

  if(isNumber(req_pieces))
    dom_query.requestPieceCount(atoi(req_pieces.c_str()));
  
  return(0);
}
