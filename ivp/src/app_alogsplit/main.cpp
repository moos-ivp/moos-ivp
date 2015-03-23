/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 2nd, 2015                                       */
/*****************************************************************/

#include <string>
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
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
  
  bool file_overwrite = false;
  if(scanArgs(argc, argv, "-f", "--force", "-force"))
    file_overwrite = true;
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  alogsplit in.alog [OPTIONS]                              " << endl;
    cout << "                                                           " << endl;
    cout << "Synopsis:                                                  " << endl;
    cout << "  Create a new MOOS .alog file by retaining only the       " << endl;
    cout << "  given MOOS variables or sources, named on the command    " << endl;
    cout << "  line, from a given .alog file.                           " << endl;
    cout << "                                                           " << endl;
    cout << "Standard Arguments:                                        " << endl;
    cout << "  in.alog  - The input logfile.                            " << endl;
    cout << "                                                           " << endl;
    cout << "Options:                                                   " << endl;
    cout << "  -h,--help         Displays this help message             " << endl;
    cout << "  -v,--version      Displays the current release version   " << endl;
    cout << endl;
    return(0);
  }

  string alogfile_in;

  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".alog")) {
      if(alogfile_in == "")
	alogfile_in = sarg;
      else {
	cout << "Only one alog file allowed." << endl;
	exit(1);
      }
    }
  }
 
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(1);
  }
  
  SplitHandler handler(alogfile_in);

  bool handled = handler.handle();
  if(!handled)
    return(1);

  return(0);
}






