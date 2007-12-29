/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_OpRegion.h                                       */
/*    DATE: May 1st, 2005 Sunday at Joe's in Maine               */
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
 
#ifndef BHV_OPREGION_HEADER
#define BHV_OPREGION_HEADER

#include "XYPolygon.h"
#include "IvPBehavior.h"

class BHV_OpRegion : public IvPBehavior {
 public:
  BHV_OpRegion(IvPDomain);
  ~BHV_OpRegion() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

 protected:
  void      polygonVerify();
  void      postPolyStatus();
  void      depthVerify();
  void      altitudeVerify();
  void      timeoutVerify();
  void      setTimeStamps();

 protected:
  XYPolygon polygon;
  double    max_depth;
  double    min_altitude;
  double    max_time;

  bool      trigger_on_poly_entry;
  double    trigger_entry_time;
  double    trigger_exit_time;
  bool      poly_entry_made;

  double    previous_time;  // Seconds
  double    current_time;
  double    delta_time;
  double    start_time;
  double    elapsed_time;

  double    secs_in_poly;
  double    secs_out_poly;

  bool      first_time;
  bool      previously_in_poly;
};
#endif










