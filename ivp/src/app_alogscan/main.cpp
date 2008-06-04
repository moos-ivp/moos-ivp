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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include "MBUtils.h"
#include "ALogScanner.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  //if((argc != 5) || (!strcmp(argv[1], "-h"))) {
  //  cout << "Usage: alogscan filename newfilename  " << endl;
  //  return(1);
  // }
  
  string alogfile = "";
  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".alog"))
      alogfile = sarg;
  }
 
  cout << "Processing on file : " << alogfile << endl;

  bool ok = false;
    
  ALogScanner scanner;
  if(scanner.openALogFile(alogfile)) {
    cout << "File " << alogfile << " found and opened. " << endl;
    if(scanner.scan()) {
      cout << "...successfully completed" << endl;
      ok = true;
    }
  }

  
  if(ok)
    exit(0);
  if(!ok) {
    cout << "ALog Scan NOT successfully completed" << endl;
    exit(-1);
  }
}






