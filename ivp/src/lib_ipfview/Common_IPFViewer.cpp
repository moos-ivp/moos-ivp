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
#include "Common_IPFViewer.h"
#include "GeomUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

Common_IPFViewer::Common_IPFViewer(int g_x, int g_y, int g_width, 
		     int g_height, const char *g_l)
 : Fl_Gl_Window(g_x, g_y, g_width, g_height, g_l)
{
  m_xRot         = -72;
  m_zRot         = 40;
  m_zoom         = 1;
  m_rad_extra    = 15;
  m_draw_frame   = false;
  m_draw_base    = true;
  m_ipf_owner    = false;
  m_ivp_function = 0;
  m_polar        = 1; 

  setParam("clear_color", "white");
  setParam("frame_color", "dark_red");

  m_frame_height = 250;
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
// Procedure: setParam

bool Common_IPFViewer::setParam(string param, string value)
{
  value = tolower(stripBlankEnds(value));

  if((param == "draw_frame") && (value == "toggle")) {
    if(!m_draw_frame) {
      m_draw_frame = true;
      m_draw_base  = true;
    }
    else if(m_draw_frame && m_draw_base)
      m_draw_base = false;
    else if (m_draw_frame && !m_draw_base)
      m_draw_frame = false;
  }
  else if((param == "draw_frame") && (value == "true"))
    m_draw_frame = true;
  else if((param == "draw_frame") && (value == "false"))
    m_draw_frame = false;
  else if((param == "draw_base") && (value == "true"))
    m_draw_base = true;
  else if((param == "draw_base") && (value == "false"))
    m_draw_base = false;
  else if(param == "reset_view") {
    if(value=="1")
      {m_xRot=-72; m_zRot=40;}
    else if(value=="2")
      {m_xRot=0;   m_zRot=0;}
    else if(value=="3")
      {m_xRot=-53; m_zRot=0;}
    else if(value=="4")
      {m_xRot=-72; m_zRot=122;}
  }
  else if(param == "frame_color") {
    if(value == "black")
      m_frame_red = m_frame_green = m_frame_blue = 0;
    else {
      vector<double> dv = colorParse(value);
      // If all zeros (black), interpret as bad color string.
      if((dv[0]==0) && (dv[1]==0) && (dv[2]==0))
	return(false);
      m_frame_red   = dv[0];
      m_frame_green = dv[1];
      m_frame_blue  = dv[2];
    }
    m_frame_color = value;
  }    
  else if((param == "clear_color") || (param == "back_color")) {
    if(value == "black")
      m_clear_red = m_clear_green = m_clear_blue = 0;
    else {
      vector<double> dv = colorParse(value);
      // If all zeros (black), interpret as bad color string.
      if((dv[0]==0) && (dv[1]==0) && (dv[2]==0))
	return(false);
      m_clear_red   = dv[0];
      m_clear_green = dv[1];
      m_clear_blue  = dv[2];
    }
    m_clear_color = value;
  }    
  else
    return(false);

  redraw();
  return(true);
}

//-------------------------------------------------------------
// Procedure: setParam

bool Common_IPFViewer::setParam(string param, double value)
{
  if(param == "mod_frame_height")
    m_frame_height += value;
  else if(param == "set_frame_height")
    m_frame_height = value;
  else if(param == "mod_zoom")
    m_zoom *= value;
  else if(param == "set_zoom")
    m_zoom = value;
  else if(param == "mod_radius")
    m_rad_extra *= value;
  else if(param == "set_radius")
    m_rad_extra = value;
  else if(param == "mod_x_rotation")
    m_xRot += value;
  else if(param == "set_x_rotation")
    m_xRot = value;
  else if(param == "mod_z_rotation")
    m_zRot += value;
  else if(param == "set_z_rotation")
    m_zRot = value;
  else
    return(false);

  redraw();
  return(true);
}

//-------------------------------------------------------------
// Procedure: printParams

void Common_IPFViewer::printParams()
{
  cout << "# lib_ipfview Common_IPFViewer -----------------" << endl;
  cout << "set_frame_height=" << m_frame_height << endl;
  cout << "set_zoom="         << m_zoom         << endl;
  cout << "set_radius="       << m_rad_extra    << endl;
  cout << "set_x_rotation="   << m_xRot         << endl;
  cout << "set_z_rotation="   << m_zRot         << endl;
  cout << "clear_color="      << m_clear_color  << endl;
  cout << "frame_color="      << m_frame_color  << endl;
  if(m_draw_frame)
    cout << "draw_frame=true"  << endl;
  else
    cout << "draw_frame=false" << endl;
  if(m_draw_base)
    cout << "draw_base=true"   << endl;
  else
    cout << "draw_base=false"  << endl;
}

//-------------------------------------------------------------
// Procedure: draw()

void Common_IPFViewer::draw()
{
  // Clear the window and the depth buffer
  glClearColor(m_clear_red, m_clear_green, m_clear_blue, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0,0,w(),h());

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
  //bool iOutline=false;

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
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  //glPopMatrix(); // added mikerb

  if(m_draw_frame)
    drawFrame();

  drawOwnPoint();
  Common_IPFViewer::drawIvPFunction();
  glPopMatrix();
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

  double b = -250;
  double t = -250 + (m_frame_height);

  glColor3f(m_frame_red/2, m_frame_green/2, m_frame_blue/2);
  //Color3f(0.6f, 0.4f, 0.6f);
  //glColor3f(cvect[0]/2, cvect[1]/2, cvect[2]/2);
  glShadeModel(GL_FLAT);
  
  // Either draw a full base or just the frame
  if(m_draw_base) {
    glBegin(GL_TRIANGLES);
    glVertex3f(-w,-w, b); 
    glVertex3f( w,-w, b); 
    glVertex3f( w, w, b);
    glVertex3f( w, w, b); 
    glVertex3f(-w, w, b); 
    glVertex3f(-w,-w, b);
    glEnd();
  }
  
  glColor3f(m_frame_red, m_frame_green, m_frame_blue);

  if(!m_draw_base) {
    glBegin(GL_LINE_STRIP);
    glVertex3f(-w,-w, b);  
    glVertex3f(w, -w, b);  
    glVertex3f(w,  w, b);
    glVertex3f(-w, w, b);  
    glVertex3f(-w,-w, b);
    glEnd();
  }

  //glColor3f(0.8f, 0.3f, 0.8f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(-w,-w, t);  
  glVertex3f(w, -w, t);  
  glVertex3f(w,  w, t);
  glVertex3f(-w, w, t);  
  glVertex3f(-w,-w, t);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(-w,-w, b);  
  glVertex3f(-w,-w, t);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(-w, w, b);  
  glVertex3f(-w, w, t);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(w, w, b);     
  glVertex3f(w, w, t);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex3f(w, -w, b);  
  glVertex3f(w, -w, t);
  glEnd();

  glFlush();
}

//-------------------------------------------------------------
// Procedure: drawOwnPoint

void Common_IPFViewer::drawOwnPoint()
{
  if((m_xRot != 0) || (m_zRot != 0))
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

