/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMV_Viewer.h                                         */
/*    DATE: Nov 11th 2004                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef MARINE_VIEWER_HEADER
#define MARINE_VIEWER_HEADER

#include <iostream>
#include <string>
#include <map>
#include "MarineViewer.h"
#include "VehicleSet.h"
#include "VarDataPair.h"
#include "VPlug_GeoShapes.h"
#include "VPlug_GeoShapesMap.h"

class PMV_Viewer : public MarineViewer
{
 public:
  PMV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  modColorScheme() {};

  bool  setParam(std::string p, std::string v="");
  bool  setParam(std::string p, double v);
  void  setConfigComplete() {m_config_complete=true;}
  
  bool  handleNodeReport(std::string, std::string&);

  bool  addGeoShape(std::string p, std::string v, std::string c, double=0);
  bool  addScopeVariable(std::string);
  bool  updateScopeVariable(std::string varname, std::string value, 
			    std::string vtime, std::string vsource);
  bool  setAppCasting(std::string);
  bool  isAppCasting() const;
  void  setActiveScope(std::string);
  bool  isScopeVariable(std::string) const;
  void  addMousePoke(std::string side, std::string key, std::string vardata_pair);
  void  setLeftMouseKey(std::string key)  {m_left_mouse_key = key;}
  void  setRightMouseKey(std::string key) {m_right_mouse_key = key;}

  std::string getStringInfo(const std::string& info_type, int precision=0);

  std::vector<VarDataPair> getLeftMousePairs(bool=true);
  std::vector<VarDataPair> getRightMousePairs(bool=true);
  std::vector<VarDataPair> getNonMousePairs(bool=true);

  std::vector<std::string> getStaleVehicles(double thresh=10);

  unsigned int shapeCount(const std::string& gtype, const std::string& vname="") const;
  double       getVehiclesShapeScale() const;
  unsigned int getDrawCount() const {return(m_draw_count);}
  double       getCurrTime() const {return(m_curr_time);}
  double       getTimeWarp() const {return(m_time_warp);}
  double       getElapsed() const {return(m_elapsed);}

  void   setVerbose(bool bval=true) {m_verbose=bval;}
  
  void   clearGeoShapes(std::string vname, std::string shape, std::string stype);

  void   setCenterView(double x, double y);
  void   setCenterView(std::string vname);
  
 private:
  void   drawVehicle(std::string, bool, std::string);
  void   drawTrailPoints(CPList&, unsigned int=0);
  void   handleMouse(int, int, std::string s="left");
  void   handleMoveMouse(int, int);
  void   setWeightedCenterView();
  
 private:
  VehicleSet  m_vehiset;

  std::string m_reference_point;
  std::string m_reference_bearing;
  double      m_curr_time;
  double      m_time_warp;
  double      m_elapsed;

  bool        m_config_complete;
  
  unsigned int m_draw_count;
  double       m_last_draw_time;

  bool         m_verbose;
  
  // Member variables for holding scoped info
  bool                     m_scoping;
  std::vector<std::string> m_var_names;
  std::vector<std::string> m_var_vals;
  std::vector<std::string> m_var_source;
  std::vector<std::string> m_var_time;
  unsigned int             m_var_index;
  unsigned int             m_var_index_prev;

  // Member variables for holding/conveying mouse/button click info
  double   m_mouse_x;
  double   m_mouse_y;
  double   m_mouse_lat;
  double   m_mouse_lon;
  int      m_lclick_ix;
  int      m_rclick_ix;
  int      m_bclick_ix;

  std::string m_left_mouse_key;
  std::string m_right_mouse_key;
  std::vector<VarDataPair> m_var_data_pairs_all;
  std::vector<VarDataPair> m_var_data_pairs_lft;
  std::vector<VarDataPair> m_var_data_pairs_rgt;
  std::vector<VarDataPair> m_var_data_pairs_non_mouse;
  
  std::string m_centric_view; // average, active, or reference
  bool        m_centric_view_sticky;

  VPlug_GeoShapesMap  m_geoshapes_map;

  std::map<std::string, std::string> m_map_vcolor;
};

#endif 

