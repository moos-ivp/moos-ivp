/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: DomainExpander.h                                     */
/*    DATE: April 18th, 2011                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
