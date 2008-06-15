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
  void   applyIPF(IvPFunction*, bool=false);
  bool   setParam(std::string, std::string);
  bool   setParam(std::string, double);
  void   printParams();
  
protected:
  void   drawFrame();
  void   drawOwnPoint();
  void   drawIvPFunction();
  void   drawQuad(Quad3D&);
  void   handleLeftMouse(int, int) {};
  void   handleRightMouse(int, int) {};

protected:
  std::string  m_clear_color;
  double       m_clear_red;
  double       m_clear_green;
  double       m_clear_blue;
  
  std::string  m_frame_color;
  double       m_frame_red;
  double       m_frame_green;
  double       m_frame_blue;

  double       m_xRot;
  double       m_zRot;
  double       m_zoom;

  double       m_rad_extra;
  int          m_polar;
  
  bool         m_ipf_owner;
  bool         m_draw_frame;
  bool         m_draw_base;
  double       m_frame_height;
  QuadSet      m_quadset;
  IvPFunction* m_ivp_function;
};

#endif 
