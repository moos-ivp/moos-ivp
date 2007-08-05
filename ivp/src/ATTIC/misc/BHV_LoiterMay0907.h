/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Loiter.h                                         */
/*    DATE: July 26th 2005 In Elba w/ M.Grunde, P.Newman         */
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
 
#ifndef BHV_LOITER_HEADER
#define BHV_LOITER_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYPolygon.h"
#include "LoiterEngine.h"

class IvPDomain;
class BHV_Loiter : public IvPBehavior {
public:
  BHV_Loiter(IvPDomain);
  ~BHV_Loiter() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

private:
  LoiterEngine loiter_engine;
  XYPolygon    poly;
  bool         poly_changed;

  bool         clockwise;
  bool         acquire_mode;    // true:acquiring, false:loitering
  int          current_waypt;
  double       current_cpa;
  double       arrival_radius;
  double       desired_speed;
  double       nm_radius;
 
  double       acquire_dist;
  int          hits;
  int          nm_hits;
  int          iterations;
};
#endif








