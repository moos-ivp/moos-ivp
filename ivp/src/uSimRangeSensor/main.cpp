/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: Massachusetts Institute of Technology           */
/*          - Department of Mechanical Engineering          */
/*          - Center for Ocean Engineering                  */
/*          - Comp. Science and Artificial Itelligence Lab  */
/*    FILE: main.cpp                                        */
/*    DATE: Feb 1st 2011                                    */
/************************************************************/

#include <iostream>
#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "SRS_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  string sMissionFile = "uSimRangeSensor.moos";
  string sMOOSName    = "uSimRangeSensor";
  bool   version_requested = false;

  for(int i=0; i<argc; i++) {
    string str = argv[i];
    if((str=="-v") || (str=="--version") || (str=="-version"))
      version_requested = true;
  }
  
  if(version_requested) {
    vector<string> svector = getReleaseInfo("uSimRangeSensor");
    unsigned int j;
    for(j=0; j<svector.size(); j++)
      cout << svector[j] << endl;
    return(0);
  }

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  SRS_MOOSApp sim_range_sensor;

  sim_range_sensor.Run(sMOOSName.c_str(), sMissionFile.c_str());
 
  return(0);
}

