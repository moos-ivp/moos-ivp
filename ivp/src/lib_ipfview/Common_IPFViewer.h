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
  void   rotate_x(double v)  {m_xRot += v; redraw();};
  void   rotate_z(double v)  {m_zRot += v; redraw();};

  void   reset1()            {m_xRot=-72; m_yRot=0; m_zRot=40; redraw();};
  void   reset2()            {m_xRot=0;   m_yRot=0; m_zRot=0; redraw();};
  void   reset3()            {m_xRot=-53; m_yRot=0; m_zRot=0; redraw();};
  void   reset4()            {m_xRot=-72; m_yRot=0; m_zRot=122; redraw();};

  void   modBackColor(double r, double g, double b)
    {m_clear_red=r; m_clear_green=g; m_clear_blue=b;};

  void   modRadius(float v)  {m_rad_extra   *= v; redraw();};
  void   modBase(float v)    {m_base_extra  += v; redraw();};
  void   modScale(float v)   {m_scale_extra *= v; redraw();};
  void   toggleFrame()       {m_draw_frame  = !m_draw_frame; redraw();};
  void   modZoom(float amt)  {m_zoom *= amt; redraw();};
  
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
