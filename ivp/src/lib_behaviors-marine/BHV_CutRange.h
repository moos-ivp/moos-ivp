/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_CutRange.h                                       */
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
 
#ifndef BHV_CUTRANGE_HEADER
#define BHV_CUTRANGE_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_CutRange : public IvPBehavior {
public:
  BHV_CutRange(IvPDomain);
  ~BHV_CutRange() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();

protected:
  double getRelevance(double, double, double, double);
  double getPriority();
  
private:
  std::string m_them_name; // Name for them in InfoBuffer

  double  m_min_util_cpa_dist;
  double  m_max_util_cpa_dist;

  double  m_max_priority_range;
  double  m_min_priority_range;

  double  m_giveup_range;
  double  m_patience;

  double  m_time_on_leg;
};
#endif










