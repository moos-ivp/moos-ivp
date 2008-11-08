/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: iMarineSimMain.cpp                                   */
/*    DATE: Jun 23rd 2006                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "IMS_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  string sMissionFile = "iMarineSim.moos";
  string sMOOSName    = "iMarineSim";
  bool   version_requested = false;

  for(int i=0; i<argc; i++) {
    string str = argv[i];
    if((str=="-v") || (str=="--version") || (str=="-version"))
      version_requested = true;
  }
  
  if(version_requested) {
    vector<string> svector = getReleaseInfo("iMarineSim");
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
  
  IMS_Model *ims_model = new IMS_Model;

  IMS_MOOSApp marine_sim;
  marine_sim.setModel(ims_model);

  marine_sim.Run(sMOOSName.c_str(), sMissionFile.c_str());
 
  return(0);
}



