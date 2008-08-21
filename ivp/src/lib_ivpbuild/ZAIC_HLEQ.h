/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_LEQ.h                                           */
/*    DATE: Jun 15th 2006                                        */
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
  virtual ~ZAIC_HLEQ() {};

  bool  setSummit(double);
  bool  setBaseWidth(double);
  bool  setMinMaxUtil(double, double);

  double getParam(std::string);

protected:  // Parameters
  double  m_summit;
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












