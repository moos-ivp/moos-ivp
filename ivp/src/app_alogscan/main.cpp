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
  
  string sort_style;

  string alogfile = "";
  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(strContains(sarg, ".alog"))
      alogfile = sarg;
    else if(strContains(sarg, "sortchars-"))
      sort_style = "bychars_descending";
    else if(strContains(sarg, "sortchars"))
      sort_style = "bychars_ascending";

    else if(strContains(sarg, "sortlines-"))
      sort_style = "bylines_descending";
    else if(strContains(sarg, "sortlines"))
      sort_style = "bylines_ascending";

    else if(strContains(sarg, "sortstart-"))
      sort_style = "bystarttime_descending";
    else if(strContains(sarg, "sortstart"))
      sort_style = "bystarttime_ascending";

    else if(strContains(sarg, "sortstop-"))
      sort_style = "bystoptime_descending";
    else if(strContains(sarg, "sortstop"))
      sort_style = "bystoptime_ascending";
  }
 
  cout << "Processing on file : " << alogfile << endl;

  ScanHandler handler;
  handler.setParam("sort_style", sort_style);
  handler.handle(alogfile);

}






