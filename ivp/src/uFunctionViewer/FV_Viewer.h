/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FV_Viewer.h                                          */
/*    DATE: May 12th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef IPF_VIEWER_HEADER
#define IPF_VIEWER_HEADER

#include <vector>
#include "MOOSLock.h"
#include "IvPBox.h"
#include "IvPFunction.h"
#include "Quad3D.h"
#include "FColorMap.h"
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"

class FV_Viewer : public Fl_Gl_Window
{
 public:
  FV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();

public:
  int   handle(int);
  void  setIPF(std::string str);

public:
  void  rotateX(int amt)         {m_xRot += amt; redraw();};
  void  rotateY(int amt)         {m_yRot += amt; redraw();};
  void  rotateZ(int amt)         {m_zRot += amt; redraw();};
  void  stretchX(float amt)      {m_xstretch *= amt; redraw();};
  void  stretchY(float amt)      {m_ystretch *= amt; redraw();};
  void  reset()                  {m_xRot=-72; m_yRot=0; m_zRot=40; redraw();};

  void  toggleFrame()            {m_draw_frame = !m_draw_frame; redraw();};
  void  modBaseIPF(float amt)    {m_base_ipf += amt; redraw();};
  void  modZoom(float amt)       {m_zoom *= amt; redraw();};
  void  modColorMap(const std::string&);
  void  modPolar(int);
  void  modZSlice(int);
  void  modIntensity(double amt)     
    {m_intensity += amt; if(m_intensity <0.5) m_intensity=0.5; 
    if(m_intensity > 1) m_intensity=1; redraw();};

protected:
  void  errCheck();
  void  drawQuad(Quad3D&);
  void  drawFrame();
  void  drawIvPFunction();
  void  drawIvPBox(const IvPBox&, double, double, int, int);
  
 private:
  float      m_zoom;
  float      m_xRot;
  float      m_yRot;
  float      m_zRot;
  float      m_base_ipf;
  float      m_xstretch;
  float      m_ystretch;
  bool       m_draw_ipf;
  bool       m_draw_frame;
  FColorMap  m_cmap;
  float      m_intensity;
  int        m_polar;
  int        m_zslice;

  CMOOSLock m_ipf_mutex;

  IvPFunction *m_ipf;
};

#endif 




