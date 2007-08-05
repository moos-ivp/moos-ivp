/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Viewer.cpp                                           */
/*    DATE: Apr 15th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Viewer.h"
#include "MBUtils.h"
#include "Quad3D.h"
#include "OF_Reflector.h"
#include "GeomUtils.h"

#define GL_PI 3.1415926f

using namespace std;

//--------------------------------------------------------------
// Constructor

Viewer::Viewer(int x, int y, 
	       int width, int height, const char *l)
  : Fl_Gl_Window(x,y,width,height,l)
{
  reset();
  m_base_aof   = -100;      // For shifting the AOF rendering
  m_base_ipf   =  100;      // For shifting the IPF rendering
  m_scale      = 1.0;       // Amplifier for the AOF rendering
  m_patch      = 20;         // Size of patch rendering the AOF
  m_ipf_ix     = 0;         // Index of the IvPFunction to draw
  m_xstretch   = 1.0;
  m_ystretch   = 1.0;
  m_draw_aof   = true;
  m_draw_ipf   = true;
  m_draw_frame = true;
  m_unif_ipf   = 0;
  m_intensity  = 0.85;
  m_unifsize   = 10;
  m_zoom       = 1.0;
  m_polar      = 0;
  m_zslice     = 0;
  m_strict_rng = false;
}

//-------------------------------------------------------------
// Procedure: handle

int Viewer::handle(int event)
{
  return Fl_Gl_Window::handle(event);
}

//-------------------------------------------------------------
// Procedure: setAOF

void Viewer::setAOF(AOF *aof)
{
  m_aof_cache.setAOF(aof);

  int zmin = m_aof_cache.getZMin();
  int zmax = m_aof_cache.getZMax();
  int amt  = zmax - zmin + 1;
  if(amt > 0)
    m_zslice = (amt / 2) + zmin;
  else
    m_zslice = 0;
}

//-------------------------------------------------------------
// Procedure: modIndexIPF

void Viewer::modIndexIPF(int new_ix)
{
  m_ipf_ix = new_ix; 

  int sz = m_ipfs.size();

  if(m_ipf_ix >= sz) 
    m_ipf_ix = m_ipfs.size()-1;

  redraw();
}

//-------------------------------------------------------------
// Procedure: runScript

void Viewer::runScript()
{
  int save_file_ix = 0;
  for(int i=0; i<360; i++) {
    //for(int i=0; i<1; i++) {
    for(int j=0; j<1; j++) {
      rotateZ(1.0);
      redraw();
      Fl::flush();
      capture(save_file_ix);
      save_file_ix++;
    }
  }
}


//-------------------------------------------------------------
// Procedure: capture

void Viewer::capture(int save_file_ix)
{
  //string collect = "1024x768";
  string collect = "720x480";
  //string collect = "800x600";
  //string collect = "640x480";

  string command;
  command += "import -quality 90 -window aof-ipf-function-viewer ";
  command += "-crop " + collect + "+50+90 save_file_";
  if(save_file_ix < 10)   command += "0";
  if(save_file_ix < 100)  command += "0";
  if(save_file_ix < 1000) command += "0";
  command += intToString(save_file_ix) + ".png";
  system(command.c_str());
}

//-------------------------------------------------------------
// Procedure: draw()

void Viewer::draw()
{
#if 0
  double clear_red   =  73.0 / 256.0;
  double clear_green =  62.0 / 256.0;
  double clear_blue  = 120.0 / 256.0;
#endif

  double clear_red   =  1.0;
  double clear_green =  1.0;
  double clear_blue  = 1.0;

  // Clear the window and the depth buffer
  glClearColor(1.0,1.0,1.0,0.0);
  glClearColor(clear_red, clear_green, clear_blue, 0.0);
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

  if(m_draw_ipf) {
    if((m_ipf_ix == -1) && (m_unif_ipf != 0))
      drawIvPFunction(m_unif_ipf);
    else if(m_ipfs.size() != 0)
      if(m_ipf_ix >= 0)
	if(m_ipf_ix < m_ipfs.size())
	  drawIvPFunction(m_ipfs[m_ipf_ix]);
  }

  if(m_draw_aof) {
    if(m_aof_cache.getAOF())
      drawAOF();
  }

  if(m_draw_frame)
    drawFrame();

  // Restore transformations
  glPopMatrix();
  
  glFlush();
  errCheck();
}

//-------------------------------------------------------------
// Procedure: drawFrame

void Viewer::drawFrame()
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

void Viewer::drawIvPFunction(IvPFunction *ipf)
{
  PDMap *pdmap = ipf->getPDMap();
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
}

//-------------------------------------------------------------
// Procedure: drawIvPBox

void Viewer::drawIvPBox(const IvPBox &box, double lval, double hval, 
			int xpts, int ypts)
{
  if(box.getDim() != 2) {
    cout << "Wrong Box Dim" << endl;
    return;
  }
  
  if(lval==hval) {
    cout << "Dubious lval, hval in Viewer::drawIvPBox()" << endl;
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

void Viewer::drawQuad(Quad3D &q)

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
// Procedure: drawAOF

void Viewer::drawAOF()
{
  AOF *aof = m_aof_cache.getAOF();
  if(!aof)
    return;

  IvPDomain domain = aof->getDomain();
  int dim = domain.size();
  if((dim != 2) && (dim != 3))
    return;
  
  int xmin = 0;
  int xmax = domain.getVarPoints(0)-1;
  int ymin = 0;
  int ymax = domain.getVarPoints(1)-1;

  int yc = ymin;
  int xc = xmin;
  Quad3D q;
  q.base = m_base_aof; 
  q.scale= m_scale; 
  q.lines= false;
  q.xpts = (xmax - xmin) + 1;
  q.ypts = (ymax - ymin) + 1;
  while(yc < ymax) {
    xc = xmin; 
    while(xc < xmax) {
      q.xl = xc;
      q.xh = xc + m_patch;
      q.yl = yc;
      q.yh = yc + m_patch;
      if(q.xh > xmax) q.xh = xmax;
      if(q.yh > ymax) q.yh = ymax;

      q.llval   = m_aof_cache.getFVal((int)q.xl, (int)q.yl, m_zslice);
      q.llval_r = m_aof_cache.getRVal((int)q.xl, (int)q.yl, m_zslice);
      q.llval_g = m_aof_cache.getGVal((int)q.xl, (int)q.yl, m_zslice);
      q.llval_b = m_aof_cache.getBVal((int)q.xl, (int)q.yl, m_zslice);

      q.hlval   = m_aof_cache.getFVal((int)q.xh, (int)q.yl, m_zslice);
      q.hlval_r = m_aof_cache.getRVal((int)q.xh, (int)q.yl, m_zslice);
      q.hlval_g = m_aof_cache.getGVal((int)q.xh, (int)q.yl, m_zslice);
      q.hlval_b = m_aof_cache.getBVal((int)q.xh, (int)q.yl, m_zslice);

      q.hhval   = m_aof_cache.getFVal((int)q.xh, (int)q.yh, m_zslice);
      q.hhval_r = m_aof_cache.getRVal((int)q.xh, (int)q.yh, m_zslice);
      q.hhval_g = m_aof_cache.getGVal((int)q.xh, (int)q.yh, m_zslice);
      q.hhval_b = m_aof_cache.getBVal((int)q.xh, (int)q.yh, m_zslice);

      q.lhval   = m_aof_cache.getFVal((int)q.xl, (int)q.yh, m_zslice);
      q.lhval_r = m_aof_cache.getRVal((int)q.xl, (int)q.yh, m_zslice);
      q.lhval_g = m_aof_cache.getGVal((int)q.xl, (int)q.yh, m_zslice);
      q.lhval_b = m_aof_cache.getBVal((int)q.xl, (int)q.yh, m_zslice);

      drawQuad(q);
      xc += m_patch;
    }
    yc += m_patch;
  }  
}

//-------------------------------------------------------------
// Procedure: errCheck

void Viewer::errCheck()
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
// Procedure: makeUniformIPF

void Viewer::makeUniformIPF(int usize)
{
  AOF *aof = m_aof_cache.getAOF();
  if(!aof)
    return;

  if(usize <= 0)
    usize = m_unifsize;
  else
    m_unifsize = usize;

  IvPBox unifbox(2,1);
  unifbox.setPTS(0, usize, usize);
  unifbox.setPTS(1, usize, usize);
  OF_Reflector reflector(aof, 1);

  reflector.setStrictRange(m_strict_rng);

#if 1
  reflector.createUniform(&unifbox, &unifbox);
#endif
#if 0
  reflector.createUniform(&unifbox, &unifbox, 10);
  reflector.createPriority(1000, 0.01);
#endif

  if(m_unif_ipf)
    delete(m_unif_ipf);

  // false means do not normalize as part of extractOF()
  m_unif_ipf = reflector.extractOF(false);
  m_ipf_ix = -1;
}

//-------------------------------------------------------------
// Procedure: modColorMap

void Viewer::modColorMap(const string &str)
{
  m_cmap.setType(str);
  m_cmap.applyMidWhite(0.3, 0);
  m_aof_cache.applyFColorMap(m_cmap);
  redraw();
}

//-------------------------------------------------------------
// Procedure: modPolar

void Viewer::modPolar(int val)
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

void Viewer::modZSlice(int amt)
{
  m_zslice += amt;

  int zmin = m_aof_cache.getZMin();
  int zmax = m_aof_cache.getZMax();
  
  if(m_zslice < zmin)
    m_zslice = zmin;
  if(m_zslice > zmax)
    m_zslice = zmax;
  redraw();
}





