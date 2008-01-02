/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Common_IPFViewer.h                                   */
/*    DATE: Feb 25th, 2007                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef COMMON_IPF_VIEWER_HEADER
#define COMMON_IPF_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "QuadSet.h"
#include "IvPFunction.h"

class Common_IPFViewer : public Fl_Gl_Window
{
 public:
  Common_IPFViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~Common_IPFViewer();

  // Pure virtuals that need to be defined
  void   draw();
  int    handle(int);

public:
  void   modBackColor(double r, double g, double b)
    {m_clear_red=r; m_clear_green=g; m_clear_blue=b;};

  void   applyIPF(IvPFunction*, bool=false);

  void   setParam(std::string, std::string);
  void   setParam(std::string, double);
  
protected:
  void   drawFrame();
  void   drawOwnPoint();
  void   drawIvPFunction();
  void   drawQuad(Quad3D&);
  void   handleLeftMouse(int, int) {};
  void   handleRightMouse(int, int) {};

protected:
  double       m_clear_red;
  double       m_clear_green;
  double       m_clear_blue;
  
  double       m_frame_red;
  double       m_frame_green;
  double       m_frame_blue;

  std::vector<std::string> m_scheme_back;
  std::vector<std::string> m_scheme_frame;
  int          m_scheme_index;

  double       m_xRot;
  double       m_yRot;
  double       m_zRot;
  double       m_zoom;

  double       m_rad_extra;
  double       m_base_extra;
  double       m_scale_extra;
  int          m_polar;
  
  bool         m_ipf_owner;
  bool         m_draw_frame;
  bool         m_draw_base;
  double       m_frame_height;
  QuadSet      m_quadset;
  IvPFunction* m_ivp_function;
};

#endif 
