/*****************************************************************/
/*    NAME: Henrik Schmidt                                       */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BHV_ClassifyYoyo.h                                   */
/*    DATE: Mar 3, 2007                                          */
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
 
#ifndef BHV_CLASSIFY_YOYO_HEADER
#define BHV_CLASSIFY_YOYO_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_ClassifyYoyo : public IvPBehavior {
public:
  BHV_ClassifyYoyo(IvPDomain);
  ~BHV_ClassifyYoyo() {};
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();
  void         onIdleState();

protected:

private: // Behavior Parameters
  std::string depth_function;
  double      period,period_segments,distance_traveled;
  double      min_depth,max_depth,max_depth_local,new_depth;
  double      deploy_speed,deploy_depth;
  double      min_altitude;

private: // Behavior State Variables
  bool        first_iteration;
  double      mark_time,mark_distance,mark_increment,start_time,iter_time;
};
#endif
 
