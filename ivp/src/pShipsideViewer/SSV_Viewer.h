/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_Viewer.h                                         */
/*    DATE: September 11th, 2007                                 */
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

#ifndef SHORESIDE_VIEWER_HEADER
#define SHORESIDE_VIEWER_HEADER

#include <vector>
#include <string>
#include "MarineViewer.h"
#include "XYCircle.h"
#include "MOOSLock.h"
#include "VehicleSet.h"

class SSV_Viewer : public MarineViewer
{
 public:
  SSV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);

  bool  setParam(std::string p, std::string v="");
  bool  setParam(std::string p, double v);
  
  bool   addStationCircle(const std::string& spec);

  std::string getStringInfo(const std::string& info_type, int precision=0);

  void   mutexLock()   {m_mutex.Lock();};
  void   mutexUnLock() {m_mutex.UnLock();};

 private:
  void   setWeightedCenterView();
  double getRelativeInfo(const std::string& info_type);

  void   drawVehicle(std::string, bool, std::string);
  void   drawPoints(CPList&);
  void   handleLeftMouse(int, int);
  void   handleRightMouse(int, int);

  void   drawRadials();
  void   drawBearingLine();
  void   drawStationCircles();

 private:
  VehicleSet m_vehiset;

  std::vector<XYCircle>  m_station_circ;

  bool   m_draw_bearing_lines;
  bool   m_draw_radial;
  int    m_radial_size;
  bool   m_centric_view;

  std::vector<double> m_radial_color;
  std::vector<double> m_bearing_color;
  

  CMOOSLock    m_mutex;

  // Member variables for holding/conveying mouse click info
  std::string  m_left_click;
  std::string  m_right_click;
  std::string  m_right_click_rp;
  std::string  m_ownship_name;
};

#endif 

