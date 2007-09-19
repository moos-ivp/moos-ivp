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
#include "VehicleShapes.h"
#include "AngleUtils.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

#define HISTORY_SIZE  1000
#define MAX_VEHICLES    20

SSV_Viewer::SSV_Viewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_time = 0;

  m_default_vehibody = "kayak";
  m_left_click_ix    = 0;
  m_right_click_ix   = 0;
}

//-------------------------------------------------------------
// Procedure: draw()

void SSV_Viewer::draw()
{
  MarineViewer::draw();

  drawGrids();
  drawPolys();
  drawCircles();

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
// Procedure: drawVehicle(ObjectPose)

void SSV_Viewer::drawVehicle(string vname, bool active, string vehibody)
{
  unsigned int i;
  
  ObjectPose opose;
  map<string,ObjectPose>::iterator p1;
  p1 = m_pos_map.find(vname);
  if(p1 != m_pos_map.end())
    opose = p1->second;
  else 
    return;

  vector<double> cvect;
  cvect.push_back(1.0);
  cvect.push_back(0.906);
  cvect.push_back(0.243);

  map<string,vector<double> >::iterator p2;
  p2 = m_color_map.find(vname);
  if(p2 != m_color_map.end())
    cvect = p2->second;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  // Determine position in terms of image percentage
  float vehicle_ix = meters2img('x', opose.getX());
  float vehicle_iy = meters2img('y', opose.getY());

  // Determine position in terms of view percentage
  float vehicle_vx = img2view('x', vehicle_ix);
  float vehicle_vy = img2view('y', vehicle_iy);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(vehicle_vx, vehicle_vy, 0); // theses are in pixel units

  glScalef(m_zoom*m_shape_scale, m_zoom*m_shape_scale, m_zoom*m_shape_scale);

  glRotatef(-opose.getTheta(),0,0,1);  

  if(vehibody == "kayak") {
    glLineWidth(5.0);
    glColor3f(0,1,0);
    if(active)
      drawGLPoly(g_kayakBody, g_kayakBodySize, 1, 0, 0, false);
    else
      drawGLPoly(g_kayakBody, g_kayakBodySize, cvect[0], cvect[1], cvect[2], false);
    drawGLPoly(g_kayakMidOpen, g_kayakMidOpenSize, 0.5, 0.5, 0.5, false);
  }

  if(vehibody == "auv") {
    drawGLPoly(g_auvBody, g_auvBodySize, 1.0, 0.843, 0.0);
    drawGLPoly(g_auvBody, g_auvBodySize, 0.0, 0.0,   0.0, 2.0);
    drawGLPoly(g_propUnit, g_propUnitSize, 0.0,0.0, 1.0);
  }


  glPopMatrix();
  
}

//-------------------------------------------------------------
// Procedure: resetVehicles()

void SSV_Viewer::resetVehicles()
{
  m_pos_map.clear();
  m_hist_map.clear();
  m_vbody_map.clear();
}
      
//-------------------------------------------------------------
// Procedure: setVehicleBodyType
//      Note: 

void SSV_Viewer::setVehicleBodyType(const string& vname, 
				    const string& vbody)
{
  m_vbody_map[vname] = tolower(stripBlankEnds(vbody));
}  


//-------------------------------------------------------------
// Procedure: updateVehiclePosition
//      Note: We don't redraw or call "redraw" in this method since
//            if there are several updates backlogged that need 
//            to be applied, we want to apply them all to have 
//            the history, but only really want to redraw the 
//            vehicles after the last update is done.

void SSV_Viewer::updateVehiclePosition(const string& tag, float x, 
				       float y, float theta, 
				       float speed, float depth)
{
  // Handle updating the ObjectPose with the new information
  ObjectPose opose(x,y,theta,speed,depth);
  map<string,ObjectPose>::iterator p1;
  p1 = m_pos_map.find(tag);
  if(p1 != m_pos_map.end())
    p1->second = opose;
  else {
    m_pos_map[tag] = opose;
  }
 
  ColoredPoint point(x,y,0,0,255);
  map<string,CPList>::iterator p2;
  p2 = m_hist_map.find(tag);
  if(p2 != m_hist_map.end()) {
    p2->second.push_back(point);
    if(p2->second.size() > HISTORY_SIZE)
      p2->second.pop_front();
  }
  else {
    list<ColoredPoint> newlist;
    newlist.push_back(point);
    m_hist_map[tag] = newlist;
  }
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



