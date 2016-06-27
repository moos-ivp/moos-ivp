/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Common_IPFViewer.cpp                                 */
/*    DATE: Feb 13, 2007                                         */
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

#include <iostream>
#include <cstring>
#include "Common_IPFViewer.h"
#include "GeomUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

Common_IPFViewer::Common_IPFViewer(int x, int y, int wid, int hgt, 
				   const char *label)
 : Fl_Gl_Window(x, y, wid, hgt, label)
{
  m_xRot         = -72;
  m_zRot         = 40;
  m_zoom         = 1;
  m_scale        = 1;
  m_base         = 0;
  m_rad_extra    = 1;
  m_draw_pin     = true;
  m_draw_frame   = true;
  m_draw_base    = true;
  m_draw_ipf     = true;
  m_polar        = 0; 
  m_draw_pclines = true;
  m_intensity    = 1.0;
  
  setParam("clear_color", "white");
  setParam("frame_color", "dark_red");

  m_frame_height = 250;

  // 1D configuration parameters
  m_xoffset     = 50;
  m_yoffset     = hgt / 3.5;
  m_grid_width  = wid  - (m_xoffset*2);
  m_grid_height = 0.5 * hgt;
}

//-------------------------------------------------------------
// Procedure: resize

void Common_IPFViewer::resize(int x, int y, int wid, int hgt)
{
  Fl_Gl_Window::resize(x, y, wid, hgt);
  // 1D configuration parameters
  m_xoffset     = 50;
  m_yoffset     = hgt / 3.5;
  m_grid_width  = wid  - (m_xoffset*2);
  m_grid_height = 0.5 * hgt;
}

//-------------------------------------------------------------
// Procedure: handle()

int Common_IPFViewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));  // temp measure??

#if 0
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
#endif
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
  else if(param == "draw_frame")
    return(setBooleanOnString(m_draw_frame, value));
  else if(param == "draw_base")
    return(setBooleanOnString(m_draw_base, value));
  else if(param == "draw_pclines") 
    return(setBooleanOnString(m_draw_pclines, value));
  else if(param == "draw_pin")
    setBooleanOnString(m_draw_pin, value);
  else if((param == "polar") && (value == "0"))
    m_polar = 0;
  else if((param == "polar") && (value == "1"))
    m_polar = 1;
  else if((param == "polar") && (value == "2"))
    m_polar = 2;
  else if(param == "reset_view") {
    if(value=="1")
      {m_xRot=-78; m_zRot=40;}
    else if(value=="2")
      {m_xRot=0;   m_zRot=0;}
    else if(value=="3")
      {m_xRot=-53; m_zRot=0;}
    else if(value=="4")
      {m_xRot=-72; m_zRot=122;}
  }
  else if(param == "frame_color")
    m_frame_color.setColor(value);
  else if((param == "clear_color") || (param == "back_color"))
    m_clear_color.setColor(value);
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
// Procedure: clear()

void Common_IPFViewer::clear()
{
  // Clear the window and the depth buffer
  
  double clear_red = m_clear_color.red();
  double clear_grn = m_clear_color.grn();
  double clear_blu = m_clear_color.blu();
  glClearColor(clear_red, clear_grn, clear_blu, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//-------------------------------------------------------------
// Procedure: draw()

void Common_IPFViewer::draw()
{
  clear();

  //if((m_quadset.size2D() == 0) && (m_quadset.size1D() == 0))
  //  return;

  glViewport(0,0,w(),h());

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(0.0, 0.25, 0.0);

  glScalef(m_zoom, m_zoom, m_zoom);

  GLfloat nRange = 750.0f;
  //GLfloat nRange = 2850.0f;

  // Reset projection matrix stack
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Establish clipping volume (left, right, bottom, top, near, far)
  glOrtho (-nRange, nRange, -nRange*h()/w(), nRange*h()/w(), -nRange, nRange);
#if 0
  if(w() <= h()) 
    glOrtho (-nRange, nRange, -nRange*h()/w(), nRange*h()/w(), -nRange, nRange);
  else 
    glOrtho (-nRange*w()/h(), nRange*w()/h(), -nRange, nRange, -nRange, nRange);
#endif
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
}


//-------------------------------------------------------------
// Procedure: calcRadExtra()

double Common_IPFViewer::calcRadExtra()
{
  IvPDomain ivp_domain = m_quadset.getDomain();

  if(!ivp_domain.hasDomain("speed"))
    return(1);
  
  unsigned int spd_pts = ivp_domain.getVarPoints("speed");
  double min_extent = w();
  if(h() < min_extent)
    min_extent = h();

  if(spd_pts >= 1)
    return(min_extent / (double)(spd_pts));

  return(1);
}


//-------------------------------------------------------------
// Procedure: drawQuadSet

bool Common_IPFViewer::drawQuadSet(const QuadSet& quadset)
{
  if(quadset.size() != 0) 
    return(drawQuadSet2D(quadset));

#if 0
  if(qdim == 1) {
    drawQuadSet1D();
    draw1DAxes(m_quadset.getDomain());
    draw1DLabels(m_quadset.getDomain());
    draw1DLine();
  }
#endif
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: drawQuadSet1D

void Common_IPFViewer::drawQuadSet1D()
{
#if 0
  double clear_red = m_clear_color.red();
  double clear_grn = m_clear_color.grn();
  double clear_blu = m_clear_color.blu();

  glClearColor(clear_red, clear_grn, clear_blu, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(m_xoffset, m_yoffset, 0);

  ColorPack max_vertc("light_green");
  
  vector<ColorPack> key_colors;
  vector<string>    key_strings;

  unsigned int fix, fsize = m_quadset.size1DFs();
  
  for(fix=0; fix<fsize; fix++) {

    vector<double>  domain_pts     = m_quadset.getDomainPts(fix);
    vector<bool>    domain_ptsx    = m_quadset.getDomainPtsX(fix);
    vector<double>  range_vals     = m_quadset.getRangeVals(fix);
    double          range_val_max  = m_quadset.getRangeValMax(0);
    string          source         = m_quadset.getSource(fix);

    ColorPack linec("firebrick");
    if((fix==1) || (fsize == 1))
      linec.setColor("royalblue");
    if(fix==2) 
      linec.setColor("green");
    if(fix>2)  
      linec.setColor("gray");
    ColorPack vertc = linec;
    vertc.shade(-0.4);

    key_strings.push_back(source);
    key_colors.push_back(linec);

     // Draw the vertices
    glEnable(GL_POINT_SMOOTH);
    glColor3f(vertc.red(), vertc.grn(), vertc.blu());
    glPointSize(3);
    glBegin(GL_POINTS);
    unsigned int j, xsize = domain_ptsx.size();

    double domain_val_max = m_quadset.getDomain().getVarHigh(0);
    double x_stretch = ((double)(m_grid_width)  / domain_val_max);
    double y_stretch = ((double)(m_grid_height) / (double)(range_val_max));
    for(j=0; j<xsize; j++) {
      if((j==0) || (domain_ptsx[j])) {
	if((j==0) || (!domain_ptsx[j-1])) {
	  double x = domain_pts[j];
	  double y = range_vals[j];
	  if(y >= range_val_max)
	    glColor3f(max_vertc.red(), max_vertc.grn(), max_vertc.blu());
	  x *= x_stretch;
	  y *= y_stretch;
	  glVertex2f(x, y);
	  glColor3f(vertc.red(), vertc.grn(), vertc.blu());
	}
      }
    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
    
    // Draw the lines
    glColor3f(linec.red(), linec.grn(), linec.blu());
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    unsigned int i, vsize = domain_pts.size();
    for(i=0; i<vsize; i++) {
      double x = domain_pts[i] * x_stretch;
      double y = range_vals[i] * y_stretch;
      glVertex2f(x,y);

    }
    
    glEnd();
    glLineWidth(1.0);
  }
  
  glFlush();
  glPopMatrix();

  draw1DKeys(key_strings, key_colors);
#endif
}

//-------------------------------------------------------------
// Procedure: drawQuadSet2D

bool Common_IPFViewer::drawQuadSet2D(const QuadSet& quadset)
{
  IvPDomain domain = quadset.getDomain();
  double calc_rad_extra = 1;
  if(domain.hasDomain("speed")) {
    unsigned int spd_pts = domain.getVarPoints("speed");
    double min_extent = w();
    if(h() < min_extent)
      min_extent = h();
    if(spd_pts >= 1)
      calc_rad_extra = min_extent / (double)(spd_pts);
  } 
  
  unsigned int quad_cnt = quadset.size();
  if(quad_cnt == 0)
    return(false);

  m_rad_extra = calc_rad_extra;
  for(unsigned int i=0; i<quad_cnt; i++)
    //for(unsigned int i=10; i<11; i++)
    drawQuad(quadset.getQuad(i));

  return(true);
}

//-------------------------------------------------------------
// Procedure: drawQuad

void Common_IPFViewer::drawQuad(Quad3D q)
{
  double x0=q.getLLX();
  double x1=q.getHLX();
  double x2=q.getHHX();
  double x3=q.getLHX();

  double y0=q.getLLY();
  double y1=q.getHLY();
  double y2=q.getHHY();
  double y3=q.getLHY();

  // Draw the insides
  glShadeModel(GL_SMOOTH);
  glBegin(GL_TRIANGLE_FAN);


  // Draw the first two vertices
  glColor3f(q.getLLR(), q.getLLG(), q.getLLB());
  glVertex3f(x0, y0, q.getLLZ());
  glColor3f(q.getLHR(), q.getLHG(), q.getLHB());
  glVertex3f(x3, y3, q.getLHZ());


  // Draw potentially many or zero interpolated vertices common
  // in polar rendering
  unsigned int psize = q.getInPtsSize();
  for(unsigned int i=0; i<psize; i++) {
    glColor3f(q.getRinHGH(i),  q.getGinHGH(i), q.getBinHGH(i));
    glVertex3f(q.getXinHGH(i), q.getYinHGH(i), q.getZinHGH(i));
    glColor3f(q.getRinLOW(i),  q.getGinLOW(i), q.getBinLOW(i));
    glVertex3f(q.getXinLOW(i), q.getYinLOW(i), q.getZinLOW(i));
    glEnd();
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(q.getRinLOW(i),  q.getGinLOW(i), q.getBinLOW(i));
    glVertex3f(q.getXinLOW(i), q.getYinLOW(i), q.getZinLOW(i));
    glColor3f(q.getRinHGH(i),  q.getGinHGH(i), q.getBinHGH(i));
    glVertex3f(q.getXinHGH(i), q.getYinHGH(i), q.getZinHGH(i));
  }

  // Draw the last two vertices
  glColor3f(q.getHHR(), q.getHHG(), q.getHHB());
  glVertex3f(x2, y2, q.getHHZ());
  glColor3f(q.getHLR(), q.getHLG(), q.getHLB());
  glVertex3f(x1, y1, q.getHLZ());

  glEnd();


  // Draw the line edges of the piece
  if(m_draw_pclines) {
    glLineWidth(0.5);
    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_LINE_STRIP);
    glVertex3f(x0, y0, q.getLLZ());

    unsigned int psize = q.getInPtsSize();
    for(unsigned int i=0; i<psize; i++) {
      double x = q.getXinLOW(i);
      double y = q.getYinLOW(i);
      double z = q.getZinLOW(i);
      glVertex3f(x, y, z);
    }
    
    glVertex3f(x1, y1, q.getHLZ());
    glVertex3f(x2, y2, q.getHHZ());

    for(unsigned int i=0; i<psize; i++) {
      unsigned int ix = psize-i-1;
      double x = q.getXinHGH(ix);
      double y = q.getYinHGH(ix);
      double z = q.getZinHGH(ix);

      glVertex3f(x, y, z);
    }

    glVertex3f(x3, y3, q.getLHZ());
    glVertex3f(x0, y0, q.getLLZ());

    glEnd();
    glLineWidth(1.0);
  }
}

//-------------------------------------------------------------
// Procedure: drawFrame

void Common_IPFViewer::drawFrame(bool full)
{
  double w = 300;

  double b = -150;
  double t = -350 + (m_frame_height);

  double frame_red = m_frame_color.red();
  double frame_grn = m_frame_color.grn();
  double frame_blu = m_frame_color.blu();

  glColor3f(frame_red/2, frame_grn/2, frame_blu/2);
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
  
  glColor3f(frame_red, frame_grn, frame_blu);

  if(!m_draw_base) {
    glBegin(GL_LINE_STRIP);
    glVertex3f(-w,-w, b);  
    glVertex3f(w, -w, b);  
    glVertex3f(w,  w, b);
    glVertex3f(-w, w, b);  
    glVertex3f(-w,-w, b);
    glEnd();
  }

  if(full) {
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
  }

  glFlush();
}

//-------------------------------------------------------------
// Procedure: drawOwnPoint

void Common_IPFViewer::drawOwnPoint()
{
  if((m_xRot != 0) || (m_zRot != 0))
    return;
  if(m_quadset.size() == 0)
    return;

  double w = 250;

  glPointSize(3.0 * m_zoom);

  glColor3f(1.0f, 1.0f, 1.0f);
  glShadeModel(GL_FLAT);
  
  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  glVertex3f(0, 0, w); 
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glFlush();
}

//-------------------------------------------------------------
// Procedure: drawMaxPoint

void Common_IPFViewer::drawMaxPoint(double crs, double spd)
{
  if(m_quadset.size() == 0)
    return;

  // Calculated the radial extent
  IvPDomain domain = m_quadset.getDomain();
  double calc_rad_extra = 1;
  if(domain.hasDomain("speed")) {
    unsigned int spd_pts = domain.getVarPoints("speed");
    double min_extent = w();
    if(h() < min_extent)
      min_extent = h();
    if(spd_pts >= 1)
      calc_rad_extra = min_extent / (double)(spd_pts);
  } 
  // Apply the radial extent
  spd *= calc_rad_extra;

  double x,y,z=250;
  projectPoint(crs, spd, 0, 0, x, y);
  
  glPointSize(3.0 * m_zoom);

  glColor3f(1.0f, 0.5, 1.0f);
  glShadeModel(GL_FLAT);

  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  glVertex3f(x, y, z); 
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glBegin(GL_LINE_STRIP);
  glVertex3f(x, y, z); 
  glVertex3f(x, y, -z); 
  glEnd();

  glFlush();
}



//-------------------------------------------------------------
// Procedure: draw1DAxes

void Common_IPFViewer::draw1DAxes(const IvPDomain& domain)
{
#if 0
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  double  domain_rng = (domain.getVarHigh(0) - domain.getVarLow(0));
  double  units = 5;
  double  lines = (domain_rng / units);
  while(lines > 20) {
    units += 5;
    lines = (domain_rng / units);
  }
  
  int mark_line = 100;
  if(units <= 10)
    mark_line = 50;
  if(units <= 5)
    mark_line = 25;

  // The actual grid_block length will vary slightly on the horizontal 
  // axis, but this is a rough average, which is good for using as the 
  // fixed grid_block length on the verticle axis.
  int grid_block = (m_grid_width / domain_rng) * units;

  glTranslatef(m_xoffset, m_yoffset, 0);

  ColorPack hash_color      = m_clear_color;
  ColorPack hash_color_dark = m_clear_color;
  hash_color.shade(-0.07);
  hash_color_dark.shade(-0.25);
  double hash_red = hash_color.red();
  double hash_grn = hash_color.grn();
  double hash_blu = hash_color.blu();

  double hash_redd = hash_color_dark.red();
  double hash_grnd = hash_color_dark.grn();
  double hash_blud = hash_color_dark.blu();
  
  // Draw Grid outline
  glColor4f(0,  0,  0,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(0,  0);
  glVertex2f(m_grid_width, 0);
  glVertex2f(m_grid_width, m_grid_height);
  glVertex2f(0, m_grid_height);
  glVertex2f(0, 0);
  glEnd();

  // Draw Vertical Hashmarks
  double total_units = units;
  double xpos = (total_units * (m_grid_width / domain_rng)) + 1;
  while(xpos < m_grid_width) {
    if(((int)(total_units) % mark_line) == 0)
      glColor4f(hash_redd,  hash_grnd,  hash_blud,  0.1);
    else
      glColor4f(hash_red,  hash_grn,  hash_blu,  0.1);

    glBegin(GL_LINE_STRIP);
    glVertex2f(xpos, 0);
    glVertex2f(xpos, m_grid_height);

    total_units += units;
    xpos = (total_units * (m_grid_width / domain_rng)) + 1;

    glEnd();
  }

  // Draw Horizontal Hashmarks
  glColor4f(hash_red,  hash_grn,  hash_blu,  0.1);
  for(int j=0; j<m_grid_height; j+=grid_block) {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, j);
    glVertex2f(m_grid_width,  j);
    glEnd();
  }

  glFlush();
  glPopMatrix();

#endif
}



//-------------------------------------------------------------
// Procedure: draw1DLabels

void Common_IPFViewer::draw1DLabels(const IvPDomain& domain)
{
#if 0
  if(m_grid_height < 200)
    gl_font(1, 10);
  else
    gl_font(1, 12);
    
  glColor4f(0, 0, 0, 0.1);

  //---------------------------------- Draw the two zeros
  ColorPack cpack("purple");
  drawText2(m_xoffset+1,  m_yoffset-14, "0", cpack, 10);
  drawText2(m_xoffset-12, m_yoffset+1,  "0", cpack, 10);
  
  // Draw the max value on the x-axis
  double domain_hgh = domain.getVarHigh(0);
  string dh_str     = doubleToStringX(domain_hgh);
  int xpos = m_xoffset + m_grid_width - 10;
  int ypos = m_yoffset - 14;
  drawText2(xpos, ypos, dh_str, cpack, 10);

  // Draw the max value on the y-axis
  double range_hgh = m_quadset.getRangeValMax();

  string rh_str    = uintToCommaString((unsigned int)(range_hgh));
  unsigned int indent  = 2 * rh_str.length() + 2;

  xpos = (m_xoffset/2) - indent;
  ypos = m_yoffset + m_grid_height - 5;
  drawText2(xpos, ypos, rh_str, cpack, 10);

  string dom_var = domain.getVarName(0);
  xpos = m_xoffset + (m_grid_width/2.2);
  ypos = m_yoffset - 14;
  drawText2(xpos, ypos, dom_var, cpack, 10);

#endif
}

//-------------------------------------------------------------
// Procedure: draw1DKeys

void Common_IPFViewer::draw1DKeys(vector<string> key_strings, 
				  vector<ColorPack> key_colors)
{
#if 0
  if(key_strings.size() != key_colors.size())
    return;
  
  gl_font(1,10);
  unsigned int i, vsize = key_strings.size();
  double xpos = m_xoffset + (m_grid_width / 2.2);
  for(i=0; i<vsize; i++)
    drawText(xpos,  m_yoffset-(35+14*i), key_strings[i]);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glLineWidth(2.0);
  for(i=0; i<vsize; i++) {
    double kred = key_colors[i].red();
    double kgrn = key_colors[i].grn();
    double kblu = key_colors[i].blu();
    
    glColor4f(kred, kgrn, kblu,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(xpos-60, m_yoffset-(32+14*i));
    glVertex2f(xpos-10, m_yoffset-(32+14*i));
    glEnd();
  }
  glLineWidth(1.0);

  glFlush();
  glPopMatrix();
#endif
}


//-------------------------------------------------------------
// Procedure: draw1DLine

void Common_IPFViewer::draw1DLine(double val, string label)
{
#if 0
  ColorPack cpack("purple");
  double kred = cpack.red();
  double kgrn = cpack.grn();
  double kblu = cpack.blu();
  
  unsigned int dom_pts = m_quadset.getDomainPts().size();
  double x_stretch     = ((double)(m_grid_width)  / (double)(dom_pts));

  unsigned int domain_ix = m_quadset.getDomainIXMax();
  double domain_val      = m_quadset.getDomain().getVal(0, domain_ix);

  // Draw the label for the line
  string mstr = "preferred_depth = " + doubleToStringX(domain_val);
 
  double xpos = m_xoffset + ((double)(domain_ix) * x_stretch);
  double ypos = m_yoffset + m_grid_height;

  glColor3f(kred, kgrn, kblu);
  gl_font(1,10);

  double text_xpos = xpos - 50;
  if(text_xpos < (m_xoffset/2))
    text_xpos = m_xoffset/2;
  drawText(text_xpos, ypos+40, mstr);

    // Draw the line
  cpack.moregray(0.7);
  kred = cpack.red();
  kgrn = cpack.grn();
  kblu = cpack.blu();
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glColor4f(kred, kgrn, kblu,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(xpos, m_yoffset);
  glVertex2f(xpos, ypos+35);
  glEnd();

  glFlush();
  glPopMatrix();
#endif
}



//-------------------------------------------------------------
// Procedure: draw1DLine

void Common_IPFViewer::draw1DLineX(double value, 
				   string label,
				   int offset,
				   ColorPack cpack)
{
#if 0
  IvPDomain domain = m_quadset.getDomain();
  if(domain.size() != 1)
    return;

  double kred = cpack.red();
  double kgrn = cpack.grn();
  double kblu = cpack.blu();
  
  double dlow = domain.getVarLow(0);
  double dhgh = domain.getVarHigh(0);
  double range = dhgh - dlow;

  value = vclip(value, dlow, dhgh);
  
  double pct = 0;
  if(range > 0)
    pct = (value - dlow) / range;

  double xpos = pct * m_grid_width;
  double ypos = m_yoffset + m_grid_height;

  // Draw the label for the line
  string mstr = label + doubleToStringX(value);
 
  glColor3f(kred, kgrn, kblu);
  gl_font(1,10);

  double text_xpos = xpos - 50;
  if(text_xpos < (m_xoffset/2))
    text_xpos = m_xoffset/2;
  drawText(text_xpos, ypos+offset, mstr);

    // Draw the line
  cpack.moregray(0.7);
  kred = cpack.red();
  kgrn = cpack.grn();
  kblu = cpack.blu();
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glColor4f(kred, kgrn, kblu,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(xpos, m_yoffset);
  glVertex2f(xpos, ypos+35);
  glEnd();

  glFlush();
  glPopMatrix();
#endif
}



//-------------------------------------------------------------
// Procedure: drawText

void Common_IPFViewer::drawText(int x, int y, string str)
{
  if(x<0) x=0;
  if(y<0) y=0;
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(x, y, 0);

  glRasterPos3f(0, 0, 0);
  gl_draw(str.c_str());

  glFlush();
  glPopMatrix();

}

//-------------------------------------------------------------
// Procedure: drawText2

void Common_IPFViewer::drawText2(double px, double py, const string& text,
				 const ColorPack& font_c, double font_size) 
{
  if(text == "") 
    return;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  if(font_c.visible()) {
    glColor3f(font_c.red(), font_c.grn(), font_c.blu());
    gl_font(1, font_size);
    glRasterPos3f(px, py, 0);
    gl_draw(text.c_str());
  }
  glFlush();
  glPopMatrix();
}





