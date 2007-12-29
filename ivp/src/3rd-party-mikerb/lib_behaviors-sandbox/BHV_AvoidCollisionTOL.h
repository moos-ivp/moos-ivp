/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AvoidCollisionTOL.h                              */
/*    DATE: Jul 5th 2005                                         */
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
 
#ifndef BHV_AVOID_COLLISION_HEADER
#define BHV_AVOID_COLLISION_HEADER

#include "IvPBehavior.h"
#include "CPAEngine.h"

class IvPDomain;
class BHV_AvoidCollisionTOL : public IvPBehavior {
public:
  BHV_AvoidCollisionTOL(IvPDomain);
  ~BHV_AvoidCollisionTOL() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  bool   getBufferInfo();
  double getRelevance();
  double getPriority();
  
private:
  std::string them_name; // Name for them in InfoBuffer
  double active_distance;
  double collision_distance;
  double all_clear_distance;

  double os_x;
  double os_y;
  double os_heading;
  double os_speed;  
  double cn_x;
  double cn_y;
  double cn_heading;
  double cn_speed;


};
#endif









