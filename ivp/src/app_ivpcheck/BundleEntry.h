/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BundleEntry.h                                        */
/*    DATE: Apr 19th 2011                                        */
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

#ifndef BUNDLE_ENTRY_HEADER
#define BUNDLE_ENTRY_HEADER

#include <string>
#include "QuadSet.h"

class BundleEntry
{
public:
  BundleEntry(const std::string&);
  ~BundleEntry() {};
  
  QuadSet       getQuadSet(IvPDomain);
  IvPFunction*  getIvPFunction();

  std::string   getIPFString()  {return(m_ipf_str);};
  unsigned int  getPieces()     {return(m_pieces);};
  double        getPriority()   {return(m_priority);};

 protected:
  std::string    m_ipf_str;
  unsigned int   m_pieces;
  double         m_priority;
};

#endif





