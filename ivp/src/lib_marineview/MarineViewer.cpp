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
  m_poly_vsize  = 5.0;
  m_poly_offon  = true;
  m_poly_labels = true;
  m_cross_offon = false;
  m_grid_offon  = true;
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
// Procedure: drawPolys

void MarineViewer::drawPolys()
{
  if(m_poly_offon == false)
    return;

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
			    float vert_r, float vert_g, float vert_b)  
{
  if(m_poly_offon == false)
    return;

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
  glPointSize(m_poly_vsize);

  // Draw the vertices in between the first and last ones
  glColor3f(vert_r, vert_g, vert_b);
  glBegin(GL_POINTS);
  for(j=0; j<vsize; j++) {
    glVertex2f(points[(j*2)], points[(j*2)+1]);
  }
  glEnd();
  glDisable(GL_POINT_SMOOTH);



  //-------------------------------- perhaps draw poly label
  if(m_poly_labels) {
    double cx = poly.get_avg_x() * m_back_img.get_pix_per_mtr();
    double cy = poly.get_avg_y() * m_back_img.get_pix_per_mtr();
    glTranslatef(cx, cy, 0);

    glColor3f(edge_r, edge_g, edge_b);
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
  if(m_poly_labels) {
    glTranslatef(0, 0, 0);
    char *buff = new char(100);
    for(j=0; j<vsize; j++) {
      double cx = points[(j*2)];
      double cy = points[(j*2)+1];
      
      glColor3f(edge_r, edge_g, edge_b);
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
  int hsize = m_hexa.size();

  for(int i=0; i<hsize; i++)
    drawPoly(m_hexa[i]);
}

//-------------------------------------------------------------
// Procedure: drawSegLists()

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
  int vsize = m_grid.size();
  for(int i=0; i<vsize; i++)
    drawGrid(m_grid[i]);
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
  int vsize = m_circ.size();
  for(int i=0; i<vsize; i++)
    drawCircle(i);
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

void MarineViewer::drawCircle(int ix)
{
  XYCircle dcircle = m_circ[ix];
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
  param = tolower(stripBlankEnds(param));
  value = tolower(stripBlankEnds(value));

  if(param == "datum_color")
    return(setColorMapping("datum_color", value));
  else if(param == "poly_edge_color")
    return(setColorMapping("poly_edge_color", value));
  else if(param == "poly_vertex_color")
    return(setColorMapping("poly_vertex_color", value));
  else if(param == "poly_fill_color")
    return(setColorMapping("poly_fill_color", value));
  else if(param == "active_vcolor")
    return(setColorMapping("active_vcolor", value));
  else if(param == "inactive_vcolor")
    return(setColorMapping("inactive_vcolor", value));
  else if(param == "cross_view")
    return(setBooleanOnString(m_cross_offon, value));
  else if(param == "tiff_type") {
    m_back_img_mod = true;
    if(value == "toggle")
      m_back_img_b_on = !m_back_img_b_on;
    else if(value == "on")
      m_back_img_b_on = true;
    else if(value == "off")
      m_back_img_b_on = false;
    else
      return(false);
  }
  else if(param == "tiff_view") 
    return(setBooleanOnString(m_tiff_offon, value));
  else if(param == "hash_view")
    return(setBooleanOnString(m_hash_offon, value));
  else if(param == "trail_view")
    return(setBooleanOnString(m_trails, value));
  else if(param == "trail_connect")
    return(setBooleanOnString(m_trail_connect, value));
  else if(param == "display_vname")
    return(setBooleanOnString(m_draw_vname, value));
  else if(param == "vname_color") {
    if(value == "toggle")
      m_vname_color = (m_vname_color+1) % 3;
  }
  else if(param == "display_datum")
    return(setBooleanOnString(m_draw_datum, value));
  else if(param == "display_polys")
    return(setBooleanOnString(m_poly_offon, value));
  else if(param == "display_poly_labels")
    return(setBooleanOnString(m_poly_labels, value));
  else if(param == "display_grids")
    return(setBooleanOnString(m_grid_offon, value));
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
  else if(param == "poly_vertex_size") {
    if((v >= 1) && (v <= 40))
      m_poly_vsize = v;
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
  bool   prior_existed = false;
  int    vsize = m_grid.size();
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
  bool ok;
  for(int i=0; i<g_size; i++)
      ok = m_grid[i].processDelta(delta);

#if 0
  if(ok)
    cout << "Successfully processed GridDelta" << endl;
  else
    cout << "Failed process of received GridDelta" << endl;
#endif
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

