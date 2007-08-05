/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Viewer.h                                             */
/*    DATE: Apr 15th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef VIEWER_HEADER
#define VIEWER_HEADER

#include <vector>
#include "AOF.h"
#include "AOF_Cache.h"
#include "IvPBox.h"
#include "IvPFunction.h"
#include "Quad3D.h"
#include "FColorMap.h"
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"

class Viewer : public Fl_Gl_Window
{
 public:
  Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();

public:
  int   handle(int);
  void  setAOF(AOF *aof);
  void  applyFColorMap()         {m_aof_cache.applyFColorMap(m_cmap);};
  void  addIPF(IvPFunction *ipf) {m_ipfs.push_back(ipf);};

public:
  void  rotateX(float amt)       {m_xRot += amt; redraw();};
  void  rotateY(float amt)       {m_yRot += amt; redraw();};
  void  rotateZ(float amt)       {m_zRot += amt; redraw();};
  void  stretchX(float amt)      {m_xstretch *= amt; redraw();};
  void  stretchY(float amt)      {m_ystretch *= amt; redraw();};
  void  reset()                  {m_xRot=-72; m_yRot=0; m_zRot=40; redraw();};

  void  toggleAOF()              {m_draw_aof = !m_draw_aof; redraw();};
  void  toggleIPF()              {m_draw_ipf = !m_draw_ipf; redraw();};
  void  toggleFrame()            {m_draw_frame = !m_draw_frame; redraw();};
  void  toggleStrict()           {m_strict_rng = !m_strict_rng; redraw();};
  void  modBaseAOF(float amt)    {m_base_aof += amt; redraw();};
  void  modBaseIPF(float amt)    {m_base_ipf += amt; redraw();};
  void  modZoom(float amt)       {m_zoom *= amt; redraw();};
  void  modIndexIPF(int amt);
  void  makeUniformIPF(int=0);
  void  modColorMap(const std::string&);
  void  modPolar(int);
  void  modZSlice(int);
  void  modIntensity(double amt)     
    {m_intensity += amt; if(m_intensity <0.5) m_intensity=0.5; 
    if(m_intensity > 1) m_intensity=1; redraw();};
  void  modScale(double amt)     
    {m_scale += amt; if(m_scale <0)  m_scale=0; redraw();};
  void  modPatchAOF(int amt)  
    {m_patch += amt; if(m_patch <=1) m_patch=1; redraw();};

  void  runScript();
  void  capture(int);


protected:
  void  errCheck();
  void  drawQuad(Quad3D&);
  void  drawFrame();
  void  drawIvPFunction(IvPFunction*);
  void  drawIvPBox(const IvPBox&, double, double, int, int);
  void  drawAOF();
  
 private:
  float      m_zoom;
  float      m_xRot;
  float      m_yRot;
  float      m_zRot;
  float      m_base_aof;
  float      m_base_ipf;
  float      m_scale;
  int        m_patch;
  int        m_ipf_ix;
  int        m_unifsize;
  float      m_xstretch;
  float      m_ystretch;
  bool       m_draw_aof;
  bool       m_draw_ipf;
  bool       m_draw_frame;
  AOF_Cache  m_aof_cache;
  FColorMap  m_cmap;
  float      m_intensity;
  int        m_polar;
  int        m_zslice;
  bool       m_strict_rng;

  std::vector<IvPFunction*> m_ipfs;
  IvPFunction *m_unif_ipf;
};

#endif 
