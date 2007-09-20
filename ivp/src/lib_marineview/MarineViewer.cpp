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
#include <tiffio.h>
#include "MarineViewer.h"
#include "MBUtils.h"
#include "FColorMap.h"
#include "ColorParse.h"

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
  m_hash_shade  = 0.3;
  m_hash_delta  = 100.0;
  m_fill_shade  = 0.7;
  m_texture_set = 0;
  m_textures    = new GLuint[1];

  m_trails      = true;
  m_trail_gap   = 1;
  m_trail_size  = 1;
  m_cross_offon = false;
  m_poly_offon  = true;
  m_tiff_offon  = true;
  m_hash_offon  = false;
  m_global_ix   = 0;

  glGenTextures(1, m_textures);
}

// ----------------------------------------------------------
// Procedure: readTiff
//   Purpose: This routine reads in a tiff file and stores it 
//            in a very simple data structure

bool MarineViewer::readTiff(const char* filename)
{
  return(m_back_img.readTiff(filename));
}

// ----------------------------------------------------------
// Procedure: setTexture
//   Purpose: 

bool MarineViewer::setTexture()
{
  //glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glBindTexture(GL_TEXTURE_2D, m_textures[0]);
  if(m_texture_set <2) {
    unsigned char *img_data = m_back_img.get_img_data();
    int img_width  = m_back_img.get_img_width();
    int img_height = m_back_img.get_img_height();
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		 img_width, img_height, 0, 
		 GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 
		 (unsigned char *)img_data);
    m_texture_set++;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
    meters_val += m_back_img.get_img_offset_x();
  }

  if(xy == 'y') {
    meters_val  = (img_val - m_back_img.get_img_centy()) * 100.0;
    meters_val  = meters_val / m_back_img.get_img_meters();
    meters_val += m_back_img.get_img_offset_y();
  }
  return(meters_val);
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
#if 1
  glTexCoord2f(0.0f, 0.0f); glVertex2f(lower_x, lower_y);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(lower_x, upper_y);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(upper_x, upper_y);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(upper_x, lower_y);
#endif
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  glFlush();
}

// ----------------------------------------------------------
// Procedure: drawHash
//   Purpose: 

void MarineViewer::drawHash()
{
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

  double img_centx  = m_back_img.get_img_centx();
  double img_centy  = m_back_img.get_img_centy();
  double img_meters = m_back_img.get_img_meters();

  double xlow = (floor(img_centx / img_meters)) * -m_hash_delta;
  double xhgh = (floor((1.0-img_centx) / img_meters)) * m_hash_delta;
  double ylow = (floor(img_centy / img_meters)) * -m_hash_delta;
  double yhgh = (floor((1.0-img_centy) / img_meters)) * m_hash_delta;

  float r = m_hash_shade;
  float g = m_hash_shade + 0.1;
  float b = m_hash_shade;
  if(g > 1.0)
    g = 1.0;

  glColor3f(r,g,b);
  glLineWidth(1.0);
  glBegin(GL_LINES);
  float xline = 0 + m_hash_delta;
  while(xline <= xhgh) {
    glVertex2f(xline, ylow);
    glVertex2f(xline, yhgh);
    xline += m_hash_delta;
  }  
  xline = 0 - m_hash_delta;
  while(xline >= xlow) {
    glVertex2f(xline, ylow);
    glVertex2f(xline, yhgh);
    xline -= m_hash_delta;
  }  

  float yline = 0 + m_hash_delta;
  while(yline <= yhgh) {
    glVertex2f(xlow, yline);
    glVertex2f(xhgh, yline);
    yline += m_hash_delta;
  }
  yline = 0 - m_hash_delta;
  while(yline >= ylow) {
    glVertex2f(xlow, yline);
    glVertex2f(xhgh, yline);
    yline -= m_hash_delta;
  }
  glEnd();
  
  r = m_hash_shade + 0.1;
  g = m_hash_shade;
  b = m_hash_shade;
  if(r > 1.0)
    r = 1.0;

  glColor3f(r,g,b);
  glBegin(GL_LINES);
  glVertex2f(0, ylow);
  glVertex2f(0, yhgh);
  glVertex2f(xlow, 0);
  glVertex2f(xhgh, 0);
  glEnd();

  glPopMatrix();
  glFlush();
}

//-------------------------------------------------------------
// Procedure: drawPolys

void MarineViewer::drawPolys()
{
  int vsize = m_poly.size();
  for(int i=0; i<vsize; i++) 
    drawPoly(m_poly[i]);
}

//-------------------------------------------------------------
// Procedure: drawPoly

void MarineViewer::drawPoly(const XYPolygon& poly, 
			    bool filled, bool dashed,
			    float edge_r, float edge_g, float edge_b,
			    float fill_r, float fill_g, float fill_b,
			    float vert_r, float vert_g, float vert_b,
			    float ver1_r, float ver1_g, float ver1_b,
			    float last_r, float last_g, float last_b)  
{
  int vsize = poly.size();
  if(vsize < 1)
    return;

  unsigned int i, j;
  float *points = new float[2*vsize];

  int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = poly.get_vx(i) - m_back_img.get_img_offset_x();
    points[pindex+1] = poly.get_vy(i) - m_back_img.get_img_offset_y();

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
    glColor4f(fill_r,fill_g,fill_b,0.1);
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
  if(vsize > 2) {
    glLineWidth(1.0);
    if(dashed) {
      glEnable(GL_LINE_STIPPLE);
      GLushort pattern = 0x5555;
      GLint factor = 5;
      glLineStipple(factor, pattern);
    }

    glColor3f(edge_r, edge_g, edge_b);

    if(poly.is_convex())
    //if(0)
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

  //glPointSize(1.0 * m_zoom);
  glPointSize(1.0);

  // Draw the first vertex
  glColor3f(ver1_r, ver1_g, ver1_b); 
  glBegin(GL_POINTS);
  glVertex2f(points[0], points[1]);
  glEnd();

  // Draw the vertices in between the first and last ones
  glColor3f(vert_r, vert_g, vert_b);
  glBegin(GL_POINTS);
  for(int k=1; k<vsize; k++) {
    glVertex2f(points[(k*2)], points[(k*2)+1]);
  }
  glEnd();

  // Draw the last vertex
  if(vsize > 1) {
    int k=vsize-1;
    glColor3f(last_r, last_g, last_b);
    glBegin(GL_POINTS);
    glVertex2f(points[(k*2)], points[(k*2)+1]);
    glEnd();
  }

  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawHexagons()

void MarineViewer::drawHexagons()
{
  int hsize = m_hexa.size();

  for(int i=0; i<hsize; i++)
    drawPoly(m_hexa[i]);
}

//-------------------------------------------------------------
// Procedure: drawSegList()

void MarineViewer::drawSegLists()
{
  int vsize = m_segl.size();

  for(int i=0; i<vsize; i++)
    drawSegList(i );
}

//-------------------------------------------------------------
// Procedure: drawSegList

void MarineViewer::drawSegList(int ix)
{
  float edge_r = 0.5;
  float edge_g = 0.5;
  float edge_b = 0.5;
  
  float vert_r = 1.0;
  float vert_g = 0.6;
  float vert_b = 0.6;

  XYSegList segl = m_segl[ix];
  int vsize = segl.size();

  unsigned int i, j;
  float *points = new float[2*vsize];

  int pindex = 0;
  for(i=0; i<vsize; i++) {
    points[pindex]   = segl.get_vx(i) - m_back_img.get_img_offset_x();
    points[pindex+1] = segl.get_vy(i) - m_back_img.get_img_offset_y();

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
    glLineWidth(1.0);
    glColor3f(edge_r, edge_g, edge_b);

    glBegin(GL_LINE_STRIP);
    for(i=0; i<vsize*2; i=i+2) {
      glVertex2f(points[i], points[i+1]);
    }
    glEnd();
  }

  // If the seglist is just a single point, draw it big!
  if(vsize==1) {
    glPointSize(2.0 * m_zoom);
    // Draw the vertices with color coding for the first and last
    
    //glColor3f(0.7,0.13,0.13);  // Firebrick red b2 22 22
    glColor3f(0.13, 0.13, 0.7);  // Blueish
    glBegin(GL_POINTS);
    glVertex2f(points[0], points[1]);
    glEnd();
  }

  //glPointSize(1.0 * m_zoom);
  glPointSize(1.0);

  // Draw the vertices in between the first and last ones
  glColor3f(vert_r, vert_g, vert_b);
  glBegin(GL_POINTS);
  for(int k=0; k<vsize; k++) {
    glVertex2f(points[(k*2)], points[(k*2)+1]);
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
  int vsize = m_grid.size();
  for(int i=0; i<vsize; i++)
    drawGrid(i);
}

//-------------------------------------------------------------
// Procedure: drawGrid

void MarineViewer::drawGrid(int ix)
{
  FColorMap cmap;

  XYGrid grid = m_grid[ix];
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

    pct = snapToStep(pct, 0.25);
    cout << "[" << pct << "]";
    
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
  int vsize = m_circ.size();
  for(int i=0; i<vsize; i++)
    drawCircle(i);
}

//-------------------------------------------------------------
// Procedure: drawCircle

void MarineViewer::drawCircle(int ix)
{
  FColorMap cmap;

  XYCircle dcircle = m_circ[ix];
  string dlabel = dcircle.getLabel();

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

  double px = dcircle.getX();
  double py = dcircle.getY();
  double rad = dcircle.getRad();

  vector<double> cvect;
  cvect.push_back(0);
  cvect.push_back(1);
  cvect.push_back(1);

  map<string,vector<double> >::iterator p2;
  p2 = m_color_map.find(dlabel);
  if(p2 != m_color_map.end())
    cvect = p2->second;

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
// Procedure: setParam

bool MarineViewer::setParam(string param, string value)
{
  param = tolower(stripBlankEnds(param));
  value = tolower(stripBlankEnds(value));

  if(param == "cross_view") {
    if(value == "toggle")
      m_cross_offon = !m_cross_offon;
    else if(value == "on")
      m_cross_offon = true;
    else if(value == "off")
      m_cross_offon = false;
    else
      return(false);
  }
  else if(param == "tiff_view") {
    if(value == "toggle")
      m_tiff_offon = !m_tiff_offon;
    else if(value == "on")
      m_tiff_offon = true;
    else if(value == "off")
      m_tiff_offon = false;
    else
      return(false);
  }
  else if(param == "hash_view") {
    if(value == "toggle")
      m_hash_offon = !m_hash_offon;
    else if(value == "on")
      m_hash_offon = true;
    else if(value == "off")
      m_hash_offon = false;
    else
      return(false);
  }
  else if(param == "trail_view") {
    if(value == "toggle")
      m_trails = !m_trails;
    else if(value == "on")
      m_trails = true;
    else if(value == "off")
      m_trails = false;
    else
      return(false);
  }
  else if(param == "poly_view") {
    if(value == "toggle")
      m_poly_offon = !m_poly_offon;
    else
      return(false);
  }
  else if(param == "zoom") {
    if(value == "reset")
      m_zoom = 1.0;
    else
      return(false);
  }
  else if(param == "clear") {
    if(value == "polygons")
      m_poly.clear();
    else if(value == "grids")
      m_grid.clear();
    else if(value == "circles")
      m_circ.clear();
    else
      return(false);
  }
  else if(param == "trail_color") {
    if(value == "toggle")
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
// Procedure: setParam

bool MarineViewer::setParam(string param, float v)
{
  param = tolower(stripBlankEnds(param));
  
  if(param == "hash_shade") {
    if((m_hash_shade+v >= 0) && (m_hash_shade+v <= 1.0))
      m_hash_shade += v;
  }
  else if(param == "hash_delta") {
    if((m_hash_delta+v >= 10) && (m_hash_delta+v <= 500))
      m_hash_delta += v;
  }
  else if(param == "back_shade") {
    if(!m_tiff_offon) {
      if((m_fill_shade+v > 0) && (m_fill_shade+v <= 1.0))
	m_fill_shade += v;
    }
  }
  else if(param == "trail_size") {
    if(m_trail_size+v >= 0.1)
      m_trail_size += v;
  }
  else if(param == "trail_gap") {
    if(m_trail_gap+v >= 1)      
      m_trail_gap += (int)v;
  }
  else if(param == "shape_scale") {
    if(m_shape_scale*v > 0.1)      
      m_shape_scale *= v;
  }
  else if(param == "zoom") {
    if(m_zoom*v > 0.1)      
      m_zoom *= v;
  }
  else if(param == "pan_x") {
    m_vshift_x += v;
  }
  else if(param == "set_pan_x") {
    m_vshift_x = v;
  }
  else if(param == "pan_y") {
    m_vshift_y += v;
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
// Procedure: addPoly

void MarineViewer::addPoly(const XYPolygon& new_poly)
{
  string new_label = new_poly.get_label();
  
  // If given poly doesn't use a label it is automatically
  // considered to be unique.
  if(new_label == "") {
    m_poly.push_back(new_poly);
    return;
  }
  
  bool prior_existed = false;

  int vsize = m_poly.size();
  for(int i=0; i<vsize; i++) {
    if(m_poly[i].get_label() == new_label) {
      m_poly[i] = new_poly;
      prior_existed = true;
    }
  }
  
  if(!prior_existed)
    m_poly.push_back(new_poly);
  
}

//-------------------------------------------------------------
// Procedure: addHexagon

void MarineViewer::addHexagon(const XYHexagon& new_hexa)
{
  string new_label = new_hexa.get_label();
  
  // If given hexa doesn't use a label it is automatically
  // considered to be unique.
  if(new_label == "") {
    m_hexa.push_back(new_hexa);
    return;
  }
  
  bool prior_existed = false;

  int vsize = m_hexa.size();
  for(int i=0; i<vsize; i++) {
    if(m_hexa[i].get_label() == new_label) {
      m_hexa[i] = new_hexa;
      prior_existed = true;
    }
  }
  
  if(!prior_existed)
    m_hexa.push_back(new_hexa);
}

//-------------------------------------------------------------
// Procedure: addSegList

void MarineViewer::addSegList(const XYSegList& new_segl)
{
  string new_label = new_segl.get_label();
  
  // If given seglist doesn't use a label it is automatically
  // considered to be unique.
  if(new_label == "") {
    m_segl.push_back(new_segl);
    return;
  }
  
  bool prior_existed = false;

  int vsize = m_segl.size();
  for(int i=0; i<vsize; i++) {
    if(m_segl[i].get_label() == new_label) {
      m_segl[i] = new_segl;
      prior_existed = true;
    }
  }
  
  if(!prior_existed)
    m_segl.push_back(new_segl);
}

//-------------------------------------------------------------
// Procedure: addCircle

void MarineViewer::addCircle(const XYCircle& new_circ)
{
  string new_label = new_circ.getLabel();
  
  // If given circ doesn't use a label it is automatically
  // considered to be unique.
  if(new_label == "") {
    m_circ.push_back(new_circ);
    return;
  }
  
  bool prior_existed = false;
  
  int vsize = m_circ.size();
  for(int i=0; i<vsize; i++) {
    if(m_circ[i].getLabel() == new_label) {
      m_circ[i] = new_circ;
      prior_existed = true;
    }
  }
  
  if(!prior_existed)
    m_circ.push_back(new_circ);
  
}

//-------------------------------------------------------------
// Procedure: addGrid

void MarineViewer::addGrid(const XYGrid& new_grid)
{
  string new_label = new_grid.getLabel();
  
  bool prior_existed = false;
  
  int vsize = m_grid.size();
  for(int i=0; i<vsize; i++) {
    if(m_grid[i].getLabel() == new_label) {
      m_grid[i] = new_grid;
      prior_existed = true;
    }
  }
  
  if(!prior_existed)
    m_grid.push_back(new_grid);
}


//-------------------------------------------------------------
// Procedure: udpateGrid

void MarineViewer::updateGrid(string delta)
{
  int g_size = m_grid.size();
  for(int i=0; i<g_size; i++)
      m_grid[i].processDelta(delta);
}


//-------------------------------------------------------------
// Procedure: colorMapping
//            "label, DarkKhaki"
//            "label, hex, bd, b7, 6b"
//            "label, 0.741, 0.718, 0.420"

void MarineViewer::colorMapping(const string& str)
{
  vector<string> pair = chompString(str, ',');
  string vname = stripBlankEnds(pair[0]);
  string color = stripBlankEnds(pair[1]);

  vector<double> cvector = colorParse(color);
  
  map<string,vector<double> >::iterator p1;
  p1 = m_color_map.find(vname);
  if(p1 != m_color_map.end())
    p1->second = cvector;
  else {
    m_color_map[vname] = cvector;
  }
}

//-------------------------------------------------------------
// Procedure: drawGLPoly

void MarineViewer::drawGLPoly(float *points, int numPoints, 
				    float r, float g, float b, 
				    float thickness)
{
  if(thickness<=0)
    glBegin(GL_POLYGON);
  else {
    glLineWidth(thickness);
    glBegin(GL_LINE_STRIP);
  }

  glColor3f(r,g,b);
  unsigned int i;
  for(i = 0; i < numPoints*2; i=i+2) {
    glVertex2f(points[i], points[i+1]-60.0);
  }
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
