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
#include <math.h>
#include <string.h>
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


using namespace std;

MarineViewer::MarineViewer(int x, int y, int w, int h, const char *l)
  : Fl_Gl_Window(x,y,w,h,l)
{
  m_zoom        = 1.0;
  m_vshift_x    = 0;
  m_vshift_y    = 0;
  m_x_origin    = 0;
  m_y_origin    = 0;
  m_shape_scale = 0.12;
  m_hash_shade  = 0.65;
  m_hash_delta  = 100;
  m_fill_shade  = 0.7;
  m_texture_set = 0;
  m_texture_init = false;
  m_textures    = new GLuint[1];

  m_trails      = true;
  m_trail_connect = false;
  m_trail_gap   = 1;
  m_trail_size  = 0.1;
  m_cross_offon = false;
  m_tiff_offon  = true;
  m_hash_offon  = false;
  m_draw_vname  = false;
  m_draw_datum  = false;
  m_size_datum  = 3.0;
  m_global_ix   = 0;
  m_vname_color = 0;

  m_back_img_b_ok = false;
  m_back_img_b_on = false;
  m_back_img_mod  = false;
//  glGenTextures(1, m_textures);
}

// ----------------------------------------------------------
// Procedure: readTiff
//   Purpose: This routine reads in a tiff file and stores it 
//            in a very simple data structure

bool MarineViewer::readTiff(string filename)
{
  return(m_back_img.readTiff(filename));
}

// ----------------------------------------------------------
// Procedure: readTiffB
//   Purpose: This routine reads in a tiff file and stores it 
//            in a very simple data structure

bool MarineViewer::readTiffB(string filename)
{
  bool ok = m_back_img_b.readTiff(filename);
  if(ok)
    m_back_img_b_ok = true;

  return(ok);
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

float MarineViewer::img2view(char xy, float img_val)

{
  float view_val = 0.0;

  float adj_img_width  = m_back_img.get_img_width()  * m_zoom;
  float adj_img_height = m_back_img.get_img_height() * m_zoom;
  
  if(xy == 'x') {
    float adj_img_pix_x = img_val * adj_img_width; 
    view_val = adj_img_pix_x + m_x_origin + w()/2;
  }
  if(xy == 'y') {
    float adj_img_pix_y = img_val * adj_img_height; 
    view_val = adj_img_pix_y + m_y_origin + h()/2;
  }
  return(view_val);
}

//-------------------------------------------------------------
// Procedure: view2img
//      Note: Derived as from img2view above

float MarineViewer::view2img(char xy, float view_val)

{
  float img_val = 0.0;

  float adj_img_width  = m_back_img.get_img_width()  * m_zoom;
  float adj_img_height = m_back_img.get_img_height() * m_zoom;

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

float MarineViewer::meters2img(char xy, float meters_val)
{
  float img_val = 0.0;
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

float MarineViewer::img2meters(char xy, float img_val)
{
  float meters_val = 0.0;
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

float MarineViewer::getCrossHairMeters(char xy)
{
  if(xy == 'x') {
    int iwidth = m_back_img.get_img_width();
    float x_pos = ((float)(iwidth) / 2.0) - (float)(m_vshift_x);
    float x_pct = m_back_img.pixToPctX(x_pos);
    float x_pct_cent = m_back_img.get_img_centx();
    float x_pct_mtrs = m_back_img.get_img_meters();
    float meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
    return(meters);   
  }
  else if (xy == 'y') {
    int iheight = m_back_img.get_img_height();
    float y_pos = ((float)(iheight) / 2.0) - (float)(m_vshift_y);
    float y_pct = m_back_img.pixToPctY(y_pos);
    float y_pct_cent = m_back_img.get_img_centy();
    float y_pct_mtrs = m_back_img.get_img_meters();
    float meters = (y_pct - y_pct_cent) / (y_pct_mtrs / 100.0);
    return(meters);
  }
  else
    return(0);
}


// ----------------------------------------------------------
// Procedure: draw
//   Purpose: 

void MarineViewer::draw()
{
  float r = m_fill_shade;
  float g = m_fill_shade;
  float b = m_fill_shade + 0.1;
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
  float shape_width  = image_width * m_zoom;
  float shape_height = image_height * m_zoom;
  float m_xx = m_vshift_x * m_zoom;
  float m_yy = m_vshift_y * m_zoom;

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

  // Draw the datum
  if(m_draw_datum)
    drawDatum();

  drawOpArea();
  drawMarkers();
}

// ----------------------------------------------------------
// Procedure: drawTiff
//   Purpose: 

void MarineViewer::drawTiff()
{
  if(m_back_img.get_img_data() == 0)
    return;

  setTexture();

  int   image_width  = m_back_img.get_img_width();
  int   image_height = m_back_img.get_img_height();
  float shape_width  = image_width * m_zoom;
  float shape_height = image_height * m_zoom;

  float lower_x = m_x_origin;
  float upper_x = m_x_origin + shape_width;
  float lower_y = m_y_origin;
  float upper_y = m_y_origin + shape_height;

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
  float r = m_hash_shade;
  float g = m_hash_shade;
  float b = m_hash_shade;

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
// Procedure: drawPolygons

void MarineViewer::drawPolygons()
{
  if(m_geoshapes.viewable("polygon_viewable_all", true) == false)
    return;

  int vsize = m_geoshapes.sizePolygons();
  if(vsize == 0)
    return;

  vector<double> edge_c, fill_c, vert_c, labl_c;
  edge_c = m_geoshapes.geocolor("polygon_edge_color", "khaki");
  fill_c = m_geoshapes.geocolor("polygon_fill_color", "dark_green");
  vert_c = m_geoshapes.geocolor("polygon_vertex_color", "red");
  labl_c = m_geoshapes.geocolor("polygon_label_color", "white");

  float line_width  = m_geoshapes.geosize("polygon_line_size");
  float vertex_size = m_geoshapes.geosize("polygon_vertex_size");
  
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
			       float line_width, float vertex_size,
			       const vector<double>& edge_c, 
			       const vector<double>& fill_c, 
			       const vector<double>& vert_c, 
			       const vector<double>& labl_c)
{
  unsigned int vsize = poly.size();
  if(vsize < 1)
    return;

  unsigned int i, j;
  float *points = new float[2*vsize];

  int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = poly.get_vx(i);
    points[pindex+1] = poly.get_vy(i);

    points[pindex]   *=  m_back_img.get_pix_per_mtr();
    points[pindex+1] *=  m_back_img.get_pix_per_mtr();

    pindex++;
    pindex++;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

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
    glPointSize(2.0 * m_zoom);
    // Draw the vertices with color coding for the first and last
    
    //glColor3f(0.7,0.13,0.13);  // Firebrick red b2 22 22
    glColor3f(0.13, 0.13, 0.7);  // Blueish
    glBegin(GL_POINTS);
    glVertex2f(points[0], points[1]);
    glEnd();
  }

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



  //-------------------------------- perhaps draw poly label
  if(m_geoshapes.viewable("polygon_viewable_labels")) {
    double cx = poly.get_avg_x() * m_back_img.get_pix_per_mtr();
    double cy = poly.get_avg_y() * m_back_img.get_pix_per_mtr();
    glTranslatef(cx, cy, 0);
    
    glColor3f(labl_c[0], labl_c[1], labl_c[2]);
    gl_font(1, 12);

    string plabel = poly.get_label();
    int slen = plabel.length();
    char *buff = new char(slen+1);
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
    char *buff = new char(100);
    for(j=0; j<vsize; j++) {
      double cx = points[(j*2)];
      double cy = points[(j*2)+1];
      
      glColor3f(edge_c[0], edge_c[1], edge_c[2]);
      gl_font(1, 12);
      
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

void MarineViewer::drawSegment(float x1, float y1, float x2, float y2, 
			       float red, float grn, float blu)
{
  float pix_per_mtr = m_back_img.get_pix_per_mtr();

  x1 *= pix_per_mtr;
  y1 *= pix_per_mtr;

  x2 *= pix_per_mtr;
  y2 *= pix_per_mtr;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

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
// Procedure: drawHexagons()

void MarineViewer::drawHexagons()
{
  int hsize = m_geoshapes.sizeHexagons();

  vector<double> edge_c, fill_c, vert_c, labl_c;
  edge_c = m_geoshapes.geocolor("polygon_edge_color", "yellow");
  fill_c = m_geoshapes.geocolor("polygon_vertex_color", "white");
  vert_c = m_geoshapes.geocolor("polygon_vertex_color", "white");
  labl_c = m_geoshapes.geocolor("polygon_label_color", "white");

  float line_width  = m_geoshapes.geosize("polygon_line_size");
  float vertex_size = m_geoshapes.geosize("polygon_vertex_size");
  
  for(int i=0; i<hsize; i++)
    drawPolygon(m_geoshapes.getHexagon(i), false, false, line_width, 
	     vertex_size, edge_c, fill_c, vert_c, labl_c);
}

//-------------------------------------------------------------
// Procedure: drawSegLists()

void MarineViewer::drawSegLists()
{
  if(!m_geoshapes.viewable("seglist_viewable_all"))
    return;

  int vsize = m_geoshapes.sizeSegLists();
  if(vsize == 0)
    return;
 
  vector<double> edge_c, vert_c, labl_c;
  edge_c = m_geoshapes.geocolor("seglist_edge_color", "yellow");
  vert_c = m_geoshapes.geocolor("seglist_vertex_color", "white");
  labl_c = m_geoshapes.geocolor("seglist_label_color", "white");
  
  float lwid = m_geoshapes.geosize("seglist_line_size");
  float vert = m_geoshapes.geosize("seglist_vertex_size");
  
  for(int i=0; i<vsize; i++) {
    XYSegList segl = m_geoshapes.getSegList(i);
    drawSegList(segl, lwid, false, vert, edge_c, vert_c, labl_c); 
  }
}

//-------------------------------------------------------------
// Procedure: drawSegList

void MarineViewer::drawSegList(const XYSegList& segl, float lwid, 
			       float vert, bool z_dash,
			       const vector<double>& edge_c,
			       const vector<double>& vert_c,
			       const vector<double>& labl_c)
{
  unsigned int vsize = segl.size();

  unsigned int i, j;
  float *points = new float[2*vsize];

  unsigned int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = segl.get_vx(i);
    points[pindex+1] = segl.get_vy(i);

    points[pindex]   *=  m_back_img.get_pix_per_mtr();
    points[pindex+1] *=  m_back_img.get_pix_per_mtr();

    pindex++;
    pindex++;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  // Now draw the edges
  if(vsize >= 2) {
    glLineWidth(2.0);
    glColor3f(edge_c[0], edge_c[1], edge_c[2]);

    glBegin(GL_LINE_STRIP);
    for(i=0; i<vsize*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
  }

  // If the seglist is just a single point, draw it big!
  if(vsize==1) {
    glPointSize(6.0 * m_zoom);
    // Draw the vertices with color coding for the first and last
    
    //glColor3f(0.7,0.13,0.13);  // Firebrick red b2 22 22
    glColor3f(0.13, 0.13, 0.7);  // Blueish
    glBegin(GL_POINTS);
    glVertex2f(points[0], points[1]);
    glEnd();
  }

  //glPointSize(1.0 * m_zoom);
  glPointSize(6.0);

  // Draw the vertices in between the first and last ones
  glColor3f(vert_c[0], vert_c[1], vert_c[2]);
  glBegin(GL_POINTS);
  for(j=0; j<vsize; j++) {
    glVertex2f(points[(j*2)], points[(j*2)+1]);
  }
  glEnd();
  
  delete [] points;
  glFlush();
  glPopMatrix();
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

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glLineWidth(1.0);  // added dec1306
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  int   i;
  float px[4];
  float py[4];

  double min_eval = grid.getMinUtilPoss();
  double max_eval = grid.getMaxUtilPoss();

  for(i=0; i<gsize; i++) {
    XYSquare element = grid.getElement(i);
    double   eval = grid.getUtil(i);
    double   pct  = (eval-min_eval)/(max_eval-min_eval);

    //cout << "[" << pct << "]";
    //pct = snapToStep(pct, 0.25);
    
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
  if(m_geoshapes.viewable("circle_viewable_all", true) == false)
    return;

  int vsize = m_geoshapes.sizeCircles();
  if(vsize == 0)
    return;

  for(int i=0; i<vsize; i++)
    drawCircle(m_geoshapes.circ(i));
}

//-------------------------------------------------------------
// Procedure: drawDatum

void MarineViewer::drawDatum()
{
  if(!m_draw_datum)
    return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  vector<double> cvect = getColorMapping("datum_color", "0.0, 1.0, 0.3");

  // Draw the vertices in between the first and last ones
  glEnable(GL_POINT_SMOOTH);
  glPointSize(m_size_datum * m_zoom);
  glColor3f(cvect[0], cvect[1], cvect[2]);
  glBegin(GL_POINTS);
  glVertex2f(0, 0);
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawCircle

void MarineViewer::drawCircle(const XYCircle& dcircle)
{
  string dlabel = dcircle.getLabel();
  double rad = dcircle.getRad();
  if(rad <= 0)
    return;
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glLineWidth(1.0);  // added dec1306
  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  double px  = dcircle.getX();
  double py  = dcircle.getY();
  px *=  m_back_img.get_pix_per_mtr();
  py *=  m_back_img.get_pix_per_mtr();

  vector<double> cvect = getColorMapping(dlabel, "yellow");

  // Draw the vertices in between the first and last ones
  glEnable(GL_POINT_SMOOTH);
  glPointSize(1+rad);
  glColor3f(cvect[0], cvect[1], cvect[2]);
  glBegin(GL_POINTS);
  glVertex2f(px, py);
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: setCommonParam

bool MarineViewer::setCommonParam(string param, string value)
{
  string p = tolower(stripBlankEnds(param));
  string v = tolower(stripBlankEnds(value));

  if(p=="datum_color")
    return(setColorMapping("datum_color", v));
  else if(p=="active_vcolor")
    return(setColorMapping("active_vcolor", v));
  else if(p=="inactive_vcolor")
    return(setColorMapping("inactive_vcolor", v));
  else if(p=="cross_view")
    return(setBooleanOnString(m_cross_offon, v));
  else if(p=="tiff_type") {
    m_back_img_mod = true;
    return(setBooleanOnString(m_back_img_b_on, v));
  }
  else if(p=="tiff_view") 
    return(setBooleanOnString(m_tiff_offon, v));
  else if(p=="hash_view")
    return(setBooleanOnString(m_hash_offon, v));
  else if(p=="trail_view")
    return(setBooleanOnString(m_trails, v));
  else if(p=="trail_connect")
    return(setBooleanOnString(m_trail_connect, v));
  else if(p=="display_vname")
    return(setBooleanOnString(m_draw_vname, v));
  else if(p=="vname_color") {
    if(v == "toggle")
      m_vname_color = (m_vname_color+1) % 3;
  }
  else if(p=="display_datum")
    return(setBooleanOnString(m_draw_datum, v));

  else if(p=="draw_marker_labels")
    m_vmarkers.setParam("viewable_labels", "toggle");
  else if(p=="marker_label_color")
    m_vmarkers.setParam("label_color", v);
  else if(p=="draw_markers")
    m_vmarkers.setParam("viewable_all", v);
  else if(p=="marker")
    m_vmarkers.addVMarker(v, m_geodesy);

  else if(p=="geodesy_init")
    initGeodesy(v);

  else if(p=="op_vertex")
    m_op_area.addVertex(v, m_geodesy);
  else if(p=="op_area_draw")
    m_op_area.setParam("viewable_all", v);
  else if(p=="op_area_labels")
    m_op_area.setParam("viewable_labels", v);
  else if(p=="op_area_config")
    m_op_area.setParam("config", v);
  else if(p=="op_area_shade")
    m_op_area.setParam("config", v);

  else if(p=="marker_scale_all") {
    if(v == "smaller")
      m_vmarkers.setParam("mod_scale_all", 0.8);
    else if(v == "bigger")
      m_vmarkers.setParam("mod_scale_all", 1.2);
    else if(v == "reset")
      m_vmarkers.setParam("set_scale_all", 1.0);
    else
      return(false);
  }

  else if(p=="zoom") {
    if(v == "reset")
      m_zoom = 1.0;
    else
      return(false);
  }
  else if(p=="clear") {
    if(v == "polygons")
      m_geoshapes.clearPolygons();
    else if(v == "seglists")
      m_geoshapes.clearSegLists();
    else if(v == "grids")
      m_geoshapes.clearGrids();
    else if(v == "circles")
      m_geoshapes.clearCircles();
    else
      return(false);
  }
  else if(p=="trail_color") {
    if(v == "toggle")
      m_trail_color = (m_trail_color+1) % 3;
    else
      return(false);
  }
  else
    return(false);

  redraw();
  return(true);

}

//-------------------------------------------------------------
// Procedure: setCommonParam

bool MarineViewer::setCommonParam(string param, float v)
{
  param = tolower(stripBlankEnds(param));
  
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
  else if(param == "trail_size") {
    m_trail_size += v;
    if(m_trail_size <= 0)
      m_trail_size = 0.05;
  }
  else if(param == "trail_gap") {
    if(m_trail_gap+v >= 1)      
      m_trail_gap += (int)v;
  }
  else if(param == "shape_scale") {
    if(m_shape_scale*v > 0.01)      
      m_shape_scale *= v;
  }
  else if(param == "datum_size") {
    if((v >= 1) && (v <= 50))
      m_size_datum = v;
  }
  else if(param == "datum_size_add") {
    m_size_datum += v;
    if(m_size_datum < 0)
      m_size_datum = 0;
  }
  else if(param == "zoom") {
    if(m_zoom*v > 0.05)      
      m_zoom *= v;
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
    return(false);
  
  redraw();
  return(true);

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
// Procedure: hasColorMapping

bool MarineViewer::hasColorMapping(string attribute)
{
  map<string, vector<double> >::iterator p;
  p = m_color_map.find(attribute);
  if(p != m_color_map.end())
    return(true);
  else
    return(false);
}

//-------------------------------------------------------------
// Procedure: getColorMapping
//      Note: If the attribute is not found, the returned color
//            vector can be determined by the optional def_color
//            argument. 

vector<double> MarineViewer::getColorMapping(string attribute, 
					     string def_color)
{
  map<string, vector<double> >::iterator p;
  p = m_color_map.find(attribute);
  if(p != m_color_map.end())
    return(p->second);
  else {
    vector<double> color_vector = colorParse(def_color);
    return(color_vector);
  }
}

//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label", "DarkKhaki"
//            "label"  " hex, bd, b7, 6b"
//            "label", "0.741, 0.718, 0.420"

bool MarineViewer::setColorMapping(string attribute, string color_str)
{
  attribute = stripBlankEnds(attribute);
  color_str = stripBlankEnds(color_str);
  
  vector<double> cvect = colorParse(color_str);
  
  m_color_map[attribute] = cvect;
  if((cvect[0]==0) && (cvect[2]==0) && (cvect[2]==0) &&
     (tolower(color_str) != "black"))
    return(false);
  return(true);    
}

//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label, DarkKhaki"
//            "label, hex, bd, b7, 6b"
//            "label, 0.741, 0.718, 0.420"

bool MarineViewer::setColorMapping(string str)
{
  string attribute = biteString(str, ',');
  return(setColorMapping(attribute, str));
}

//-------------------------------------------------------------
// Procedure: drawGLPoly

void MarineViewer::drawGLPoly(float *points, int numPoints, 
				    float r, float g, float b, 
				    float thickness, float scale)
{
  if(thickness<=0)
    glBegin(GL_POLYGON);
  else {
    glLineWidth(thickness);
    glBegin(GL_LINE_STRIP);
  }

  glColor3f(r,g,b);
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

void MarineViewer::drawCommonVehicle(string vname, ObjectPose opose, 
				     double red, double grn, double blu, 
				     string vehibody, int outer_line)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  // Determine position in terms of image percentage
  float vehicle_ix = meters2img('x', opose.getX());
  float vehicle_iy = meters2img('y', opose.getY());

  // Determine position in terms of view percentage
  float vehicle_vx = img2view('x', vehicle_ix);
  float vehicle_vy = img2view('y', vehicle_iy);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(vehicle_vx, vehicle_vy, 0); // theses are in pixel units

  glScalef(m_zoom*m_shape_scale, m_zoom*m_shape_scale, m_zoom*m_shape_scale);

  glRotatef(-opose.getTheta(),0,0,1);  

  if(vehibody == "kayak") {
    double cx = g_auvCtrX * g_auvScale;
    double cy = g_auvCtrY * g_auvScale;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_kayakBody, g_kayakBodySize, red, grn, blu, 0, g_kayakScale);    
    if(outer_line)
      drawGLPoly(g_kayakBody, g_kayakBodySize, 0,0,0, outer_line, g_kayakScale);    
    drawGLPoly(g_kayakMidOpen, g_kayakMidOpenSize, 0.5, 0.5, 0.5, 0, g_kayakScale);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "auv") {
    double cx = g_auvCtrX * g_auvScale;
    double cy = g_auvCtrY * g_auvScale;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_auvBody, g_auvBodySize, red, grn, blu, 0, g_auvScale);
    if(outer_line > 0)
      drawGLPoly(g_auvBody, g_auvBodySize, 0,0,0, outer_line, g_auvScale);
    drawGLPoly(g_propUnit, g_propUnitSize, 0,0,1, 0, g_auvScale);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "glider") {
    double cx = g_gliderCtrX * g_gliderScale;
    double cy = g_gliderCtrY * g_gliderScale;
    glTranslatef(-cx, -cy, 0);
    drawGLPoly(g_gliderWing, g_gliderWingSize, red, grn, blu, 0, g_gliderScale);
    drawGLPoly(g_gliderWing, g_gliderWingSize, 0,0,0, 1, g_gliderScale);
    drawGLPoly(g_gliderBody, g_gliderBodySize, red, grn, blu, 0, g_gliderScale);
    drawGLPoly(g_gliderBody, g_gliderBodySize, 0,0,0, 1, g_gliderScale);
    glTranslatef(cx, cy, 0);
  }
  else if(vehibody == "track")
    {  
      double cx = g_shipCtrX * g_shipScale;
      double cy = g_shipCtrY * g_shipScale;
      glTranslatef(-cx, -cy, 0);
      drawGLPoly(g_shipBody, g_shipBodySize, red, grn, blu, 0, g_shipScale);
      if(outer_line > 0)
	drawGLPoly(g_shipBody, g_shipBodySize, 0.0, 0.0, 0.0, outer_line, g_shipScale);
      glTranslatef(cx, cy, 0);
    }
  else 
    {  // vehibody == "ship" is the default
      double cx = g_shipCtrX * g_shipScale;
      double cy = g_shipCtrY * g_shipScale;
      glTranslatef(-cx, -cy, 0);
      drawGLPoly(g_shipBody, g_shipBodySize, red, grn, blu, 0, g_shipScale);
      if(outer_line > 0)
	drawGLPoly(g_shipBody, g_shipBodySize, 0.0, 0.0, 0.0, outer_line, g_shipScale);
      glTranslatef(cx, cy, 0);
    }

  if(m_draw_vname) {
    if(m_vname_color == 0)
      glColor3f(0, 0, 0.6);
    else if(m_vname_color == 1)
      glColor3f(1, 1, 1);
    else
      glColor3f(1, 0, 0);
    gl_font(1, 12);
    if(m_zoom > 4)
      gl_font(1, 14);
    double offset = 100.0;
    offset = offset * (1/m_zoom);

    int slen = vname.length();
    char *buff = new char(slen+1);
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
  float marker_ix = meters2img('x', x);
  float marker_iy = meters2img('y', y);

  // Determine position in terms of view percentage
  float marker_vx = img2view('x', marker_ix);
  float marker_vy = img2view('y', marker_iy);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(marker_vx, marker_vy, 0); // theses are in pixel units

  double mz = sqrt(m_zoom);
  glScalef(mz*shape_scale, mz*shape_scale, mz*shape_scale);

  int vsize = color_vectors.size();
  double r1, r2, g1, g2, b1, b2;
  if(vsize >= 1) {
    r1 = color_vectors[0][0]; 
    g1 = color_vectors[0][1]; 
    b1 = color_vectors[0][2];
  }
  if(vsize >= 2) {
    r2 = color_vectors[1][0];
    g2 = color_vectors[1][1];
    b2 = color_vectors[1][2];
  }

  if(mtype == "gateway") {
    if(vsize < 2) r2=0, g2=0, b2=0;
    if(vsize < 1) r1=1, g1=1, b1=0;
    glTranslatef(-g_gatewayCtrX, -g_gatewayCtrY, 0);
    drawGLPoly(g_gatewayBody, g_gatewayBodySize, r1, g1, b1);    
    drawGLPoly(g_gatewayBody, g_gatewayBodySize, 0,0,0, 1);    
    drawGLPoly(g_gatewayMidBody, g_gatewayMidBodySize, r2, g2, b2);
    glTranslatef(g_gatewayCtrX, g_gatewayCtrY, 0);
  }

  else if(mtype == "efield") {
    if(vsize < 2) r2=1, g2=0.843, b2=0;
    if(vsize < 1) r1=0, g1=0, b1=0;
    glTranslatef(-g_efieldCtrX, -g_efieldCtrY, 0);
    drawGLPoly(g_efieldBody, g_efieldBodySize, r1, g1, b1);    
    drawGLPoly(g_efieldMidBody, g_efieldMidBodySize, r2, g2, b2);
    drawGLPoly(g_efieldMidBody, g_efieldMidBodySize, 0,0,0, 1);
    glTranslatef(g_efieldCtrX, g_efieldCtrY, 0);
  }

  else if(mtype == "diamond") {
    if(vsize < 1) r1=1, g1=0, b1=0;
    glTranslatef(-g_diamondCtrX, -g_diamondCtrY, 0);
    drawGLPoly(g_diamondBody, g_diamondBodySize, r1, g1, b1);    
    drawGLPoly(g_diamondBody, g_diamondBodySize, 0,0,0, 1);    
    glTranslatef(g_diamondCtrX, g_diamondCtrY, 0);
  }

  else if(mtype == "triangle") {
    if(vsize < 1) r1=1, g1=1, b1=0;
    glTranslatef(-g_triangleCtrX, -g_triangleCtrY, 0);
    drawGLPoly(g_triangleBody, g_triangleBodySize, r1, g1, b1);    
    drawGLPoly(g_triangleBody, g_triangleBodySize, 0,0,0, 1);    
    glTranslatef(g_triangleCtrX, g_triangleCtrY, 0);
  }

  else if(mtype == "square") {
    if(vsize < 1) r1=0, g1=1, b1=0;
    glTranslatef(-g_squareCtrX, -g_squareCtrY, 0);
    drawGLPoly(g_squareBody, g_squareBodySize, r1, g1, b1);    
    drawGLPoly(g_squareBody, g_squareBodySize, 0,0,0, 1);    
    glTranslatef(g_squareCtrX, g_squareCtrY, 0);
  }

  else if(mtype == "kelp") {
    if(vsize < 1) r1=0, g1=0.54, b1=0.54;
    glTranslatef(-g_kelpCtrX, -g_kelpCtrY, 0);
    drawGLPoly(g_kelpBody, g_kelpBodySize, r1, g1, b1);    
    drawGLPoly(g_kelpBody, g_kelpBodySize, 0,0,0, 1);    
    glTranslatef(g_kelpCtrX, g_kelpCtrY, 0);
  }

  if(label != "") {
      glColor3f(label_color[0], label_color[1], label_color[2]);
    gl_font(1, 12);
    if(m_zoom > 4)
      gl_font(1, 14);
    double offset = 5.0;
    offset = offset * (1/m_zoom);

    int slen = label.length();
    char *buff = new char(slen+1);
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

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);


  unsigned int index = 0;
  unsigned int asize = m_op_area.size();

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
    for(int i=0; i<vsize; i++) {
      xpos[i] *= m_back_img.get_pix_per_mtr();
      ypos[i] *= m_back_img.get_pix_per_mtr();
    }

    // Draw the edges 
    glLineWidth(lwidth);
    glColor3f(lcolor[0], lcolor[1], lcolor[2]);
    
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
      gl_font(1, 12);
      for(int k=0; k<vsize; k++) {
	int slen = labels[k].length();
	char *buff = new char(slen+1);
	glRasterPos3f(xpos[k]+8, ypos[k]+8, 0);
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

  return(m_geodesy.Initialise(dlat, dlon));
}


