/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "XYOval.h"
#include "XYArrow.h"
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
  virtual void modColorScheme() {};

  bool   addTiffFile(std::string file);

  void   clearBackground();
  bool   initGeodesy(double, double);
  bool   initGeodesy(const std::string&);
  bool   setTexture();
  bool   applyTiffFiles();
  std::string geosetting(const std::string& s);
  std::string vehisetting(const std::string& s);
  void   clear(std::string vname, std::string shape, std::string stype);

  void   setAutoZoom(double, double);
  void   autoZoom();
  
  bool   handleNoTiff();
  void   setVerbose(bool bval=true);
  void   setZoom(double dval) {m_zoom = dval;}
  
  double getStaleReportThresh() {return(m_vehi_settings.getStaleReportThresh());}
  double getStaleRemoveThresh() {return(m_vehi_settings.getStaleRemoveThresh());}

  std::string getTiffFileA();
  std::string getInfoFileA();
  
  std::string getTiffFileB();
  std::string getInfoFileB();
  
  double getZoom() {return(m_zoom);}
  double getPanX() {return(m_vshift_x);}
  double getPanY() {return(m_vshift_y);}
  double getImgWidthMtrs();
  
  std::vector<std::string> getTiffFiles() const;
  std::vector<std::string> getInfoFiles() const;

  unsigned int getTiffFileCount() const {return(m_tif_files.size());}
  std::string  getTiffFileCurrent() const;
  
protected:
  void   drawTiff();
  double img2view(char, double) const;
  double view2img(char, double) const;
  double meters2img(char, double, bool verbose=false) const;
  double img2meters(char, double) const;

  // For backward compatibility
  void   drawHash(double xl=0, double xh=0, double yl=0, double yh=0)
  {drawFastHash();}

  void   drawFastHash();

  void   drawSegment(double, double, double, double,
		     double, double, double);

  void   drawOpArea(const OpAreaSpec&);
  void   drawDatum(const OpAreaSpec&);

  void   drawCommonVehicle(const NodeRecord&,
			   const BearingLine&, 
			   const ColorPack& body_color,
			   const ColorPack& vname_color,
			   bool vname_draw, 
			   unsigned int line=0,
			   double transparency=1);

  void  drawMarkers(const std::map<std::string, XYMarker>&, double tstamp=0);
  void  drawMarker(const XYMarker&, double tstamp=0);

  void  drawTextBoxes(const std::map<std::string, XYTextBox>&, double tstamp=0);
  void  drawTextBox(const XYTextBox&, double tstamp=0);

  void  drawPolygons(const std::vector<XYPolygon>&, double timestamp=0);
  void  drawPolygon(const XYPolygon&);
  
  void  drawSegLists(const std::map<std::string, XYSegList>&, double timestamp=0);
  void  drawSegList(const XYSegList&);

  void  drawSeglrs(const std::map<std::string, XYSeglr>&, double timestamp=0);
  void  drawSeglr(const XYSeglr&);

  void  drawWedges(const std::vector<XYWedge>&);
  void  drawWedge(const XYWedge&);
  
  void  drawVectors(const std::vector<XYVector>&);
  void  drawVector(const XYVector&);

  void  drawGrids(const std::vector<XYGrid>&);
  void  drawGrid(const XYGrid&);

  //void  drawConvexGrids(const std::vector<XYConvexGrid>&);
  //void  drawConvexGrid(const XYConvexGrid&);

  void  drawConvexGrids(std::vector<XYConvexGrid>);
  void  drawConvexGrid(XYConvexGrid);

  void  drawCircles(const std::map<std::string, XYCircle>&, double timestamp=0);
  void  drawCircle(XYCircle, double timestamp=0);

  void  drawOvals(const std::map<std::string, XYOval>&, double timestamp=0);
  void  drawOval(XYOval, double timestamp=0);

  void  drawArrows(const std::map<std::string, XYArrow>&, double timestamp=0);
  void  drawArrow(XYArrow, double timestamp=0);

  void  drawRangePulses(const std::vector<XYRangePulse>&, double timstamp);
  void  drawRangePulse(const XYRangePulse&, double timestamp);
  
  void  drawCommsPulses(const std::vector<XYCommsPulse>&, double timstamp);
  void  drawCommsPulse(const XYCommsPulse&, double timestamp);
  
  void  drawPoints(const std::map<std::string, XYPoint>&, double timestamp=0);

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

  bool coordInView(double x, double y);
  bool coordInViewX(double x, double y);

protected:
  std::vector<BackImg>     m_back_imgs;
  std::vector<std::string> m_tif_files;
  
  bool     m_textures_init;
  
  unsigned int m_curr_back_img_ix;

  BackImg   m_back_img;

  bool      m_verbose;

  double    m_extrapolate;
  
  double    m_targ_zoom;
  double    m_targ_vshift_x;
  double    m_targ_vshift_y;
  
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










