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
#include "ScanHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  //if((argc != 5) || (!strcmp(argv[1], "-h"))) {
  //  cout << "Usage: alogscan filename newfilename  " << endl;
  //  return(1);
  // }

  bool   reverse_requested  = false;
  bool   help_requested     = false;
  bool   version_requested  = false;
  bool   app_stat_requested = false;
  string sort_style = "bychars_ascending";

  string alogfile = "";
  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".alog"))
      alogfile = sarg;
    else if(strContains(sarg, "-h"))
      help_requested = true;

    else if((sarg=="-v") || (sarg=="--version") || (sarg=="-version")) 
      version_requested = true;
    else if(strContains(sarg, "-chars"))
      sort_style = "bychars_descending";
    else if(strContains(sarg, "-lines"))
      sort_style = "bylines_descending";
    else if(strContains(sarg, "-sortlines"))
      sort_style = "bylines_ascending";
    else if(strContains(sarg, "-start"))
      sort_style = "bystarttime_descending";
    else if(strContains(sarg, "-stop"))
      sort_style = "bystoptime_descending";
    else if(strContains(sarg, "-appstat"))
      app_stat_requested = true;
  }
 
  //----------------------------------------------------------------
  // Check if version output is requested
  if(version_requested) {
    vector<string> svector = getReleaseInfo("alogscan");
    for(int i=0; i<svector.size(); i++)
      cout << svector[i] << endl;
    return(0);
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
  }


  if(help_requested) {
    cout << "Usage: alogscan [OPTIONS] alog-file           " << endl;
    cout << "                                              " << endl;
    cout << "Options:                                      " << endl;
    cout << "  -chars   Sort by total characters (default) " << endl;
    cout << "  -lines   Sort by total lines (descending)   " << endl;
    cout << "  -start   Sort by starting time (descending) " << endl;
    cout << "  -stop    Sort by stop time (descending)     " << endl;
    cout << "  -appstat Output application statistics      " << endl;
    cout << "  -r       Reverse the sorting output         " << endl;
    cout << "  -h       Output this message                " << endl;
    exit(0);
  }    
  
  cout << "Processing on file : " << alogfile << endl;

  ScanHandler handler;
  handler.setParam("sort_style", sort_style);
  handler.handle(alogfile);
  
  if(app_stat_requested)
    handler.appStatReport();

}






