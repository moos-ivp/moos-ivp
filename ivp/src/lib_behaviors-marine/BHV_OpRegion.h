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
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();
  void         onIdleState()     {postErasablePolygon();};
  void         onCompleteState() {postErasablePolygon();};

 protected:
  void      polygonVerify();
  void      postPolyStatus();
  void      depthVerify();
  void      altitudeVerify();
  void      timeoutVerify();
  void      setTimeStamps();
  void      handleVisualHint(std::string);
  void      postViewablePolygon();
  void      postErasablePolygon();

 protected: // Parameters
  std::vector<XYPolygon> m_polygons;
  XYPolygon m_polygon;
  double    m_max_depth;
  double    m_min_altitude;
  double    m_max_time;
  double    m_trigger_entry_time;
  double    m_trigger_exit_time;
  bool      m_trigger_on_poly_entry;

  // Visual hints affecting properties of polygons/points
  std::string m_hint_vertex_color;
  std::string m_hint_edge_color;
  double      m_hint_vertex_size;
  double      m_hint_edge_size;

 protected: // State Variables
  bool      m_poly_entry_made;
  double    m_previous_time;  // Seconds
  double    m_current_time;
  double    m_delta_time;
  double    m_start_time;
  double    m_elapsed_time;

  double    m_secs_in_poly;
  double    m_secs_out_poly;

  bool      m_first_time;
  bool      m_previously_in_poly;


};
#endif










