/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CommonMarineViewer.h                                 */
/*    DATE: May 31st, 2005                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef COMMON_MARINE_VIEWER_HEADER
#define COMMON_MARINE_VIEWER_HEADER

#include <iostream>
#include <vector>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "XYGrid.h"
#include "XYPolygon.h"
#include "XYCircle.h"

class CommonMarineViewer : public Fl_Gl_Window
{
 public:
  CommonMarineViewer(int x,int y,int w,int h,const char *l=0);
  ~CommonMarineViewer() {};
  
  // Pure virtuals that need to be defined
  virtual void  draw();
  virtual int   handle(int event)
    {return(Fl_Gl_Window::handle(event));};
  virtual void  setGlobalIndex(int i) {
    std::cout << "In CommonMarineViewer::setGlobalIndex()" << std::endl;
    m_global_ix = i;};

  void report() {std::cout << std::flush;};

public:
  bool  readTiff(const char *f=0);
  bool  setTexture();
  void  alterShapeScale(float v) {m_shape_scale = m_shape_scale*v; redraw();};

  void  zoom(float v);
  void  zoomReset();
  void  panY(float amt);
  void  panX(float amt);

protected:
  float img2view(const char*, float);
  float view2img(const char*, float);
  float meters2img(const char*, float);
  float img2meters(const char*, float);
  float meters2LatLon(const char*, float);
  void  drawCommon();
  void  drawTiff();
  void  drawHash();
  void  drawCrossHairs();

public:
  void  toggleCrossView()        {m_cross_offon = !m_cross_offon;};
  void  setCrossView(bool v)     {m_cross_offon = v;};
  void  toggleTiffView()         {m_tiff_offon = !m_tiff_offon;};
  void  setTiffView(bool v)      {m_tiff_offon = v;};
  void  toggleHashView()         {m_hash_offon = !m_hash_offon;};
  void  setHashView(bool v)      {m_hash_offon = v;};
  void  togglePolyView()         {m_poly_offon = !m_poly_offon;};
  void  toggleTrails()           {m_trails = !m_trails; redraw();};
  void  toggleTrailColor()       {m_trail_color = (m_trail_color+1)%3; redraw();};
  void  alterTrailGap(int v)     {if(m_trail_gap+v  >= 1) m_trail_gap+=v; redraw();};
  void  alterTrailSize(float v)  {if(m_trail_size+v >= 0.1) m_trail_size+=v; redraw();};
  void  addPoly(XYPolygon v);
  void  addGrid(XYGrid v);
  void  addCircle(XYCircle v);
  void  hashShade(float amt);
  void  hashDelta(float amt);
  void  backShade(float amt);
  void  clearPoly()              {m_poly.clear();};
  void  clearGrid()              {m_grid.clear();};
  void  clearCirc()              {m_circ.clear();};
  void  updateGrid(std::string);
  void  updateGrid(std::string, std::string);

protected:
  void  drawPolys();
  void  drawPoly(const XYPolygon&, 
		 bool filled=false, bool dashed=false,
		 float=0.40, float=0.40, float=0.40,   
		 float=0.20, float=0.80, float=0.20,   
		 float=0.00, float=0.00, float=0.00,   
		 float=0.00, float=0.00, float=0.00,   
		 float=0.00, float=0.00, float=0.00);

  void  drawGrids();
  void  drawGrid(int ix);
  void  drawCircles();
  void  drawCircle(int ix);
  void  drawGLPoly(float *points, int numPoints, 
		   float r, float g, float b, float thickness=0);
  
protected:
  BackImg    m_back_img;  

  float      m_zoom;
  float      m_vshift_x; 
  float      m_vshift_y; 
  float      m_x_origin;
  float      m_y_origin;
  float      m_shape_scale;
  float      m_hash_shade;
  float      m_hash_delta;
  float      m_fill_shade;
  int        m_texture_set;
  GLuint*    m_textures;

  bool       m_trails;
  int        m_trail_color;
  int        m_trail_gap;
  float      m_trail_size;
  bool       m_cross_offon;
  bool       m_poly_offon;
  bool       m_tiff_offon;
  bool       m_hash_offon;
  int        m_global_ix;

  std::vector<XYCircle>  m_circ;
  std::vector<XYPolygon> m_poly;
  std::vector<XYGrid>    m_grid;
};

#endif 

