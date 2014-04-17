/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAICViewer.cpp                                       */
/*    DATE: June 17th, 2006                                      */
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
#include <iostream>
#include <cmath>
#include <cstring>
#include "ZAICViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "IO_Utilities.h"

using namespace std;

ZAICViewer::ZAICViewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  m_curr_mode  = 0;  // 0:summit 1:peakwidth 2:basewidth 3:summitdelta

  ipf_one   = 0;
  ipf_two   = 0;
  ipf_max   = 0;
  ipf_tot   = 0;
  ipf_cur   = 0;
  x_offset  = 50;
  y_offset  = 50;

  x_grid_width    = 0;
  y_grid_height   = 0;

  int domain_width = gw - (x_offset*2);
  m_domain.addDomain("x", 0, domain_width, domain_width+1);
  m_zaic_1 = 0;
  m_zaic_2 = 0;
  m_zaic_C = 0;
  m_zaic_H = 0;

  setParam("gridsize",  "default");
  setParam("gridshade", "default");
  setParam("backshade", "default");
  setParam("lineshade", "default");
  setParam("draw_mode", "default");
  setParam("wrap_mode", "default");
  setZAIC(2);
}

//-------------------------------------------------------------
// Procedure: setZAIC

void ZAICViewer::setZAIC(int amt)
{
  double dom_size = (double)(m_domain.getVarPoints(0));

  double summit1 = (dom_size / 4.0);
  double summit2 = (dom_size / 4.0)*3;
  double peak    = (dom_size / 10.0);
  double base    = (dom_size / 10.0);
  
  m_zaic_1 = new ZAIC_PEAK(m_domain, "x");  
  m_zaic_1->setSummit(summit1);
  m_zaic_1->setPeakWidth(peak);
  m_zaic_1->setBaseWidth(base);
  m_zaic_1->setSummitDelta(25);
  m_zaic_1->setMinMaxUtil(0, 150);

  if(amt == 2) {
    m_zaic_2 = new ZAIC_PEAK(m_domain, "x");  
    m_zaic_2->setSummit(summit2);
    m_zaic_2->setPeakWidth(peak);
    m_zaic_2->setBaseWidth(base);
    m_zaic_2->setSummitDelta(30);
    m_zaic_2->setMinMaxUtil(0, 100);
  }

  m_zaic_1->setValueWrap(true);
  m_zaic_2->setValueWrap(true);

  m_zaic_C = m_zaic_1;
  m_zaic_H = m_zaic_C;
  setIPFunctions();
}

//-------------------------------------------------------------
// Procedure: draw()

void ZAICViewer::draw()
{
  glClearColor(0.5,0.5,0.5 ,0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, w(), h());

  drawAxes();
  //drawLabels();
  drawPieces();
}

//-------------------------------------------------------------
// Procedure: handle()

int ZAICViewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

//-------------------------------------------------------------
// Procedure: resize()

void ZAICViewer::resize(int gx, int gy, int gw, int gh)
{
  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: currMode

void ZAICViewer::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 5))
    m_curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: moveX

void ZAICViewer::moveX(double delta)
{
  double dom_low  = m_domain.getVarLow(0);
  double dom_high = m_domain.getVarHigh(0);
  
  if(m_curr_mode==0) {   // Altering Summit
    double summit = m_zaic_C->getParam("summit");
    summit += delta;
    if(summit < dom_low)
      summit = dom_low;
    if(summit > dom_high)
      summit = dom_high;
    m_zaic_C->setSummit(summit);
  }
  else if(m_curr_mode == 1) { // Altering Peak Width
    double peak_width = m_zaic_C->getParam("peakwidth");
    peak_width += delta;
    if(peak_width < 0)
      peak_width = 0;
    m_zaic_C->setPeakWidth(peak_width);
  }
  else if(m_curr_mode == 2) { // Altering Base Width
    double base_width = m_zaic_C->getParam("basewidth");
    base_width += delta;
    if(base_width < 0)
      base_width = 0;
    m_zaic_C->setBaseWidth(base_width);
    cout << "Basewidth: " << base_width << endl;
  }
  else if(m_curr_mode == 3) { // Altering Summit Delta
    double summit_delta = m_zaic_C->getParam("summitdelta");
    summit_delta += delta;
    if(summit_delta < 0)
      summit_delta = 0;
    if(summit_delta > 100)
      summit_delta = 100;
    m_zaic_C->setSummitDelta(summit_delta);
  }
  else if(m_curr_mode == 4) { // Altering Max Utility
    double minutil = m_zaic_C->getParam("minutil");
    double maxutil = m_zaic_C->getParam("maxutil");
    double new_max = maxutil + delta;
    if(new_max < 0)
      new_max = 0;
    if(new_max > 200)
      new_max = 200;
    m_zaic_C->setMinMaxUtil(minutil, new_max);
  }
  else if(m_curr_mode == 5) { // Altering Min Utility
    double minutil = m_zaic_C->getParam("minutil");
    double maxutil = m_zaic_C->getParam("maxutil");
    double new_min = minutil + delta;
    if(new_min < 0)
      new_min = 0;
    if(new_min > maxutil)
      new_min = maxutil;
    m_zaic_C->setMinMaxUtil(new_min, maxutil);
  }
  else
    cout << "Uh-Oh!  Mode problem" << endl;

  setIPFunctions();
}


//-------------------------------------------------------------
// Procedure: setIPFunctions()

void ZAICViewer::setIPFunctions()
{
  if(!m_zaic_1 || !m_zaic_2 || !m_zaic_C)
    return;

  if(ipf_cur)  delete(ipf_cur);
  if(ipf_one)  delete(ipf_one);
  if(ipf_two)  delete(ipf_two);
  if(ipf_max)  delete(ipf_max);
  if(ipf_tot)  delete(ipf_tot);
  
  ipf_cur = m_zaic_C->extractOF();
  ipf_one = m_zaic_1->extractOF();
  ipf_two = m_zaic_2->extractOF();

  double summit  = m_zaic_2->getParam("summit");
  double bwidth  = m_zaic_2->getParam("basewidth");
  double pwidth  = m_zaic_2->getParam("peakwidth");
  double delta   = m_zaic_2->getParam("summitdelta");
  double minutil = m_zaic_2->getParam("minutil");
  double maxutil = m_zaic_2->getParam("maxutil");

  ZAIC_PEAK zaic_max = *m_zaic_1;
  int ix = zaic_max.addComponent();
  zaic_max.setParams(summit, pwidth, bwidth, delta, minutil, maxutil, ix);
  ipf_max = zaic_max.extractOF();

  ZAIC_PEAK zaic_tot = *m_zaic_1;
  ix = zaic_tot.addComponent();
  zaic_tot.setParams(summit, pwidth, bwidth, delta, minutil, maxutil, ix);
  ipf_tot = zaic_max.extractOF(false);
}

//-------------------------------------------------------------
// Procedure: toggleIndex

void ZAICViewer::toggleIndex()
{
  if((m_zaic_C == m_zaic_1) && (m_zaic_2))
    m_zaic_C = m_zaic_2;
  else if((m_zaic_C == m_zaic_2) && (m_zaic_1))
    m_zaic_C = m_zaic_1;
  else
    return;
    
  m_zaic_H = m_zaic_C;
  setIPFunctions();
  redraw();
}

//-------------------------------------------------------------
// Procedure: getSummit()
//            getBaseWidth()
//            getPeakWidth()
//            getSummitDelta()
//            getMaxUtil()
//            getMinUtil()

double ZAICViewer::getSummit() {
  return(m_zaic_C->getParam("summit"));
}

double ZAICViewer::getBaseWidth() {
  return(m_zaic_C->getParam("basewidth"));
}

double ZAICViewer::getPeakWidth() {
  return(m_zaic_C->getParam("peakwidth"));
}

double ZAICViewer::getSummitDelta() {
  return(m_zaic_C->getParam("summitdelta"));
}

double ZAICViewer::getMaxUtil() {
  return(m_zaic_C->getParam("maxutil"));
}

double ZAICViewer::getMinUtil() {
  return(m_zaic_C->getParam("minutil"));
}

//-------------------------------------------------------------
// Procedure: drawAxes

void ZAICViewer::drawAxes()
{
  //int x_len = 600;
  //int y_len = 400;


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
  glVertex2f(x_grid_width, 0);
  glVertex2f(x_grid_width, y_grid_height);
  glVertex2f(0, y_grid_height);
  glVertex2f(0, 0);
  glEnd();
  
  double hashc = 0.6 * m_grid_shade;
  // Draw Vertical Hashmarks
  for(unsigned int i=0; i<m_domain.getVarPoints(0); i+=m_grid_block_size) {
    glColor4f(hashc,  hashc,  hashc,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(i, 0);
    glVertex2f(i, y_grid_height);
    glEnd();
  }

  // Draw Horizontal Hashmarks
  for(int j=0; j<y_grid_height; j+=m_grid_block_size) {
    glColor4f(hashc,  hashc,  hashc,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, j);
    glVertex2f(x_grid_width,  j);
    glEnd();
  }

  // Draw Grid outline
  glColor4f(1.0,  1.0,  1.0,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(0,  0);
  glVertex2f(x_grid_width, 0);
  glVertex2f(x_grid_width, y_grid_height);
  glVertex2f(0, y_grid_height);
  glVertex2f(0, 0);
  glEnd();
  
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawLabels

void ZAICViewer::drawLabels()
{
  if(m_color_scheme == 0)
      glColor3f(0, 0, 0.6);

  gl_font(1, 12);

  //---------------------------------- Draw the two zeros
  drawText(x_offset+3, y_offset-12, "0");
  drawText(x_offset-12, y_offset+3, "0");

  //---------------------------------- Draw the domain max
  int dom_size = m_domain.getVarPoints(0);
  string dom_str = intToString(dom_size-1);
  drawText(x_offset+dom_size-10, y_offset-12, dom_str);

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

void ZAICViewer::drawText(int x, int y, string str)
{
  glRasterPos3f(x, y, 0);
  gl_draw(str.c_str());
}

//-------------------------------------------------------------
// Procedure: drawPieces

void ZAICViewer::drawPieces()
{
  PDMap *pdmap = 0;
  if(m_draw_mode == 1) 
    pdmap = ipf_one->getPDMap();
  else if(m_draw_mode == 2) 
    pdmap = ipf_two->getPDMap();
  else if(m_draw_mode == 3)
    pdmap = ipf_one->getPDMap();
  else if(m_draw_mode == 4)
    pdmap = ipf_max->getPDMap();
  else if(m_draw_mode == 5)
    pdmap = ipf_tot->getPDMap();

  if(!pdmap)
    return;

  //pdmap->print(false);
  pdmap->print(true);

  int i, pc_count = pdmap->size();
  for(i=0; i<pc_count; i++)
    drawPiece(pdmap->bx(i));

  // If draw_mode is 1, then draw the other element
  if(m_draw_mode == 3) {
    pdmap = ipf_two->getPDMap();
    pc_count = pdmap->size();
    for(i=0; i<pc_count; i++)
      drawPiece(pdmap->bx(i));
  }
}


//-------------------------------------------------------------
// Procedure: drawPiece

void ZAICViewer::drawPiece(const IvPBox *piece, int mode)
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

void ZAICViewer::setParam(string param, string value)
{
  if(param == "gridsize") {
    if((value == "reset") || (value == "default")) {
      int y_max = h() - (2 * y_offset);
      y_grid_height = y_max;
      x_grid_width = m_domain.getVarPoints(0);
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
  else if(param == "wrap_mode") {
    if((value == "reset") || (value == "default")) 
      m_wrap = true;
    else
      setBooleanOnString(m_wrap, value);

    if(m_zaic_1)
      m_zaic_1->setValueWrap(m_wrap);
    if(m_zaic_2)
      m_zaic_2->setValueWrap(m_wrap);
    setIPFunctions();
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
  else if(param == "draw_mode") {
    if((value == "one") || (value == "default"))
      m_draw_mode = 1;
    else if(value == "two")
      m_draw_mode = 2;
    else if(value == "both")
      m_draw_mode = 3;
    else if(value == "max")
      m_draw_mode = 4;
    else if(value == "sum")
      m_draw_mode = 5;
    else if(value == "toggle") {
      m_draw_mode++;
      if(m_draw_mode > 5)
	m_draw_mode = 1;
    }
    if(m_draw_mode == 1)
      m_zaic_C = m_zaic_1;
    else if(m_draw_mode == 2)
      m_zaic_C = m_zaic_2;
    else
      m_zaic_C = m_zaic_H;
  }
}
      






