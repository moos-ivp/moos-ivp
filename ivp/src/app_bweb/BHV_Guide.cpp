/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_Guide.cpp                                        */
/*    DATE: Oct 29th 2022                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iterator>
#include <iostream>
#include <cstring>
#include "BHV_Guide.h"
#include "OpenURL.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Procedure: setBehaviorName()

bool BHV_Guide::setBehaviorName(string bhv)
{
  bhv = tolower(bhv);

  if(bhv == "waypoint")
    m_bhv_name = bhv;
  else if(bhv == "loiter")
    m_bhv_name = bhv;
  else {
    cout << "Unrecognized behavior: [" << bhv << "]" << endl;
    cout << "NOTE: This tool is new. Check back later." << endl;    
    return(false);
  }
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setAction()

bool BHV_Guide::setAction(string str)
{
  str = tolower(str);
  if(str == "web")
    m_action = str;
  else if(str == "example")
    m_action = str;
  else if(str == "params")
    m_action = str;
  else if(str == "mission")
    m_action = str;
  else
    return(false);

  return(true);
}

//------------------------------------------------------------
// Procedure: guide()

void BHV_Guide::guide()
{
  if(m_action == "web") {
    bool handled = true;
    if(m_bhv_name == "waypoint")
      openURLX("https://oceanai.mit.edu/ivpman/bhvs/Waypoint");
    else if(m_bhv_name == "loiter")
      openURLX("https://oceanai.mit.edu/ivpman/bhvs/Loiter");
    else
      handled = false;

    if(!handled)
      cout << "Unknown behavior: [" << m_bhv_name << "]" << endl;
  }
  
}





