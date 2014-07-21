/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VZAIC_Viewer.cpp                                     */
/*    DATE: May 5th, 2010                                        */
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

#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <cstring>
#include "VZAIC_Viewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "IO_Utilities.h"

using namespace std;

VZAIC_Viewer::VZAIC_Viewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  x_offset  = 50;
  y_offset  = 50;

  m_grid_width    = 0;
  m_grid_height   = 0;
  m_draw_labels   = true;

  int domain_width = gw - (x_offset*2);
  m_domain.addDomain("x", 0, domain_width, domain_width+1);

  m_zaic = new ZAIC_Vector(m_domain, "x");

  cout << "Constructor - m_domain size:" << m_domain.getVarPoints(0) << endl;

  setParam("gridsize",  "default");
  setParam("gridshade", "default");
  setParam("backshade", "default");
  setParam("lineshade", "default");
}

//-------------------------------------------------------------
// Procedure: draw()

void VZAIC_Viewer::draw()
{
  glClearColor(0.5,0.5,0.5 ,0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, w(), h());

  drawAxes();
  if(m_draw_labels)
    drawLabels();
  drawPieces();
}

//-------------------------------------------------------------
// Procedure: handle()

int VZAIC_Viewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

//-------------------------------------------------------------
// Procedure: resize()

void VZAIC_Viewer::resize(int gx, int gy, int gw, int gh)
{
  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: setZAIC

void VZAIC_Viewer::setZAIC(ZAIC_Vector *zaic)
{
  if(m_zaic)
    delete(m_zaic);
  m_zaic = zaic;

  if(!m_zaic)
    return;

  m_domain     = m_zaic->getIvPDomain();
  m_grid_width = m_zaic->size();
}

//-------------------------------------------------------------
// Procedure: getMinUtil()
// Procedure: getMaxUtil()

double VZAIC_Viewer::getMinUtil()
{
  if(!m_zaic)
    return(0);
  return(m_zaic->getParam("minutil"));
}

double VZAIC_Viewer::getMaxUtil()
{
  if(!m_zaic)
    return(0);
  return(m_zaic->getParam("maxutil"));
}

//-------------------------------------------------------------
// Procedure: drawAxes

void VZAIC_Viewer::drawAxes()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(x_offset, y_offset, 0);
  
  // Draw Grid Background
  double r=(0.43 * m_back_shade);
  double g=(0.43 * m_back_shade);
  double b=(0.58 * m_back_shade);
  glColor4f(r,g,b,0);
  glBegin(GL_POLYGON);
  glVertex2f(0,  0);
  glVertex2f(m_grid_width, 0);

  cout << "m_grid_width:" << m_grid_width << endl;

  glVertex2f(m_grid_width, m_grid_height);
  glVertex2f(0, m_grid_height);
  glVertex2f(0, 0);
  glEnd();
  
  double hashc = 0.6 * m_grid_shade;
  // Draw Vertical Hashmarks
  unsigned int i;
  for(i=0; i<m_domain.getVarPoints(0); i+=m_grid_block_size) {
    glColor4f(hashc,  hashc,  hashc,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(i, 0);
    glVertex2f(i, m_grid_height);
    glEnd();
  }

  // Draw Horizontal Hashmarks
  for(int j=0; j<m_grid_height; j+=m_grid_block_size) {
    glColor4f(hashc,  hashc,  hashc,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, j);
    glVertex2f(m_grid_width,  j);
    glEnd();
  }

  // Draw Grid outline
  glColor4f(1.0,  1.0,  1.0,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(0,  0);
  glVertex2f(m_grid_width, 0);
  glVertex2f(m_grid_width, m_grid_height);
  glVertex2f(0, m_grid_height);
  glVertex2f(0, 0);
  glEnd();
  
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawLabels

void VZAIC_Viewer::drawLabels()
{
  glColor3f(0, 0, 0.6);

  gl_font(1, 12);

  //---------------------------------- Draw the two zeros
  drawText(x_offset+3, y_offset-15, "0");
  drawText(x_offset-12, y_offset+3, "0");

  //---------------------------------- Draw the domain max
  unsigned int dom_size = m_domain.getVarPoints(0);
  string dom_str = uintToString(dom_size-1);
  drawText(x_offset+dom_size-10, y_offset-15, dom_str);

  //---------------------------------- Draw the domain label
  drawText(x_offset+(dom_size/2)-30, y_offset-16, "DOMAIN  x");

  //---------------------------------- Draw the util axis
  drawText(x_offset-23, y_offset+98, "100");
  drawText(x_offset-23, y_offset+198, "200");
  drawText(x_offset-35, y_offset+233, "UTIL");
  drawText(x_offset-33, y_offset+218, "f(x)");
}

//-------------------------------------------------------------
// Procedure: drawText

void VZAIC_Viewer::drawText(int x, int y, string str)
{
  glRasterPos3f(x, y, 0);
  gl_draw(str.c_str());
}

//-------------------------------------------------------------
// Procedure: drawPieces

void VZAIC_Viewer::drawPieces()
{
  if(!m_zaic) {
    cout << "Null ZAIC_Vector - no pieces to draw!" << endl;
    return;
  }

  PDMap *pdmap = 0;
  IvPFunction *ipf = m_zaic->extractIvPFunction();
  if(ipf)
    pdmap = ipf->getPDMap();

  if(!pdmap) {
    cout << "NULL PDMap - no pieces to draw!" << endl;
    m_zaic->print();
    return;
  }

  //pdmap->print(false);
  pdmap->print(true);

  int i, pc_count = pdmap->size();
  if(pc_count == 0) {
    cout << "Empty PDMap - no pieces to draw!" << endl;
    return;
  }
  for(i=0; i<pc_count; i++)
    drawPiece(pdmap->bx(i));

  m_zaic->print();

  delete(ipf);
}


//-------------------------------------------------------------
// Procedure: drawPiece

void VZAIC_Viewer::drawPiece(const IvPBox *piece, int mode)
{
  double red_val = 1.0 * m_line_shade;
  double grn_val = 1.0 * m_line_shade;
  double blu_val = 1.0 * m_line_shade;

  if(mode==1) {
    red_val = 0.7;
    grn_val = 0.1;
    blu_val = 0.4;
  }

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
  glTranslatef(x_offset, y_offset, 0);

  double x1, x2, y1, y2, m, b;

  m  = piece->wt(0);
  b  = piece->wt(1);
  x1 = piece->pt(0,0);
  x2 = piece->pt(0,1);

  // double x_base  = m_domain.getVarLow(0);
  // double x_delta = m_domain.getVarDelta(0);
  // double dx1 = x_base = (x1 * x_delta);
  // double dx2 = x_base = (x2 * x_delta);
  y1 = (m * (double)(x1)) + b;
  y2 = (m * (double)(x2)) + b;

  glColor4f(red_val, grn_val, blu_val,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();

  glPointSize(3.0);
  if(x1==x2)
    glColor3f(1.0,  0.3, 0.3);
  else
    glColor3f(0.3, 0.3, 1.0);
  glBegin(GL_POINTS);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
  
  
  glFlush();
  glPopMatrix();  
}


//-------------------------------------------------------------
// Procedure: setParam

void VZAIC_Viewer::setParam(string param, string value)
{
  if(param == "gridsize") {
    if((value == "reset") || (value == "default")) {
      int y_max = h() - (2 * y_offset);
      m_grid_height = y_max;
      m_grid_width = m_domain.getVarPoints(0);
      m_grid_block_size = 50;
    }
    else if(value == "up")
      m_grid_block_size++;
    else if(value == "down") {
      m_grid_block_size--;
      if(m_grid_block_size < 10)
	m_grid_block_size = 10;
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
  else if(param == "draw_labels")
    setBooleanOnString(m_draw_labels, value);
}





