/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <tiffio.h>
#include "MarineViewer.h"
#include "MBUtils.h"
#include "FColorMap.h"
#include "ColorParse.h"
#include "Shape_Ship.h"
#include "Shape_Kayak.h"
#include "Shape_AUV.h"
#include "Shape_Glider.h"
#include "Shape_Gateway.h"
#include "Shape_Diamond.h"
#include "Shape_Triangle.h"
#include "Shape_EField.h"
#include "Shape_Square.h"
#include "Shape_Kelp.h"
#include "XYBuildUtils.h"


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
  //m_fill_shade  = 0.7;
  m_fill_shade  = 0.55;
  m_texture_set = 0;
  m_texture_init = false;
  m_textures    = new GLuint[1];

  m_cross_offon = false;
  m_tiff_offon  = true;
  m_hash_offon  = false;

  m_back_img_b_ok = false;
  m_back_img_b_on = false;
  m_back_img_mod  = false;
  //  glGenTextures(1, m_textures);
}

//-------------------------------------------------------------
// Procedure: setParam
//      Note: A mutex is used since the member variables being set
//            are perhaps being altered or read by another thread.

bool MarineViewer::setParam(string param, string value)
{
  string p = tolower(stripBlankEnds(param));
  string v = tolower(stripBlankEnds(value));

  mutexLock();
  bool handled = false;
  if(p=="cross_view")
    handled = setBooleanOnString(m_cross_offon, v);
  else if(p=="tiff_type") {
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
  else if(p=="marker")
    handled = m_vmarkers.addVMarker(v, m_geodesy);
  else if(p=="op_vertex")
    handled = m_op_area.addVertex(v, m_geodesy);
  else if(p=="zoom") {
    handled = (p=="reset");
    if(handled)
      m_zoom = 1.0;
  }
  else if(p=="view_polygon")
    handled = m_geoshapes.addPolygon(value);
  else if(p=="view_seglist")
    handled = m_geoshapes.addSegList(value);
  else if(p=="view_point")
    handled = m_geoshapes.addPoint(value);
  else if(p=="grid_config")
    handled = m_geoshapes.addGrid(value);
  else if(p=="grid_delta")
    handled = m_geoshapes.updateGrid(value);
  else {
    handled = handled || m_op_area.setParam(p,v);
    handled = handled || m_vmarkers.setParam(p,v);
    handled = handled || m_geoshapes.setParam(p,v);
  }

  mutexUnLock();
  return(handled);

}

//-------------------------------------------------------------
// Procedure: setCommonParam
//      Note: A mutex is used since the member variables being set
//            are perhaps being altered or read by another thread.

bool MarineViewer::setParam(string param, double v)
{
  param = tolower(stripBlankEnds(param));
  
  mutexLock();
  bool handled = true;
  if(param == "hash_shade") {
    if((m_hash_shade+v >= 0) && (m_hash_shade+v <= 1.0))
      m_hash_shade += v;
  }
  else if(param == "hash_delta") {
    if(((v >= 10) && (v <= 1000)) || (v==-1))
      m_hash_delta = v;
  }
  else if(param == "back_shade") {
    if(!m_tiff_offon) {
      if((m_fill_shade+v > 0) && (m_fill_shade+v <= 1.0))
	m_fill_shade += v;
    }
  }
  else if(param == "zoom") {
    m_zoom *= v;
    if(m_zoom < 0.00001)      
      m_zoom = 0.00001;
  }
  else if(param == "pan_x") {
    double pix_shift = v * m_back_img.get_pix_per_mtr();
    m_vshift_x += pix_shift;
  }
  else if(param == "set_pan_x") {
    m_vshift_x = v;
  }
  else if(param == "pan_y") {
    double pix_shift = v * m_back_img.get_pix_per_mtr();
    m_vshift_y += pix_shift;
  }
  else if(param == "set_pan_y") {
    m_vshift_y = v;
  }
  else 
    handled = false;
  
  mutexUnLock();
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
    int img_width;
    int img_height;
    
    if(m_back_img_b_ok && m_back_img_b_on) {
      img_data = m_back_img_b.get_img_data();
      img_width  = m_back_img_b.get_img_width();
      img_height = m_back_img_b.get_img_height();
    }
    else {
      img_data = m_back_img.get_img_data();
      img_width  = m_back_img.get_img_width();
      img_height = m_back_img.get_img_height();
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

  double adj_img_width  = m_back_img.get_img_width()  * m_zoom;
  double adj_img_height = m_back_img.get_img_height() * m_zoom;
  
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

  double adj_img_width  = m_back_img.get_img_width()  * m_zoom;
  double adj_img_height = m_back_img.get_img_height() * m_zoom;

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
    img_val  = ((meters_val/100.0) * m_back_img.get_img_meters());
    img_val += m_back_img.get_img_centx();
  }
  if(xy == 'y') {
    img_val  = ((meters_val/100.0) * m_back_img.get_img_meters());
    img_val += m_back_img.get_img_centy();
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
    meters_val  = (img_val - m_back_img.get_img_centx()) * 100.0;
    meters_val  = meters_val / m_back_img.get_img_meters();
  }

  if(xy == 'y') {
    meters_val  = (img_val - m_back_img.get_img_centy()) * 100.0;
    meters_val  = meters_val / m_back_img.get_img_meters();
  }
  return(meters_val);
}

// ----------------------------------------------------------
// Procedure: getCrossHairMeters
//      Note: 

double MarineViewer::getCrossHairMeters(char xy)
{
  if(xy == 'x') {
    int iwidth = m_back_img.get_img_width();
    double x_pos = ((double)(iwidth) / 2.0) - (double)(m_vshift_x);
    double x_pct = m_back_img.pixToPctX(x_pos);
    double x_pct_cent = m_back_img.get_img_centx();
    double x_pct_mtrs = m_back_img.get_img_meters();
    double meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
    return(meters);   
  }
  else if (xy == 'y') {
    int iheight = m_back_img.get_img_height();
    double y_pos = ((double)(iheight) / 2.0) - (double)(m_vshift_y);
    double y_pct = m_back_img.pixToPctY(y_pos);
    double y_pct_cent = m_back_img.get_img_centy();
    double y_pct_mtrs = m_back_img.get_img_meters();
    double meters = (y_pct - y_pct_cent) / (y_pct_mtrs / 100.0);
    return(meters);
  }
  else
    return(0);
}


// ----------------------------------------------------------
// Procedure: draw
//   Purpose: This is the "root" drawing routine - it is typically
//            invoked in the draw routines of subclasses. 
//      Note: A mutex is put around all the drawing calls since it
//            is accessing information that is perhaps being 
//            altered by another thread.

void MarineViewer::draw()
{
  mutexLock();
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

  int   image_width  = m_back_img.get_img_width();
  int   image_height = m_back_img.get_img_height();
  double shape_width  = image_width * m_zoom;
  double shape_height = image_height * m_zoom;
  double m_xx = m_vshift_x * m_zoom;
  double m_yy = m_vshift_y * m_zoom;

  m_x_origin = -shape_width/2  + m_xx;
  m_y_origin = -shape_height/2 + m_yy;

  // Draw the background image if the tiff flag is set
  if(m_tiff_offon)
    drawTiff();

  // Draw the hash mark grid if the hash flag is set
  if(m_hash_offon)
    drawHash();

  // Draw the crosshairs if the tiff flag is set
  if(m_cross_offon)
    drawCrossHairs();

  if(m_op_area.viewable("datum")) {
    XYPoint point00;
    double vertex_size = m_op_area.getDatumSize();
    vector<double> vertex_color = m_op_area.getDatumColor();
    drawPoint(point00, vertex_size, vertex_color, vertex_color);
  }

  drawOpArea();
  drawMarkers();
  mutexUnLock();
}

// ----------------------------------------------------------
// Procedure: drawTiff
// Notes: No mutex is used here despite its accessing of data structures
//        written to by other threads. This is because this is a 
//        PRIVATE class function called only by a function which 
//        is using its own mutex.

void MarineViewer::drawTiff()
{
  if(m_back_img.get_img_data() == 0)
    return;

  setTexture();

  int   image_width  = m_back_img.get_img_width();
  int   image_height = m_back_img.get_img_height();
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

void MarineViewer::drawHash()
{
  double r = m_hash_shade;
  double g = m_hash_shade;
  double b = m_hash_shade;

  double hash_delta = getHashDelta();

  double xl = m_back_img.get_x_at_img_left();
  double xr = m_back_img.get_x_at_img_right();
  double yb = m_back_img.get_y_at_img_bottom();
  double yt = m_back_img.get_y_at_img_top();
  
  double xw = xr-xl;
  double yw = yt-yb;

  double xlow = snapToStep((xl-(xw/2)), hash_delta);
  double xhgh = snapToStep((xr+(xw/2)), hash_delta);
  double ylow = snapToStep((yb-(yw/2)), hash_delta);
  double yhgh = snapToStep((yt+(yw/2)), hash_delta);

  for(double i=xlow; i<xhgh; i+=hash_delta)
    drawSegment(i, ylow, i, yhgh, r, g, b);
  for(double j=ylow; j<yhgh; j+=hash_delta)
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

void MarineViewer::drawGLPoly(double *points, int numPoints, 
			      const vector<double>& cvect, 
			      double thickness, double scale)
{
  if(cvect.size() != 3)
    return;
  
  if(thickness<=0)
    glBegin(GL_POLYGON);
  else {
    glLineWidth(thickness);
    glBegin(GL_LINE_STRIP);
  }

  glColor3f(cvect[0], cvect[1], cvect[2]);
  for(int i=0; i<numPoints*2; i=i+2)
    glVertex2f(points[i]*scale, points[i+1]*scale);

  glEnd();
}

//-------------------------------------------------------------
// Procedure: drawCrossHairs

void MarineViewer::drawCrossHairs()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glLineWidth(1.0);
  glColor3f(0.757,0.757,0.757);

  // draw a grid
  glBegin(GL_LINES);
  
  // draw horizontal line
  glVertex2f(0, h()/2);
  glVertex2f(w(), h()/2);
  
  // draw vertical line
  glVertex2f(w()/2, 0);
  glVertex2f(w()/2, h());
  
  glEnd();
  glPopMatrix();
  glFlush();
}

//-------------------------------------------------------------
// Procedure: drawCommonVehicle

void MarineViewer::drawCommonVehicle(const string& vname, 
				     const ObjectPose& opose, 
				     const vector<double>& body_color,
				     const vector<double>& vname_color,
				     const string& vehibody, 
				     double shape_scale, 
				     bool  vname_draw, int outer_line)
{
  if(body_color.size() != 3)
    return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double xpos = 0;
  double ypos = 0;
  if(opose.isLatLonSet()) {
    double dlat = opose.getLat();
    double dlon = opose.getLon();
    //    m_geode
  }

  // Determine position in terms of image percentage
  double vehicle_ix = meters2img('x', opose.getX());
  double vehicle_iy = meters2img('y', opose.getY());

  // Determine position in terms of view percentage
  double vehicle_vx = img2view('x', vehicle_ix);
  double vehicle_vy = img2view('y', vehicle_iy);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(vehicle_vx, vehicle_vy, 0); // theses are in pixel units

  glScalef(m_zoom*shape_scale, m_zoom*shape_scale, m_zoom*shape_scale);

  glRotatef(-opose.getTheta(),0,0,1);  

  vector<double> black(3,0);
  vector<double> gray(3,0.5);

  if(vehibody == "kayak") {
    double cx = g_auvCtrX * g_auvScale;
    double cy = g_auvCtrY * g_auvScale;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_kayakBody, g_kayakBodySize, body_color, 0, g_kayakScale);    
    if(outer_line)
      drawGLPoly(g_kayakBody, g_kayakBodySize, black, outer_line, g_kayakScale);    
    drawGLPoly(g_kayakMidOpen, g_kayakMidOpenSize, gray, 0, g_kayakScale);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "auv") {
    vector<double> blue = colorParse("blue");
    double cx = g_auvCtrX * g_auvScale;
    double cy = g_auvCtrY * g_auvScale;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_auvBody, g_auvBodySize, body_color, 0, g_auvScale);
    if(outer_line > 0)
      drawGLPoly(g_auvBody, g_auvBodySize, black, outer_line, g_auvScale);
    drawGLPoly(g_propUnit, g_propUnitSize, blue, 0, g_auvScale);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "glider") {
    double cx = g_gliderCtrX * g_gliderScale;
    double cy = g_gliderCtrY * g_gliderScale;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_gliderWing, g_gliderWingSize, body_color,  0, g_gliderScale);
    drawGLPoly(g_gliderWing, g_gliderWingSize, black, 1, g_gliderScale);
    drawGLPoly(g_gliderBody, g_gliderBodySize, body_color,  0, g_gliderScale);
    drawGLPoly(g_gliderBody, g_gliderBodySize, black, 1, g_gliderScale);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "track") {  
      double cx = g_shipCtrX * g_shipScale;
      double cy = g_shipCtrY * g_shipScale;
      glTranslatef(-cx, -cy, 0);
      drawGLPoly(g_shipBody, g_shipBodySize, body_color, 0, g_shipScale);
      if(outer_line > 0)
	drawGLPoly(g_shipBody, g_shipBodySize, black, outer_line, g_shipScale);
      glTranslatef(cx, cy, 0);
    }
  else {  // vehibody == "ship" is the default
      vector<double> blue = colorParse("blue");
      double cx = g_shipCtrX * g_shipScale;
      double cy = g_shipCtrY * g_shipScale;
      glTranslatef(-cx, -cy, 0);
      drawGLPoly(g_shipBody, g_shipBodySize, body_color, 0, g_shipScale);
      if(outer_line > 0)
	drawGLPoly(g_shipBody, g_shipBodySize, blue, outer_line, g_shipScale);
      glTranslatef(cx, cy, 0);
  }

  if(vname_draw) {
    glColor3f(vname_color[0], vname_color[1], vname_color[2]);
    gl_font(1, 10);
    if(m_zoom > 4)
      gl_font(1, 12);
    double offset = 100.0;
    offset = offset * (1/m_zoom);

    int slen = vname.length();
    char *buff = new char[slen+1];
    glRasterPos3f(offset,offset,0);
    strncpy(buff, vname.c_str(), slen);
    buff[slen] = '\0';
    gl_draw(buff, slen);
    delete(buff);
  }

  glPopMatrix();
}


//-------------------------------------------------------------
// Procedure: drawCommonMarker

void MarineViewer::drawCommonMarker(double x, double y, double shape_scale, 
				    const string& mtype, const string& label, 
				    const vector<double>& label_color,
				    const vector<vector<double> >& color_vectors)
{
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

  double mz = sqrt(m_zoom);
  glScalef(mz*shape_scale, mz*shape_scale, mz*shape_scale);

  int vsize = color_vectors.size();

  vector<double> cvect1, cvect2;

  if(vsize >= 1) 
    cvect1 = color_vectors[0];
  if(vsize >= 2) 
    cvect2 = color_vectors[1];

  vector<double> black(3,0);
  if(mtype == "gateway") {
    if(cvect2.size() != 3) cvect2 = black;
    if(cvect1.size() != 3) cvect1 = colorParse("green");
    glTranslatef(-g_gatewayCtrX, -g_gatewayCtrY, 0);
    drawGLPoly(g_gatewayBody, g_gatewayBodySize, cvect1);    
    drawGLPoly(g_gatewayBody, g_gatewayBodySize, black, 1);    
    drawGLPoly(g_gatewayMidBody, g_gatewayMidBodySize, cvect2);
    glTranslatef(g_gatewayCtrX, g_gatewayCtrY, 0);
  }

  else if(mtype == "efield") {
    if(cvect2.size() != 3) cvect2 = colorParse("1.0, 0.843, 0.0");
    if(cvect1.size() != 3) cvect1 = black;
    glTranslatef(-g_efieldCtrX, -g_efieldCtrY, 0);
    drawGLPoly(g_efieldBody, g_efieldBodySize, cvect1);    
    drawGLPoly(g_efieldMidBody, g_efieldMidBodySize, cvect2);
    drawGLPoly(g_efieldMidBody, g_efieldMidBodySize, black, 1);
    glTranslatef(g_efieldCtrX, g_efieldCtrY, 0);
  }

  else if(mtype == "diamond") {
    if(cvect1.size() != 3) cvect1 = colorParse("orange");
    glTranslatef(-g_diamondCtrX, -g_diamondCtrY, 0);
    drawGLPoly(g_diamondBody, g_diamondBodySize, cvect1);    
    drawGLPoly(g_diamondBody, g_diamondBodySize, black, 1);    
    glTranslatef(g_diamondCtrX, g_diamondCtrY, 0);
  }

  else if(mtype == "triangle") {
    if(cvect1.size() != 3) cvect1 = colorParse("red");
    glTranslatef(-g_triangleCtrX, -g_triangleCtrY, 0);
    drawGLPoly(g_triangleBody, g_triangleBodySize, cvect1);    
    drawGLPoly(g_triangleBody, g_triangleBodySize, black, 1);    
    glTranslatef(g_triangleCtrX, g_triangleCtrY, 0);
  }

  else if(mtype == "square") {
    if(cvect1.size() != 3) cvect1 = colorParse("blue");
    glTranslatef(-g_squareCtrX, -g_squareCtrY, 0);
    drawGLPoly(g_squareBody, g_squareBodySize, cvect1);    
    drawGLPoly(g_squareBody, g_squareBodySize, black, 1);    
    glTranslatef(g_squareCtrX, g_squareCtrY, 0);
  }

  else if(mtype == "kelp") {
    if(cvect1.size() != 3) cvect1 = colorParse("0, 0.54, 0.54");
    glTranslatef(-g_kelpCtrX, -g_kelpCtrY, 0);
    drawGLPoly(g_kelpBody, g_kelpBodySize, cvect1);    
    drawGLPoly(g_kelpBody, g_kelpBodySize, black, 1);    
    glTranslatef(g_kelpCtrX, g_kelpCtrY, 0);
  }

  if(label != "") {
    glColor3f(label_color[0], label_color[1], label_color[2]);
    gl_font(1, 10);
    if(m_zoom > 4)
      gl_font(1, 12);
    double offset = 5.0;
    offset = offset * (1/m_zoom);

    int slen = label.length();
    char *buff = new char[slen+1];
    //glRasterPos3f(offset, offset, 0);
    glRasterPos3f(offset, offset, 0);
    strncpy(buff, label.c_str(), slen);
    buff[slen] = '\0';
    gl_draw(buff, slen);
    delete(buff);
  }

  glPopMatrix();
}


//-------------------------------------------------------------
// Procedure: drawMarkers

void MarineViewer::drawMarkers()
{
  if(m_vmarkers.viewable() == false)
    return;

  double gscale = m_vmarkers.getMarkerGScale(); 

  vector<vector<double> > color_vectors;
  vector<double> label_color = m_vmarkers.getLabelColor();
  
  unsigned int vsize = m_vmarkers.size();
  for(unsigned int i=0; i<vsize; i++) {
    string mtype = m_vmarkers.getMarkerType(i);
    string label;
    if(m_vmarkers.viewable("labels"))
      label = m_vmarkers.getMarkerLabel(i);
    double xpos  = m_vmarkers.getMarkerXPos(i);
    double ypos  = m_vmarkers.getMarkerYPos(i);
    double scale = m_vmarkers.getMarkerScale(i) * gscale;
    color_vectors = m_vmarkers.getMarkerColorVectors(i);
    drawCommonMarker(xpos, ypos, scale, mtype, label, label_color, color_vectors);
  }
}

//-------------------------------------------------------------
// Procedure: drawOpArea

void MarineViewer::drawOpArea()
{
  if(m_op_area.viewable() == false)
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
  unsigned int asize = m_op_area.size();

  double line_shade  = m_op_area.getLineShade();
  double label_shade = m_op_area.getLabelShade();

  while(index < asize) {
    string group  = m_op_area.getGroup(index);
    double lwidth = m_op_area.getLWidth(index);
    bool   dashed = m_op_area.getDashed(index);
    bool   looped = m_op_area.getLooped(index);

    vector<double> lcolor = m_op_area.getLColor(index);
    vector<double> vcolor = m_op_area.getVColor(index);

    vector<double> xpos, ypos;
    vector<string> labels;

    bool done = false;
    while(!done) {
      double x = m_op_area.getXPos(index);
      double y = m_op_area.getYPos(index);
      string label = m_op_area.getLabel(index);

      index++;
      if((index >= asize) || (group != m_op_area.getGroup(index)))
	done = true;
      xpos.push_back(x);
      ypos.push_back(y);
      labels.push_back(label);
    }
    
    int vsize = xpos.size();
    double pix_per_mtr = m_back_img.get_pix_per_mtr();
    for(int i=0; i<vsize; i++) {
      xpos[i] *= pix_per_mtr;
      ypos[i] *= pix_per_mtr;
    }

    // Draw the edges 
    glLineWidth(lwidth);
    glColor3f(lcolor[0]*line_shade, lcolor[1]*line_shade, 
	      lcolor[2]*line_shade);
    
    if(looped)
      glBegin(GL_LINE_LOOP);
    else
      glBegin(GL_LINE_STRIP);
    for(int j=0; j<vsize; j++) {
      glVertex2f(xpos[j],  ypos[j]);
    }
    glEnd();

    if(m_op_area.viewable("labels")) {
      glColor3f(lcolor[0], lcolor[1], lcolor[2]);
      gl_font(1, 10);
      for(int k=0; k<vsize; k++) {
	int slen = labels[k].length();
	char *buff = new char[slen+1];
	// +3 below is a draw offset
	glRasterPos3f(xpos[k]+3, ypos[k]+3, 0);
	strncpy(buff, labels[k].c_str(), slen);
	buff[slen] = '\0';
	gl_draw(buff, slen);
	delete(buff);
      }
    }
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

void MarineViewer::drawPolygons()
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the option "true" argument means return
  // true if nothing is known about the parameter.
  if(m_geoshapes.viewable("polygon_viewable_all", true) == false)
    return;

  int vsize = m_geoshapes.sizePolygons();
  if(vsize == 0)
    return;

  vector<double> edge_c, fill_c, vert_c, labl_c;
  edge_c = m_geoshapes.geocolor("polygon_edge_color", "aqua");
  fill_c = m_geoshapes.geocolor("polygon_fill_color", "dark_green");
  vert_c = m_geoshapes.geocolor("polygon_vertex_color", "red");
  labl_c = m_geoshapes.geocolor("polygon_label_color", "white");

  double line_width  = m_geoshapes.geosize("polygon_line_size");
  double vertex_size = m_geoshapes.geosize("polygon_vertex_size");
  
  for(int i=0; i<vsize; i++) {
    XYPolygon poly = m_geoshapes.getPolygon(i);
    drawPolygon(poly, false, false, line_width, vertex_size, 
		edge_c, fill_c, vert_c, labl_c);
  }
}

//-------------------------------------------------------------
// Procedure: drawPoly

void MarineViewer::drawPolygon(const XYPolygon& poly, 
			       bool filled, bool dashed,
			       double line_width, double vertex_size,
			       const vector<double>& edge_c, 
			       const vector<double>& fill_c, 
			       const vector<double>& vert_c, 
			       const vector<double>& labl_c)
{
  unsigned int vsize = poly.size();
  if(vsize < 1)
    return;

  unsigned int i, j;
  double *points = new double[2*vsize];
  
  double pix_per_mtr = m_back_img.get_pix_per_mtr();
  int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = poly.get_vx(i) * pix_per_mtr;
    points[pindex+1] = poly.get_vy(i) * pix_per_mtr;
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
  if((vsize > 2) && poly.is_convex() && filled) {
    glEnable(GL_BLEND);
    glColor4f(fill_c[0], fill_c[1], fill_c[2], 0.1);
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
  if(vsize > 1) {
    glLineWidth(line_width);
    if(dashed) {
      glEnable(GL_LINE_STIPPLE);
      GLushort pattern = 0x5555;
      GLint factor = 5;
      glLineStipple(factor, pattern);
    }

    glColor3f(edge_c[0], edge_c[1], edge_c[2]);

    if(poly.is_convex())
      glBegin(GL_LINE_LOOP);
    else
      glBegin(GL_LINE_STRIP);
    for(i=0; i<vsize*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
    if(dashed)
      glDisable(GL_LINE_STIPPLE);
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

    // Draw the vertices in between the first and last ones
    glColor3f(vert_c[0], vert_c[1], vert_c[2]);
    glBegin(GL_POINTS);
    for(j=0; j<vsize; j++) {
      glVertex2f(points[(j*2)], points[(j*2)+1]);
    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
  }


  //-------------------------------- perhaps draw poly label
  if(m_geoshapes.viewable("polygon_viewable_labels")) {
    double cx = poly.get_avg_x() * m_back_img.get_pix_per_mtr();
    double cy = poly.get_avg_y() * m_back_img.get_pix_per_mtr();
    glTranslatef(cx, cy, 0);
    
    glColor3f(labl_c[0], labl_c[1], labl_c[2]);
    gl_font(1, 10);
    string plabel = poly.get_label();
    int slen = plabel.length();
    char *buff = new char[slen+1];
    glRasterPos3f(0, 0, 0);
    strncpy(buff, plabel.c_str(), slen);
    buff[slen] = '\0';
    gl_draw(buff, slen);
    delete(buff);
  }
  //-------------------------------- perhaps draw poly label


#if 0
  //-------------------------------- perhaps draw poly vertex labels
  if(m_geoshapes.viewable("polygon_viewable_vertex_labels") {
    glTranslatef(0, 0, 0);
    char *buff = new char[100];
    for(j=0; j<vsize; j++) {
      double cx = points[(j*2)];
      double cy = points[(j*2)+1];
      
      glColor3f(edge_c[0], edge_c[1], edge_c[2]);
      gl_font(1, 10);
      
      string vlabel = intToString(j);
      int slen = vlabel.length();
      glRasterPos3f(cx, cy, 0);
      strncpy(buff, vlabel.c_str(), slen);
      buff[slen] = '\0';
      gl_draw(buff, slen);
    }
    delete(buff);
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
  double pix_per_mtr = m_back_img.get_pix_per_mtr();

  x1 *= pix_per_mtr;
  y1 *= pix_per_mtr;

  x2 *= pix_per_mtr;
  y2 *= pix_per_mtr;

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

void MarineViewer::drawSegLists()
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the option "true" argument means return
  // true if nothing is known about the parameter.
  if(!m_geoshapes.viewable("seglist_viewable_all"))
    return;

  int vsize = m_geoshapes.sizeSegLists();
  if(vsize == 0)
    return;
 
  vector<double> edge_c, vert_c, labl_c;
  edge_c = m_geoshapes.geocolor("seglist_edge_color", "yellow");
  vert_c = m_geoshapes.geocolor("seglist_vertex_color", "white");
  labl_c = m_geoshapes.geocolor("seglist_label_color", "white");
  
  double lwid = m_geoshapes.geosize("seglist_edge_width", 1);
  double vert = m_geoshapes.geosize("seglist_vertex_size", 2);
  
  for(int i=0; i<vsize; i++) {
    XYSegList segl = m_geoshapes.getSegList(i);
    drawSegList(segl, lwid, vert, false, edge_c, vert_c, labl_c); 
  }
}

//-------------------------------------------------------------
// Procedure: drawSegList

void MarineViewer::drawSegList(const XYSegList& segl, double lwid, 
			       double vertex_size, bool z_dash,
			       const vector<double>& edge_c,
			       const vector<double>& vert_c,
			       const vector<double>& labl_c)
{
  unsigned int vsize = segl.size();

  unsigned int i, j;
  double *points = new double[2*vsize];

  double pix_per_mtr = m_back_img.get_pix_per_mtr();
  unsigned int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = segl.get_vx(i) * pix_per_mtr;
    points[pindex+1] = segl.get_vy(i) * pix_per_mtr;
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
  if(vsize >= 2) {
    glLineWidth(lwid);
    glColor3f(edge_c[0], edge_c[1], edge_c[2]);

    glBegin(GL_LINE_STRIP);
    for(i=0; i<vsize*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
  }

  // If the seglist is just a single point, draw it bigger (x 1.4)
  if(vsize==1) {
    glEnable(GL_POINT_SMOOTH);
    glPointSize((vertex_size * 1.4) * m_zoom);
    // Draw the vertices with color coding for the first and last
    
    glColor3f(vert_c[0], vert_c[1], vert_c[2]);
    glBegin(GL_POINTS);
    glVertex2f(points[0], points[1]);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
  }
  else {
    if(vertex_size > 0) {
      glPointSize(vertex_size * sqrt(m_zoom));
      // Draw the vertices in between the first and last ones
      glColor3f(vert_c[0], vert_c[1], vert_c[2]);
      glEnable(GL_POINT_SMOOTH);
      glBegin(GL_POINTS);
      for(j=0; j<vsize; j++) {
	glVertex2f(points[(j*2)], points[(j*2)+1]);
      }
      glEnd();
      glDisable(GL_POINT_SMOOTH);
    }
  }

  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawPointList

void MarineViewer::drawPointList(const vector<double>& xvect,
				 const vector<double>& yvect,
				 double vertsize, 
				 const vector<double>& vert_c,
				 bool  point_edges_viewable)
{
  unsigned int vsize = xvect.size();
  if(yvect.size() != xvect.size())
    return;

  unsigned int i, j;
  double *points = new double[2*vsize];

  double pix_per_mtr = m_back_img.get_pix_per_mtr();
  unsigned int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = xvect[i] * pix_per_mtr;
    points[pindex+1] = yvect[i] * pix_per_mtr;
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

  if(point_edges_viewable) {
    glLineWidth(1.0);
    glColor3f(vert_c[0], vert_c[1], vert_c[2]);
    glBegin(GL_LINE_STRIP);
    for(i=0; i<vsize*2; i=i+2)
      glVertex2f(points[i], points[i+1]);
    glEnd();
  }

  glPointSize(vertsize * sqrt(m_zoom));

  // Draw the vertices in between the first and last ones
  glColor3f(vert_c[0], vert_c[1], vert_c[2]);
  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  for(j=0; j<vsize; j++) {
    glVertex2f(points[(j*2)], points[(j*2)+1]);
  }
  glEnd();
  glDisable(GL_POINT_SMOOTH);
  
  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawHexagons()

void MarineViewer::drawHexagons()
{
  int hsize = m_geoshapes.sizeHexagons();

  vector<double> edge_c, fill_c, vert_c, labl_c;
  edge_c = m_geoshapes.geocolor("polygon_edge_color", "yellow");
  fill_c = m_geoshapes.geocolor("polygon_vertex_color", "white");
  vert_c = m_geoshapes.geocolor("polygon_vertex_color", "white");
  labl_c = m_geoshapes.geocolor("polygon_label_color", "white");

  double line_width  = m_geoshapes.geosize("polygon_line_size");
  double vertex_size = m_geoshapes.geosize("polygon_vertex_size");
  
  for(int i=0; i<hsize; i++)
    drawPolygon(m_geoshapes.getHexagon(i), false, false, line_width, 
	     vertex_size, edge_c, fill_c, vert_c, labl_c);
}

//-------------------------------------------------------------
// Procedure: drawGrids

void MarineViewer::drawGrids()
{
  if(m_geoshapes.viewable("grid_viewable_all", true) == false)
    return;

  int vsize = m_geoshapes.sizeGrids();
  if(vsize == 0)
    return;

  for(int i=0; i<vsize; i++)
    drawGrid(m_geoshapes.grid(i));
}

//-------------------------------------------------------------
// Procedure: drawGrid

void MarineViewer::drawGrid(const XYGrid& grid)
{
  FColorMap cmap;

  int gsize = grid.size();
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

  int   i;
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
// Procedure: drawCircles

void MarineViewer::drawCircles()
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the option "true" argument means return
  // true if nothing is known about the parameter.
  if(m_geoshapes.viewable("circle_viewable_all", true) == false)
    return;

  int vsize = m_geoshapes.sizeCircles();
  if(vsize == 0)
    return;

  vector<double> edge_c = colorParse("blue");
  vector<double> fill_c = colorParse("dark_blue");
  vector<double> vert_c = colorParse("blue");
  vector<double> labl_c = colorParse("white");
  
  for(int i=0; i<vsize; i++)
    drawCircle(m_geoshapes.circ(i), 16, true, 
	       edge_c, fill_c, vert_c, labl_c);
}

//-------------------------------------------------------------
// Procedure: drawCircle

void MarineViewer::drawCircle(const XYCircle& circle, int pts, bool filled,
			      const vector<double>& edge_c,
			      const vector<double>& fill_c,
			      const vector<double>& vert_c,
			      const vector<double>& labl_c)
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
  poly_str += intToString(pts);
  
  XYPolygon poly = stringToPoly(poly_str);

  // Now set points to the actual size vs. the requested size
  unsigned int actual_pts = poly.size();

  if(actual_pts == 0)
    return;

  unsigned int i;
  double *points = new double[2 * actual_pts];

  double pix_per_mtr = m_back_img.get_pix_per_mtr();
  unsigned int pindex = 0;
  for(i=0; i<actual_pts; i++) {
    points[pindex]   = poly.get_vx(i) * pix_per_mtr;
    points[pindex+1] = poly.get_vy(i) * pix_per_mtr;
    pindex += 2;
  }

  glColor3f(edge_c[0], edge_c[1], edge_c[2]);
  glBegin(GL_LINE_LOOP);
  for(i=0; i<actual_pts*2; i=i+2) {
    glVertex2f(points[i], points[i+1]);
  }
  glEnd();
  
  // If filled option is on, draw the interior of the circle
  if(filled) {
    glEnable(GL_BLEND);
    glColor4f(fill_c[0], fill_c[1], fill_c[2], 0.1);
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
// Procedure: drawPoints

void MarineViewer::drawPoints()
{
  // If the viewable parameter is set to false just return. In 
  // querying the parameter the option "true" argument means return
  // true if nothing is known about the parameter.
  if(m_geoshapes.viewable("point_viewable_all", true) == false)
    return;

  // If no points are present just return.
  int vsize = m_geoshapes.sizePoints();

  if(vsize == 0)
    return;

  // The second argument to geocolor is what is returned if no color
  // mapping is present in the geoshapes data structure.
  vector<double> vert_c, labl_c;
  vert_c = m_geoshapes.geocolor("point_vertex_color", "red");
  labl_c = m_geoshapes.geocolor("point_label_color", "aqua_marine");

  // The second argument to geosize is what is returned if no size
  // mapping is present in the geoshapes data structure.
  double vertex_size = m_geoshapes.geosize("point_vertex_size", 3);
  
  for(int i=0; i<vsize; i++) {
    XYPoint point = m_geoshapes.point(i);
    if((point.get_size() > 0) && (point.active()))
      drawPoint(point, vertex_size, vert_c, labl_c);
  }
}


//-------------------------------------------------------------
// Procedure: drawPoint

void MarineViewer::drawPoint(const XYPoint& point, double vertex_size,
			     const vector<double>& vert_c,
			     const vector<double>& labl_c)
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

  double px  = point.get_vx() * m_back_img.get_pix_per_mtr();
  double py  = point.get_vy() * m_back_img.get_pix_per_mtr();

  glPointSize(vertex_size * sqrt(m_zoom));
  glColor3f(vert_c[0], vert_c[1], vert_c[2]); 
  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  glVertex2f(px, py);
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  // Now draw the point labels if turned on
  if(m_geoshapes.viewable("point_viewable_labels")) {
    glColor3f(labl_c[0], labl_c[1], labl_c[2]);
    gl_font(1, 10);
    string plabel = point.get_label();
    int slen = plabel.length();
    char *buff = new char[slen+1];
    glRasterPos3f(px+1, py+1, 0);
    strncpy(buff, plabel.c_str(), slen);
    buff[slen] = '\0';
    gl_draw(buff, slen);
    delete(buff);
  }

  glFlush();
  glPopMatrix();
}

