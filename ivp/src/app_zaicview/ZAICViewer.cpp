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
  summit       = 2.5;
  peak_width   = 1.0;
  base_width   = 1.0;
  summit_delta = 20;

  summit_leq   = false;
  summit_heq   = false;
  wrap         = false;

  xlow  = 0 - (gw / 2);
  xhigh = gw / 2;
  ylow  = 0 - (gh / 2);
  yhigh = gh / 2;

  curr_mode = 0;  // 0:summit 1:peakwidth 2:basewidth 3:summitdelta
  
  zaic_peak = 0;
  zaic_leq  = 0;
  ipf       = 0;
  x_offset  = 50;
  y_offset  = 50;

  x_grid_width    = 0;
  y_grid_height   = 0;
  grid_block_size = 0;
  
  domain.addDomain("x", 0, 5, 501);
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
  if((new_mode >= 0) && (new_mode <= 3))
    curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: toggleLEQ
// Procedure: toggleHEQ

void ZAICViewer::toggleLEQ()
{
  summit_leq = !summit_leq;
  if(summit_leq)
    summit_heq = false;
  setIPFunction();
}

void ZAICViewer::toggleHEQ()
{
  summit_heq = !summit_heq;
  if(summit_heq)
    summit_leq = false;
  setIPFunction();
}

void ZAICViewer::toggleWRAP()
{
  wrap = !wrap;
  if((!summit_leq) && (!summit_heq))
    setIPFunction();
}
//-------------------------------------------------------------
// Procedure: moveX

void ZAICViewer::moveX(double delta)
{
  double dom_low  = domain.getVarLow(0);
  double dom_high = domain.getVarHigh(0);
  
  if(curr_mode==0) {   // Altering Summit
    summit += delta;
    if(summit < dom_low)
      summit = dom_low;
    if(summit > dom_high)
      summit = dom_high;
  }
  else if(curr_mode == 1) { // Altering Peak Width
    peak_width += delta;
    if(peak_width < 0)
      peak_width = 0;
  }
  else if(curr_mode == 2) { // Altering Base Width
    base_width += delta;
    if(base_width < 0)
      base_width = 0;
  }
  else if(curr_mode == 3) { // Altering Summit Delta
    summit_delta += delta *10;
    if(summit_delta < 0)
      summit_delta = 0;
    //if(summit_delta > 50)
    //  summit_delta = 50;
  }
  else
    cout << "Uh-Oh!  Mode problem" << endl;

  setIPFunction();
}


//-------------------------------------------------------------
// Procedure: setIPFunction()

void ZAICViewer::setIPFunction()
{
  if(zaic_peak)   delete(zaic_peak);
  if(zaic_leq)    delete(zaic_leq);
  if(zaic_heq)    delete(zaic_heq);

  zaic_peak  = 0;
  zaic_leq   = 0;
  zaic_heq   = 0;

  if(summit_leq) {
    cout << "Making new ZAIC_PEAK_LEQ!!!!" << endl;
    zaic_leq = new ZAIC_LEQ(domain, "x");
    zaic_leq->setSummit(summit);
    zaic_leq->setBaseWidth(base_width);
    ipf = zaic_leq->extractOF();
  }
  if(summit_heq) {
    cout << "Making new ZAIC_PEAK_HEQ!!!!" << endl;
    zaic_heq = new ZAIC_HEQ(domain, "x");
    zaic_heq->setSummit(summit);
    zaic_heq->setBaseWidth(base_width);
    ipf = zaic_heq->extractOF();
  }

  else {
    if(!wrap) {
      cout << "Making new ZAIC_PEAK!!!!" << endl;
      zaic_peak = new ZAIC_PEAK(domain, "x");
      zaic_peak->setSummit(summit);
      zaic_peak->setPeakWidth(peak_width);
      zaic_peak->setBaseWidth(base_width);
      zaic_peak->setSummitDelta(summit_delta);
      ipf = zaic_peak->extractOF();
    }
    else {
      cout << "Making new WZAIC_PEAK!!!!" << endl;
      zaic_peak = new ZAIC_PEAK(domain, "x");
      zaic_peak->setSummit(summit);
      zaic_peak->setValueWrap(true);
      zaic_peak->setPeakWidth(peak_width);
      zaic_peak->setBaseWidth(base_width);
      zaic_peak->setSummitDelta(summit_delta);
      ipf = zaic_peak->extractOF();
    }
  }

  if(ipf)
    cout << "Pieces = " << ipf->getPDMap()->size() << endl;
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
  if(!ipf)
    return;

  PDMap *pdmap = ipf->getPDMap();
  if(!pdmap)
    return;

  pdmap->print();

  int pc_count = pdmap->size();
  for(int i=0; i<pc_count; i++)
    drawPiece(pdmap->bx(i));
}


//-------------------------------------------------------------
// Procedure: drawPiece

void ZAICViewer::drawPiece(const IvPBox *piece)
{
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

  glColor4f(1.0,  1.0,  1.0,  0.1);
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




