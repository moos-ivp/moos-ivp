/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CBA_Model.cpp                                        */
/*    DATE: Aug 17th, 2011 (Waterloo)                            */
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

#include <cstdlib>
#include <iostream>
#include "CBA_Model.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

CBA_Model::CBA_Model()
{
  // Configuration variables
  m_verbose = true;

  // State variables
  m_iterations = 0;
  m_curr_time  = 0;

  m_max_tasks = 100;
  m_bundle.setMaxTasks(m_max_tasks);
  m_path.setMaxTasks(m_max_tasks);
}

//------------------------------------------------------------
// Procedure: setParam

bool CBA_Model::setParam(string param, string value, unsigned int pass)
{
  param = tolower(param);

  if(pass == 0) {
    if(param == "verbose")
      return(setBooleanOnString(m_verbose, value));
    else
      return(false);
  }
  else if(pass == 1) {
    if(param == "default_beacon_freq")
      return(true);
    else
      return(false);
  }
  else if(pass == 2) {
    if(param == "beacon")
      return(true);
    else
      return(false);
  }
  return(true);
}


//------------------------------------------------------------
// Procedure: handleMsg

bool CBA_Model::handleMsg(string key, double dval, string sval,
			  double mtime, bool isdouble, bool isstring,
			  string source)
{
  if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
    return(false);

  return(true);
}

//------------------------------------------------------------
// Procedure: setCurrTIme

void CBA_Model::setCurrTime(double new_time)
{
  if(new_time > m_curr_time)
    m_curr_time = new_time;
}

//------------------------------------------------------------
// Procedure: iterate()

void CBA_Model::iterate()
{
  m_iterations++;
}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> CBA_Model::getMessages(bool clear)
{
  if(!clear)
    return(m_messages);
  
  vector<VarDataPair> rval = m_messages;
  m_messages.clear();
  return(rval);
}


//------------------------------------------------------------
// Procedure: print()

void CBA_Model::print() const
{
  cout << "CBBA Model: " << endl;
}

//---------------------------------------------------------
// Procedure: addMessage()

void CBA_Model::addMessage(const string& varname, const string& value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: addMessage()

void CBA_Model::addMessage(const string& varname, double value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

