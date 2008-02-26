/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HCutRange.h                                       */
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
 
#ifndef BHV_HCUTRANGE_HEADER
#define BHV_HCUTRANGE_HEADER

#include <string>
#include "IvPBehavior.h"

using namespace std;

class IvPDomain;
class BHV_HCutRange : public IvPBehavior {
public:
  BHV_HCutRange(IvPDomain);
  ~BHV_HCutRange() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState() ;

protected:
  double getRelevance(double, double, double, double);
  double getPriority();
  int decode(string);
  int state;  
  
private:
  std::string m_them_name; // Name for them in InfoBuffer
  double range_max; // if target not in range, 0 relevance
  double range_min; // if target not in range, 0 relevance
  double contact_x,contact_y,contact_heading,contact_speed,contact_time;
  double cnCRS, cnSPD,cnX,cnY,cnTime;
  int contact_id,them_id,new_state; 
  bool my_contact ;
  double obsolete;
};
#endif








