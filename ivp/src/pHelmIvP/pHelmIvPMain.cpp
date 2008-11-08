/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: pHelmIvP.cpp                                         */
/*    DATE: Oct 12th 2004                                        */
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

#include <iostream>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "HelmIvP.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  bool version_requested = false;

  for(int i=0; i<argc; i++) {
    string str = argv[i];
    if((str=="-v") || (str=="--version") || (str=="-version"))
      version_requested = true;
  }
  
  if(version_requested) {
    vector<string> svector = getReleaseInfo("pHelmIvP");
    for(int i=0; i<svector.size(); i++)
      if(!strContains(svector[i], "ree"))
	cout << svector[i] << endl;
    return(0);
  }


  HelmIvP helmIvP;
  
  char *sMissionFile = "pHelmIvP.moos";
  if(argc > 1)
    sMissionFile = argv[1];

  for(int i=2; i<argc; i++)
    helmIvP.addBehaviorFile(argv[i]);

  helmIvP.Run("pHelmIvP", sMissionFile);
  
  return(0);
}










