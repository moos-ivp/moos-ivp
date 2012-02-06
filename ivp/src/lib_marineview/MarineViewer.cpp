/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MarineViewer.cpp                                     */
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

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include "glext.h" // http://www.opengl.org/registry/api/glext.h
//#include "tiffio.h" // CWG - TIFFIO Not needed
#else
#include <tiffio.h>
#endif

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "MarineViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "FColorMap.h"
#include "ColorParse.h"
#include "Shape_Ship.h"
#include "Shape_Kayak.h"
#include "Shape_AUV.h"
#include "Shape_Glider.h"
#include "Shape_Gateway.h"
#include "Shape_Diamond.h"
#include "Shape_Circle.h"
#include "Shape_Triangle.h"
#include "Shape_EField.h"
#include "Shape_Square.h"
#include "Shape_Kelp.h"
#include "XYFormatUtilsPoly.h"
#include "BearingLine.h"

using namespace std;

MarineViewer::MarineViewer(int x, int y, int w, int h, const char *l)
  : Fl_Gl_Window(x,y,w,h,l)
{
  m_zoom        = 1.0;
  m_vshift_x    = 0;
  m_vshift_y    = 0;
  m_x_origin    = 0;
  m_y_origin    = 0;

  m_hash_shade  = 0.65;
  m_hash_delta  = 100;
  m_fill_shade  = 0.55;
  m_texture_set = 0;
  m_texture_init = false;
  m_textures    = new GLuint[1];

  m_tiff_offon  = true;
  m_hash_offon  = false;

  m_back_img_b_ok = false;
  m_back_img_b_on = false;
  m_back_img_mod  = false;
  //  glGenTextures(1, m_textures);
}

//-------------------------------------------------------------

MarineViewer::~MarineViewer()
{
   delete [] m_textures;
}

//-------------------------------------------------------------
// Procedure: handle()

int MarineViewer::handle(int event)
{
  int result;
 
  if(event == FL_MOUSEWHEEL) {
    if(Fl::event_dy () < 0) {
      setParam("zoom", 1.05);
      redraw();
    }
    else if(Fl::event_dy () > 0) {
      setParam("zoom", 0.95);
      redraw();
    }
    result = 1;
  }
  else {
    result = Fl_Gl_Window::handle (event);
  }
  return(result);
}

//-------------------------------------------------------------
// Procedure: setParam

bool MarineViewer::setParam(string param, string value)
{
  string p = tolower(stripBlankEnds(param));

  // For some, we want value without tolower performed.
  value = stripBlankEnds(value);
  string v = tolower(value);
  
  bool handled = false;
  if(p=="tiff_type") {
    m_back_img_mod = true;
    handled = setBooleanOnString(m_back_img_b_on, v);
  }
  else if(p=="tiff_view") 
    handled = setBooleanOnString(m_tiff_offon, v);
  else if(p=="tiff_file")
    handled = m_back_img.readTiff(value);
  else if(p=="tiff_file_b") {
    handled = m_back_img_b.readTiff(value);
    if(handled)
      m_back_img_b_ok = true;
  }
  else if(p=="hash_view")
    handled = setBooleanOnString(m_hash_offon, v);
  else if(p=="geodesy_init")
    handled = initGeodesy(v);
  else if(p=="zoom") {
    handled = (v=="reset");
    if(handled)
      m_zoom = 1.0;
  }
  else if(p=="datum") {
    string lat = stripBlankEnds(biteString(value, ','));
    string lon = stripBlankEnds(value);
    handled = false;
    if(isNumber(lat) && isNumber(lon)) {
      double d_lat = atof(lat.c_str());
      double d_lon = atof(lon.c_str());
      m_back_img.setDatumLatLon(d_lat, d_lon);
      handled = true;
    }
  }
  else {
    handled = handled || m_drop_points.setParam(p,v);
    handled = handled || m_vehi_settings.setParam(p,v);
    handled = handled || m_geo_settings.setParam(p,v);
  }
  return(handled);
}

//-------------------------------------------------------------
// Procedure: setParam

bool MarineViewer::setParam(string param, double v)
{
  param = tolower(stripBlankEnds(param));
  
  bool handled = true;
  if(param == "hash_shade_mod") {
    m_hash_shade = vclip((m_hash_shade+v), 0, 1);
    cout << "hash_shade:" << m_hash_shade << endl;
  }
  if(param == "hash_shade") {
    m_hash_shade = vclip(v, 0, 1);
  }
  else if(param == "hash_delta") {
    if(((v >= 10) && (v <= 1000)) || (v==-1))
      m_hash_delta = (int)v;
  }
  else if(param == "back_shade_delta") {
    if(!m_tiff_offon) {
      if((m_fill_shade+v > 0) && (m_fill_shade+v <= 1.0))
	m_fill_shade += v;
    }
  }
  else if(param == "back_shade") {
    if((v >= 0) && (v <= 1.0))
      m_fill_shade = v;
  }
  else if(param == "zoom") {
    m_zoom *= v;
    if(m_zoom < 0.00001)      
      m_zoom = 0.00001;
  }
  else if(param == "pan_x") {
    double pix_shift = v * m_back_img.get_pix_per_mtr_x();
    m_vshift_x += pix_shift;
    cout << "pan_x:" << m_vshift_x << endl;
  }
  else if(param == "set_pan_x") {
    m_vshift_x = v;
    cout << "set_pan_x:" << m_vshift_x << endl;
   }
  else if(param == "pan_y") {
    double pix_shift = v * m_back_img.get_pix_per_mtr_y();
    m_vshift_y += pix_shift;
    cout << "pan_y:" << m_vshift_y << endl;
 }
  else if(param == "set_pan_y") {
    m_vshift_y = v;
    cout << "set_pan_y:" << m_vshift_y << endl;
  }
  else
    handled = false;
  
  return(handled);

}

// ----------------------------------------------------------
// Procedure: setTexture
//   Purpose: 

bool MarineViewer::setTexture()
{
  //static bool texture_init;
  
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  if(!m_texture_init) {
    glGenTextures(1, m_textures);
    m_texture_init = true;
  }	  
  
  glBindTexture(GL_TEXTURE_2D, m_textures[0]);
  if((m_texture_set <2) || m_back_img_mod) {
    unsigned char *img_data;
    unsigned int img_width;
    unsigned int img_height;
    
    if(m_back_img_b_ok && m_back_img_b_on) {
      img_data = m_back_img_b.get_img_data();
      img_width  = m_back_img_b.get_img_pix_width();
      img_height = m_back_img_b.get_img_pix_height();
    }
    else {
      img_data = m_back_img.get_img_data();
      img_width  = m_back_img.get_img_pix_width();
      img_height = m_back_img.get_img_pix_height();
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		 img_width, img_height, 0, 
		 GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 
		 (unsigned char *)img_data);
    m_texture_set++;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_back_img_mod = false;
  }
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: img2view

double MarineViewer::img2view(char xy, double img_val)

{
  double view_val = 0.0;

  double adj_img_width  = (double)(m_back_img.get_img_pix_width())  * m_zoom;
  double adj_img_height = (double)(m_back_img.get_img_pix_height()) * m_zoom;
  
  if(xy == 'x') {
    double adj_img_pix_x = img_val * adj_img_width; 
    view_val = adj_img_pix_x + m_x_origin + w()/2;
  }
  if(xy == 'y') {
    double adj_img_pix_y = img_val * adj_img_height; 
    view_val = adj_img_pix_y + m_y_origin + h()/2;
  }
  return(view_val);
}

//-------------------------------------------------------------
// Procedure: view2img
//      Note: Derived as from img2view above

double MarineViewer::view2img(char xy, double view_val)

{
  double img_val = 0.0;

  double adj_img_width  = (double)(m_back_img.get_img_pix_width())  * m_zoom;
  double adj_img_height = (double)(m_back_img.get_img_pix_height()) * m_zoom;

  if(xy == 'x') {
    img_val = ((view_val - m_x_origin) - w()/2);
    img_val = img_val / (adj_img_width);
  }
  
  if(xy == 'y') {
    img_val = ((view_val - m_y_origin) - h()/2);
    img_val = img_val / (adj_img_height);
  }

  return(img_val);
}

//-------------------------------------------------------------
// Procedure: meters2img

double MarineViewer::meters2img(char xy, double meters_val)
{
  double img_val = 0.0;
  if(xy == 'x') {
    double range = m_back_img.get_img_mtr_width();
    double pct = (meters_val - m_back_img.get_x_at_img_ctr()) / range;
    img_val = pct + 0.5;
  }
  if(xy == 'y') {
    double range = m_back_img.get_img_mtr_height();
    double pct = (meters_val - m_back_img.get_y_at_img_ctr()) / range;
    img_val = pct + 0.5;
  }
  return(img_val);
}

// ----------------------------------------------------------
// Procedure: img2meters
//      Note: Derived as from meters2img above

double MarineViewer::img2meters(char xy, double img_val)
{
  double meters_val = 0.0;
  if(xy == 'x') {
    double range = (m_back_img.get_x_at_img_right() - 
		    m_back_img.get_x_at_img_left());
    meters_val = (img_val - m_back_img.get_img_centx()) * range;
  }

  if(xy == 'y') {
    double range = (m_back_img.get_y_at_img_top() - 
		    m_back_img.get_y_at_img_bottom());
    meters_val = (img_val - m_back_img.get_img_centy()) * range;
  }
  return(meters_val);
}

// ----------------------------------------------------------
// Procedure: draw
//   Purpose: This is the "root" drawing routine - it is typically
//            invoked in the draw routines of subclasses. 

void MarineViewer::draw()
{
  double r = m_fill_shade;
  double g = m_fill_shade;
  double b = m_fill_shade + 0.1;
  if(b > 1.0)
    b = 1.0;
  glClearColor(r,g,b,0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, w(), h());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  unsigned int image_width  = m_back_img.get_img_pix_width();
  unsigned int image_height = m_back_img.get_img_pix_height();
  double shape_width  = image_width * m_zoom;
  double shape_height = image_height * m_zoom;
  double m_xx = m_vshift_x * m_zoom;
  double m_yy = m_vshift_y * m_zoom;

  m_x_origin = -shape_width/2  + m_xx;
  m_y_origin = -shape_height/2 + m_yy;

  // Draw the background image if the tiff flag is set
  if(m_tiff_offon)
    drawTiff();
}

// ----------------------------------------------------------
// Procedure: drawTiff

void MarineViewer::drawTiff()
{
  if(m_back_img.get_img_data() == 0)
    return;

  setTexture();

  unsigned int image_width  = m_back_img.get_img_pix_width();
  unsigned int image_height = m_back_img.get_img_pix_height();
  double shape_width  = image_width * m_zoom;
  double shape_height = image_height * m_zoom;

  double lower_x = m_x_origin;
  double upper_x = m_x_origin + shape_width;
  double lower_y = m_y_origin;
  double upper_y = m_y_origin + shape_height;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(w()/2, h()/2, 0);

  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f); glVertex2f(lower_x, lower_y);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(lower_x, upper_y);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(upper_x, upper_y);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(upper_x, lower_y);

  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  glFlush();
}

// ----------------------------------------------------------
// Procedure: drawHash
//   Purpose: Draw the hash marks based local coordinate positions
//            on the image. Due to the snapToStep, there should 
//            always be a hash line exactly on the datum (0,0).

void MarineViewer::drawHash(double xl, double xr, double yb, double yt)
{
  if((xl>xr) || (yb>yt))
    return;

  double r = m_hash_shade;
  double g = m_hash_shade;
  double b = m_hash_shade;

  double hash_delta = getHashDelta();

  // If specs not given use a default region defined by the image
  if((xl==0) && (xr==0) && (yb==0) && (yt==0)) {
    xl = m_back_img.get_x_at_img_left();
    xr = m_back_img.get_x_at_img_right();
    yb = m_back_img.get_y_at_img_bottom();
    yt = m_back_img.get_y_at_img_top();
  }

  if((xl>=xr) || (yb>=yt))
    return;

  // Make sure the has region aspect ratio of 1.0
  double xwid = xr - xl;
  double ywid = yt - yb;
  if(xwid > ywid) {
    yb -= ((xwid - ywid) / 2);
    yt += ((xwid - ywid) / 2);
  }
  else if(ywid > xwid) {
    xl -= ((ywid - xwid) / 2);
    xr += ((ywid - xwid) / 2);
  }

  double xw = xr-xl;
  double yw = yt-yb;

  double xlow = snapToStep((xl-(xw/2)), hash_delta);
  double xhgh = snapToStep((xr+(xw/2)), hash_delta);
  double ylow = snapToStep((yb-(yw/2)), hash_delta);
  double yhgh = snapToStep((yt+(yw/2)), hash_delta);
  
  for(double i=xlow; i<=xhgh; i+=hash_delta)
    drawSegment(i, ylow, i, yhgh, r, g, b);
  for(double j=ylow; j<=yhgh; j+=hash_delta)
    drawSegment(xlow, j, xhgh, j, r, g, b);
}

//-------------------------------------------------------------
// Procedure: getHashDelta

double MarineViewer::getHashDelta()
{
  if(m_hash_delta == -1) {
    if(m_zoom < 0.7)       return(1000);
    else if(m_zoom < 3.5)  return(500);
    else if(m_zoom < 7)    return(200);
    else if(m_zoom < 15)   return(100);
    else if(m_zoom < 47)   return(50);
    else return(10);
  }
  else
    return(m_hash_delta);
}

//-------------------------------------------------------------
// Procedure: drawGLPoly

void MarineViewer::drawGLPoly(double *points, unsigned int numPoints, 
			      ColorPack cpack, double thickness, 
			      double scale)
{
  if(thickness<=0)
    glBegin(GL_POLYGON);
  else {
    glLineWidth(thickness);
    glBegin(GL_LINE_STRIP);
  }

  glColor3f(cpack.red(), cpack.grn(), cpack.blu());
  unsigned int i;
  for(i=0; i<numPoints*2; i=i+2)
    glVertex2f(points[i]*scale, points[i+1]*scale);

  glEnd();
}

//-------------------------------------------------------------
// Procedure: drawCommonVehicle

void MarineViewer::drawCommonVehicle(const NodeRecord& record, 
				     const BearingLine& bng_line, 
				     const ColorPack& body_color,
				     const ColorPack& vname_color,
				     bool  vname_draw, 
				     unsigned int outer_line)
{
  string vname    = record.getName();
  string vehibody = tolower(record.getType());
  double vlength  = record.getLength();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  // Determine position in terms of image percentage
  double vehicle_ix = meters2img('x', record.getX());
  double vehicle_iy = meters2img('y', record.getY());

  // Determine position in terms of view percentage
  double vehicle_vx = img2view('x', vehicle_ix);
  double vehicle_vy = img2view('y', vehicle_iy);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(vehicle_vx, vehicle_vy, 0); // theses are in pixel units

  glScalef(m_zoom, m_zoom, m_zoom);
  glRotatef(-record.getHeading(),0,0,1);  

  ColorPack black(0,0,0);
  ColorPack gray(0.5, 0.5, 0.5);

  // The raw numerical values representing the shapes should be interpreted
  // as being in "meters". When passed to GLPoly, they are interpreted as
  // being in pixels. So There are two components to drawing the vehicle
  // as "actual size". First convert the pixels to meters and then multiply
  // by the (shapelength / length_in_meters). 
  // Since we know pix_per_mtr first, set "factor" initially to that.
  // Then when we know what kind of vehicle we're drawing, adjust the 
  // factor accordingly.
  double factor_x = m_back_img.get_pix_per_mtr_x();
  double factor_y = m_back_img.get_pix_per_mtr_y();
  
  if(vehibody == "kayak") {
    if(vlength > 0) {
      factor_x *= (vlength / g_kayakLength);
      factor_y *= (vlength / g_kayakLength);
    }
    double cx = g_kayakCtrX * factor_x;
    double cy = g_kayakCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_kayakBody, g_kayakBodySize, body_color, 0, factor_x);    
    if(outer_line)
      drawGLPoly(g_kayakBody, g_kayakBodySize, black, outer_line, factor_x);    
    drawGLPoly(g_kayakMidOpen, g_kayakMidOpenSize, gray, 0, factor_x);
    glTranslatef(cx, cy, 0);
  }
  else if((vehibody == "auv") || (vehibody == "uuv")) {
    if(vlength > 0) {
      factor_x *= (vlength / g_auvLength);
      factor_y *= (vlength / g_auvLength);
    }
    ColorPack blue = colorParse("blue");
    double cx = g_auvCtrX * factor_x;
    double cy = g_auvCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_auvBody, g_auvBodySize, body_color, 0, factor_x);
    if(outer_line > 0)
      drawGLPoly(g_auvBody, g_auvBodySize, black, outer_line, factor_x);
    drawGLPoly(g_propUnit, g_propUnitSize, blue, 0, factor_x);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "glider") {
    if(vlength > 0) {
      factor_x *= (vlength / g_gliderLength);
      factor_y *= (vlength / g_gliderLength);
    }
    double cx = g_gliderCtrX * factor_x;
    double cy = g_gliderCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_gliderWing, g_gliderWingSize, body_color,  0, factor_x);
    drawGLPoly(g_gliderWing, g_gliderWingSize, black, 1, factor_x);
    drawGLPoly(g_gliderBody, g_gliderBodySize, body_color,  0, factor_x);
    drawGLPoly(g_gliderBody, g_gliderBodySize, black, 1, factor_x);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "ship") {  
    if(vlength > 0) {
      factor_x *= (vlength / g_shipLength);
      factor_y *= (vlength / g_shipLength);
    }
    double cx = g_shipCtrX * factor_x;
    double cy = g_shipCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_shipBody, g_shipBodySize, body_color, 0, factor_x);
    if(outer_line > 0)
      drawGLPoly(g_shipBody, g_shipBodySize, black, outer_line, factor_x);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "track") {
    double cx = g_shipCtrX * factor_x;
    double cy = g_shipCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_shipBody, g_shipBodySize, body_color, 0, factor_x);
    if(outer_line > 0)
      drawGLPoly(g_shipBody, g_shipBodySize, black, outer_line, factor_x);
    glTranslatef(cx, cy, 0);
  }
  else {  // vehibody == "auv" is the default
    ColorPack blue("blue");
    if(vlength > 0) {
      factor_x *= (vlength / g_auvLength);
      factor_y *= (vlength / g_auvLength);
    }
    double cx = g_auvCtrX * factor_x;
    double cy = g_auvCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_auvBody, g_auvBodySize, body_color, 0, factor_x);
    if(outer_line > 0)
      drawGLPoly(g_auvBody, g_auvBodySize, blue, outer_line, factor_x);
    drawGLPoly(g_propUnit, g_propUnitSize, blue, 0, factor_x);
    glTranslatef(cx, cy, 0);
  }

  if(vname_draw) {
    glColor3f(vname_color.red(), vname_color.grn(), vname_color.blu());
    gl_font(1, 10);
    if(m_zoom > 4)
      gl_font(1, 12);
    double offset = 3.0 * (1/m_zoom);
    glRasterPos3f(offset, offset,0);
    gl_draw(vname.c_str());
  }

  if(bng_line.isValid() && m_vehi_settings.isViewableBearingLines()) {
    double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
    double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();
    double pix_per_mtr = (pix_per_mtr_x + pix_per_mtr_y) / 2.0;
    double bearing = bng_line.getBearing();
    double range   = bng_line.getRange() * pix_per_mtr;
    double lwidth  = bng_line.getVectorWidth();
    bool absolute  = bng_line.isBearingAbsolute();
    string bcolor  = bng_line.getVectorColor();
    ColorPack vpack(bcolor);

    double bx, by;
    projectPoint(bearing, range, 0, 0, bx, by);

    if(absolute)
      glRotatef(record.getHeading(),0,0,1);  

    glLineWidth(lwidth);
    glBegin(GL_LINE_STRIP);
    glColor3f(vpack.red(), vpack.grn(), vpack.blu());
    glVertex2f(0, 0);
    glVertex2f(bx, by);
    glEnd();
    glLineWidth(1);
  }

  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawMarkers

void MarineViewer::drawMarkers(const vector<XYMarker>& markers)
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the option "true" argument means return
  // true if nothing is known about the parameter.
  if(m_geo_settings.viewable("marker_viewable_all", true) == false)
    return;

  unsigned int i, vsize = markers.size();
  for(i=0; i<vsize; i++)
    drawMarker(markers[i]);
}

//-------------------------------------------------------------
// Procedure: drawMarker

void MarineViewer::drawMarker(const XYMarker& marker)
{
  if(!marker.active())
    return;
  if(!marker.valid())
    return;

  double    gscale = m_geo_settings.geosize("marker_scale");
  ColorPack labelc = m_geo_settings.geocolor("marker_label_color", "white");

  vector<ColorPack> color_vector;

  string mtype   = marker.get_type();
  string label   = marker.get_label();
  string message = marker.get_msg();
  double x       = marker.get_vx();
  double y       = marker.get_vy();
  double shape_width = marker.get_width() * gscale;

  if(shape_width <= 0)
    return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  // Determine position in terms of image percentage
  double marker_ix = meters2img('x', x);
  double marker_iy = meters2img('y', y);

  // Determine position in terms of view percentage
  double marker_vx = img2view('x', marker_ix);
  double marker_vy = img2view('y', marker_iy);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(marker_vx, marker_vy, 0); // theses are in pixel units
  glScalef(m_zoom, m_zoom, m_zoom);

  double factor_x = m_back_img.get_pix_per_mtr_x();
  double factor_y = m_back_img.get_pix_per_mtr_y();

  ColorPack cpack1 = marker.get_color("primary_color");
  ColorPack cpack2 = marker.get_color("secondary_color");

  unsigned int bw = 1; // border width

  ColorPack black(0,0,0);
  if(mtype == "gateway") {
    factor_x *= (shape_width / g_gatewayWidth);
    factor_y *= (shape_width / g_gatewayWidth);
    if(!cpack2.set()) cpack2 = black;
    if(!cpack1.set()) cpack1.setColor("green");
    double cx = g_gatewayCtrX * factor_x;
    double cy = g_gatewayCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_gatewayBody, g_gatewayBodySize, cpack1, 0, factor_x);    
    drawGLPoly(g_gatewayBody, g_gatewayBodySize, black, bw, factor_x);    
    drawGLPoly(g_gatewayMidBody, g_gatewayMidBodySize, cpack2, 0, factor_x);
    glTranslatef(cx, cy, 0);
  }

  else if(mtype == "efield") {
    factor_x *= (shape_width / g_efieldWidth);
    factor_y *= (shape_width / g_efieldWidth);
    if(!cpack2.set()) cpack2.setColor("1.0, 0.843, 0.0");
    if(!cpack1.set()) cpack1 = black;
    double cx = g_efieldCtrX * factor_x;
    double cy = g_efieldCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_efieldBody, g_efieldBodySize, cpack1, 0, factor_x);    
    drawGLPoly(g_efieldMidBody, g_efieldMidBodySize, cpack2, 0, factor_x);
    drawGLPoly(g_efieldMidBody, g_efieldMidBodySize, black, bw, factor_x);
    glTranslatef(cx, cy, 0);
  }

  else if(mtype == "diamond") {
    factor_x *= (shape_width / g_diamondWidth);
    factor_y *= (shape_width / g_diamondWidth);
    if(!cpack1.set()) cpack1.setColor("orange");
    double cx = g_diamondCtrX * factor_x;
    double cy = g_diamondCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_diamondBody, g_diamondBodySize, cpack1, 0, factor_x);
    drawGLPoly(g_diamondBody, g_diamondBodySize, black, bw, factor_x);
    glTranslatef(cx, cy, 0);
  }

  else if(mtype == "circle") {
    factor_x *= (shape_width / g_circleWidth);
    factor_y *= (shape_width / g_circleWidth);
    if(!cpack1.set()) cpack1.setColor("orange");
    double cx = g_circleCtrX * factor_x;
    double cy = g_circleCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_circleBody, g_circleBodySize, cpack1, 0, factor_x);
    drawGLPoly(g_circleBody, g_circleBodySize, black, bw, factor_x);    
    glTranslatef(cx, cy, 0);
  }

  else if(mtype == "triangle") {
    factor_x *= (shape_width / g_triangleWidth);
    factor_y *= (shape_width / g_triangleWidth);
    if(!cpack1.set()) cpack1.setColor("red");
    double cx = g_triangleCtrX * factor_x;
    double cy = g_triangleCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_triangleBody, g_triangleBodySize, cpack1, 0, factor_x);
    drawGLPoly(g_triangleBody, g_triangleBodySize, black, bw, factor_x);
    glTranslatef(cx, cy, 0);
  }

  else if(mtype == "square") {
    factor_x *= (shape_width / g_squareWidth);
    factor_y *= (shape_width / g_squareWidth);
    if(!cpack1.set()) cpack1.setColor("blue");
    double cx = g_squareCtrX * factor_x;
    double cy = g_squareCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_squareBody, g_squareBodySize, cpack1, 0, factor_x);
    drawGLPoly(g_squareBody, g_squareBodySize, black, bw, factor_x);
    glTranslatef(cx, cy, 0);
  }

  else if(mtype == "kelp") {
    factor_x *= (shape_width / g_kelpWidth);
    factor_y *= (shape_width / g_kelpWidth);
    if(!cpack1.set()) cpack1.setColor("0, 0.54, 0.54");
    double cx = g_kelpCtrX * factor_x;
    double cy = g_kelpCtrY * factor_y;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_kelpBody, g_kelpBodySize, cpack1, 0, factor_x);
    drawGLPoly(g_kelpBody, g_kelpBodySize, black, bw, factor_y);
    glTranslatef(cx, cy, 0);
  }

  bool draw_labels = m_geo_settings.viewable("marker_viewable_labels");

  if(draw_labels && ((label != "") || (message != ""))) {
    glColor3f(labelc.red(), labelc.grn(), labelc.blu());
    gl_font(1, 10);
    if(m_zoom > 4)
      gl_font(1, 12);
    double offset = 4.0 * (1/m_zoom);
    glRasterPos3f(offset, offset, 0);
    if(message != "")
      gl_draw(message.c_str());
    else
      gl_draw(label.c_str());
  }

  glPopMatrix();
}


//-------------------------------------------------------------
// Procedure: drawDatum

void MarineViewer::drawDatum(const OpAreaSpec& op_area)
{
  if(op_area.viewable("datum")) {
    XYPoint datum_point;
    datum_point.set_vertex_size(op_area.getDatumSize());
    datum_point.set_color("vertex", op_area.getDatumColor().str());
    drawPoint(datum_point);
  }
}

//-------------------------------------------------------------
// Procedure: drawOpArea

void MarineViewer::drawOpArea(const OpAreaSpec& op_area)
{
  if(op_area.viewable() == false)
    return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);


  unsigned int index = 0;
  unsigned int asize = op_area.size();

  double line_shade  = op_area.getLineShade();
  //double label_shade = op_area.getLabelShade();

  while(index < asize) {
    string group  = op_area.getGroup(index);
    double lwidth = op_area.getLWidth(index);
    bool   looped = op_area.getLooped(index);

    ColorPack lcolor = op_area.getLColor(index);
    ColorPack vcolor = op_area.getVColor(index);

    vector<double> xpos, ypos;
    vector<string> labels;

    bool done = false;
    while(!done) {
      double x = op_area.getXPos(index);
      double y = op_area.getYPos(index);
      string label = op_area.getLabel(index);

      index++;
      if((index >= asize) || (group != op_area.getGroup(index)))
	done = true;
      xpos.push_back(x);
      ypos.push_back(y);
      labels.push_back(label);
    }
    
    unsigned int i, vsize = xpos.size();
    double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
    double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();
    for(i=0; i<vsize; i++) {
      xpos[i] *= pix_per_mtr_x;
      ypos[i] *= pix_per_mtr_y;
    }

    // Draw the edges 
    glLineWidth(lwidth);
    glColor3f(lcolor.red()*line_shade, lcolor.grn()*line_shade, 
	      lcolor.blu()*line_shade);
    
    if(looped)
      glBegin(GL_LINE_LOOP);
    else
      glBegin(GL_LINE_STRIP);
    for(i=0; i<vsize; i++) {
      glVertex2f(xpos[i],  ypos[i]);
    }
    glEnd();

    if(op_area.viewable("labels")) {
      glColor3f(lcolor.red(), lcolor.grn(), lcolor.blu());
      gl_font(1, 10);
      for(i=0; i<vsize; i++) {
	double offset = 3.0 * (1/m_zoom);
	glRasterPos3f(xpos[i]+offset, ypos[i]+offset, 0);
	gl_draw(labels[i].c_str());
      }
    }
    glLineWidth(1);
  }

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: initGeodesy
//      Note: A check on legal boundaries is made here since it
//            does not appear to be checked for in the MOOSGeodesy
//            initialization implementation.

bool MarineViewer::initGeodesy(double lat, double lon)
{
  if((lat > 90) || (lat < -90))
    return(false);
  
  if((lon > 180) || (lon < -180))
    return(false);

  return(m_geodesy.Initialise(lat, lon));
}

//-------------------------------------------------------------
// Procedure: initGeodesy
//      Note: After the double values are determined be legitimate
//            numbers, a call is made to the initGeodesy function
//            to check on the legal boundaries since it does not
//            appear to be checked for in the MOOSGeodesy
//            initialization implementation.

bool MarineViewer::initGeodesy(const string& str)
{
  vector<string> svector = parseString(str, ',');
  if(svector.size() != 2)
    return(false);
  
  string slat = stripBlankEnds(svector[0]);
  string slon = stripBlankEnds(svector[1]);
  
  if((!isNumber(slat)) || (!isNumber(slon)))
    return(false);
  
  double dlat = atof(slat.c_str());
  double dlon = atof(slon.c_str());

  return(initGeodesy(dlat, dlon));
}

//-------------------------------------------------------------
// Procedure: drawPolygons

void MarineViewer::drawPolygons(const vector<XYPolygon>& polys)
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the option "true" argument means return
  // true if nothing is known about the parameter.
  if(!m_geo_settings.viewable("polygon_viewable_all", true))
    return;

  unsigned int i, vsize = polys.size();
  for(i=0; i<vsize; i++) 
    if(polys[i].active()) 
      drawPolygon(polys[i]);
}

//-------------------------------------------------------------
// Procedure: drawPolygon

void MarineViewer::drawPolygon(const XYPolygon& poly)
{
  ColorPack edge_c, fill_c, vert_c, labl_c;
  edge_c = m_geo_settings.geocolor("polygon_edge_color", "aqua");
  fill_c = m_geo_settings.geocolor("polygon_fill_color", "invisible");
  vert_c = m_geo_settings.geocolor("polygon_vertex_color", "red");
  labl_c = m_geo_settings.geocolor("polygon_label_color", "white");
  if(poly.color_set("label"))            // label_color
    labl_c = poly.get_color("label");
  if(poly.color_set("vertex"))           // vertex_color
    vert_c = poly.get_color("vertex");
  if(poly.color_set("edge"))             // edge_color
    edge_c = poly.get_color("edge");
  if(poly.color_set("fill"))             // fill_color
    fill_c = poly.get_color("fill");
  
  double transparency = 0.2;
  if(poly.transparency_set())
    transparency = poly.get_transparency();

  double line_width  = m_geo_settings.geosize("polygon_line_size");
  double vertex_size = m_geo_settings.geosize("polygon_vertex_size");
  if(poly.edge_size_set())         // edge_size
    line_width = poly.get_edge_size();
  if(poly.vertex_size_set())       // vertex_size
    vertex_size = poly.get_vertex_size();
  
  unsigned int vsize = poly.size();
  if(vsize < 1)
    return;
  
  unsigned int i, j;
  double *points = new double[2*vsize];
  
  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();
  int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = poly.get_vx(i) * pix_per_mtr_x;
    points[pindex+1] = poly.get_vy(i) * pix_per_mtr_y;
    pindex += 2;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);
  
  // Fill in the interior of polygon if it is a valid polygon
  // with greater than two vertices. (Two vertex polygons are
  // "valid" too, but we decide here not to draw the interior
  if((vsize > 2) && poly.is_convex() && fill_c.visible()) {
    glEnable(GL_BLEND);
    glColor4f(fill_c.red(), fill_c.grn(), fill_c.blu(), transparency);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    for(i=0; i<vsize*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
    glDisable(GL_BLEND);
  }
  
  // Now draw the edges - if the polygon is invalid, don't draw
  // the last edge.
  if((vsize > 1) && (line_width > 0)) {
    glLineWidth(line_width);
    glColor3f(edge_c.red(), edge_c.grn(), edge_c.blu());
    
    if(poly.is_convex())
      glBegin(GL_LINE_LOOP);
    else
      glBegin(GL_LINE_STRIP);
    for(i=0; i<vsize*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
    glLineWidth(1.0);
  }

  // If the polygon is just a single point, draw it big!
  if(vsize==1) {
    glPointSize(1.2 * m_zoom);
    // Draw the vertices with color coding for the first and last
    
    //glColor3f(0.7,0.13,0.13);  // Firebrick red b2 22 22
    glColor3f(0.13, 0.13, 0.7);  // Blueish
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2f(points[0], points[1]);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
  }

  if(vertex_size > 0) {
    glEnable(GL_POINT_SMOOTH);
    glPointSize(vertex_size);

    glColor3f(vert_c.red(), vert_c.grn(), vert_c.blu());
    glBegin(GL_POINTS);
    for(j=0; j<vsize; j++) 
      glVertex2f(points[(j*2)], points[(j*2)+1]);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
  }

  // Draw the labels unless either the viewer has it shut off OR if 
  // the publisher of the polygon requested it not to be viewed, by
  // setting the color to be "invisible".
  bool draw_labels = m_geo_settings.viewable("polygon_viewable_labels");
  if(draw_labels && labl_c.visible()) {
    double cx = poly.get_avg_x() * m_back_img.get_pix_per_mtr_x();
    //double cy = poly.get_avg_y() * m_back_img.get_pix_per_mtr();
    double my = poly.get_max_y() * m_back_img.get_pix_per_mtr_y();
    glTranslatef(cx, my, 0);
    
    glColor3f(labl_c.red(), labl_c.grn(), labl_c.blu());
    gl_font(1, 10);
    string plabel = poly.get_msg();
    if(plabel == "")
      plabel = poly.get_label();
    if((plabel != "") && (plabel != "_null_")) {
      glRasterPos3f(0, 0, 0);
      gl_draw(plabel.c_str());
    }
  }
  //-------------------------------- perhaps draw poly label


#if 0
  //-------------------------------- perhaps draw poly vertex labels
  if(m_geo_settings.viewable("polygon_viewable_vertex_labels")) {
    glTranslatef(0, 0, 0);
    for(j=0; j<vsize; j++) {
      double cx = points[(j*2)];
      double cy = points[(j*2)+1];
      
      glColor3f(edge_c.red(), edge_c.grn(), edge_c.blu());
      gl_font(1, 10);
      
      string vlabel = intToString(j);
      int slen = vlabel.length();
      glRasterPos3f(cx, cy, 0);
      gl_draw(vlabel.c_str());
    }
  }
  //-------------------------------- perhaps draw poly vertex_labels
#endif

  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawSegment
//      Note: points are given in meter in local coordinates.

void MarineViewer::drawSegment(double x1, double y1, double x2, double y2, 
			       double red, double grn, double blu)
{
  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();

  x1 *= pix_per_mtr_x;
  y1 *= pix_per_mtr_y;
  
  x2 *= pix_per_mtr_x;
  y2 *= pix_per_mtr_y;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);
  
  glColor3f(red, grn, blu);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawSegLists()

void MarineViewer::drawSegLists(const vector<XYSegList>& segls)
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the option "true" argument means return
  // true if nothing is known about the parameter.
  if(!m_geo_settings.viewable("seglist_viewable_all", "true"))
    return;
  
  unsigned int i, vsize = segls.size();
  for(i=0; i<vsize; i++)
    if(segls[i].active()) 
      drawSegList(segls[i]); 
}

//-------------------------------------------------------------
// Procedure: drawSegList

void MarineViewer::drawSegList(const XYSegList& segl)
{
  ColorPack edge_c, vert_c, labl_c;
  edge_c = m_geo_settings.geocolor("seglist_edge_color", "yellow");
  vert_c = m_geo_settings.geocolor("seglist_vertex_color", "white");
  labl_c = m_geo_settings.geocolor("seglist_label_color", "white");
  if(segl.color_set("label"))          // label_color
    labl_c = segl.get_color("label");
  if(segl.color_set("vertex"))         // vertex_color
    vert_c = segl.get_color("vertex");
  if(segl.color_set("edge"))           // edge_color
    edge_c = segl.get_color("edge");
  
  double lwid = m_geo_settings.geosize("seglist_edge_width", 1);
  double vertex_size = m_geo_settings.geosize("seglist_vertex_size", 2);
  if(segl.edge_size_set())       // edge_size
    lwid = segl.get_edge_size();
  if(segl.vertex_size_set())     // vertex_size
    vertex_size = segl.get_vertex_size();
  
  unsigned int vsize = segl.size();

  unsigned int i, j;
  double *points = new double[2*vsize];

  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();
  unsigned int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = segl.get_vx(i) * pix_per_mtr_x;
    points[pindex+1] = segl.get_vy(i) * pix_per_mtr_y;
    pindex += 2;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);
  
  // First draw the edges
  if((vsize >= 2) && edge_c.visible()) {
    glLineWidth(lwid);
    glColor3f(edge_c.red(), edge_c.grn(), edge_c.blu());

    glBegin(GL_LINE_STRIP);
    for(i=0; i<vsize*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
    glLineWidth(1.0);
  }

  // If the seglist is just a single point, draw it bigger (x 1.5)
  if(vert_c.visible()) {
    if(vsize==1) {
      glEnable(GL_POINT_SMOOTH);
      glPointSize(vertex_size * 1.5);
      // Draw the vertices with color coding for the first and last
      
      glColor3f(vert_c.red(), vert_c.grn(), vert_c.blu());
      glBegin(GL_POINTS);
      glVertex2f(points[0], points[1]);
      glEnd();
      glDisable(GL_POINT_SMOOTH);
    }
    else {
      if(vertex_size > 0) {
	glPointSize(vertex_size);
	// Draw the vertices in between the first and last ones
	glColor3f(vert_c.red(), vert_c.grn(), vert_c.blu());
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	for(j=0; j<vsize; j++) {
	  glVertex2f(points[(j*2)], points[(j*2)+1]);
	}
	glEnd();
	glDisable(GL_POINT_SMOOTH);
      }
    }
  }

  // Draw the labels unless either the viewer has it shut off OR if 
  // the publisher of the seglist requested it not to be viewed, by
  // setting the color to be "invisible".
  bool draw_labels = m_geo_settings.viewable("seglist_viewable_labels");
  if(draw_labels && labl_c.visible()) {
    double cx = segl.get_avg_x() * m_back_img.get_pix_per_mtr_x();
    //double cy = poly.get_avg_y() * m_back_img.get_pix_per_mtr();
    double my = segl.get_max_y() * m_back_img.get_pix_per_mtr_y();
    glTranslatef(cx, my, 0);
    
    glColor3f(labl_c.red(), labl_c.grn(), labl_c.blu());
    gl_font(1, 10);
    string plabel = segl.get_msg();
    if(plabel == "")
      plabel = segl.get_label();
    if(plabel != "") {
      glRasterPos3f(0, 0, 0);
      gl_draw(plabel.c_str());
    }
  }
  //-------------------------------- perhaps draw seglist label

  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawVectors()

void MarineViewer::drawVectors(const vector<XYVector>& vects)
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the optional "true" argument means return
  // true if nothing is known about the parameter.
  if(!m_geo_settings.viewable("vector_viewable_all", "true"))
    return;
  
  unsigned int i, vsize = vects.size();
  for(i=0; i<vsize; i++)
    if(vects[i].active())
      drawVector(vects[i]); 
}

//-------------------------------------------------------------
// Procedure: drawVector

void MarineViewer::drawVector(const XYVector& vect)
{
  // Determine the color properties
  ColorPack edge_c, vert_c, labl_c;
  edge_c = m_geo_settings.geocolor("vector_edge_color", "yellow");
  vert_c = m_geo_settings.geocolor("vector_vertex_color", "white");
  labl_c = m_geo_settings.geocolor("vector_label_color", "white");
  if(vect.color_set("label"))          
    labl_c = vect.get_color("label");
  if(vect.color_set("vertex"))         
    vert_c = vect.get_color("vertex");
  if(vect.color_set("edge"))           
    edge_c = vect.get_color("edge");
  
  // Determine the size properties
  double lwid = m_geo_settings.geosize("vector_edge_width", 1);
  double vertex_size = m_geo_settings.geosize("vector_vertex_size", 2);
  if(vect.edge_size_set())   
    lwid = vect.get_edge_size();
  if(vect.vertex_size_set()) 
    vertex_size = vect.get_vertex_size();

    // Do the drawing
  double *points = new double[8];

  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();

  double vang  = vect.ang();
  double ovang = angle360(vang-180);

  double vzoom = m_geo_settings.geosize("vector_length_zoom", 2);
  double vmag  = vect.mag() * vzoom;

  // First determine the point on the end of the vector
  double hx, hy;
  projectPoint(vang, vmag, vect.xpos(), vect.ypos(), hx, hy);

  double head_size = vect.headsize();
  if(head_size < 0)
    head_size = 4.0;

  // Then determine the head points
  double hx1, hx2, hy1, hy2;
  projectPoint(ovang+30, head_size, hx, hy, hx1, hy1);
  projectPoint(ovang-30, head_size, hx, hy, hx2, hy2);

  points[0]   = vect.xpos() * pix_per_mtr_x;
  points[1]   = vect.ypos() * pix_per_mtr_y;
  points[2]   = hx * pix_per_mtr_x;
  points[3]   = hy * pix_per_mtr_y;

  points[4]   = hx1 * pix_per_mtr_x;
  points[5]   = hy1 * pix_per_mtr_y;
  points[6]   = hx2 * pix_per_mtr_x;
  points[7]   = hy2 * pix_per_mtr_y;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  // First draw the vector stem
  glLineWidth(lwid);
  glColor3f(edge_c.red(), edge_c.grn(), edge_c.blu());

  glBegin(GL_LINE_STRIP);
  glVertex2f(points[0], points[1]);
  glVertex2f(points[2], points[3]);
  glEnd();

  // Draw the vector vertex point
  glEnable(GL_POINT_SMOOTH);
  glPointSize(vertex_size);
  glColor3f(vert_c.red(), vert_c.grn(), vert_c.blu());
  glBegin(GL_POINTS);
  glVertex2f(points[0], points[1]);
  glEnd();
  glDisable(GL_POINT_SMOOTH);
  
  // Then draw the vector head
  glBegin(GL_POLYGON);
  glVertex2f(points[4], points[5]);
  glVertex2f(points[6], points[7]);
  glVertex2f(points[2], points[3]);
  glEnd();

  glLineWidth(1.0);

  // Draw the labels unless either the viewer has it shut off OR if 
  // the publisher of the vector requested it not to be viewed, by
  // setting the color to be "invisible".
  bool draw_labels = m_geo_settings.viewable("vector_viewable_labels");
  if(draw_labels && labl_c.visible()) {
    double cx = vect.xpos() * m_back_img.get_pix_per_mtr_x();
    double cy = vect.ypos() * m_back_img.get_pix_per_mtr_y();
    glTranslatef(cx, cy, 0);
    
    glColor3f(labl_c.red(), labl_c.grn(), labl_c.blu());
    gl_font(1, 10);
    string plabel = vect.get_msg();
    if(plabel == "")
      plabel = vect.get_label();
    if(plabel != "") {
      glRasterPos3f(0, 0, 0);
      gl_draw(plabel.c_str());
    }
  }
  //-----------------------------end perhaps draw vect label

  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawHexagons()

void MarineViewer::drawHexagons()
{
#if 0
  unsigned int i, hsize = m_geoshapes.sizeHexagons();
  for(i=0; i<hsize; i++)
    if(m_geoshapes.getHexagon(i).active())
      drawPolygon(m_geoshapes.getHexagon(i), false, false);
#endif
}

//-------------------------------------------------------------
// Procedure: drawGrids

void MarineViewer::drawGrids(const vector<XYGrid>& grids)
{
  if(m_geo_settings.viewable("grid_viewable_all", true) == false)
    return;

  unsigned int i, vsize = grids.size();
  for(i=0; i<vsize; i++)
    drawGrid(grids[i]);
}

//-------------------------------------------------------------
// Procedure: drawGrid

void MarineViewer::drawGrid(const XYGrid& grid)
{
  FColorMap cmap;

  unsigned int gsize = grid.size();
  if(gsize == 0)
    return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glLineWidth(1.0);  // added dec1306
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  unsigned int i;
  double px[4];
  double py[4];

  double min_eval = grid.getMinUtilPoss();
  double max_eval = grid.getMaxUtilPoss();

  for(i=0; i<gsize; i++) {
    XYSquare element = grid.getElement(i);
    double   eval = grid.getUtil(i);
    double   pct  = (eval-min_eval)/(max_eval-min_eval);

    double   r    = cmap.getIRVal(pct);
    double   g    = cmap.getIGVal(pct);
    double   b    = cmap.getIBVal(pct);
    
    px[0] = element.getVal(0,0);
    py[0] = element.getVal(1,0);
    px[1] = element.getVal(0,1);
    py[1] = element.getVal(1,0);
    px[2] = element.getVal(0,1);
    py[2] = element.getVal(1,1);
    px[3] = element.getVal(0,0);
    py[3] = element.getVal(1,1);
    
    glEnable(GL_BLEND);
    glColor4f(r,g,b,1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);

    for(int j=0; j<4; j++)
      glVertex2f(px[j], py[j]);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINE_LOOP);
    for(int k=0; k<4; k++)
      glVertex2f(px[k], py[k]);
    glEnd();
  }

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawConvexGrids

void MarineViewer::drawConvexGrids(const vector<XYConvexGrid>& grids)
{
  if(m_geo_settings.viewable("grid_viewable_all", true) == false)
    return;

  unsigned int i, vsize = grids.size();
  for(i=0; i<vsize; i++)
    drawConvexGrid(grids[i]);
}

//-------------------------------------------------------------
// Procedure: drawConvexGrid

void MarineViewer::drawConvexGrid(const XYConvexGrid& grid)
{
  FColorMap cmap;

  unsigned int gsize = grid.size();
  if(gsize == 0)
    return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glLineWidth(0.5);  // added dec1306
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  unsigned int i;
  double px[4];
  double py[4];

  double min_eval, max_eval, range = 0;

  //cout << "min_limited:" << grid.cellVarMinLimited() << endl;
  //cout << "max_limited:" << grid.cellVarMaxLimited() << endl << endl;


  if(grid.cellVarMaxLimited() && grid.cellVarMinLimited()) {
    min_eval = grid.getMinLimit();
    max_eval = grid.getMaxLimit();
    range  = max_eval - min_eval;
  }
  else {
    min_eval = grid.getMin();
    max_eval = grid.getMax();
    range  = max_eval - min_eval;
  }
  
  double cell_transparency = m_geo_settings.transparency("grid_transparency", 0.3);
  double edge_transparency = cell_transparency * 0.6;
  
  for(i=0; i<gsize; i++) {
    XYSquare element = grid.getElement(i);

    px[0] = element.getVal(0,0) * m_back_img.get_pix_per_mtr_x();
    py[0] = element.getVal(1,0) * m_back_img.get_pix_per_mtr_y();
    px[1] = element.getVal(0,1) * m_back_img.get_pix_per_mtr_x();
    py[1] = element.getVal(1,0) * m_back_img.get_pix_per_mtr_y();
    px[2] = element.getVal(0,1) * m_back_img.get_pix_per_mtr_x();
    py[2] = element.getVal(1,1) * m_back_img.get_pix_per_mtr_y();
    px[3] = element.getVal(0,0) * m_back_img.get_pix_per_mtr_x();
    py[3] = element.getVal(1,1) * m_back_img.get_pix_per_mtr_y();

    // Draw the internal parts of the cells if range is nonzero.
    if(range > 0) {
      double   eval = grid.getVal(i);
      double   pct  = (eval-min_eval)/(range);
      double   r    = cmap.getIRVal(pct);
      double   g    = cmap.getIGVal(pct);
      double   b    = cmap.getIBVal(pct);
      
      glEnable(GL_BLEND);
      //glColor4f(r,g,b,0.3);
      glColor4f(r,g,b,cell_transparency);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBegin(GL_POLYGON);
      for(int j=0; j<4; j++)
	glVertex2f(px[j], py[j]);
      glEnd();
      glDisable(GL_BLEND);
    }

    // Begin Draw the cell edges
    glEnable(GL_BLEND);
    glColor4f(0.6,0.6,0.6,edge_transparency);
    glBegin(GL_LINE_LOOP);
    for(int k=0; k<4; k++)
      glVertex2f(px[k], py[k]);
    glEnd();
    glDisable(GL_BLEND);
    // End Draw the cell edges
    
  }

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawCircles

void MarineViewer::drawCircles(const vector<XYCircle>& circles)
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the optional "true" argument means return
  // true if nothing is known about the parameter.
  if(!m_geo_settings.viewable("circle_viewable_all", true))
    return;

  unsigned int i, vsize = circles.size();
  
  for(i=0; i<vsize; i++)
    if(circles[i].active())
      drawCircle(circles[i], 180); 
  
}

//-------------------------------------------------------------
// Procedure: drawCircle

void MarineViewer::drawCircle(const XYCircle& circle, unsigned int pts)
{
  ColorPack edge_c("blue");
  ColorPack vert_c("blue");
  ColorPack labl_c("white");
  ColorPack fill_c("invisible");

  if(circle.color_set("label"))          // label_color
    labl_c = circle.get_color("label");
  if(circle.color_set("vertex"))         // vertex_color
    vert_c = circle.get_color("vertex");
  if(circle.color_set("edge"))           // edge_color
    edge_c = circle.get_color("edge");  
  if(circle.color_set("fill"))             // fill_color
    fill_c = circle.get_color("fill");

  double transparency = 0.2;
  if(circle.transparency_set())
    transparency = circle.get_transparency();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glLineWidth(1.0);  // added dec1306
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  double px  = circle.getX();
  double py  = circle.getY();
  double rad = circle.getRad();

  string poly_str = "radial:";
  poly_str += doubleToString(px,2) + ",";
  poly_str += doubleToString(py,2) + ",";
  poly_str += doubleToString(rad,2) + ",";
  poly_str += uintToString(pts);
  
  XYPolygon poly = string2Poly(poly_str);

  // Now set points to the actual size vs. the requested size
  unsigned int actual_pts = poly.size();

  if(actual_pts == 0)
    return;

  double *points = new double[2 * actual_pts];

  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();
  unsigned int i, pindex = 0;
  for(i=0; i<actual_pts; i++) {
    points[pindex]   = poly.get_vx(i) * pix_per_mtr_x;
    points[pindex+1] = poly.get_vy(i) * pix_per_mtr_y;
    pindex += 2;
  }

  glColor3f(edge_c.red(), edge_c.grn(), edge_c.blu());
  glBegin(GL_LINE_LOOP);
  for(i=0; i<actual_pts*2; i=i+2) {
    glVertex2f(points[i], points[i+1]);
  }
  glEnd();

  // If filled option is on, draw the interior of the circle
  if(fill_c.visible()) {
    glEnable(GL_BLEND);
    glColor4f(fill_c.red(), fill_c.grn(), fill_c.blu(), transparency);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    for(i=0; i<actual_pts*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
    glDisable(GL_BLEND);
  }

  delete [] points;
  glFlush();
  glPopMatrix();

}

//-------------------------------------------------------------
// Procedure: drawRangePulses

void MarineViewer::drawRangePulses(const vector<XYRangePulse>& pulses,
				   double timestamp)
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the optional "true" argument means return
  // true if nothing is known about the parameter.
  if(!m_geo_settings.viewable("pulses_viewable_all", true))
    return;

  unsigned int i, vsize = pulses.size();

  for(i=0; i<vsize; i++)
    if(pulses[i].active())
      drawRangePulse(pulses[i], timestamp);
}

//-------------------------------------------------------------
// Procedure: drawRangePulse

void MarineViewer::drawRangePulse(const XYRangePulse& pulse,
				  double timestamp)
{
  ColorPack edge_c("blue");
  ColorPack fill_c("light_blue");
  if(pulse.color_set("edge"))           // edge_color
    edge_c = pulse.get_color("edge");
  if(pulse.color_set("fill"))           // fill_color
    fill_c = pulse.get_color("fill");
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glLineWidth(1.0);  // added dec1306
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  vector<double> points = pulse.get_circle(timestamp, 90);
  unsigned int vsize   = points.size();
  unsigned int i, pcnt = vsize;

  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();
  for(i=0; i<pcnt; i=i+2) {
    points[i]   *= pix_per_mtr_x;
    points[i+1] *= pix_per_mtr_y;
  }

  // Draw the circle line
  glColor3f(edge_c.red(), edge_c.grn(), edge_c.blu());
  glBegin(GL_LINE_LOOP);
  for(i=0; i<pcnt; i=i+2) 
    glVertex2f(points[i], points[i+1]);
  glEnd();

  // Determine the fill degree [0,1]. 1 is completely opaque
  double fill_degree = pulse.get_fill(timestamp);

  // If filled option is on, draw the interior of the circle
  if((fill_degree > 0) && fill_c.visible()) {
    glEnable(GL_BLEND);
    glColor4f(fill_c.red(), fill_c.grn(), fill_c.blu(), fill_degree);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    for(i=0; i<pcnt; i=i+2)
      glVertex2f(points[i], points[i+1]);
    glEnd();
    glDisable(GL_BLEND);
  }

  glFlush();
  glPopMatrix();  
}

//-------------------------------------------------------------
// Procedure: drawCommsPulses

void MarineViewer::drawCommsPulses(const vector<XYCommsPulse>& pulses,
				   double timestamp)
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the optional "true" argument means return
  // true if nothing is known about the parameter.
  if(!m_geo_settings.viewable("comms_pulses_viewable_all", true))
    return;

  unsigned int i, vsize = pulses.size();

  for(i=0; i<vsize; i++)
    if(pulses[i].active())
      drawCommsPulse(pulses[i], timestamp);
}

//-------------------------------------------------------------
// Procedure: drawCommsPulse

void MarineViewer::drawCommsPulse(const XYCommsPulse& pulse,
				  double timestamp)
{
  ColorPack fill_c("light_blue");
  if(pulse.color_set("fill"))           // fill_color
    fill_c = pulse.get_color("fill");
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glLineWidth(1.0); 
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  vector<double> points = pulse.get_triangle(timestamp);
  unsigned int vsize   = points.size();
  unsigned int i, pcnt = vsize;

  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();
  for(i=0; i<pcnt; i=i+2) {
    points[i]   *= pix_per_mtr_x;
    points[i+1] *= pix_per_mtr_y;
  }

#if 0
  // Draw the triangle line
  glColor3f(fill_c.red(), fill_c.grn(), fill_c.blu());
  glBegin(GL_LINE_LOOP);
  for(i=0; i<pcnt; i=i+2) 
    glVertex2f(points[i], points[i+1]);
  glEnd();
#endif

  // Determine the fill degree [0,1]. 1 is completely opaque
  //double fill_degree = pulse.get_fill(timestamp);
  double fill_degree = 0.5;

  // If filled option is on, draw the interior of the triangle
  if((fill_degree > 0) && fill_c.visible()) {
    glEnable(GL_BLEND);
    glColor4f(fill_c.red(), fill_c.grn(), fill_c.blu(), fill_degree);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    for(i=0; i<pcnt; i=i+2)
      glVertex2f(points[i], points[i+1]);
    glEnd();
    glDisable(GL_BLEND);
  }

  glFlush();
  glPopMatrix();  
}

//-------------------------------------------------------------
// Procedure: drawPoints

void MarineViewer::drawPoints(const vector<XYPoint>& points)
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the option "true" argument means return
  // true if nothing is known about the parameter.
  if(!m_geo_settings.viewable("point_viewable_all", true))
    return;

  unsigned int i, vsize = points.size();
  for(i=0; i<vsize; i++)
    if(points[i].active())
      drawPoint(points[i]);
}

//-------------------------------------------------------------
// Procedure: drawPoint

void MarineViewer::drawPoint(const XYPoint& point)
{
  // The second argument to geocolor is what is returned if no color
  // mapping is present in the geo_settings data structure.
  ColorPack vert_c, labl_c;
  vert_c = m_geo_settings.geocolor("point_vertex_color", "red");
  labl_c = m_geo_settings.geocolor("point_label_color", "aqua_marine");  
  if(point.color_set("label"))
    labl_c = point.get_color("label");
  if(point.color_set("vertex"))
    vert_c = point.get_color("vertex");

  // The second argument to geosize is what is returned if no size
  // mapping is present in the geo_settings data structure.
  double vertex_size = m_geo_settings.geosize("point_vertex_size", 3);
  if(point.vertex_size_set())
    vertex_size = point.get_vertex_size();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  double px  = point.get_vx() * m_back_img.get_pix_per_mtr_x();
  double py  = point.get_vy() * m_back_img.get_pix_per_mtr_y();

  if(vert_c.visible()) {
    glPointSize(vertex_size);
    glColor3f(vert_c.red(), vert_c.grn(), vert_c.blu()); 
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2f(px, py);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
  }

  // Draw the labels unless either the viewer has it shut off OR if 
  // the publisher of the point requested it not to be viewed, by
  // setting the color to be "invisible".
  bool draw_labels = m_geo_settings.viewable("point_viewable_labels");
  if(draw_labels && labl_c.visible()) {
    glColor3f(labl_c.red(), labl_c.grn(), labl_c.blu());
    gl_font(1, 12);
    
    string plabel = point.get_msg();
    if(plabel == "")
      plabel = point.get_label();
    if(plabel != "") {    
      double offset = 3.0 * (1/m_zoom);
      glRasterPos3f(px+offset, py+offset, 0);
      gl_draw(plabel.c_str());
    }
  }

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawDropPoints

void MarineViewer::drawDropPoints()
{
  if(!m_drop_points.viewable())
    return;

  unsigned int i, vsize = m_drop_points.size();
  for(i=0; i<vsize; i++)
    drawPoint(m_drop_points.point(i));
}

//-------------------------------------------------------------
// Procedure: drawText

void MarineViewer::drawText(double px, double py, const string& text,
			    const ColorPack& font_c, double font_size) 
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);
  
  px *= m_back_img.get_pix_per_mtr_x();
  py *= m_back_img.get_pix_per_mtr_y();

  if(font_c.visible()) {
    glColor3f(font_c.red(), font_c.grn(), font_c.blu());
    gl_font(1, font_size);
    glRasterPos3f(px, py, 0);
    gl_draw(text.c_str());
  }
  glFlush();
  glPopMatrix();
}


