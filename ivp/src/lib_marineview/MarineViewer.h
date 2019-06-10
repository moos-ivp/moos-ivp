/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MarineViewer.h                                       */
/*    DATE: May 31st, 2005                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef COMMON_MARINE_VIEWER_HEADER
#define COMMON_MARINE_VIEWER_HEADER

// Defined to silence GL deprecation warnings in OSX
#define GL_SILENCE_DEPRECATION

#include <string>
#include <vector>
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "XYGrid.h"
#include "XYConvexGrid.h"
#include "XYPolygon.h"
#include "XYSegList.h"
#include "XYSeglr.h"
#include "XYCircle.h"
#include "XYHexagon.h"
#include "XYRangePulse.h"
#include "XYCommsPulse.h"
#include "OpAreaSpec.h"
#include "VPlug_GeoShapes.h"
#include "VPlug_GeoSettings.h"
#include "VPlug_VehiSettings.h"
#include "VPlug_DropPoints.h"
#include "ColorPack.h"
#include "BearingLine.h"
#include "NodeRecord.h"
#include "Seglr.h"

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
  std::string geosetting(const std::string& s);
  std::string vehisetting(const std::string& s);
  void   clear(std::string vname, std::string shape, std::string stype);

  void   handleNoTiff();

  double getStaleReportThresh() {return(m_vehi_settings.getStaleReportThresh());}
  double getStaleRemoveThresh() {return(m_vehi_settings.getStaleRemoveThresh());}

  std::string getTiffFileA() {return(m_back_img.getTiffFile());}
  std::string getInfoFileA() {return(m_back_img.getInfoFile());}
  
  std::string getTiffFileB() {return(m_back_img_b.getTiffFile());}
  std::string getInfoFileB() {return(m_back_img_b.getInfoFile());}

  double getZoom() {return(m_zoom);}
  double getPanX() {return(m_vshift_x);}
  double getPanY() {return(m_vshift_y);}

protected:
  bool   readTiff(std::string);
  bool   readTiffB(std::string);

  void   drawTiff();
  double img2view(char, double);
  double view2img(char, double);
  double meters2img(char, double);
  double img2meters(char, double);

  void   drawHash(double xl=0, double xr=0, double yb=0, double yt=0);
  void   drawFastHash(double xl=0, double xr=0, double yb=0, double yt=0);
  void   drawSegment(double, double, double, double, double, double, double);

  void   drawOpArea(const OpAreaSpec&);
  void   drawDatum(const OpAreaSpec&);

  void   drawCommonVehicle(const NodeRecord&,
			   const BearingLine&, 
			   const ColorPack& body_color,
			   const ColorPack& vname_color,
			   bool vname_draw, 
			   unsigned int line=0);

  void  drawMarkers(const std::map<std::string, XYMarker>&);
  void  drawMarker(const XYMarker&);

  void  drawPolygons(const std::vector<XYPolygon>&);
  void  drawPolygon(const XYPolygon&);
  
  void  drawSegLists(const std::vector<XYSegList>&);
  void  drawSegList(const XYSegList&);

  void  drawSeglrs(const std::vector<XYSeglr>&);
  void  drawSeglr(const XYSeglr&);

  void  drawWedges(const std::vector<XYWedge>&);
  void  drawWedge(const XYWedge&);
  
  void  drawVectors(const std::vector<XYVector>&);
  void  drawVector(const XYVector&);

  void  drawGrids(const std::vector<XYGrid>&);
  void  drawGrid(const XYGrid&);

  void  drawConvexGrids(const std::vector<XYConvexGrid>&);
  void  drawConvexGrid(const XYConvexGrid&);

  void  drawCircles(const std::map<std::string, XYCircle>&, double timestamp=0);
  void  drawCircle(const XYCircle&, double timestamp);

  void  drawRangePulses(const std::vector<XYRangePulse>&, double timstamp);
  void  drawRangePulse(const XYRangePulse&, double timestamp);
  
  void  drawCommsPulses(const std::vector<XYCommsPulse>&, double timstamp);
  void  drawCommsPulse(const XYCommsPulse&, double timestamp);
  
  void  drawPoints(const std::map<std::string, XYPoint>&);

  //  void  drawPoints(const std::map<std::string, XYPoint>&);
  void  drawPoint(const XYPoint&);

  void  drawDropPoints();
  void  drawText(double px, double py, const std::string&, 
		 const ColorPack&, double font_size);
  void  drawTextX(double px, double py, const std::string&, 
		  const ColorPack&, double font_size);

  void  drawHexagons();

  void  drawGLPoly(double *points, unsigned int numPoints, 
		   ColorPack fill_color,
		   double thickness=0, double scale=1, 
		   double alpha=100);

  void gl_draw_aux(const std::string);
  
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

  double    m_hash_shade;
  double    m_fill_shade;

  VPlug_GeoSettings  m_geo_settings;
  VPlug_VehiSettings m_vehi_settings;
  VPlug_DropPoints   m_drop_points;
  CMOOSGeodesy       m_geodesy;
  bool               m_geodesy_initialized;
  OpAreaSpec         m_op_area;

  Fl_Group*          m_main_window;
  
  std::string m_param_warning;
};

#endif 









