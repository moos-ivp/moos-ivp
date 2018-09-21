/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_LEQ_Viewer.cpp                                  */
/*    DATE: Apr 6th, 2008                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <iostream>
#include <math.h>
#include <string.h>
#include "ZAIC_LEQ_Viewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "ColorParse.h"
#include "IO_Utilities.h"

using namespace std;

ZAIC_LEQ_Viewer::ZAIC_LEQ_Viewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  m_curr_mode  = 0;  // 0:summit 1:basewidth 2:minutil 3:maxutil
  m_leq_mode   = true;

  m_ipf     = 0;
  x_offset  = 50;
  y_offset  = 50;

  x_grid_width    = 0;
  y_grid_height   = 0;

  int domain_width = gw - (x_offset*2);
  m_domain.addDomain("x", 0, domain_width, domain_width+1);
  m_zaic_leq = 0;
  m_zaic_heq = 0;
  m_color_scheme = 0;

  setColorScheme(m_color_scheme);

  setParam("gridsize",  "default");
  setParam("gridshade", "default");
  setParam("backshade", "default");
  setParam("lineshade", "default");
  initZAICs();
}

//-------------------------------------------------------------
// Procedure: setColorScheme

void ZAIC_LEQ_Viewer::setColorScheme(int index)
{
  if(index == 0) {
    m_color_scheme = 0;
    m_label_color   = "0.0,0.0,0.6";
    m_line_color    = "white";
    m_display_color = "0.43,0.43,0.58";
    m_point_color   = "blue";
  }
  else if(index == 1) {
    m_label_color   = "black";
    m_line_color    = "black";
    m_display_color = "hex:df,db,c3";
    m_point_color   = "red";
  }
}

//-------------------------------------------------------------
// Procedure: setZAIC

ZAIC_LEQ_Viewer::~ZAIC_LEQ_Viewer()
{
  if(m_zaic_leq)
    delete(m_zaic_leq);
  if(m_zaic_heq)
    delete(m_zaic_heq);
}


//-------------------------------------------------------------
// Procedure: initZAICs

void ZAIC_LEQ_Viewer::initZAICs()
{
  double dom_size = (double)(m_domain.getVarPoints(0));

  double summit = (dom_size / 4.0);
  double base   = (dom_size / 10.0);
  double minutil = 20;
  double maxutil = 120;

  m_zaic_leq = new ZAIC_LEQ(m_domain, "x");  
  m_zaic_leq->setSummit(summit);
  m_zaic_leq->setBaseWidth(base);
  m_zaic_leq->setMinMaxUtil(minutil, maxutil);

  m_zaic_heq = new ZAIC_HEQ(m_domain, "x");  
  m_zaic_heq->setSummit(summit);
  m_zaic_heq->setBaseWidth(base);
  m_zaic_heq->setMinMaxUtil(minutil, maxutil);

  setIPFunction();
}

//-------------------------------------------------------------
// Procedure: draw()

void ZAIC_LEQ_Viewer::draw()
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

int ZAIC_LEQ_Viewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

//-------------------------------------------------------------
// Procedure: resize()

void ZAIC_LEQ_Viewer::resize(int gx, int gy, int gw, int gh)
{
  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: currMode

void ZAIC_LEQ_Viewer::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 4))
    m_curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: moveX

void ZAIC_LEQ_Viewer::moveX(double delta)
{
  double dom_low  = m_domain.getVarLow(0);
  double dom_high = m_domain.getVarHigh(0);
  
  if(m_curr_mode==0) {   // Altering Summit
    double summit = m_zaic_leq->getParam("summit");
    summit += delta;
    m_zaic_leq->setSummit(summit);
    m_zaic_heq->setSummit(summit);
  }
  else if(m_curr_mode == 1) { // Altering Base Width
    double base_width = m_zaic_leq->getParam("basewidth");
    base_width += delta;
    if(base_width < 0)
      base_width = 0;
    m_zaic_leq->setBaseWidth(base_width);
    m_zaic_heq->setBaseWidth(base_width);
  }
  else if(m_curr_mode == 2) { // Altering Min Utility
    double minutil = m_zaic_leq->getParam("minutil");
    double maxutil = m_zaic_leq->getParam("maxutil");
    double new_min = minutil + delta;
    if(new_min < 0)
      new_min = 0;
    if(new_min > maxutil)
      new_min = maxutil;
    if(new_min < maxutil) {
      m_zaic_leq->setMinMaxUtil(new_min, maxutil);
      m_zaic_heq->setMinMaxUtil(new_min, maxutil);
    }
  }
  else if(m_curr_mode == 3) { // Altering Max Utility
    double minutil = m_zaic_leq->getParam("minutil");
    double maxutil = m_zaic_leq->getParam("maxutil");
    double new_max = maxutil + delta;
    if(new_max < 0)
      new_max = 0;
    if(new_max > 200)
      new_max = 200;
    if(new_max > minutil) {
      m_zaic_leq->setMinMaxUtil(minutil, new_max);
      m_zaic_heq->setMinMaxUtil(minutil, new_max);
    }
  }
  else if(m_curr_mode==4) {   // Altering Summit
    double summit_delta = m_zaic_leq->getParam("summit_delta");
    summit_delta += delta;
    m_zaic_leq->setSummitDelta(summit_delta);
    m_zaic_heq->setSummitDelta(summit_delta);
  }
  else
    cout << "Uh-Oh!  Mode problem" << endl;

  setIPFunction();
}


//-------------------------------------------------------------
// Procedure: setIPFunction()

void ZAIC_LEQ_Viewer::setIPFunction()
{
  if(!m_zaic_leq || !m_zaic_heq)
    return;
  
  if(m_ipf)  
    delete(m_ipf);

  cout << "In setIPFunction(): Mode: " << m_leq_mode << endl;

  if(m_leq_mode)
    m_ipf = m_zaic_leq->extractOF();
  else
    m_ipf = m_zaic_heq->extractOF();
}

//-------------------------------------------------------------
// Procedure: getSummit()
//            getBaseWidth()

double ZAIC_LEQ_Viewer::getSummit() {
  return(m_zaic_leq->getParam("summit"));
}

double ZAIC_LEQ_Viewer::getSummitDelta() {
  return(m_zaic_leq->getParam("summit_delta"));
}

double ZAIC_LEQ_Viewer::getBaseWidth() {
  return(m_zaic_leq->getParam("basewidth"));
}

double ZAIC_LEQ_Viewer::getMinUtil() {
  return(m_zaic_leq->getParam("minutil"));
}

double ZAIC_LEQ_Viewer::getMaxUtil() {
  return(m_zaic_leq->getParam("maxutil"));
}

//-------------------------------------------------------------
// Procedure: drawAxes

void ZAIC_LEQ_Viewer::drawAxes()
{
  int x_len = 600;
  int y_len = 400;


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(x_offset, y_offset, 0);

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
  glVertex2f(x_grid_width, 0);
  glVertex2f(x_grid_width, y_grid_height);
  glVertex2f(0, y_grid_height);
  glVertex2f(0, 0);
  glEnd();
  
  double hashc = 0.6 * m_grid_shade;
  // Draw Vertical Hashmarks
  for(int i=0; i<m_domain.getVarPoints(0); i+=m_grid_block_size) {
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

void ZAIC_LEQ_Viewer::drawLabels()
{
  if(m_color_scheme == 0)
      glColor3f(0, 0, 0.6);
  if(m_color_scheme == 1)
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

#if 0
  //---------------------------------- Draw the grid Size:
  string gs = "GridSize: ";
  gs += intToString(x_grid_width);
  gs += "x";
  gs += intToString(y_grid_height);
  cout << gs << endl;
  drawText(225, 5, "a b");
#endif
}

//-------------------------------------------------------------
// Procedure: drawText

void ZAIC_LEQ_Viewer::drawText(int x, int y, string str)
{
  int slen = str.length();
  char *buff = new char(slen+1);
  strncpy(buff, str.c_str(), slen);
  buff[slen] = '\0';
  glRasterPos3f(x, y, 0);
  gl_draw(buff, slen);
  cout << "buff: " << buff << endl;
  delete(buff);
}

//-------------------------------------------------------------
// Procedure: drawPieces

void ZAIC_LEQ_Viewer::drawPieces()
{
  if(!m_ipf)
    return;

  PDMap *pdmap = m_ipf->getPDMap();
  if(!pdmap)
    return;

  pdmap->print(false);

  int i, pc_count = pdmap->size();
  for(i=0; i<pc_count; i++)
    drawPiece(pdmap->bx(i));
}


//-------------------------------------------------------------
// Procedure: drawPiece

void ZAIC_LEQ_Viewer::drawPiece(const IvPBox *piece, int mode)
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
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
  
  
  glFlush();
  glPopMatrix();  
}


//-------------------------------------------------------------
// Procedure: setParam

void ZAIC_LEQ_Viewer::setParam(string param, string value)
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
      

