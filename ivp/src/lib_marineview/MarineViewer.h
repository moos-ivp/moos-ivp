/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarineViewer.h                                       */
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

#include <map>
#include <string>
#include <vector>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "XYGrid.h"
#include "XYPolygon.h"
#include "XYSegList.h"
#include "XYCircle.h"
#include "XYHexagon.h"
#include "ObjectPose.h"

class MarineViewer : public Fl_Gl_Window
{
 public:
  MarineViewer(int x,int y,int w,int h,const char *l=0);
  ~MarineViewer() {};
  
  // Pure virtuals that need to be defined
  virtual void  draw();
  virtual int   handle(int event)
    {return(Fl_Gl_Window::handle(event));};

  virtual void setGlobalIndex(unsigned int i) {m_global_ix = i;};

  virtual bool setParam(std::string p, std::string v) = 0;
  virtual bool setParam(std::string p, float v)       = 0;

  bool  setCommonParam(std::string, std::string);
  bool  setCommonParam(std::string, float);

public:
  bool   readTiff(std::string);
  bool   readTiffB(std::string);
  bool   setTexture();

  void   addPoly(const XYPolygon&);
  void   addGrid(const XYGrid&);
  void   addCircle(const XYCircle&);
  void   addHexagon(const XYHexagon&);
  void   addSegList(const XYSegList&);
  void   colorMapping(const std::string&);
  void   updateGrid(std::string);
  double getHashDelta();
  float  getCrossHairMeters(char);

protected:
  float  img2view(char, float);
  float  view2img(char, float);
  float  meters2img(char, float);
  float  img2meters(char, float);
  void   drawCommon();
  void   drawTiff();
  void   drawHash();
  void   drawCrossHairs();
  void   drawPolys();
  void   drawSegment(float, float, float, float, float, float, float);
  void   drawPoly(const XYPolygon&, 
		  bool filled=false, bool dashed=false,
		  float=0.40, float=0.40, float=0.40,   
		  float=0.20, float=0.80, float=0.20,   
		  float=0.00, float=0.00, float=0.00,   
		  float=0.00, float=0.00, float=0.00,   
		  float=0.00, float=0.00, float=0.00);

  void   drawDatum();
  void   drawHexagons();
  void   drawSegLists();
  void   drawSegList(int ix);
  void   drawGrids();
  void   drawGrid(const XYGrid&);
  void   drawCircles();
  void   drawCircle(int ix);
  void   drawGLPoly(float *points, int numPoints, float r, float g, 
		    float b, float thickness=0, float scale=1);
  void   drawCommonVehicle(std::string vname, ObjectPose, double r, 
			   double g, double b, std::string body, int line=0);
  void   drawCommonMarker(double x, double y, double scale, std::string);
  
protected:
  BackImg    m_back_img;
  BackImg    m_back_img_b;
  bool       m_back_img_b_ok;
  bool       m_back_img_b_on;
  bool       m_back_img_mod;

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
  bool       m_trail_connect;
  int        m_trail_color;
  int        m_trail_gap;
  float      m_trail_size;
  bool       m_cross_offon;
  bool       m_poly_offon;
  bool       m_grid_offon;
  bool       m_tiff_offon;
  bool       m_hash_offon;
  bool       m_draw_vname;
  int        m_vname_color;
  bool       m_draw_datum;
  float      m_size_datum;
  unsigned int m_global_ix;

  std::vector<XYCircle>  m_circ;
  std::vector<XYPolygon> m_poly;
  std::vector<XYGrid>    m_grid;
  std::vector<XYHexagon> m_hexa;
  std::vector<XYSegList> m_segl;

  std::map<std::string, std::vector<double> >  m_color_map;

};

#endif 


