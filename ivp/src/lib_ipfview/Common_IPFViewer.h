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
#include "ColorPack.h"
#include "FColorMap.h"

class Common_IPFViewer : public Fl_Gl_Window
{
 public:
  Common_IPFViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~Common_IPFViewer() {};

  void   draw();
  int    handle(int);

public:
  void   applyIPF(const std::string& ipf_str, const IvPDomain&);
  void   applyIPF(const std::string& ipf_str);
  bool   setParam(std::string, std::string);
  bool   setParam(std::string, double);
  void   printParams();
  void   setVNameIPF(std::string s)   {m_active_ipf_vname = s;};
  void   setSourceIPF(std::string s)  {m_active_ipf_source = s;};
  void   setPiecesIPF(std::string s)  {m_active_ipf_pieces = s;};
  void   setIterIPF(std::string s)    {m_active_ipf_iter = s;};
  void   setLabelColor(std::string);
  void   setClearColor(std::string);
  void   setFrameColor(std::string);
 
protected:
  void   drawFrame();
  void   drawOwnPoint();
  void   drawIvPFunction();
  void   drawQuad(Quad3D&);
  void   handleLeftMouse(int, int) {};
  void   handleRightMouse(int, int) {};
  
  QuadSet setQuadSetFromIPF(const std::string&, IvPDomain);

  void   drawText(double px, double py, const std::string&, 
		  const ColorPack&, double font_size);

protected:
  ColorPack    m_clear_color;
  ColorPack    m_frame_color;
  ColorPack    m_label_color;
  FColorMap    m_color_map;

  double       m_xRot;
  double       m_zRot;
  double       m_zoom;

  double       m_rad_extra;
  int          m_polar;
  
  bool         m_draw_frame;
  bool         m_draw_base;
  double       m_frame_height;
  QuadSet      m_quadset;

  std::string  m_active_ipf_vname;
  std::string  m_active_ipf_source;
  std::string  m_active_ipf_pieces;
  std::string  m_active_ipf_iter;
};

#endif 
