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
#include <map>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "ObjectPose.h"
#include "ColoredPoint.h"
#include "MarineViewer.h"

class SSV_Viewer : public MarineViewer
{
 public:
  SSV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);

public:
  void  updateVehiclePosition(std::string, float x, float y, 
			      float theta, float spd, float dep=0);  
  void  setVehicleBodyType(std::string, std::string);
  void  setOwnShipName(std::string);

  void  resetVehicles();
  float getMetersX(int);
  float getMetersY(int);
  float getSpd(int);
  float getDep(int);
  float getCrs(int);

  std::string getVehiName(int);
  std::string getLeftClick()     {return(m_left_click);};
  std::string getRightClick()    {return(m_right_click);};
  int   getLeftClickIX()         {return(m_left_click_ix);};
  int   getRightClickIX()        {return(m_right_click_ix);};

  void  clearTrails()            {m_pos_map.clear(); m_hist_map.clear();};
  void  setTime(float v)         {m_time = v;};
  float getTime()                {return(m_time);};
  int   getDataIndex()           {return(m_global_ix);};

  bool  setParam(std::string, std::string);
  bool  setParam(std::string, float);

 private:
  void  drawVehicle(std::string, bool, std::string);
  void  drawPoints(CPList&);
  void  drawPoint(float, float, int color=0);
  void  handleLeftMouse(int, int);
  void  handleRightMouse(int, int);
  void  drawRadials();

  ObjectPose getObjectPoseByIndex(int);

 private:
  // Mapping from Vehicle Name to Vehicle Position
  std::map<std::string, ObjectPose>   m_pos_map;
  // Mapping from Vehicle Name to Vehicle Position History
  std::map<std::string, CPList>       m_hist_map;
  // Mapping from Vehicle Name to Vehicle Body Type
  std::map<std::string, std::string>  m_vbody_map;

  float m_time;

  std::string m_default_vehibody;

  // Member variables for holding/conveying mouse click info
  std::string m_left_click;
  std::string m_right_click;

  int    m_left_click_ix;
  int    m_right_click_ix;
  bool   m_centric_view;
  int    m_radial_size;

  std::string m_ownship_name;
};

#endif 

