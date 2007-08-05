/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarineViewer.h                                       */
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
#include "IMG.h"
#include "CommonMarineViewer.h"

class MarineViewer : public CommonMarineViewer
{
 public:
  MarineViewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);

public:
  void  updateVehiclePosition(std::string, float x, float y, 
			      float theta, float spd, float dep=0);  
  void  updateKViewMap(std::string, std::string);
  void  setInfoBHV(std::string, std::string);
  void  resetVehicles();
  float getImgPctX(int);
  float getImgPctY(int);
  float getMetersX(int);
  float getMetersY(int);
  float getLonX(int);
  float getLatY(int);
  float getSpd(int);
  float getDep(int);
  float getCrs(int);
  std::string getKName(int);
  std::string getInfoBHV(std::string);

  void  setTime(float v)         {m_time = v;};
  float getTime()                {return(m_time);};
  int   getDataIndex()           {return(m_global_ix);};
  void  toggleGridsTop()         {m_grids_top = !m_grids_top; redraw();};

  void  alterImageRes(float v)   {if(m_img_res+v >= 0) m_img_res+=v; redraw();};
  void  alterImageRadius(int v)  {if(m_img_radius+v >= 10) m_img_radius+=v; redraw();};
  void  clearImages()            {images.clear();};
  void  addImage(IMG img)        {images.push_back(img);};

 private:
  void  drawVehicle(ObjectPose, bool);
  void  drawImages();
  void  drawIMG(std::string, const IMG&);
  void  drawPoints(CPList&);
  void  drawPoint(float, float, int color=0);
  ObjectPose getObjectPoseByIndex(int);

 private:
  std::map<std::string, ObjectPose>  kpos_map;
  std::map<std::string, CPList>      khist_map;

  std::map<std::string, std::string> kbhv_map;
  std::map<std::string, std::string> kview_map;
  std::vector<IMG> images;

  float      m_img_res;
  int        m_img_radius;
  float      m_time;
  bool       m_grids_top;
};

#endif 

