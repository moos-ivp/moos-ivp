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

#include "IvPDomain.h"
#include "AOF_Cache.h"
#include "IvPFunction.h"
#include "FColorMap.h"
#include "Common_IPFViewer.h"
#include "OF_Rater.h"

class Viewer : public Common_IPFViewer
{
 public:
  Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void   draw();

public:
  int    handle(int);
  void   setAOF(AOF *aof);

public:
  bool   setParam(std::string, std::string);
  bool   setParam(std::string, double);
  void   printParams();

  void   toggleAOF()              {m_draw_aof = !m_draw_aof; redraw();};
  void   toggleIPF()              {m_draw_ipf = !m_draw_ipf; redraw();};
  void   toggleStrict()           {m_strict_rng = !m_strict_rng; redraw();};
  void   toggleUniformAug();
  void   toggleSmartAug();
  void   makeUniformIPF(int=0);
  void   makeUniformIPFxN(int amt);
  void   modColorMap(const std::string&);
  void   modPatchAOF(int amt);
  void   modUniformAug(int amt);
  void   modSmartAugAmt(int amt);

  void   runScript();
  void   capture(int);

  void   takeSamples(int amt)     {m_rater.takeSamples(amt);};
  double getParam(const std::string&, bool&);

protected:
  void   drawIvPFunction(IvPFunction*);
  void   drawIvPBox(const IvPBox&, double, double, int, int);
  void   drawAOF();
  void   drawFocusBox();

private:
  float      m_base_aof;
  float      m_base_ipf;
  float      m_scale;
  int        m_patch;
  int        m_unifsize;
  bool       m_draw_aof;
  bool       m_draw_ipf;
  AOF_Cache  m_aof_cache;
  IvPDomain  m_domain;
  FColorMap  m_cmap;
  bool       m_strict_rng;
  double     m_create_time;

  IvPFunction *m_unif_ipf;
  OF_Rater     m_rater;

  bool       m_smart_refine;
  int        m_smart_count;

  bool       m_focus_box;
  int        m_focus_box_x;
  int        m_focus_box_y;
  int        m_focus_box_len;
  int        m_focus_unif_len;
};

#endif 
