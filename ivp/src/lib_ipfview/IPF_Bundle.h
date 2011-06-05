/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPF_Bundle.h                                         */
/*    DATE: Apr 19th 2011                                        */
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

#ifndef FUNCTION_BUNDLE_HEADER
#define FUNCTION_BUNDLE_HEADER

#include <string>
#include "QuadSet.h"
#include "IPF_Entry.h"

// Contains all the functions (IPF_Entries) for a given iteration

class IPF_Bundle
{
public:
  IPF_Bundle() {};
  ~IPF_Bundle() {};
  
  void addIPF(const std::string&);
  
  QuadSet getCollectiveQuadSet(std::string s="collective-hdgspd");
  QuadSet getQuadSet(unsigned int);
  QuadSet getQuadSet(std::string source);

  double       getPriority(std::string source) const;
  unsigned int getPieces(std::string source) const;
  unsigned int size() const   {return(m_entries.size());};

  std::string getSource(unsigned int);
  std::string getIPFString(unsigned int);

  IvPDomain   getDomain() const {return(m_ivp_domain);};
  IvPDomain   getDomain(std::string source) const;

  std::vector<std::string> getIPFStrings();
  std::vector<std::string> getSources() const {return(m_sources);};
  
 protected: // both vectors keyed on a single IPF string
  std::vector<IPF_Entry>   m_entries;  
  std::vector<std::string> m_sources;  // source behavior name 

  unsigned int  m_iteration;
  IvPDomain     m_ivp_domain;
};

#endif

