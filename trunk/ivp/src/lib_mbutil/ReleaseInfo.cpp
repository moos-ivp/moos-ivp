/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MBUtils.cpp                                          */
/*    DATE: (1996-2005)                                          */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "ReleaseInfo.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showReleaseInfo

void showReleaseInfo(string app, string license_info)
{
  string pad = "";
  if(app.length() < 22)
    pad = padString("", (22-app.length()));
  cout << "********************************************************************" << endl;
  cout << "* " + app + " - Part of the MOOS-IvP Release Bundle     " +  pad +"*" << endl;
  //cout << "* Version 17.7 Released Jul 31st, 2017, www.moos-ivp.org          *" << endl;
  cout << "* Version 19.8.1 Post-Release Development Branch (trunk)           *" << endl;
  cout << "* M.Benjamin, H.Schmidt and J.Leonard (MIT), P.Newman (Oxford)     *" << endl;
  if(license_info == "gpl") {
    cout << "* This is free software; see the source for copying conditions.    *" << endl;
  }
  cout << "********************************************************************" << endl;
  cout << "" << endl;
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit(string app, string license_info)
{
  showReleaseInfo(app, license_info);
  exit(0);
}






