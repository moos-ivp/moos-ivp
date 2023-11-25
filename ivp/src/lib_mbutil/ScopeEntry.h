/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ScopeEntry.h                                         */
/*    DATE: May 9th 2012                                         */
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

#ifndef SCOPE_ENTRY_HEADER
#define SCOPE_ENTRY_HEADER

#include <string>

class ScopeEntry
{
 public:
  ScopeEntry() {}
  ~ScopeEntry() {}

  void  setValue(const std::string& s)      {m_value = s;}
  void  setType(const std::string& s)       {m_type = s;}
  void  setSource(const std::string& s)     {m_source = s;}
  void  setSrcAux(const std::string& s)     {m_srcaux = s;}
  void  setTime(const std::string& s)       {m_time = s;}
  void  setCommunity(const std::string& s)  {m_community = s;}
  void  setIteration(const std::string& s)  {m_iteration = s;}
  void  setNameBHV(const std::string& s)    {m_name_bhv = s;}

  const std::string& getValue() const      {return(m_value);}
  const std::string& getType() const       {return(m_type);}
  const std::string& getSource() const     {return(m_source);}
  const std::string& getSrcAux() const     {return(m_srcaux);}
  const std::string& getTime() const       {return(m_time);}
  const std::string& getCommunity() const  {return(m_community);}
  const std::string& getIteration() const  {return(m_iteration);}
  const std::string& getNameBHV() const    {return(m_name_bhv);}

  bool isEqual(const ScopeEntry& entry) const
  {
    return((entry.getValue()     == m_value)  &&
	   (entry.getType()      == m_type)   &&
	   (entry.getSource()    == m_source) &&
	   (entry.getSrcAux()    == m_srcaux) &&
	   (entry.getCommunity() == m_community) &&
	   (entry.getIteration() == m_iteration) &&
	   (entry.getNameBHV()   == m_name_bhv)  &&
	   (entry.getTime()      == m_time));
  }

 protected:
  std::string m_value;
  std::string m_type;
  std::string m_source;
  std::string m_srcaux;
  std::string m_time;
  std::string m_community;

  // In cases where the source auxilliary containts behavior info. 
  // SRC_AUX = pHelmIvP:45:wpt_survey
  std::string m_iteration;
  std::string m_name_bhv;
};

#endif 





