/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeRecord.cpp                                       */
/*    DATE: Feb 7th, 2011                                        */
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
#include "NodeRecord.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

NodeRecord::NodeRecord()
{
  // State variables
  m_x         = 0;
  m_y         = 0;
  m_speed     = 0;
  m_timestamp = 0;
  m_valid     = false;

  // Configuration variables: Note defaults may also be set in 
  // objects that use this data structure, e.g., SRS_Model.
  m_query_freq   = 30;
  m_query_range = 100;
}


//------------------------------------------------------------
// Procedure: getSpec()

string NodeRecord::getSpec() const
{
  string str;
  
  str += "name=" + m_name;
  str += ",x=" + doubleToStringX(m_x,2);
  str += ",y=" + doubleToStringX(m_y,2);
  str += ",speed" + doubleToStringX(m_speed,1);
  str += ",tstamp=" + doubleToStringX(m_timestamp,2);
  str += ",query_range="  + doubleToStringX(m_query_range,1);
  str += ",query_freq="  + doubleToStringX(m_query_freq,1);
  
  return(str);
}


//------------------------------------------------------------
// Procedure: queryAllowed
//   Purpose: Determine if a query may be made from this node at
//            this time. Queries can only be made so often, and
//            otherwise have to wait.

bool NodeRecord::queryAllowed(double curr_time) const
{
  double elapsed_time = curr_time - m_timestamp;
  if(elapsed_time > m_query_freq)
    return(true);
  else
    return(false);
}

//------------------------------------------------------------
// Procedure: getElapsedTime()

double NodeRecord::getElapsedTime(double curr_time) const
{
  return(curr_time - m_timestamp);
}


