/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPAViewer.h                                          */
/*    DATE: Feb 12, 2007                                         */
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

#ifndef CPA_VIEWER_HEADER
#define CPA_VIEWER_HEADER

#include <vector>
#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "MarineViewer.h"
#include "CPA_Model.h"
#include "XYPolygon.h"

class CPAViewer : public MarineViewer
{
 public:
  CPAViewer(CPA_Model*, int x,int y,int w,int h, const char *l=0);
  ~CPAViewer() {};

  // Pure virtuals that need to be defined
  void   draw();
  int    handle(int);
  void   handle_left_mouse(int, int);
  void   handle_right_mouse(int, int);

  bool  setParam(std::string p, std::string v)
    {return(MarineViewer::setParam(p,v));};
  bool  setParam(std::string p, double v)
    {return(MarineViewer::setParam(p,v));};

  void   resetPan();

 protected:
  void   draw_cn_vector(int, double=6, double=0.5, double=0.5, double=0.5);
  void   draw_os_point(double=6, double=0.5, double=0.5, double=0.5);

  void   configureRadials(unsigned int radials, double dist);

 private:
  CPA_Model* m_cpa_model;
  double     m_snap_val;

  double     m_start_pan_x;
  double     m_start_pan_y;

  std::vector<XYPolygon> m_radials;

  VPlug_GeoShapes  m_geoshapes;
};

#endif 

