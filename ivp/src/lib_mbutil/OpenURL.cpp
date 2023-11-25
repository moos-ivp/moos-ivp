/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: OpenURL.cpp                                          */
/*    DATE: Jan 23rd, 2022                                       */
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

#include <cstdlib>
#include <iostream>
#include "OpenURL.h"
#include "MBUtils.h"

#ifdef __linux__
  #define OPEN_URL_FUNCTION "xdg-open "
#elif _WIN32
  #define OPEN_URL_FUNCTION "xdg-open "
#else
  #define OPEN_URL_FUNCTION "open "
#endif

using namespace std;

//---------------------------------------------------------
// Procedure: openURL

void openURL(string url)
{
  if(!strBegins(url, "http"))
    return;

  string system_cmd = OPEN_URL_FUNCTION + url;
  system(system_cmd.c_str());
}

//---------------------------------------------------------
// Procedure: openURLX

void openURLX(string url)
{
  cout << "Opening browser to: " << url << endl;
  openURL(url);
  exit(0);
}









