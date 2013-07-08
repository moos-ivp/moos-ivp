/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BNGViewer.h                                          */
/*    DATE: May 21, 2013                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef BNG_VIEWER_HEADER
#define BNG_VIEWER_HEADER

#include <vector>
#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "MarineViewer.h"
#include "BNG_Model.h"
#include "XYPolygon.h"

class BNGViewer : public MarineViewer
{
 public:
  BNGViewer(BNG_Model*, int x,int y,int w,int h, const char *l=0);
  ~BNGViewer() {};

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
  BNG_Model* m_cpa_model;
  double     m_snap_val;

  double     m_start_pan_x;
  double     m_start_pan_y;

  std::vector<XYPolygon> m_radials;

  VPlug_GeoShapes  m_geoshapes;
};

#endif 


