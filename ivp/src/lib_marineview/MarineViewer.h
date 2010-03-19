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
#include "OpAreaSpec.h"
#include "MOOSGeodesy.h"
#include "VPlug_GeoShapes.h"
#include "VPlug_GeoSettings.h"
#include "VPlug_VehiSettings.h"
#include "VPlug_DropPoints.h"
#include "VPlug_Markers.h"
#include "ColorPack.h"
#include "BearingLine.h"

class MarineViewer : public Fl_Gl_Window
{
 public:
  MarineViewer(int x,int y,int w,int h,const char *l=0);
  ~MarineViewer();
  
  // Pure virtuals that need to be defined
  virtual void draw();
  virtual int  handle(int event);
  virtual bool setParam(std::string p, std::string v="");
  virtual bool setParam(std::string p, double v);

  bool   initGeodesy(double, double);
  bool   initGeodesy(const std::string&);
  bool   setTexture();
  double getHashDelta();

protected:
  bool   readTiff(std::string);
  bool   readTiffB(std::string);

  void   drawTiff();
  double img2view(char, double);
  double view2img(char, double);
  double meters2img(char, double);
  double img2meters(char, double);

  void   drawHash();
  void   drawSegment(double, double, double, double, double, double, double);

  void   drawMarkers();
  void   drawOpArea();

  void   drawGLPoly(double *points, int numPoints, 
		    ColorPack fill_color,
		    double thickness=0, double scale=1);
  void   drawCommonVehicle(const std::string& vname, 
			   const ObjectPose&, 
			   const BearingLine&, 
			   const ColorPack& body_color,
			   const ColorPack& vname_color,
			   const std::string& body, double shape_length,
			   bool vname_draw, int line=0);
  void   drawCommonMarker(double x, double y, double shape_width, 
			  const std::string& mtype, 
			  const std::string& label, 
			  const ColorPack& label_color, 
			  const std::vector<ColorPack>& color_packs);


  void  drawPolygons(const std::vector<XYPolygon>&);
  void  drawPolygon(const XYPolygon&, bool filled, bool dashed,
		    double line_width, double vertex_size,
		    const ColorPack& edge_color, 
		    const ColorPack& fill_color,
		    const ColorPack& vert_color,
		    const ColorPack& labl_color);
  
  void  drawSegLists(const std::vector<XYSegList>&);
  void  drawSegList(const XYSegList&, double lwid, double vsize, bool zdash,
		    const ColorPack& edge_color,
		    const ColorPack& vert_color,
		    const ColorPack& labl_color);

  void  drawGrids(const std::vector<XYGrid>&);
  void  drawGrid(const XYGrid&);

  void  drawCircles(const std::vector<XYCircle>&);
  void  drawCircle(const XYCircle&, int pts, bool filled, 
		   const ColorPack& edge_color,
		   const ColorPack& fill_color,
		   const ColorPack& vert_color,
		   const ColorPack& labl_color);

  void  drawPoints(const std::vector<XYPoint>&);
  void  drawPoint(const XYPoint&, double vertex_size,
		  const ColorPack& vert_color,
		  const ColorPack& labl_color);

  void  drawPointList(const std::vector<double>& xvect,
		      const std::vector<double>& yvect, double vsize,
		      const ColorPack& vert_color, 
		      bool point_edges_viewable=false);

  void  drawDropPoints();
  void  drawText(double px, double py, const std::string&, 
		 const ColorPack&, double font_size);

  void  drawHexagons();

protected:
  BackImg   m_back_img;
  BackImg   m_back_img_b;
  bool      m_back_img_b_ok;
  bool      m_back_img_b_on;
  bool      m_back_img_mod;

  double    m_zoom;
  double    m_vshift_x; 
  double    m_vshift_y; 
  double    m_x_origin;
  double    m_y_origin;
  bool      m_texture_init;
  GLuint*   m_textures;
  int       m_texture_set;
  bool      m_tiff_offon;

  double    m_hash_shade;
  double    m_hash_delta;
  double    m_fill_shade;
  bool      m_hash_offon;

  OpAreaSpec         m_op_area;
  VPlug_Markers      m_vmarkers;
  VPlug_GeoShapes    m_geoshapes;
  VPlug_GeoSettings  m_geo_settings;
  VPlug_VehiSettings m_vehi_settings;
  VPlug_DropPoints   m_drop_points;
  CMOOSGeodesy       m_geodesy;

  std::string m_param_warning;
};

#endif 
