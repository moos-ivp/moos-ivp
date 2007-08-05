/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarineViewer.cpp                                     */
/*    DATE: Nov 11th 2004                                        */
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
#include "MarineViewer.h"
#include "KayakShapes.h"
#include "AngleUtils.h"
#include "MBUtils.h"

using namespace std;

#define HISTORY_SIZE  1000
#define MAX_VEHICLES    20

MarineViewer::MarineViewer(int x, int y, int w, int h, const char *l)
  : CommonMarineViewer(x,y,w,h,l)
{
  m_img_res     = 0;
  m_img_radius  = 50;
  m_time        = 0;
  m_grids_top   = true;
}

//-------------------------------------------------------------
// Procedure: draw()

void MarineViewer::draw()
{
  CommonMarineViewer::draw();

  // Next draw the objective function images, if present. 
  if(m_grids_top) {
    drawImages();
    drawGrids();
  }
  else {
    drawGrids();
    drawImages();
  }

  drawPolys();
  drawCircles();

  // Next draw the vehicle shapes. If the vehicle index is the 
  // one "active", draw it in a different color.
  int ix = 0;
  map<string,ObjectPose>::iterator p1;
  for(p1=kpos_map.begin(); p1!=kpos_map.end(); p1++) {
    bool active = (ix == m_global_ix);
    drawVehicle(p1->second, active);
    ix++;
  }

  // Finally, draw the history points for each vehicle.
  map<string,CPList>::iterator p2;
  for(p2=khist_map.begin(); p2!=khist_map.end(); p2++)
    drawPoints(p2->second);

  glFlush();
}

//-------------------------------------------------------------
// Procedure: handle

int MarineViewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

//-------------------------------------------------------------
// Procedure: drawVehicle(ObjectPose)

void MarineViewer::drawVehicle(ObjectPose opose, bool active)
{
  unsigned int i;
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  // Determine position in terms of image percentage
  float vehicle_ix = meters2img("x", opose.getX());
  float vehicle_iy = meters2img("y", opose.getY());

  // Determine position in terms of view percentage
  float vehicle_vx = img2view("x", vehicle_ix);
  float vehicle_vy = img2view("y", vehicle_iy);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(vehicle_vx, vehicle_vy, 0); // theses are in pixel units

  glScalef(m_zoom*m_shape_scale, m_zoom*m_shape_scale, m_zoom*m_shape_scale);

  glRotatef(-opose.getTheta(),0,0,1);  

#if 1
  glLineWidth(5.0);
  glColor3f(0,1,0);
  if(active)
    drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.0, 0.0, false);
  else
    drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.906, 0.243, false);
  drawGLPoly(g_kayakMidOpen, g_kayakMidOpenSize, 0.5, 0.5, 0.5, false);
#endif

#if 0
  if(active)
    drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.0, 0.0);
  else
    drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.906, 0.243);
  drawGLPoly(g_kayakMidOpen, g_kayakMidOpenSize, 0.5, 0.5, 0.5);
#endif

#if 0
  drawGLPoly(g_triangleShape, g_triangleShapeSize, 1.0, 0.0, 0.0);
#endif


  glPopMatrix();
  
}

//-------------------------------------------------------------
// Procedure: drawImages()

void MarineViewer::drawImages()
{
  // One of the vehicles is typically "active", i.e. the one whose
  // info is reflected in the data fields. We'd like the image
  // for this vehicle always to be on top, i.e., drawn last.

  // So first we identify the name of the active vehicle, and draw
  // all the other vehicle images first.

  int ix = 0;
  string active_vehicle_name = "";
  map<string,ObjectPose>::iterator p;
  for(p=kpos_map.begin(); p!=kpos_map.end(); p++) {
    bool active = (ix == m_global_ix);
    if(!active) {
      string vehicle_name = p->first;
      string bhv_name = kview_map[vehicle_name];
      for(int i=0; i<images.size(); i++) {
	if(images[i].get_vname() == vehicle_name)
	  if(images[i].get_bname() == bhv_name)
	    drawIMG(vehicle_name, images[i]);
      }
    }
    else
      active_vehicle_name = p->first;    
    ix++;
  }

  // Then we draw the image associated with the active vehicle.
  if(active_vehicle_name != "") {
    string bhv_name = kview_map[active_vehicle_name];
    for(int i=0; i<images.size(); i++) {
      if(images[i].get_vname() == active_vehicle_name)
	if(images[i].get_bname() == bhv_name)
	  drawIMG(active_vehicle_name, images[i]);
    }
  }
  
}
//-------------------------------------------------------------
// Procedure: drawIMG

void MarineViewer::drawIMG(string tag, const IMG &img)
{
  ObjectPose opose;
  map<string,ObjectPose>::iterator p1;
  p1 = kpos_map.find(tag);
  if(p1 != kpos_map.end())
    opose = p1->second;
  else {
    cout << "MarineViewer::drawIMG(): NO RECORD OF: " << tag << endl;
    return;
  }

  // Determine position in terms of image percentage
  float vehicle_ix = meters2img("x", opose.getX());
  float vehicle_iy = meters2img("y", opose.getY());

  // Determine position in terms of view percentage
  int kx = (int)(img2view("x", vehicle_ix));
  int ky = (int)(img2view("y", vehicle_iy));

  double img_hval = img.get_hval();
  double img_lval = img.get_lval();
  double img_range = img_hval - img_lval;

  //cout << "IMG for vehicle " << tag << ",  behavior " << img.get_bname() << endl;
  //cout << "highval = " << img_hval << ", lowval = " << img_lval << endl;
  
  int radius = m_img_radius;
  int ymax   = img.get_ymax();

  glPointSize(3.0f);
  glBegin(GL_POINTS);

  // The variable pix_val determines the color weight, 
  // between 0.0 and 1.0 that will be rendered.

  for(int i=kx-radius; i<=kx+radius; i++)
    for(int j=ky-radius; j<=ky+radius; j++) {
      double pix_dist = hypot((double)(i-kx), (double)(j-ky));
      if(!((i==kx) && (j==ky))) {
	if(pix_dist < (double)(radius)) {
	  
	  double angle = relAng(kx, ky, i, j);
	  double ypct  = (pix_dist / (double)radius) * (double)ymax;
	  
	  int crs  = (int)(angle);
	  int yval = (int)(ypct);
	  double img_val = img.get_ival_ix(crs, yval);
	  double pix_val = 1.0 - (img_val - img_lval) / img_range;
	  pix_val = snapToStep(pix_val, m_img_res);

	  glColor3f(pix_val, pix_val, pix_val);
	  glVertex2f(i, j);
	}
      }
    }
  glEnd();

}

//-------------------------------------------------------------
// Procedure: setInfoBHV

void MarineViewer::setInfoBHV(string tag, string info)
{
  kbhv_map[tag] = info;
}
      
//-------------------------------------------------------------
// Procedure: resetVehicles()

void MarineViewer::resetVehicles()
{
  kpos_map.clear();
  khist_map.clear();
  kbhv_map.clear();
  kview_map.clear();
}
      
//-------------------------------------------------------------
// Procedure: getInfoBHV

string MarineViewer::getInfoBHV(string tag)
{
  string rstring = "";
  map<string,string>::iterator p;
  p = kbhv_map.find(tag);
  if(p != kbhv_map.end())
    rstring = p->second;
  return(rstring);
}
      
//-------------------------------------------------------------
// Procedure: updateVehiclePosition
//      Note: We don't redraw or call "redraw" in this method since
//            if there are several updates backlogged that need 
//            to be applied, we want to apply them all to have 
//            the history, but only really want to redraw the 
//            vehicles after the last update is done.

void MarineViewer::updateVehiclePosition(string tag, float x, float y, 
					 float theta, float speed, 
					 float depth)
{
  // Handle updating the ObjectPose with the new information
  ObjectPose opose(x,y,theta,speed,depth);
  map<string,ObjectPose>::iterator p1;
  p1 = kpos_map.find(tag);
  if(p1 != kpos_map.end())
    p1->second = opose;
  else {
    kpos_map[tag] = opose;
  }
 
  map<string,string>::iterator p0;
  p0 = kview_map.find(tag);
  if(p0 == kview_map.end())
    kview_map[tag] = "collective";
    
  ColoredPoint point(x,y,0,0,255);
  map<string,CPList>::iterator p2;
  p2 = khist_map.find(tag);
  if(p2 != khist_map.end()) {
    p2->second.push_back(point);
    if(p2->second.size() > HISTORY_SIZE)
      p2->second.pop_front();
  }
  else {
    list<ColoredPoint> newlist;
    newlist.push_back(point);
    khist_map[tag] = newlist;
  }
}

//-------------------------------------------------------------
// Procedure: updateKViewMap

void MarineViewer::updateKViewMap(string vname, string bhv_name)
{
  kview_map[vname] = bhv_name;
}

// ----------------------------------------------------------
// Procedure: getImgPctX
//   Purpose: For a given x position, return its position, in 
//            terms of percentage of the background image.
//            Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the x position 
//            of the cross-hairs

float MarineViewer::getImgPctX(int index)
{
  if(m_cross_offon) {
    float x_pos = (m_back_img.get_img_width() / 2.0) - m_vshift_x;
    float x_pct = m_back_img.pixToPctX(x_pos);
    return(x_pct);
  }
  
  ObjectPose opose = getObjectPoseByIndex(index);
  return(meters2img("x", opose.getX()));
}

// ----------------------------------------------------------
// Procedure: getImgPctY
//   Purpose: For a given y position, return its position, in 
//            terms of percentage of the background image.
//            Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the y position 
//            of the cross-hairs

float MarineViewer::getImgPctY(int index)
{
  if(m_cross_offon) {
    float y_pos = (m_back_img.get_img_height() / 2.0) - m_vshift_y;
    float y_pct = m_back_img.pixToPctY(y_pos);
    return(y_pct);
  }
  
  ObjectPose opose = getObjectPoseByIndex(index);
  return(meters2img("y", opose.getY()));
}

// ----------------------------------------------------------
// Procedure: getMetersX
//   Purpose: For a given x position, return its position, in 
//            terms of delta meters from the zero position.
//            Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the x position 
//            of the cross-hairs

float MarineViewer::getMetersX(int index)
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

float MarineViewer::getMetersY(int index)
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
// Procedure: getLonX
//   Purpose: For a given x position, return its position, in 
//            terms of longitude.
//            Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the x position 
//            of the cross-hairs

float MarineViewer::getLonX(int index)
{
  if(m_cross_offon) {
    float x_pos = ((float)(w()) / 2.0) - (float)(m_vshift_x);
    float x_pct = m_back_img.pixToPctX(x_pos);
    float x_pct_cent = m_back_img.get_img_centx();
    float x_pct_mtrs = m_back_img.get_img_meters();
    float meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
    return(meters2LatLon("lon", meters));
  }

  ObjectPose opose = getObjectPoseByIndex(index);
  return(meters2LatLon("lon", opose.getX()));
}

// ----------------------------------------------------------
// Procedure: getLatY
//   Purpose: For a given x position, return its position, in 
//            terms of latitude.
//            Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the x position 
//            of the cross-hairs

float MarineViewer::getLatY(int index)
{
  if(m_cross_offon) {
    float y_pos = ((float)(h()) / 2.0) - (float)(m_vshift_y);
    float y_pct = m_back_img.pixToPctY(y_pos);
    float y_pct_cent = m_back_img.get_img_centy();
    float y_pct_mtrs = m_back_img.get_img_meters();
    float meters = (y_pct - y_pct_cent) / (y_pct_mtrs / 100.0);
    return(meters2LatLon("lat", meters));
  }

  ObjectPose opose = getObjectPoseByIndex(index);
  return(meters2LatLon("lat", opose.getY()));
}

// ----------------------------------------------------------
// Procedure: getSpd
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. -1 indicates it the x position 
//            of the cross-hairs

float MarineViewer::getSpd(int index)
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

float MarineViewer::getDep(int index)
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

float MarineViewer::getCrs(int index)
{
  if(m_cross_offon)
    return(0.0);

  ObjectPose opose = getObjectPoseByIndex(index);
  return(opose.getTheta());
}

// ----------------------------------------------------------
// Procedure: getKName
//   Purpose: Index indicates which of the MAX_VEHICLES vehicles
//            is being queried. Anything outside this range 
//            results in an empty string being returned.

string MarineViewer::getKName(int index)
{
  if((m_cross_offon) || (index == -1))
    return("cross-hairs");

  int ix = index;
  map<string,ObjectPose>::iterator p;
  for(p=kpos_map.begin(); p!=kpos_map.end(); p++) {
    if(ix==0)
      return(p->first);
    else
      ix--;
  }
  return("");
}

//-------------------------------------------------------------
// Procedure: drawPoints

void MarineViewer::drawPoints(CPList &cps)
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

void MarineViewer::drawPoint(float px, float py, int color)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Determine position in terms of image percentage
  float pt_ix = meters2img("x", px);
  float pt_iy = meters2img("y", py);

  // Determine position in terms of view percentage
  float pt_vx = img2view("x", pt_ix);
  float pt_vy = img2view("y", pt_iy);

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

ObjectPose MarineViewer::getObjectPoseByIndex(int index)
{
  int ix = index;
  map<string,ObjectPose>::iterator p;
  for(p=kpos_map.begin(); p!=kpos_map.end(); p++) {
    if(ix==0)
      return(p->second);
    else
      ix--;
  }

  ObjectPose op;
  return(op);
}








