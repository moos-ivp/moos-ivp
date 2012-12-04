/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Shadow.h                                         */
/*    DATE: May 10th 2005                                        */
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
#ifndef BHV_SHADOW_HEADER
#define BHV_SHADOW_HEADER

#include <string>
#include "IvPContactBehavior.h"

class IvPDomain;
class BHV_Shadow : public IvPContactBehavior {
public:
  BHV_Shadow(IvPDomain);
  ~BHV_Shadow() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  bool   updateInfoIn();

  double getRelevance();
  double getPriority();
  
private: // Configuration Parameters
  double  m_pwt_outer_dist;     // if target not in range, 0 relevance
  double  m_hdg_basewidth;
  double  m_hdg_peakwidth;
  double  m_spd_basewidth;
  double  m_spd_peakwidth;
};
#endif












