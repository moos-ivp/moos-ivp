/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RealmCast.h                                          */
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

#ifndef REALM_CAST_HEADER
#define REALM_CAST_HEADER

#include <string>

class RealmCast
{
 public:
  RealmCast();
  virtual ~RealmCast() {};
  
  void  msg(const std::string& str)     {m_messages = str;};
  void  setProcName(std::string s)      {m_proc_name=s;}; 
  void  setNodeName(std::string s)      {m_node_name=s;}; 
  void  setCount(unsigned int val)      {m_count=val;}
  
  std::string::size_type size() const   {return(m_messages.size());}
  
  std::string  getProcName() const      {return(m_proc_name);}
  std::string  getNodeName() const      {return(m_node_name);}
  unsigned int getCount() const         {return(m_count);}
  
  std::string  getRealmCastString() const;
  std::string  getFormattedString() const;
  
 protected: // Configuration vars
  std::string  m_proc_name;
  std::string  m_node_name;
  std::string  m_messages;

  unsigned int m_count;
};

RealmCast string2RealmCast(std::string);

#endif

