/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HTrail.h                                          */
/*    DATE: Jul 3rd 2005 Sunday morning at Brueggers             */
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
 
#ifndef BHV_HTRAIL_HEADER
#define BHV_HTRAIL_HEADER

#include "IvPBehavior.h"

using namespace std;


class IvPDomain;
class BHV_HTrail : public IvPBehavior {
public:
  BHV_HTrail(IvPDomain);
  ~BHV_HTrail() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState() ;

protected:
  double getRelevance(double, double, double, double, double, double);
  double getPriority();
  int decode(string);
  int state;  

private:
  std::string them_name; // Name for them in InfoBuffer
  double trail_range;
  double trail_angle;
  double radius;
  double max_range;
  double obsolete;
  double speed_delta;
  int them_id;
  
  double contact_x,contact_y,contact_heading,contact_speed,contact_time;
  double cnCRS, cnSPD,cnX,cnY,cnTime,curr_time;
  int contact_id,new_state; 
  bool my_contact ;
};
#endif








