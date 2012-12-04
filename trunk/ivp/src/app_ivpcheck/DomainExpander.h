/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DomainExpander.h                                     */
/*    DATE: April 18th, 2011                                     */
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

#ifndef DOMAIN_EXPANDER_HEADER
#define DOMAIN_EXPANDER_HEADER

#include <vector>
#include <string>
#include "IvPDomain.h"
#include "Demuxer.h"
#include "IvPBox.h"

class DomainExpander
{
 public:
  DomainExpander();
  ~DomainExpander() {};

  void   setIvPDomain(std::string);
  void   setIvPDomain(IvPDomain);
  bool   isFullyExpanded() {return(m_fully_expanded);};
  IvPBox getCurrDomainPointBox(bool inc=true);

  void   incNextDomainPoint(bool=false);

 protected:
  IvPDomain m_ivp_domain;
  bool      m_fully_expanded; 

  std::vector<unsigned int> m_current_point;
};

#endif


