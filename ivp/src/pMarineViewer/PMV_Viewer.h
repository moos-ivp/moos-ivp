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
#include "MOOSGeodesy.h"
#include "MOOSLock.h"

class PMV_Viewer : public MarineViewer
{
 public:
  PMV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);

  bool  setParam(std::string p, std::string v)
    {return(setCommonParam(p,v));};
  bool  setParam(std::string p, float v)
    {return(setCommonParam(p,v));};
  
public:
  void  updateVehiclePosition(std::string vname, float x, float y, 
			      float theta, float spd, float dep=0);  
  void  setVehicleBodyType(const std::string&, const std::string&);
  void  resetVehicles();
  float getMetersX(int);
  float getMetersY(int);
  float getSpd(int);
  float getDep(int);
  float getCrs(int);
  float getAgeAIS(int);
  bool  getLatLon(int, double&, double&);
  bool  initGeodesy(double, double);
  void  mutexLock()   {m_mutex.Lock();};
  void  mutexUnLock() {m_mutex.UnLock();};

  std::string getVehiName(int);
  std::string getVehiType(int);
  std::string getLeftClick()     {return(m_left_click);};
  std::string getRightClick()    {return(m_right_click);};
  int   getLeftClickIX()         {return(m_left_click_ix);};
  int   getRightClickIX()        {return(m_right_click_ix);};

  void  clearTrails()            {m_pos_map.clear(); m_hist_map.clear();};
  void  setTime(float v)         {m_curr_time = v;};
  float getTime()                {return(m_curr_time);};
  int   getDataIndex()           {return(m_global_ix);};
  void  cycleIndex();

 private:
  void  drawVehicle(std::string, bool, std::string);
  void  drawPoints(CPList&);
  void  drawPoint(float, float, int color=0);
  void  handleLeftMouse(int, int);
  void  handleRightMouse(int, int);

  ObjectPose getObjectPoseByIndex(int);

  CMOOSGeodesy m_geodesy;

 private:
  // Mapping from Vehicle Name to Vehicle Position
  std::map<std::string, ObjectPose>   m_pos_map;
  // Mapping from Vehicle Name to Vehicle Position History
  std::map<std::string, CPList>       m_hist_map;
  // Mapping from Vehicle Name to Vehicle Body Type
  std::map<std::string, std::string>  m_vbody_map;
  // Mapping from Vehicle Name to Time of last AIS report
  std::map<std::string, double>       m_ais_map;

  float m_curr_time;

  std::string m_default_vehibody;

  // Member variables for holding/conveying mouse click info
  std::string m_left_click;
  std::string m_right_click;
  int   m_left_click_ix;
  int   m_right_click_ix;

  CMOOSLock  m_mutex;
};

#endif 


