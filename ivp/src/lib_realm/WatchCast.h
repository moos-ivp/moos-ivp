/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WatchCast.h                                          */
/*    DATE: Dec 18th 2020                                        */
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

#ifndef REALM_CAST_VAR_HEADER
#define REALM_CAST_VAR_HEADER

#include <string>
#include <map>

class WatchCast
{
 public:
  WatchCast();
  ~WatchCast() {}

  void setNode(std::string s)      {m_node=s;}
  void setVarName(std::string s)   {m_varname=s;}
  void setSource(std::string s)    {m_source=s;}
  void setCommunity(std::string s) {m_community=s;}
  void setSVal(std::string s)      {m_sval=s; m_sval_set=true;}
  void setDVal(double d)           {m_dval=d; m_dval_set=true;}
  void setLocTime(double d)        {m_loc_time=d;}
  void setUtcTime(double d)        {m_utc_time=d;}

  std::string get_spec() const;

  std::string getNode() const      {return(m_node);}
  std::string getVarName() const   {return(m_varname);}
  std::string getSource() const    {return(m_source);}
  std::string getCommunity() const {return(m_community);}
  std::string getSVal() const      {return(m_sval);}

  double  getDVal() const    {return(m_dval);}
  double  getLocTime() const {return(m_loc_time);}
  double  getUtcTime() const {return(m_utc_time);}
  bool    isDouble() const   {return(m_dval_set);}
  bool    valid() const;
  
 private: 
  std::string m_node;
  std::string m_varname;
  std::string m_source;
  double      m_loc_time;
  double      m_utc_time;
  double      m_dval;
  std::string m_sval;
  std::string m_community;

  bool        m_sval_set;
  bool        m_dval_set;
};

WatchCast string2WatchCast(std::string);

#endif 

