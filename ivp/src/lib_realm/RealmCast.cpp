/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RealmCast.cpp                                        */
/*    DATE: Dec 4th 2020                                         */
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
#include <sstream>
#include "MBUtils.h"
#include "RealmCast.h"

using namespace std;

//----------------------------------------------------------------
// Constructor(s)

RealmCast::RealmCast()
{
  m_count = 0;
}

//----------------------------------------------------------------
// Procedure: getRealmCastString()
//   Example: str = "node=henry!Q!proc=pFoobar!Q!count=!Q!
//                   messages=now is the!Z!time for all!Z!good men to"

string RealmCast::getRealmCastString() const
{
  string osep = "!Q!"; // outer separator
  string isep = "!Z!"; // inner separator
  
  stringstream ss;
  ss << "node="  << m_node_name << osep;
  ss << "proc="  << m_proc_name << osep;
  if(m_count > 0)
    ss << "count="  << m_count << osep;
    
  
  // Add the messages (the free-form content of the realmcast)
  ss << "messages=";
  string messages_copy = m_messages;
  while(messages_copy != "")
    ss << biteString(messages_copy, '\n') << isep;
 
  return(ss.str());
}

//----------------------------------------------------------------
// Procedure: getFormattedString()

string RealmCast::getFormattedString() const
{
  stringstream ss;

  string title_str = m_proc_name;
  string counter_str = m_node_name + " (" + uintToString(m_count) + ")";

  int pad_amt = 39 - counter_str.length();

  if(pad_amt > 0)
    title_str = padString(title_str, pad_amt, false);

  title_str = title_str + counter_str;

  
  ss << "=======================================" << endl;
  ss << title_str << endl;
  ss << "=======================================" << endl;
  
  ss << m_messages << endl;
  
  return(ss.str());
}

//----------------------------------------------------------------
// Procedure: string2RealmCast
//   Example: str = "node=henry!Q!proc=pFoobar!Q!
//                   messages=now is the!Z!time for all!Z!good men to"

RealmCast string2RealmCast(std::string str)
{
  string osep = "!Q!"; // outer separator
  string isep = "!Z!"; // inner separator

  char c_osep = 30;  // ASCII 30 is "Record separator"
  char c_isep = 29;  // ASCII 29 is "Group separator"
  
  string str_osep(1, c_osep);
  string str_isep(1, c_isep);

  str = findReplace(str, osep, str_osep);
  str = findReplace(str, isep, str_isep);
  
  RealmCast relcast;
  
  string rc_str = str;
  while(rc_str != "") {
    string pair  = biteStringX(rc_str, c_osep);
    string param = biteStringX(pair, '=');
    string value = pair;
    if(param == "node") {
      relcast.setNodeName(value);
    }
    else if(param == "proc") {
      relcast.setProcName(value);
    }
    else if(param == "count") {
      relcast.setCount(atoi(value.c_str()));
    }
    else if(param == "messages") {  // May not be nec. Just set the rest
      stringstream ss;
      while (value != "")
	ss << biteString(value, c_isep) << endl;
      relcast.msg(ss.str());
    }
  }
  
  return(relcast);
}

