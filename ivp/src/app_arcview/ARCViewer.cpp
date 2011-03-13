/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ARCViewer.cpp                                        */
/*    DATE: Jan  16th, 2011                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <iostream>
#include <cmath>
#include <cstring>
#include "ARCViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "IO_Utilities.h"

using namespace std;

ARCViewer::ARCViewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  m_curr_mode  = 0;  // 0:summit 1:peakwidth 2:basewidth 3:summitdelta

  m_ipf   = 0;
  x_offset  = 50;
  y_offset  = 50;

  x_grid_width    = 0;
  y_grid_height   = 0;

  int domain_width = gw - (x_offset*2);
  m_domain.addDomain("x", 0, domain_width, domain_width+1);
  m_zaic = 0;

  setParam("gridsize",  "default");
  setParam("gridshade", "default");
  setParam("backshade", "default");
  setParam("lineshade", "default");
  setParam("draw_mode", "default");
  setParam("wrap_mode", "default");
  setARC(2);
}

//-------------------------------------------------------------
// Procedure: setARC

void ARCViewer::setARC(int amt)
{
  double dom_size = (double)(m_domain.getVarPoints(0));

  double summit = (dom_size / 4.0);
  double peak   = (dom_size / 10.0);
  double base   = (dom_size / 10.0);
  
  m_zaic = new ZAIC_PEAK(m_domain, "x");  
  m_zaic->setSummit(summit);
  m_zaic->setPeakWidth(peak);
  m_zaic->setBaseWidth(base);
  m_zaic->setSummitDelta(25);
  m_zaic->setMinMaxUtil(0, 150);

  m_zaic->setValueWrap(true);
  setIPFunctions();
}

//-------------------------------------------------------------
// Procedure: draw()

void ARCViewer::draw()
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

int ARCViewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

//-------------------------------------------------------------
// Procedure: resize()

void ARCViewer::resize(int gx, int gy, int gw, int gh)
{
  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: currMode

void ARCViewer::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 5))
    m_curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: moveX

void ARCViewer::moveX(double delta)
{
  double dom_low  = m_domain.getVarLow(0);
  double dom_high = m_domain.getVarHigh(0);
  
  if(m_curr_mode==0) {   // Altering Summit
    double summit = m_zaic->getParam("summit");
    summit += delta;
    if(summit < dom_low)
      summit = dom_low;
    if(summit > dom_high)
      summit = dom_high;
    m_zaic->setSummit(summit);
  }
  else if(m_curr_mode == 1) { // Altering Peak Width
    double peak_width = m_zaic->getParam("peakwidth");
    peak_width += delta;
    if(peak_width < 0)
      peak_width = 0;
    m_zaic->setPeakWidth(peak_width);
  }
  else if(m_curr_mode == 2) { // Altering Base Width
    double base_width = m_zaic->getParam("basewidth");
    base_width += delta;
    if(base_width < 0)
      base_width = 0;
    m_zaic->setBaseWidth(base_width);
    cout << "Basewidth: " << base_width << endl;
  }
  else if(m_curr_mode == 3) { // Altering Summit Delta
    double summit_delta = m_zaic->getParam("summitdelta");
    summit_delta += delta;
    if(summit_delta < 0)
      summit_delta = 0;
    if(summit_delta > 100)
      summit_delta = 100;
    m_zaic->setSummitDelta(summit_delta);
  }
  else if(m_curr_mode == 4) { // Altering Max Utility
    double minutil = m_zaic->getParam("minutil");
    double maxutil = m_zaic->getParam("maxutil");
    double new_max = maxutil + delta;
    if(new_max < 0)
      new_max = 0;
    if(new_max > 200)
      new_max = 200;
    m_zaic->setMinMaxUtil(minutil, new_max);
  }
  else if(m_curr_mode == 5) { // Altering Min Utility
    double minutil = m_zaic->getParam("minutil");
    double maxutil = m_zaic->getParam("maxutil");
    double new_min = minutil + delta;
    if(new_min < 0)
      new_min = 0;
    if(new_min > maxutil)
      new_min = maxutil;
    m_zaic->setMinMaxUtil(new_min, maxutil);
  }
  else
    cout << "Uh-Oh!  Mode problem" << endl;

  setIPFunctions();
}


//-------------------------------------------------------------
// Procedure: setIPFunctions()

void ARCViewer::setIPFunctions()
{
  if(!m_zaic)
    return;

  if(m_ipf)  
    delete(m_ipf);
  
  m_ipf = m_zaic->extractOF();
}

//-------------------------------------------------------------
// Procedure: getSummit()
//            getBaseWidth()
//            getPeakWidth()
//            getSummitDelta()
//            getMaxUtil()
//            getMinUtil()

double ARCViewer::getSummit() {
  return(m_zaic->getParam("summit"));
}

double ARCViewer::getBaseWidth() {
  return(m_zaic->getParam("basewidth"));
}

double ARCViewer::getPeakWidth() {
  return(m_zaic->getParam("peakwidth"));
}

double ARCViewer::getSummitDelta() {
  return(m_zaic->getParam("summitdelta"));
}

double ARCViewer::getMaxUtil() {
  return(m_zaic->getParam("maxutil"));
}

double ARCViewer::getMinUtil() {
  return(m_zaic->getParam("minutil"));
}

//-------------------------------------------------------------
// Procedure: drawAxes

void ARCViewer::drawAxes()
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

void ARCViewer::drawLabels()
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

void ARCViewer::drawText(int x, int y, string str)
{
  int slen = str.length();
  char *buff = new char(slen+1);
  strncpy(buff, str.c_str(), slen);
  buff[slen] = '\0';
  glRasterPos3f(x, y, 0);
  gl_draw(buff, slen);
  delete(buff);
}

//-------------------------------------------------------------
// Procedure: drawPieces

void ARCViewer::drawPieces()
{
  PDMap *pdmap = m_ipf->getPDMap();
  if(!pdmap)
    return;

  pdmap->print(true);

  int i, pc_count = pdmap->size();
  for(i=0; i<pc_count; i++)
    drawPiece(pdmap->bx(i));
}


//-------------------------------------------------------------
// Procedure: drawPiece

void ARCViewer::drawPiece(const IvPBox *piece, int mode)
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

  double x1, x2, y1, y2, dx1, dx2, m, b;

  m = piece->wt(0);
  b = piece->wt(1);

  double x_base  = m_domain.getVarLow(0);
  double x_delta = m_domain.getVarDelta(0);

  x1 = piece->pt(0,0);
  x2 = piece->pt(0,1);
  dx1 = x_base = (x1 * x_delta);
  dx2 = x_base = (x2 * x_delta);
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

void ARCViewer::setParam(string param, string value)
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

    if(m_zaic)
      m_zaic->setValueWrap(m_wrap);
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
}
      

