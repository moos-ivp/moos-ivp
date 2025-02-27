/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RectHullGenerator.cpp                                */
/*    DATE: Aug 21st 2020                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cmath>
#include "RectHullGenerator.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RectHullGenerator::RectHullGenerator()
{
  m_rotation_angle = 0;

  m_wid = 0;
  m_hgt = 0;
}

//---------------------------------------------------------
// Procedure: addPoint

void RectHullGenerator::addPoint(double x, double y)
{
  XYPoint pt(x,y);
  m_original_pts.push_back(pt);
}

//---------------------------------------------------------
// Procedure: addPoint

void RectHullGenerator::addPoint(XYPoint point)
{
  m_original_pts.push_back(point);
}


//---------------------------------------------------------
// Procedure: setRotationAngle()
//      Note: Rotation angle will be mapped into the domain of
//            of [0,180), since 45 ~ 225 etc.

void RectHullGenerator::setRotationAngle(double rotation_angle)
{
  m_rotation_angle = angle360(rotation_angle);

  if(m_rotation_angle >= 180)
    m_rotation_angle -= 180;

}


//---------------------------------------------------------
// Procedure: generateRectHull
//                                                          
//                                                          
//                 o---------------------------------o   
//                 |                                 |           
//                 |                                 |           
//                 |                                 |           
//                 |                                 |           
//                 |                                 |    
//     axx,ayy     |                                 |     bxx,byy       
//        o        |                                 |        o 
//         \       |                                 |       /   
//           \     |                                 |     /     
//             \   |                                 |   /       
//               \ |                                 | /         
//                 o---------------------------------o   
//           ax,ay   \                             /   bx,by              
//                     \                         /                  
//                       \                     /                    
//                         \                 /                      
//                           \             /                          
//                             \         /                              
//                               \     /                                  
//                                 \ /                                     
//                                  o                                       
//                                ix,iy                                                  
//                                                                  
//                        
        
XYPolygon RectHullGenerator::generateRectHull()
{
  // =======================================================
  // Part 1: Set the simple rectilinear bounds
  // =======================================================
  double xmin = 0;
  double xmax = 0;
  double ymin = 0;
  double ymax = 0;

  for(unsigned int i=0; i<m_original_pts.size(); i++) {
    double px = m_original_pts[i].get_vx();
    double py = m_original_pts[i].get_vy();
    if((i == 0) || (px < xmin))
      xmin = px;
    if((i == 0) || (px > xmax))
      xmax = px;
    if((i == 0) || (py < ymin))
      ymin = py;
    if((i == 0) || (py > ymax))
      ymax = py;
  }

  // =======================================================
  // Part 2: Handle special case of 0 or 90 degree rotation
  // =======================================================
  if((m_rotation_angle == 0) || (m_rotation_angle == 90)) {
    m_rpoly = XYPolygon();
    m_rpoly.add_vertex(xmin, ymin);
    m_rpoly.add_vertex(xmax, ymin);
    m_rpoly.add_vertex(xmax, ymax);
    m_rpoly.add_vertex(xmin, ymax);

    if(m_rotation_angle == 0) {
      m_wid = (xmax - xmin);
      m_hgt = (ymax - ymin);
    }
    if(m_rotation_angle == 90) {
      m_hgt = (xmax - xmin);
      m_wid = (ymax - ymin);
    }
    return(m_rpoly);
  }
  
  // =======================================================
  // Part 3: Handle the General Case
  // =======================================================
  double ax = xmin;
  double ay = ymin;
  double bx = xmax;
  double by = ymin;

  double axx = 0;
  double ayy = 0;
  double bxx = 0;
  double byy = 0;

  double ix  = 0;
  double iy  = 0;

  double anga = 0;
  double angb = 0;
  
  if((m_rotation_angle > 0) && (m_rotation_angle < 90)) {
    projectPoint(m_rotation_angle-90, 10, ax,ay, axx,ayy);
    projectPoint(m_rotation_angle, 10, bx,by, bxx,byy);
    anga = m_rotation_angle;
    angb = angle360(m_rotation_angle - 90);
  }
  else {
    projectPoint(m_rotation_angle-180, 10, ax,ay, axx,ayy);
    projectPoint(m_rotation_angle-90, 10, bx,by, bxx,byy);
    anga = angle360(m_rotation_angle - 90);
    angb = angle360(m_rotation_angle - 180);
  }

  //bool cross_ok = linesCross(ax,ay,axx,ayy, bx,by,bxx,byy, ix,iy);
  
  double ahmin = 0;
  double ahmax = 0;
  double bhmin = 0;
  double bhmax = 0;
  
  for(unsigned int i=0; i<m_original_pts.size(); i++) {
    double px = m_original_pts[i].get_vx();
    double py = m_original_pts[i].get_vy();
    double ah = distPointToLine(px,py, ax,ay, axx,ayy);
    double bh = distPointToLine(px,py, bx,by, bxx,byy);
    
    if((i==0) || (ah < ahmin))
      ahmin = ah;
    if((i==0) || (ah > ahmax))
      ahmax = ah;  
    if((i==0) || (bh < bhmin))
      bhmin = bh;
    if((i==0) || (bh > bhmax))
      bhmax = bh;
  }

  cout << "m_rotation_angle:" << m_rotation_angle << endl;
  if((m_rotation_angle > 0) && (m_rotation_angle < 90)) {
    m_wid = bhmax - bhmin;
    m_hgt = ahmax - ahmin;
  }
  else {
    m_hgt = bhmax - bhmin;
    m_wid = ahmax - ahmin;
  }

  double jx,jy;
  double kx,ky;
  projectPoint(angb, bhmin, ix,iy, jx,jy);
  projectPoint(angb, bhmax, ix,iy, kx,ky);

  double x1,y1;
  double x2,y2;
  double x3,y3;
  double x4,y4;
  projectPoint(anga, ahmin, jx,jy, x1,y1);
  projectPoint(anga, ahmax, jx,jy, x4,y4);
  projectPoint(anga, ahmin, kx,ky, x2,y2);
  projectPoint(anga, ahmax, kx,ky, x3,y3);

  m_rpoly = XYPolygon();
  m_rpoly.add_vertex(x1,y1);
  m_rpoly.add_vertex(x2,y2);
  m_rpoly.add_vertex(x3,y3);
  m_rpoly.add_vertex(x4,y4);

  return(m_rpoly);
}

