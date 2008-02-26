/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    MOD.: Henrik Schmidt                                       */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_PeriodicSurface.h                                */
/*    DATE: Feb 21st 2007                                        */
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
 
#ifndef BHV_HPERIODIC_SURFACE_HEADER
#define BHV_HPERIODIC_SURFACE_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_HPeriodicSurface : public IvPBehavior {
public:
  BHV_HPeriodicSurface(IvPDomain);
  ~BHV_HPeriodicSurface() {};
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();
  void         onIdleState() {checkForMarking();};

protected:
  void   checkForMarking();

private: // Behavior Parameters
  std::string mark_variable;
  std::string status_variable;
  std::string speed_function;
  double      period;
  double      zero_speed_depth,surface_speed_depth;
  double      speed_to_surface,speed_at_surface,deploy_speed,deploy_depth;
  double      max_time_at_surface;

private: // Behavior State Variables
  bool        first_iteration;
  bool        first_marking_string;
  bool        first_marking_double;
  bool        surfacing;
  double      mark_time;
  std::string curr_mark_string_val;
  double      curr_mark_double_val;
};
#endif

