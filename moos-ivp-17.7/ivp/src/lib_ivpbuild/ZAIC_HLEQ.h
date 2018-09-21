/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_LEQ.h                                           */
/*    DATE: Jun 15th 2006                                        */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef OF_ZAIC_HLEQ_HEADER
#define OF_ZAIC_HLEQ_HEADER

#include <string>
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_HLEQ {
public:
  ZAIC_HLEQ(IvPDomain g_domain, const std::string& g_varname);
  virtual ~ZAIC_HLEQ() {}

  bool  setSummit(double);
  bool  setSummitDelta(double=1);
  bool  setBaseWidth(double);
  bool  setMinMaxUtil(double=0, double=100);

  double getParam(std::string);
  bool   stateOK()          {return(m_state_ok);}
  std::string getWarnings() {return(m_warning);}

  IvPDomain    getIvPDomain() {return(m_ivp_domain);}
  
protected:  // Parameters
  double  m_summit;
  double  m_summit_delta;
  double  m_basewidth;
  double  m_minutil;
  double  m_maxutil;

protected: // State values
  int     m_ipt_low;
  int     m_ipt_one;
  int     m_ipt_two;
  int     m_ipt_high;
  int     i_basewidth;

  int     m_domain_pts;
  int     m_domain_ix;
  double  m_domain_high;
  double  m_domain_low;
  double  m_domain_delta;

  bool         m_state_ok;
  std::string  m_warning;
  IvPDomain    m_ivp_domain;
};
#endif





