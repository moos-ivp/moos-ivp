/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FV_Viewer.cpp                                        */
/*    DATE: May 12th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "FV_Viewer.h"
#include "MBUtils.h"
#include "Quad3D.h"
#include "OF_Reflector.h"
#include "GeomUtils.h"
#include "FunctionEncoder.h"

#define GL_PI 3.1415926f

using namespace std;

//--------------------------------------------------------------
// Constructor

FV_Viewer::FV_Viewer(int x, int y, 
	       int width, int height, const char *l)
  : Fl_Gl_Window(x,y,width,height,l)
{
  reset();
  m_base_ipf   =  100;      // For shifting the IPF rendering
  m_xstretch   = 1.0;
  m_ystretch   = 1.0;
  m_draw_frame = true;
  m_intensity  = 0.85;
  m_zoom       = 1.0;
  m_polar      = 0;
  m_zslice     = 0;

  m_ipf        = 0;
}

//-------------------------------------------------------------
// Procedure: handle

int FV_Viewer::handle(int event)
{
  return Fl_Gl_Window::handle(event);
}

//-------------------------------------------------------------
// Procedure: setIPF()

void FV_Viewer::setIPF(string str)
{
#if 1
  m_ipf_mutex.Lock();
  if(m_ipf)
    delete(m_ipf);
  m_ipf = 0;
  m_ipf_mutex.UnLock();
#endif

  m_ipf = StringToIvPFunction(str);
  redraw();
}


//-------------------------------------------------------------
// Procedure: draw()

void FV_Viewer::draw()
{
  // Clear the window and the depth buffer
  glClearColor(0.0,0.0,0.7,0.0);
  glClearColor(1.0,1.0,1.0,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0,0,w(),h());
  errCheck();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(0.0, 0.25, 0.0);

  glScalef(m_zoom, m_zoom, m_zoom);

  GLfloat nRange = 450.0f;

  // Reset projection matrix stack
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // Establish clipping volume (left, right, bottom, top, near, far)
  if(w() <= h()) 
    glOrtho (-nRange, nRange, -nRange*h()/w(), nRange*h()/w(), -nRange, nRange);
  else 
    glOrtho (-nRange*w()/h(), nRange*w()/h(), -nRange, nRange, -nRange, nRange);

  // Set color shading model to flat
  glShadeModel(GL_FLAT);
  
  // Clock wise wound polygons are front facing, this is reversed
  // because we are using triangle fans
  glFrontFace(GL_CW);

  bool iCull=false;
  bool iDepth=true;
  bool iOutline=false;

  // Turn culling on if flag is set
  if(iCull)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
  
  // Enable depth testing if flag is set
  if(iDepth)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);

  // Draw back side as a polygon only, if flag is set
  if(iOutline)
    glPolygonMode(GL_BACK,GL_LINE);
  else
    glPolygonMode(GL_BACK,GL_FILL);
  

  // Save matrix state and do the rotation
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_yRot, 0.0f, 1.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  drawIvPFunction();

  if(m_draw_frame)
    drawFrame();

  // Restore transformations
  glPopMatrix();
  
  glFlush();
  errCheck();
}

//-------------------------------------------------------------
// Procedure: drawFrame

void FV_Viewer::drawFrame()
{
  double w = 250;

  glColor3f(0.6f, 0.4f, 0.6f);
  glShadeModel(GL_FLAT);
  
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_TRIANGLES);
  glVertex3f(-w,-w,-w); glVertex3f( w,-w,-w); glVertex3f( w, w,-w);
  glVertex3f( w, w,-w); glVertex3f(-w, w,-w); glVertex3f(-w,-w,-w);
  glEnd();
  //glDisable(GL_BLEND);

  glColor3f(0.8f, 0.3f, 0.8f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(-w,-w, w);  glVertex3f(w, -w,w);  glVertex3f(w,w, w);
  glVertex3f(-w, w, w);  glVertex3f(-w,-w,w);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(-w,-w,-w);  glVertex3f(-w, -w, w);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(-w, w, -w);  glVertex3f(-w, w, w);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(w,w,-w);     glVertex3f(w, w, w);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(w, -w, -w);  glVertex3f(w, -w, w);
  glEnd();
}

//-------------------------------------------------------------
// Procedure: drawIvPFunction

void FV_Viewer::drawIvPFunction()
{
  if(m_ipf==0)
    return;

  m_ipf_mutex.Lock();

  PDMap *pdmap = m_ipf->getPDMap();
  int amt = pdmap->size();

  double hval = pdmap->getMaxWT();
  double lval = pdmap->getMinWT();
  
  IvPBox univ = pdmap->getUniverse();
  int xpts = (univ.pt(0,1) - univ.pt(0,0)) + 1;
  int ypts = (univ.pt(1,1) - univ.pt(1,0)) + 1;
  
  for(int i=0; i<amt; i++) {
    const IvPBox* ibox = pdmap->bx(i);
    drawIvPBox(*ibox, lval, hval, xpts, ypts);
  }
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: drawIvPBox

void FV_Viewer::drawIvPBox(const IvPBox &box, double lval, double hval, 
			int xpts, int ypts)
{
  if(box.getDim() != 2) {
    cout << "Wrong Box Dim" << endl;
    return;
  }
  
  if(lval==hval) {
    cout << "Dubious lval, hval in FV_Viewer::drawIvPBox()" << endl;
    return;
  }

  Quad3D q;
  q.base = m_base_ipf;
  q.xl   = box.pt(0,0);
  q.xh   = box.pt(0,1);
  q.yl   = box.pt(1,0);
  q.yh   = box.pt(1,1);
  q.xpts = xpts;
  q.ypts = ypts;

  int degree = box.getDegree();
  double pct;

  IvPBox ebox(2,1);

  if(degree == 0)
    q.llval = q.hlval = q.hhval = q.lhval = box.maxVal();
  else if(degree != 1) {
    cout << "Wrong Degree" << endl;
    return;
  }
  else {
    ebox.setPTS(0, (int)q.xl, (int)q.xl);
    ebox.setPTS(1, (int)q.yl, (int)q.yl);
    q.llval = box.ptVal(&ebox);
    pct = (q.llval-lval)/(hval-lval);
    q.llval_r = m_cmap.getIRVal(pct);
    q.llval_g = m_cmap.getIGVal(pct);
    q.llval_b = m_cmap.getIBVal(pct);

    ebox.setPTS(0, (int)q.xh, (int)q.xh);
    q.hlval = box.ptVal(&ebox);
    pct = (q.hlval-lval)/(hval-lval);
    q.hlval_r = m_cmap.getIRVal(pct);
    q.hlval_g = m_cmap.getIGVal(pct);
    q.hlval_b = m_cmap.getIBVal(pct);

    ebox.setPTS(1, (int)q.yh, (int)q.yh);
    q.hhval = box.ptVal(&ebox);
    pct = (q.hhval-lval)/(hval-lval);
    q.hhval_r = m_cmap.getIRVal(pct);
    q.hhval_g = m_cmap.getIGVal(pct);
    q.hhval_b = m_cmap.getIBVal(pct);

    ebox.setPTS(0, (int)q.xl, (int)q.xl);
    q.lhval = box.ptVal(&ebox);
    pct = (q.lhval-lval)/(hval-lval);
    q.lhval_r = m_cmap.getIRVal(pct);
    q.lhval_g = m_cmap.getIGVal(pct);
    q.lhval_b = m_cmap.getIBVal(pct);
  }

  drawQuad(q);
}

//-------------------------------------------------------------
// Procedure: drawQuad

void FV_Viewer::drawQuad(Quad3D &q)

{
  q.xl *= m_xstretch;
  q.xh *= m_xstretch;
  q.yl *= m_ystretch;
  q.yh *= m_ystretch;

  q.llval_r *= m_intensity;   q.hlval_r *= m_intensity;
  q.llval_g *= m_intensity;   q.hlval_g *= m_intensity;
  q.llval_b *= m_intensity;   q.hlval_b *= m_intensity;

  q.hhval_r *= m_intensity;   q.lhval_r *= m_intensity;
  q.hhval_g *= m_intensity;   q.lhval_g *= m_intensity;
  q.hhval_b *= m_intensity;   q.lhval_b *= m_intensity;

  q.llval = (q.llval * q.scale) + q.base; 
  q.hlval = (q.hlval * q.scale) + q.base; 
  q.hhval = (q.hhval * q.scale) + q.base; 
  q.lhval = (q.lhval * q.scale) + q.base; 

  double x0,x1,x2,x3,y0,y1,y2,y3;
  if(m_polar == 1) {
    double delta = 360.0 / q.xpts;
    projectPoint(q.xl*delta, q.yl/2, 0, 0, x0, y0);
    projectPoint(q.xh*delta, q.yl/2, 0, 0, x1, y1);
    projectPoint(q.xh*delta, q.yh/2, 0, 0, x2, y2);
    projectPoint(q.xl*delta, q.yh/2, 0, 0, x3, y3);
  }
  else if(m_polar == 2) {
    double delta = 360.0 / q.ypts;
    projectPoint(q.yl*delta, q.xl/2, 0, 0, y0, x0);
    projectPoint(q.yh*delta, q.xl/2, 0, 0, y1, x1);
    projectPoint(q.yh*delta, q.xh/2, 0, 0, y2, x2);
    projectPoint(q.yl*delta, q.xh/2, 0, 0, y3, x3);
  }      
  else {
    q.xl -= 250;  q.xh -= 250; q.yl -= 250;  q.yh -= 250;
    x0=q.xl; x1=q.xh; x2=q.xh; x3=q.xl;
    y0=q.yl; y1=q.yl; y2=q.yh; y3=q.yh;
  }

  glShadeModel(GL_SMOOTH);
  glBegin(GL_TRIANGLE_FAN);
  glColor3f(q.llval_r, q.llval_g, q.llval_b);
  glVertex3f(x0, y0, q.llval);
  
  glColor3f(q.hlval_r, q.hlval_g, q.hlval_b);
  glVertex3f(x1, y1, q.hlval);
  
  glColor3f(q.hhval_r, q.hhval_g, q.hhval_b);
  glVertex3f(x2, y2, q.hhval);
  
  glColor3f(q.lhval_r, q.lhval_g, q.lhval_b);
  glVertex3f(x3, y3, q.lhval);
  glEnd();

  if(q.lines) {
    glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_LINE_STRIP);
    glVertex3f(x0, y0, q.llval);
    glVertex3f(x1, y1, q.hlval);
    glVertex3f(x2, y2, q.hhval);
    glVertex3f(x3, y3, q.lhval);
    glEnd();
  }
}

//-------------------------------------------------------------
// Procedure: errCheck

void FV_Viewer::errCheck()
{
  GLenum err = glGetError();
  
  if(err == GL_NO_ERROR)
    return;
  else {
    cout << "WARNING!!!!! GL ERROR DETECTED!!!!" << endl;
    //cout << gluErrorString(err) << endl;
  }
}

//-------------------------------------------------------------
// Procedure: modColorMap

void FV_Viewer::modColorMap(const string &str)
{
  m_cmap.setType(str);
  redraw();
}

//-------------------------------------------------------------
// Procedure: modPolar

void FV_Viewer::modPolar(int val)
{
  m_polar = val;
  if(m_polar < 0)
    m_polar = 0;
  if(m_polar > 2)
    m_polar = 2;
  redraw();
}

//-------------------------------------------------------------
// Procedure: modZSlice

void FV_Viewer::modZSlice(int amt)
{
  m_zslice += amt;

#if 0
  int zmin = m_aof_cache.getZMin();
  int zmax = m_aof_cache.getZMax();
  
  if(m_zslice < zmin)
    m_zslice = zmin;
  if(m_zslice > zmax)
    m_zslice = zmax;
#endif

  redraw();
}





