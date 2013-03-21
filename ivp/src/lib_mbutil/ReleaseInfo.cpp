/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MBUtils.cpp                                          */
/*    DATE: (1996-2005)                                          */
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
#include "ReleaseInfo.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showReleaseInfo
// 

void showReleaseInfo(string app, string license_info)
{
  string pad = "";
  if(app.length() < 22)
    pad = padString("", (22-app.length()));
  cout << "********************************************************************" << endl;
  cout << "* " + app + " - Part of the MOOS-IvP Release Bundle     " +  pad +"*" << endl;
  cout << "* Version 13.2.2, Released Mar 21st, 2013, www.moos-ivp.org        *" << endl;
  cout << "* M.Benjamin, H.Schmidt and J.Leonard (MIT), P.Newman (Oxford)     *" << endl;
  if(license_info == "gpl") {
    cout << "* Copyright (C) 2008 Free Software Foundation, Inc.                *" << endl;
    cout << "* This is free software; see the source for copying conditions.    *" << endl;
  }
  cout << "********************************************************************" << endl;
  cout << "" << endl;
}


