/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Common_IPFViewer.cpp                                 */
/*    DATE: Feb 13, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
//#include <GL/glu.h>
#include "Common_IPFViewer.h"
#include "GeomUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

Common_IPFViewer::Common_IPFViewer(int g_x, int g_y, int g_width, 
		     int g_height, const char *g_l)
 : Fl_Gl_Window(g_x, g_y, g_width, g_height, g_l)
{
  m_xRot        = -72;
  m_yRot         = 0;
  m_zRot         = 40;
  m_zoom         = 1;
  m_rad_extra    = 15;
  m_base_extra   = 0;
  m_scale_extra  = 1;
  m_draw_frame   = false;
  m_ipf_owner    = false;
  m_ivp_function = 0;
  m_polar        = 1; 

  m_clear_red    = 0;
  m_clear_green  = 0.5;
  m_clear_blue   = 0;
}

//-------------------------------------------------------------
// Destructor

Common_IPFViewer::~Common_IPFViewer()
{
  if(m_ipf_owner && m_ivp_function)
    delete(m_ivp_function);
}


//-------------------------------------------------------------
// Procedure: handle()

int Common_IPFViewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));  // temp measure??

  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handleLeftMouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handleRightMouse(vx, vy);
    }
    return(1);
    break;  
  default:
    return(Fl_Gl_Window::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: draw()

void Common_IPFViewer::draw()
{
  // Clear the window and the depth buffer
  glClearColor(m_clear_red, m_clear_green, m_clear_blue, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0,0,w(),h());
  //errCheck();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(0.0, 0.25, 0.0);

  glScalef(m_zoom, m_zoom, m_zoom);

  //GLfloat nRange = 450.0f;
  GLfloat nRange = 650.0f;

  // Reset projection matrix stack
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // Establish clipping volume (left, right, bottom, top, near, far)
  if(w() <= h()) 
    glOrtho (-nRange, nRange, -nRange*h()/w(), nRange*h()/w(), -nRange, nRange);
  else 
    glOrtho (-nRange*w()/h(), nRange*w()/h(), -nRange, nRange, -nRange, nRange);
  //glOrtho (-nRange*w()/h()-100, nRange*w()/h()+100, -nRange-100, 
  //     nRange+100, -nRange-100, nRange+100);

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


  // Save matrix state and do the rotation
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_yRot, 0.0f, 1.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  //glPopMatrix(); // added mikerb

  if(m_draw_frame)
    drawFrame();

  drawOwnPoint();
  Common_IPFViewer::drawIvPFunction();
}

// ----------------------------------------------------------
// Procedure: applyIPF
//   Purpose: 

void Common_IPFViewer::applyIPF(IvPFunction *g_ivp_function,
				bool g_ipf_owner)
{
  if(m_ipf_owner && m_ivp_function)
    delete(m_ivp_function);
  m_ivp_function = g_ivp_function;
  m_ipf_owner = g_ipf_owner;
  
  m_quadset.clear();
  
  if(m_ivp_function) {
    
    IvPDomain domain = m_ivp_function->getPDMap()->getDomain();
  
    // Case where ipf defined only over COURSE
    if((domain.hasDomain("course")) && (!domain.hasDomain("speed"))) {
      IvPDomain spd_domain;
      spd_domain.addDomain("speed", 0, 5, 41);
      ZAIC_PEAK spd_zaic(spd_domain, "speed");
      spd_zaic.setSummit(2.5);
      spd_zaic.setPeakWidth(20);
      IvPFunction *spd_of = spd_zaic.extractOF();
      OF_Coupler coupler;
      IvPFunction *new_ipf = coupler.couple(m_ivp_function, spd_of);
      //delete(m_ivp_function);
      m_ivp_function = new_ipf;
    }
    // Case where ipf defined only over SPEED
    if((!domain.hasDomain("course")) && (domain.hasDomain("speed"))) {
      IvPDomain crs_domain;
      crs_domain.addDomain("course", 0, 359, 360);
      ZAIC_PEAK crs_zaic(crs_domain, "course");
      crs_zaic.setSummit(180);
      crs_zaic.setPeakWidth(360);
      IvPFunction *crs_of = crs_zaic.extractOF();
      OF_Coupler coupler;
      m_ivp_function = coupler.couple(m_ivp_function, crs_of);
    }
  }



  if(m_ivp_function != 0) {
    FColorMap cmap;
    //if(colormap_type != "")
    //  cmap.setType(colormap_type);
    
    m_quadset.applyIPF(m_ivp_function, "course", "speed");

    //m_quadset->setAdjust(low_adjust, high_adjust);

    double lowval = m_ivp_function->getPDMap()->getMinWT();
    double hghval = m_ivp_function->getPDMap()->getMaxWT();
    m_quadset.applyColorMap(cmap, lowval, hghval);
  }

}


//-------------------------------------------------------------
// Procedure: drawIvPFunction

void Common_IPFViewer::drawIvPFunction()
{
  int quad_cnt = m_quadset.size();
  for(int i=0; i<quad_cnt; i++) {
    Quad3D quad = m_quadset.getQuad(i);
    drawQuad(quad);
  }
}

//-------------------------------------------------------------
// Procedure: drawQuad

void Common_IPFViewer::drawQuad(Quad3D &q)

{
  double m_intensity   = 1.0;

  if(m_polar == 2) {
    q.xl *= m_rad_extra;
    q.xh *= m_rad_extra;
  }
  else if(m_polar == 1) {
    q.yl *= m_rad_extra;
    q.yh *= m_rad_extra;
  }

  q.llval_r *= m_intensity;   q.hlval_r *= m_intensity;
  q.llval_g *= m_intensity;   q.hlval_g *= m_intensity;
  q.llval_b *= m_intensity;   q.hlval_b *= m_intensity;

  q.hhval_r *= m_intensity;   q.lhval_r *= m_intensity;
  q.hhval_g *= m_intensity;   q.lhval_g *= m_intensity;
  q.hhval_b *= m_intensity;   q.lhval_b *= m_intensity;

  q.llval = (q.llval * q.scale * m_scale_extra) + q.base + m_base_extra; 
  q.hlval = (q.hlval * q.scale * m_scale_extra) + q.base + m_base_extra; 
  q.hhval = (q.hhval * q.scale * m_scale_extra) + q.base + m_base_extra; 
  q.lhval = (q.lhval * q.scale * m_scale_extra) + q.base + m_base_extra; 

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
    glLineWidth(0.5);
    glColor3f(1.0, 1.0, 1.0);
    //glColor3f(0.4, 0.4, 0.4);
    //glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_LINE_STRIP);
    glVertex3f(x0, y0, q.llval+1);
    glVertex3f(x1, y1, q.hlval+1);
    glVertex3f(x2, y2, q.hhval+1);
    glVertex3f(x3, y3, q.lhval+1);
    glEnd();
    glLineWidth(1.0);
  }
}

//-------------------------------------------------------------
// Procedure: drawFrame

void Common_IPFViewer::drawFrame()
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

  glFlush();
}

//-------------------------------------------------------------
// Procedure: drawOwnPoint

void Common_IPFViewer::drawOwnPoint()
{
  if((m_xRot != 0) || (m_yRot != 0) || (m_zRot != 0))
    return;

  double w = 250;

  glPointSize(8.0 * m_zoom);

  glColor3f(1.0f, 1.0f, 1.0f);
  glShadeModel(GL_FLAT);
  
  glBegin(GL_POINTS);
  glVertex3f(0, 0, w); 
  glEnd();

  glFlush();
}

