/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT Dept. of Mechanical Engineering             */
/*    FILE: NodeRecord.cpp                                  */
/*    DATE: Feb 7th, 2011                                   */
/************************************************************/

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

