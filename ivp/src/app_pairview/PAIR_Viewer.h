/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PAIR_Viewer.h                                        */
/*    DATE: Jan 9th, 2025                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef PAIR_VIEWER_HEADER
#define PAIR_VIEWER_HEADER

#include <vector>
#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "MarineViewer.h"
#include "PairModel.h"
#include "XYPolygon.h"

class PAIR_Viewer : public MarineViewer
{
 public:
  PAIR_Viewer(PairModel*, int x, int y, int w, int h, const char *l=0);
  ~PAIR_Viewer() {};

  // Pure virtuals that needs to be defined
  void   modColorScheme() {};

  // Virtuals defined
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
  void   initCenterView();
  void   setCenterView();
  void   toggleAlertCircle()   {m_draw_alert_circle = !m_draw_alert_circle;};
  void   toggleWarningCircle() {m_draw_warning_circle = !m_draw_warning_circle;};

  void   drawOwnship();
  void   drawContact();
  void   drawAlertCircle();
  void   drawWarningCircle();
  bool   getClearPending();

 protected:
  void   draw_vector(std::string node);

  void   configureRadials(unsigned int radials, double dist);

 private:
  PairModel* m_pair_model;
  double     m_snap_val;

  bool       m_init_center_view;
  
  double     m_start_pan_x;
  double     m_start_pan_y;

  double     m_shape_scale;
  bool       m_draw_alert_circle;
  bool       m_draw_warning_circle;

  ColorPack  m_cpack_os_vector;
  ColorPack  m_cpack_cn_vector;

  VPlug_GeoShapes  m_geoshapes;
};

#endif 




