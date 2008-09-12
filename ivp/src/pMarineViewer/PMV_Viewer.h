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

#include <string>
#include "MarineViewer.h"
#include "MOOSLock.h"
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
  
  std::string getStringInfo(const std::string& info_type, int precision=0);

  void  mutexLock()   {m_mutex.Lock();};
  void  mutexUnLock() {m_mutex.UnLock();};

 private:
  void   setWeightedCenterView();
  void   drawVehicle(std::string, bool, std::string);
  void   drawPoints(CPList&, int=0);
  void   handleLeftMouse(int, int);
  void   handleRightMouse(int, int);


 private:
  VehicleSet  m_vehiset;

  // Member variables for holding/conveying mouse click info
  std::string m_left_click;
  std::string m_right_click;
  bool        m_centric_view;
  bool        m_centric_view_sticky;
  CMOOSLock   m_mutex;
};

#endif 


