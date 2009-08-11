/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_Viewer.h                                         */
/*    DATE: Nov 11th 2004                                        */
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

#ifndef MARINE_VIEWER_HEADER
#define MARINE_VIEWER_HEADER

#include <iostream>
#include <string>
#include "MarineViewer.h"
#include "VehicleSet.h"

class PMV_Viewer : public MarineViewer
{
 public:
  PMV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);

  bool  setParam(std::string p, std::string v="");
  bool  setParam(std::string p, double v);
  bool  addScopeVariable(std::string);
  bool  updateScopeVariable(std::string varname, std::string value, 
			    std::string vtime, std::string vsource);
  void  setActiveScope(std::string);
  void  setLeftMouseContext(std::string s)  {m_left_click_context=s;};
  void  setRightMouseContext(std::string s) {m_right_click_context=s;};

  std::string getStringInfo(const std::string& info_type, int precision=0);

 private:
  void   drawVehicle(std::string, bool, std::string);
  void   drawTrailPoints(CPList&, int=0);
  void   handleLeftMouse(int, int);
  void   handleRightMouse(int, int);
  void   handleMoveMouse(int, int);
  void   setWeightedCenterView();

 private:
  VehicleSet       m_vehiset;

  std::string m_reference_point;
  std::string m_reference_bearing;

  // Member variables for holding scoped info
  std::vector<std::string> m_var_names;
  std::vector<std::string> m_var_vals;
  std::vector<std::string> m_var_source;
  std::vector<std::string> m_var_time;
  int                      m_var_index;
  int                      m_var_index_prev;

  // Member variables for holding/conveying mouse/button click info
  double   m_mouse_x;
  double   m_mouse_y;
  double   m_mouse_lat;
  double   m_mouse_lon;

  std::string m_left_click;
  std::string m_right_click;
  std::string m_button_one;
  std::string m_button_two;
  std::string m_left_click_context;
  std::string m_right_click_context;

  std::string m_centric_view; // average, active, or reference
  bool        m_centric_view_sticky;
};

#endif 


