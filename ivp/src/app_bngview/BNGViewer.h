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
#include "PairModel.h"
#include "XYPolygon.h"

class BNGViewer : public MarineViewer
{
 public:
  BNGViewer(PairModel*, int x,int y,int w,int h, const char *l=0);
  ~BNGViewer() {};

  // Pure virtuals that need to be defined
  void   draw();
  int    handle(int);
  void   handle_left_mouse(int, int);
  void   handle_right_mouse(int, int);

  bool   setParam(std::string p, std::string v)
    {return(MarineViewer::setParam(p,v));};
  bool   setParam(std::string p, double v)
    {return(MarineViewer::setParam(p,v));};

  void   shapeScaleBigger()  {m_shape_scale *= 1.25;};
  void   shapeScaleSmaller() {m_shape_scale *= 0.8;};
  void   shapeScaleReset()   {m_shape_scale = 1;};
  void   setCenterView();
  void   toggleCurrentCircle() {m_draw_current_circle = !m_draw_current_circle;};
  void   toggleTriggerCircle() {m_draw_trigger_circle = !m_draw_trigger_circle;};

  void   drawOwnship();
  void   drawContact();
  void   drawCurrentRangeCircle();
  void   drawTriggerRangeCircle();
  void   resetPan();

 protected:
  void   draw_vector(std::string node);

  void   configureRadials(unsigned int radials, double dist);

 private:
  PairModel* m_pair_model;
  double     m_snap_val;

  double     m_start_pan_x;
  double     m_start_pan_y;

  double     m_shape_scale;
  bool       m_draw_current_circle;
  bool       m_draw_trigger_circle;
  double     m_trigger_range;

  ColorPack  m_cpack_os_vector;
  ColorPack  m_cpack_cn_vector;

  std::vector<XYPolygon> m_radials;

  VPlug_GeoShapes  m_geoshapes;
};

#endif 
