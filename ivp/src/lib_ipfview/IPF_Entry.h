/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPF_Entry.h                                          */
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

#ifndef BUNDLE_ENTRY_HEADER
#define BUNDLE_ENTRY_HEADER

#include <string>
#include "QuadSet.h"
#include "IvPDomain.h"
#include "IvPFunction.h"

class IPF_Entry
{
public:
  IPF_Entry(const std::string&);
  ~IPF_Entry() {};
  
  QuadSet       getQuadSet(IvPDomain);
  IvPFunction*  getIvPFunction();

  std::string   getIPFString() const  {return(m_ipf_str);};
  unsigned int  getPieces() const     {return(m_pieces);};
  double        getPriority() const   {return(m_priority);};
  IvPDomain     getDomain() const     {return(m_ivp_domain);};

 protected:
  std::string    m_ipf_str;
  unsigned int   m_pieces;
  double         m_priority;
  IvPDomain      m_ivp_domain;

  QuadSet        m_quadset;
};

#endif

