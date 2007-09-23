/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_Viewer.cpp                                       */
/*    DATE: September 11th, 2007                                 */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>
#include "SSV_Viewer.h"
#include "AngleUtils.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

#define HISTORY_SIZE  1000
#define MAX_VEHICLES    20

SSV_Viewer::SSV_Viewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_time        = 0;
  m_tiff_offon  = 0;

  m_default_vehibody = "kayak";
  m_left_click_ix    = 0;
  m_right_click_ix   = 0;
  m_centric_view     = true;
  m_radial_size      = 100;
}

//-------------------------------------------------------------
// Procedure: draw()

void SSV_Viewer::draw()
{
  MarineViewer::draw();

  drawGrids();
  drawPolys();
  drawCircles();
  drawRadials();

  // Next draw the vehicle shapes. If the vehicle index is the 
  // one "active", draw it in a different color.
  int ix = 0;
  map<string,ObjectPose>::iterator p1;
  for(p1=m_pos_map.begin(); p1!=m_pos_map.end(); p1++) {
    string vname = p1->first;
    bool active = (ix == m_global_ix);
    
    string vehibody = m_vbody_map[vname];
    if(vehibody == "")
      vehibody = m_default_vehibody;
    drawVehicle(vname, active, vehibody);
    ix++;
  }

  // Finally, draw the history points for each vehicle.
  map<string,CPList>::iterator p2;
  for(p2=m_hist_map.begin(); p2!=m_hist_map.end(); p2++)
    drawPoints(p2->second);

  glFlush();
}

//-------------------------------------------------------------
// Procedure: handle

int SSV_Viewer::handle(int event)
{
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
// Procedure: updateVehiclePosition
//      Note: We don't redraw or call "redraw" in this method since
//            if there are several updates backlogged that need 
//            to be applied, we want to apply them all to have 
//            the history, but only really want to redraw the 
//            vehicles after the last update is done.

void SSV_Viewer::updateVehiclePosition(string vname, float x, 
				       float y, float theta, 
				       float speed, float depth)
{
  vname = toupper(vname);
  // Handle updating the ObjectPose with the new information
  ObjectPose opose(x,y,theta,speed,depth);
  map<string,ObjectPose>::iterator p1;
  p1 = m_pos_map.find(vname);
  if(p1 != m_pos_map.end())
    p1->second = opose;
  else {
    m_pos_map[vname] = opose;
  }
 
  ColoredPoint point(x,y,0,0,255);
  map<string,CPList>::iterator p2;
  p2 = m_hist_map.find(vname);
  if(p2 != m_hist_map.end()) {
    p2->second.push_back(point);
    if(p2->second.size() > HISTORY_SIZE)
      p2->second.pop_front();
  }
  else {
    list<ColoredPoint> newlist;
    newlist.push_back(point);
    m_hist_map[vname] = newlist;
  }
  
  if((m_centric_view) && (toupper(vname) == m_ownship_name)) {
    setParam("set_pan_x", -x);
    setParam("set_pan_y", -y);
  }
}

//-------------------------------------------------------------
// Procedure: setVehicleBodyType
//      Note: 

void SSV_Viewer::setVehicleBodyType(string vname, string vbody)
{
  vname = toupper(vname);
  m_vbody_map[vname] = tolower(stripBlankEnds(vbody));
}  


//-------------------------------------------------------------
// Procedure: setOwnShipName

void SSV_Viewer::setOwnShipName(string vname)
{
  vname = toupper(vname);
  m_ownship_name = toupper(vname);
}
      
//-------------------------------------------------------------
// Procedure: resetVehicles()

void SSV_Viewer::resetVehicles()
{
  m_pos_map.clear();
  m_hist_map.clear();
  m_vbody_map.clear();
}
      
// ----------------------------------------------------------
// Procedure: getMetersX
//   Purpose: For a given x position, return its position, in 
//            terms of delta meters from the zero position.
//            Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the x position 
//            of the cross-hairs

float SSV_Viewer::getMetersX(int index)
{
  if(m_cross_offon) {
    int iwidth = m_back_img.get_img_width();
    float x_pos = ((float)(iwidth) / 2.0) - (float)(m_vshift_x);
    float x_pct = m_back_img.pixToPctX(x_pos);
    float x_pct_cent = m_back_img.get_img_centx();
    float x_pct_mtrs = m_back_img.get_img_meters();
    float meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
    return(meters);
  }
  
  ObjectPose opose = getObjectPoseByIndex(index);
  return(opose.getX());
}

// ----------------------------------------------------------
// Procedure: getMetersY
//   Purpose: For a given y position, return its position, in 
//            terms of delta meters from the zero position.
//            Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the y position 
//            of the cross-hairs

float SSV_Viewer::getMetersY(int index)
{
  if(m_cross_offon) {
    int iheight = m_back_img.get_img_height();
    float y_pos = ((float)(iheight) / 2.0) - (float)(m_vshift_y);
    float y_pct = m_back_img.pixToPctY(y_pos);
    float y_pct_cent = m_back_img.get_img_centy();
    float y_pct_mtrs = m_back_img.get_img_meters();
    float meters = (y_pct - y_pct_cent) / (y_pct_mtrs / 100.0);
    return(meters);
  }

  ObjectPose opose = getObjectPoseByIndex(index);
  return(opose.getY());
}

// ----------------------------------------------------------
// Procedure: getSpd
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the x position 
//            of the cross-hairs

float SSV_Viewer::getSpd(int index)
{
  if(m_cross_offon)
    return(0.0);
    
  ObjectPose opose = getObjectPoseByIndex(index);
  return(opose.getSpeed());
}

// ----------------------------------------------------------
// Procedure: getDep
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. 

float SSV_Viewer::getDep(int index)
{
  if(m_cross_offon)
    return(0.0);
  
  ObjectPose opose = getObjectPoseByIndex(index);
  return(opose.getDepth());
}

// ----------------------------------------------------------
// Procedure: getCrs
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the x position 
//            of the cross-hairs

float SSV_Viewer::getCrs(int index)
{
  if(m_cross_offon)
    return(0.0);

  ObjectPose opose = getObjectPoseByIndex(index);
  return(opose.getTheta());
}

// ----------------------------------------------------------
// Procedure: getVehiName
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. Anything outside this range 
//            results in an empty string being returned.

string SSV_Viewer::getVehiName(int index)
{
  if((m_cross_offon) || (index == -1))
    return("cross-hairs");

  int ix = index;
  map<string,ObjectPose>::iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    if(ix==0)
      return(p->first);
    else
      ix--;
  }
  return("");
}

//-------------------------------------------------------------
// Procedure: drawVehicle(ObjectPose)

void SSV_Viewer::drawVehicle(string vname, bool active, string vehibody)
{
  ObjectPose opose;
  map<string,ObjectPose>::iterator p1;
  p1 = m_pos_map.find(vname);
  if(p1 != m_pos_map.end())
    opose = p1->second;
  else 
    return;

  // The default "non-active vehicle" color
  double red = 1.0;
  double grn = 0.906;
  double blu = 0.243;

  map<string,vector<double> >::iterator p2;
  p2 = m_color_map.find(vname);
  if(p2 != m_color_map.end()) {
    red = p2->second[0];
    grn = p2->second[1];
    blu = p2->second[2];
  }
  
  // Set the color for the "active" vehicle.
  if(active) {
    red=1.0; grn=0; blu=0;
  }

  drawCommonVehicle(opose, red, grn, blu, vehibody);
}

//-------------------------------------------------------------
// Procedure: drawPoints

void SSV_Viewer::drawPoints(CPList &cps)
{
  if(!m_trails) 
    return;

  list<ColoredPoint>::iterator p;

  int i=0;
  for(p=cps.begin(); p!=cps.end(); p++) {
    if((i % m_trail_gap) == 0) {
      if(p->isValid()) 
	drawPoint(p->m_x, p->m_y, m_trail_color);
    }
    i++;
  }
}

//-------------------------------------------------------------
// Procedure: drawPoint

void SSV_Viewer::drawPoint(float px, float py, int color)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Determine position in terms of image percentage
  float pt_ix = meters2img('x', px);
  float pt_iy = meters2img('y', py);

  // Determine position in terms of view percentage
  float pt_vx = img2view('x', pt_ix);
  float pt_vy = img2view('y', pt_iy);

  glTranslatef(pt_vx, pt_vy, 0); // theses are in pixel units

  float pt_size = ((float)(m_trail_size) / 1.0) * m_zoom;

  glEnable(GL_POINT_SMOOTH);
  glPointSize(pt_size);
  
  if(color==0)  glColor3f(0, 0, 1);
  if(color==1)  glColor3f(0, 1, 0);
  if(color==2)  glColor3f(1, 0, 0);
  
  glBegin(GL_POINTS);
  glVertex2f(0, 0);
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: getObjectPoseByIndex

ObjectPose SSV_Viewer::getObjectPoseByIndex(int index)
{
  int ix = index;
  map<string,ObjectPose>::iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    if(ix==0)
      return(p->second);
    else
      ix--;
  }

  ObjectPose op;
  return(op);
}



//-------------------------------------------------------------
// Procedure: handleLeftMouse

void SSV_Viewer::handleLeftMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);

  m_left_click =  "x=" + doubleToString(sx,1) + ",";
  m_left_click += "y=" + doubleToString(sy,1);

  m_left_click_ix++;
  
  cout << "Left Mouse click at [" << m_left_click << "] meters." << endl;
}


//-------------------------------------------------------------
// Procedure: handleRightMouse

void SSV_Viewer::handleRightMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);
  
  m_right_click =  "x=" + doubleToString(sx,1) + ",";
  m_right_click += "y=" + doubleToString(sy,1);

  m_right_click_ix++;
  
  cout << "Right Mouse click at [" << m_right_click << "] meters." << endl;
}


//-------------------------------------------------------------
// Procedure: setParam

bool SSV_Viewer::setParam(string param, string value)
{
  if(MarineViewer::setCommonParam(param, value))
    return(true);

  param = tolower(stripBlankEnds(param));
  value = tolower(stripBlankEnds(value));

  if(param == "centric_view") {
    if(value == "toggle")
      m_centric_view = !m_centric_view;
    else if(value == "on")
      m_centric_view = true;
    else if(value == "off")
      m_centric_view = false;
    else
      return(false);
  }
  else
    return(false);

  redraw();
  return(true);

}

//-------------------------------------------------------------
// Procedure: setParam

bool SSV_Viewer::setParam(string param, float v)
{
  // Intercept and disallow any panning if in "centric" mode.
  if(((param == "pan_x") || (param == "pan_y")) && (m_centric_view))
    return(true);

  if(MarineViewer::setCommonParam(param, v))
    return(true);
  
  param = tolower(stripBlankEnds(param));
  
  if(param == "radial_size") {
    m_radial_size = (int)(v);
    if(m_radial_size < 0)
      m_radial_size = 0;
    if(m_radial_size > 2000);
       m_radial_size = 2000;
  }
  else 
    return(false);
  
  redraw();
  return(true);

}


//-------------------------------------------------------------
// Procedure: drawRadials

void SSV_Viewer::drawRadials()
{
  // First determine the position of "ownship". If this isn't 
  // known we return right away.

  ObjectPose opose;
  map<string,ObjectPose>::iterator p1;
  p1 = m_pos_map.find(m_ownship_name);
  if(p1 != m_pos_map.end())
    opose = p1->second;
  else
    return;

  double px = opose.getX();
  double py = opose.getY();
  int    psize = 60;

  bool   dashed = false;
  double red=1, grn=0, blu=0;
  
  string poly_str = "radial:";
  poly_str += doubleToString(px,2) + ",";
  poly_str += doubleToString(py,2) + ",";
  poly_str += doubleToString(m_radial_size,2) + ",";
  poly_str += intToString(psize);
  
  XYPolygon poly;
  poly.initialize(poly_str);

  unsigned int i, j;
  float *points = new float[2*psize];
  
  int pindex = 0;
  for(i=0; i<psize; i++) {
    points[pindex]   = poly.get_vx(i) - m_back_img.get_img_offset_x();
    points[pindex+1] = poly.get_vy(i) - m_back_img.get_img_offset_y();

    points[pindex]   *=  m_back_img.get_pix_per_mtr();
    points[pindex+1] *=  m_back_img.get_pix_per_mtr();

    pindex += 2;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  // Draw the edges 
  glLineWidth(1.0);
  if(dashed) {
    glEnable(GL_LINE_STIPPLE);
    GLushort pattern = 0x5555;
    GLint factor = 5;
    glLineStipple(factor, pattern);
  }

  glColor3f(red, grn, blu);

  glBegin(GL_LINE_LOOP);
  for(i=0; i<psize*2; i=i+2) {
    glVertex2f(points[i], points[i+1]);
  }
  glEnd();
  if(dashed)
    glDisable(GL_LINE_STIPPLE);

  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: setCurrent

void SSV_Viewer::setCurrent(string vname)
{
  if(vname == "ownship")
    vname = m_ownship_name;

  vname = toupper(vname);

  int the_index = -1;
  int cur_index = 0;
  map<string,ObjectPose>::iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    if((p->first == vname) || (p->first == tolower(vname)))
      the_index = cur_index;
    cur_index++;
  }
  
  if(the_index != -1)
    m_global_ix = the_index;
}
