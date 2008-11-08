/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: RepeaterMain.cpp                                */
/*    DATE: Oct 5th 2007                                    */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Repeater.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  string sMissionFile = "Repeater.moos";
  string sMOOSName = "uRepeater";

  bool version_requested = false;

  for(int i=0; i<argc; i++) {
    string str = argv[i];
    if((str=="-v") || (str=="--version") || (str=="-version"))
      version_requested = true;
  }
  
  if(version_requested) {
    vector<string> svector = getReleaseInfo("uRepeater");
    for(int i=0; i<svector.size(); i++)
      cout << svector[i] << endl;
    return(0);
  }

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  Repeater Repeater;
  
  Repeater.Run(sMOOSName.c_str(), sMissionFile.c_str());

  return(0);
}

