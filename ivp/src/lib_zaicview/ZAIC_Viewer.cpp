/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_Viewer.cpp                                      */
/*    DATE: June 2nd, 2015                                       */
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

#include <iostream>
#include <cmath>
#include "ZAIC_Viewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "ColorParse.h"
#include "IO_Utilities.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ZAIC_Viewer::ZAIC_Viewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  m_model = 0;
  
  m_xoffset = 50;
  m_yoffset = 50;

  m_grid_wid  = gw - (m_xoffset * 2);
  m_grid_hgt  = 0;
  m_cell_pix_wid = 50;
  m_cell_pix_hgt = 50;
  m_cell_dom_wid = 50;
  
  m_draw_labels = true;
  m_pieces = 0;
  
  m_dompts = 0;
  m_pixels_per_dompt = 0;

  m_color_scheme = 1;
  setColorScheme(m_color_scheme);

  setParam("gridsize",  "default");
  setParam("gridshade", "default");
  setParam("backshade", "default");
  setParam("lineshade", "default");

  // The use_high_res_GL function is supported in more recent FLTK
  // packages. FLTK on older non-MacOS systems may not have this
  // feature. It is mostly needed to support Mac Retina displays.
#ifdef __APPLE__
  Fl::use_high_res_GL(1);
#endif
}

//-------------------------------------------------------------
// Destructor

ZAIC_Viewer::~ZAIC_Viewer()
{
  if(m_model)
    delete(m_model);
}

//-------------------------------------------------------------
// Procedure: setModel

void ZAIC_Viewer::setModel(ZAIC_Model *model)
{
  if(m_model && (m_model != model))
    delete(m_model);
  m_model = model;
}

//-------------------------------------------------------------
// Procedure: draw()

void ZAIC_Viewer::draw()
{
  if(!m_model)
    return;

  if(m_model->getIvPDomain().size() == 0)
    return;

  m_dompts = m_model->getIvPDomain().getVarPoints(0);
  m_pixels_per_dompt = (double)(m_grid_wid) / (double)(m_dompts);

  m_cell_dom_wid = m_cell_pix_wid / m_pixels_per_dompt;

  glClearColor(0.5,0.5,0.5 ,0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // The pixel_w/h() functions are supported in more recent FLTK
  // packages. FLTK on older non-MacOS systems may not have this
  // feature. It is mostly needed to support Mac Retina displays.
#ifdef __APPLE__
  glViewport(0, 0, pixel_w(), pixel_h());
#else
  glViewport(0, 0, w(), h());
#endif
  
  drawAxes();
  if(m_draw_labels)
    drawLabels();
  drawPieces();
}

//-------------------------------------------------------------
// Procedure: setColorScheme

void ZAIC_Viewer::setColorScheme(int index)
{
  if(index == 0) {
    m_color_scheme = 0;
    m_label_color   = "0.0,0.0,0.6";
    m_line_color    = "white";
    m_display_color = "0.43,0.43,0.58";
    m_point_color   = "blue";
  }
  else if(index == 1) {
    m_color_scheme = 1;
    m_label_color   = "black";
    m_line_color    = "black";
    m_display_color = "hex:df,db,c3";
    m_point_color   = "red";
  }
}

//-------------------------------------------------------------
// Procedure: handle()

int ZAIC_Viewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

//-------------------------------------------------------------
// Procedure: resize()

void ZAIC_Viewer::resize(int gx, int gy, int gw, int gh)
{
  Fl_Gl_Window::resize(gx, gy, gw, gh);

  m_grid_wid = gw - (m_xoffset * 2);
}

//-------------------------------------------------------------
// Procedure: drawAxes

void ZAIC_Viewer::drawAxes()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(m_xoffset, m_yoffset, 0);

  // Draw Grid Background
  vector<double> cvect = colorParse(m_display_color);
  
  //double r=(0.43 * m_back_shade);
  //double g=(0.43 * m_back_shade);
  //double b=(0.58 * m_back_shade);
  double r=(cvect[0] * m_back_shade);
  double g=(cvect[1] * m_back_shade);
  double b=(cvect[2] * m_back_shade);
  glColor4f(r,g,b,0);
  glBegin(GL_POLYGON);
  glVertex2f(0,  0);
  glVertex2f(m_grid_wid, 0);
  glVertex2f(m_grid_wid, m_grid_hgt);
  glVertex2f(0, m_grid_hgt);
  glVertex2f(0, 0);
  glEnd();
  
  double hashc = 0.6 * m_grid_shade;
  // Draw Vertical Hashmarks

  for(int i=0; i<=m_grid_wid; i+=m_cell_pix_wid) {
    glColor4f(hashc,  hashc,  hashc,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(i, 0);
    glVertex2f(i, m_grid_hgt);
    glEnd();
  }

  // Draw Horizontal Hashmarks
  for(int j=0; j<m_grid_hgt; j+=m_cell_pix_hgt) {
    glColor4f(hashc,  hashc,  hashc,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, j);
    glVertex2f(m_grid_wid,  j);
    glEnd();
  }

#if 0  
  // Draw Grid outline
  glColor4f(1.0,  1.0,  1.0,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(0, 0);
  glVertex2f(m_grid_wid, 0);
  glVertex2f(m_grid_wid, m_grid_hgt);
  glVertex2f(0, m_grid_hgt);
  glVertex2f(0, 0);
  glEnd();
#endif
  
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawLabels

void ZAIC_Viewer::drawLabels()
{
  if(m_color_scheme == 0)
      glColor3f(0, 0, 0.6);
  if(m_color_scheme == 1)
      glColor3f(0, 0, 0.6);

  gl_font(1, 12);

  //---------------------------------- Draw the two zeros
  drawText(m_xoffset+3, m_yoffset-12, "0");
  drawText(m_xoffset-12, m_yoffset+3, "0");

  //---------------------------------- Draw the domain max
  string dom_str  = uintToString(m_dompts-1);

  int    dom_x = (double)(m_dompts) * m_pixels_per_dompt;
  drawText(m_xoffset+dom_x-10, m_yoffset-12, dom_str);

  int    grid_x = m_cell_pix_wid;
  drawText(m_xoffset+grid_x-10, m_yoffset-12, intToString(m_cell_dom_wid));

  
  //---------------------------------- Draw the domain label
  drawText(m_xoffset+(dom_x/2)-30, m_yoffset-16, "DOMAIN  x");

  //---------------------------------- Draw the util axis
  drawText(m_xoffset-23, m_yoffset+98,  "50");
  drawText(m_xoffset-23, m_yoffset+198, "100");
  drawText(m_xoffset-35, m_yoffset+233, "UTIL");
  drawText(m_xoffset-33, m_yoffset+218, "f(x)");
}

//-------------------------------------------------------------
// Procedure: drawText

void ZAIC_Viewer::drawText(int x, int y, string str)
{
#if 0
  int slen = str.length();
  char *buff = new char(slen+1);
  strncpy(buff, str.c_str(), slen);
  buff[slen] = '\0';
  glRasterPos3f(x, y, 0);
  gl_draw(buff, slen);
  delete(buff);
#endif
  gl_draw(str.c_str(), str.length());
}

//-------------------------------------------------------------
// Procedure: drawPieces

void ZAIC_Viewer::drawPieces()
{
  IvPFunction *ipf = m_model->getIvPFunction();
  if(!ipf) {
    m_pieces = 0;
    return;
  }
  
  PDMap *pdmap = ipf->getPDMap();
  if(!pdmap || (pdmap->size() == 0)) {
    m_pieces = 0;
    return;
  }
  m_pieces = pdmap->size();
  
  if(m_verbose)
    pdmap->print(true);
  
  int pc_count = pdmap->size();
  for(int i=0; i<pc_count; i++)
    drawPiece(pdmap->bx(i));
  delete(pdmap);
}

//-------------------------------------------------------------
// Procedure: drawPiece

void ZAIC_Viewer::drawPiece(const IvPBox *piece, int mode)
{
  // Handle the line color
  vector<double> cvect = colorParse(m_line_color);
  double red_val = cvect[0] * m_line_shade;
  double grn_val = cvect[1] * m_line_shade;
  double blu_val = cvect[2] * m_line_shade;

  if(red_val < 0) red_val=0;
  if(red_val > 1) red_val=1;
  if(grn_val < 0) grn_val=0;
  if(grn_val > 1) grn_val=1;
  if(blu_val < 0) blu_val=0;
  if(blu_val > 1) blu_val=1;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  //glTranslatef(0, 0, 0);
  glTranslatef(m_xoffset, m_yoffset, 0);

  double m = piece->wt(0);
  double b = piece->wt(1);

  //double x_base  = m_model->getIvPDomain().getVarLow(0);
  //double x_delta = m_model->getIvPDomain().getVarDelta(0);

  double x1 = piece->pt(0,0);
  double x2 = piece->pt(0,1);
  //double dx1 = x_base = (x1 * x_delta);
  //double dx2 = x_base = (x2 * x_delta);
  double y1 = 2*((m * (double)(x1)) + b);
  double y2 = 2*((m * (double)(x2)) + b);

  glColor4f(red_val, grn_val, blu_val,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x1 * m_pixels_per_dompt, y1);
  glVertex2f(x2 * m_pixels_per_dompt, y2);
  glEnd();

  // Handle the point color
  cvect = colorParse(m_point_color);
  red_val = cvect[0];
  grn_val = cvect[1];
  blu_val = cvect[2];

  glPointSize(3.0);
  if(x1==x2)
    glColor3f(1.0,  0.3, 0.3);
  else
    glColor3f(red_val, grn_val, blu_val);
  glBegin(GL_POINTS);
  glVertex2f(x1 * m_pixels_per_dompt, y1);
  glVertex2f(x2 * m_pixels_per_dompt, y2);
  glEnd();
    
  glFlush();
  glPopMatrix();  
}


//-------------------------------------------------------------
// Procedure: setParam

void ZAIC_Viewer::setParam(string param, string value)
{
  if(param == "gridsize") {
    if((value == "reset") || (value == "default")) {
      int y_max = h() - (2 * m_yoffset);
      m_grid_hgt = y_max;
      m_cell_pix_wid = 50;
    }
    else if(value == "up") 
      m_cell_pix_wid++;
    else if(value == "down") {
      m_cell_pix_wid--;
      if(m_cell_pix_wid < 20)
	m_cell_pix_wid = 20;
    }
  }
  else if(param == "backshade") {
    if((value == "reset") || (value == "default")) 
      m_back_shade = 1.0;
    else if(value == "down")
      m_back_shade -= 0.02;
    else if(value == "up")
      m_back_shade += 0.02;
    if(m_back_shade < 0)
      m_back_shade = 0;
    if(m_back_shade > 10)
      m_back_shade = 10;
  }
  else if(param == "gridshade") {
    if((value == "reset") || (value == "default")) 
      m_grid_shade = 1.0;
    else if(value == "down")
      m_grid_shade -= 0.02;
    else if(value == "up")
      m_grid_shade += 0.02;
    if(m_grid_shade < 0)
      m_grid_shade = 0;
    if(m_grid_shade > 10)
      m_grid_shade = 10;
  }
  else if(param == "lineshade") {
    if((value == "reset") || (value == "default")) 
      m_line_shade = 1.0;
    else if(value == "down")
      m_line_shade -= 0.02;
    else if(value == "up")
      m_line_shade += 0.02;
    if(m_line_shade < 0)
      m_line_shade = 0;
    if(m_line_shade > 10)
      m_line_shade = 10;
  }
  else if(param == "labels") {
    if(value == "toggle")
      m_draw_labels = !m_draw_labels;
    else if(value == "false")
      m_draw_labels = false;
    else if(value == "true")
      m_draw_labels = true;
  }
  else if(param == "color_scheme") {
    if(value == "toggle") {
      if(m_color_scheme == 0)
	setColorScheme(1);
      else if(m_color_scheme == 1)
	setColorScheme(0);
    }
  }
}





