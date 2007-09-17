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

class PMV_Viewer : public MarineViewer
{
 public:
  PMV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);

public:
  void  updateVehiclePosition(const std::string&, float x, float y, 
			      float theta, float spd, float dep=0);  
  void  resetVehicles();
  float getMetersX(int);
  float getMetersY(int);
  float getSpd(int);
  float getDep(int);
  float getCrs(int);

  std::string getKName(int);
  std::string getLeftClick()     {return(m_left_click);};
  std::string getRightClick()    {return(m_right_click);};
  int   getLeftClickIX()         {return(m_left_click_ix);};
  int   getRightClickIX()        {return(m_right_click_ix);};

  void  clearTrails()            {m_pos_map.clear(); m_hist_map.clear();};
  void  setTime(float v)         {m_time = v;};
  float getTime()                {return(m_time);};
  int   getDataIndex()           {return(m_global_ix);};

 private:
  void  drawVehicle(std::string, bool);
  void  drawPoints(CPList&);
  void  drawPoint(float, float, int color=0);
  void  handleLeftMouse(int, int);
  void  handleRightMouse(int, int);

  ObjectPose getObjectPoseByIndex(int);

 private:
  std::map<std::string, ObjectPose>  m_pos_map;
  std::map<std::string, CPList>      m_hist_map;

  std::vector<std::string> m_moos_messages;
  
  std::string m_left_click;
  std::string m_right_click;

  int   m_left_click_ix;
  int   m_right_click_ix;

  float m_time;
};

#endif 

