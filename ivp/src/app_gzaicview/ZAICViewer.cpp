/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAICViewer.cpp                                       */
/*    DATE: June 17th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <iostream>
#include <math.h>
#include "ZAICViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

ZAICViewer::ZAICViewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  domain.addDomain("x", 0, 359, 360);
  zaic = new ZAIC_PEAK(domain, "x");

  zaic->setSummit(180.0);
  zaic->setPeakWidth(90);
  zaic->setBaseWidth(45);
  zaic->setSummitDelta(15);
  zaic->setMinMaxUtil(0, 100);

#if 0
  int index1 = zaic->addSummit();

  zaic->setSummit(80, index1);
  zaic->setBaseWidth(180, index1);
  zaic->setPeakWidth(0, index1);
  zaic->setSummitDelta(0, index1);
  zaic->setMinMaxUtil(0, 100, index1);
#endif

  zaic->setValueWrap(true);

  xlow  = 0 - (gw / 2);
  xhigh = gw / 2;
  ylow  = 0 - (gh / 2);
  yhigh = gh / 2;

  curr_mode = 0;  // 0:summit 1:peakwidth 2:basewidth 3:summitdelta
  
  ipf_max   = 0;
  ipf_tot   = 0;
  x_offset  = 50;
  y_offset  = 50;

  x_grid_width    = 0;
  y_grid_height   = 0;
  grid_block_size = 0;
  draw_total      = true;

  zaic_index = 0;
  setIPFunction();
}

//-------------------------------------------------------------
// Procedure: draw()

void ZAICViewer::draw()
{
  glClearColor(0.5,0.5,0.5 ,0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, w(), h());

  setGridSize();
  drawAxes();
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
  xlow  = 0 - (gw / 2);
  xhigh = gw / 2;
  ylow  = 0 - (gh / 2);
  yhigh = gh / 2;

  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: currMode

void ZAICViewer::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 4))
    curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: toggleWRAP()

void ZAICViewer::toggleWRAP()
{
  if(!zaic)
    return;
  bool wrap = zaic->getValueWrap();
  zaic->setValueWrap(!wrap);
  setIPFunction();
}
//-------------------------------------------------------------
// Procedure: moveX

void ZAICViewer::moveX(double delta)
{
  if(!zaic)
    return;

  double dom_low  = domain.getVarLow(0);
  double dom_high = domain.getVarHigh(0);
  
  if(curr_mode==0) {   // Altering Summit
    double summit = zaic->getParam("summit", zaic_index);
    summit += delta;
    if(summit < dom_low)
      summit = dom_low;
    if(summit > dom_high)
      summit = dom_high;
    zaic->setSummit(summit, zaic_index);
  }
  else if(curr_mode == 1) { // Altering Peak Width
    double peak_width = zaic->getParam("peakwidth", zaic_index);
    peak_width += delta;
    if(peak_width < 0)
      peak_width = 0;
    zaic->setPeakWidth(peak_width, zaic_index);
  }
  else if(curr_mode == 2) { // Altering Base Width
    double base_width = zaic->getParam("basewidth", zaic_index);
    base_width += delta;
    if(base_width < 0)
      base_width = 0;
    zaic->setBaseWidth(base_width, zaic_index);
  }
  else if(curr_mode == 3) { // Altering Summit Delta
    double summit_delta = zaic->getParam("summitdelta", zaic_index);
    summit_delta += delta;
    if(summit_delta < 0)
      summit_delta = 0;
    if(summit_delta > 100)
      summit_delta = 100;
    zaic->setSummitDelta(summit_delta, zaic_index);
  }
  else if(curr_mode == 4) { // Altering Summit Delta
    double summit_height = zaic->getParam("maxutil", zaic_index);
    summit_height += delta;
    if(summit_height < 0)
      summit_height = 0;
    if(summit_height > 150)
      summit_height = 150;
    zaic->setMinMaxUtil(0, summit_height, zaic_index);
  }
  else
    cout << "Uh-Oh!  Mode problem" << endl;

  setIPFunction();
}


//-------------------------------------------------------------
// Procedure: setIPFunction()

void ZAICViewer::setIPFunction()
{
  if(!zaic)
    return;

  if(ipf_max) 
    delete(ipf_max);

  if(ipf_tot) 
    delete(ipf_tot);
  
  ipf_max = zaic->extractOF();
  ipf_tot = zaic->extractOF(false);

  if(ipf_max)
    cout << "Pieces = " << ipf_max->getPDMap()->size() << endl;
}

//-------------------------------------------------------------
// Procedure: alterIndex

void ZAICViewer::alterIndex(int delta)
{
  if(!zaic)
    return;
  int summit_count = zaic->getSummitCount();

  zaic_index += delta;
  if(zaic_index < 0)
    zaic_index = 0;
  if(zaic_index > (summit_count - 1))
    zaic_index = summit_count - 1;
}

//-------------------------------------------------------------
// Procedure: getSummit()
//            getBaseWidth()
//            getPeakWidth()
//            getSummitDelta()

double ZAICViewer::getSummit() 
{
  if(!zaic)
    return(0);
  return(zaic->getParam("summit", zaic_index));
}

double ZAICViewer::getBaseWidth() 
{
  if(!zaic)
    return(0);
  return(zaic->getParam("basewidth", zaic_index));
}

double ZAICViewer::getPeakWidth() 
{
  if(!zaic)
    return(0);
  return(zaic->getParam("peakwidth", zaic_index));
}

double ZAICViewer::getSummitDelta() 
{
  if(!zaic)
    return(0);
  return(zaic->getParam("summitdelta", zaic_index));
}

double ZAICViewer::getSummitHeight() 
{
  if(!zaic)
    return(0);
  return(zaic->getParam("maxutil", zaic_index));
}

//-------------------------------------------------------------
// Procedure: setGridSize

void ZAICViewer::setGridSize()
{
  int x_max = w() - (2 * x_offset);
  int y_max = h() - (2 * y_offset);

  y_grid_height = y_max;
  
  int dom_size = domain.getVarPoints(0);
  
  x_grid_width = dom_size;

  grid_block_size = 50;
}

//-------------------------------------------------------------
// Procedure: drawAxes

void ZAICViewer::drawAxes()
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
  double r=0.65;
  double g=0.65;
  double b=0.9;
  glEnable(GL_BLEND);
  glColor4f(r,g,b,0.1);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_POLYGON);
  glVertex2f(0,  0);
  glVertex2f(x_grid_width, 0);
  glVertex2f(x_grid_width, y_grid_height);
  glVertex2f(0, y_grid_height);
  glVertex2f(0, 0);
  glEnd();
  glDisable(GL_BLEND);

  // Draw Vertical Hashmarks
  for(int i=0; i<domain.getVarPoints(0); i+=grid_block_size) {
    glColor4f(0.6,  0.6,  0.6,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(i, 0);
    glVertex2f(i, y_grid_height);
    glEnd();
  }

  // Draw Horizontal Hashmarks
  for(int j=0; j<y_grid_height; j+=grid_block_size) {
    glColor4f(0.6,  0.6,  0.6,  0.1);
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
// Procedure: drawPieces

void ZAICViewer::drawPieces()
{
  PDMap *pdmap_max = 0;
  PDMap *pdmap_tot = 0;

  if(!ipf_max)
    return;

  // Draw the IPF from a ZAIC in "Maximum" mode
  pdmap_max = ipf_max->getPDMap();
  if(!pdmap_max)
    return;

  pdmap_max->print();
  //pdmap_max->print(false);

  int pc_count = pdmap_max->size();
  for(int i=0; i<pc_count; i++)
    drawPiece(pdmap_max->bx(i));


  // Draw the IPF from a ZAIC in "Total" mode
  if(!draw_total)
    return;

  pdmap_tot = ipf_tot->getPDMap();
  if(!pdmap_tot) {
    return;
  }

  pc_count = pdmap_tot->size();
  for(int j=0; j<pc_count; j++)
    drawPiece(pdmap_tot->bx(j), 1);
}


//-------------------------------------------------------------
// Procedure: drawPiece

void ZAICViewer::drawPiece(const IvPBox *piece, int mode)
{
  double red_val = 1.0;
  double grn_val = 1.0;
  double blu_val = 1.0;

  if(mode==1) {
    red_val = 0.7;
    grn_val = 0.1;
    blu_val = 0.4;
  }

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

  double x_base  = domain.getVarLow(0);
  double x_delta = domain.getVarDelta(0);

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




