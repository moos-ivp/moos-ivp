/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TurnGenWilliamson.cpp                                */
/*    DATE: July 29th 2020                                       */
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
#include <cmath>
#include <algorithm>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "TurnGenWilliamson.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

TurnGenWilliamson::TurnGenWilliamson()
{
  // Init config vars
  m_lane_gap = 0;
  m_bias     = 0;

  m_desired_extent = 0;
  m_natural_extent = 0;
  m_auto_turn_dir  = -1;
  
  // Init state vars
  m_ang_w = 0;
  m_dist_b = 0;
  m_max_bias = 0;
  
  m_theta1 = 0;
  m_theta2 = 0;
}

//------------------------------------------ Overloaded Virtual
// Procedure: setTurnRadius()                

bool TurnGenWilliamson::setTurnRadius(double radius)
{
  bool ok = TurnGenerator::setTurnRadius(radius);

  // Adjust the max bias which depends on the lane gap
  m_max_bias = m_turn_radius - (m_lane_gap / 2);
  
  if(m_bias > m_max_bias)
    m_bias = m_max_bias;
  else if(m_bias < -m_max_bias)
    m_bias = -m_max_bias;

  return(ok);
}

//------------------------------------------ Overloaded Virtual
// Procedure: setEndPos()                    
//      Note: Differs from the superclass version in that the
//            end heading is automatically set to be 180 from
//            the ownship heading, if ownship heading is set.

void TurnGenWilliamson::setEndPos(double endx, double endy)
{
  m_endx = endx;
  m_endy = endy;

  m_endx_set = true;
  m_endy_set = true;

  if(m_start_osh_set) {
    m_endh = angle360(m_start_osh + 180);
    m_endh_set = true;
  }

  
}

//------------------------------------------ Overloaded Virtual
// Procedure: setEndHeading()                    
//      Note: Differs from the superclass version in that the
//            end heading dan only be set to 180 of ownship

void TurnGenWilliamson::setEndHeading(double endh)
{
}

//-------------------------------------------------------------
// Procedure: setLaneGap()

void TurnGenWilliamson::setLaneGap(double lane_gap)
{
  m_lane_gap = lane_gap;
  if(m_lane_gap < 0)
    m_lane_gap = 0;

  // Adjust the max bias which depends on the lane gap
  m_max_bias = m_turn_radius - (m_lane_gap / 2);

  if(m_bias > m_max_bias)
    m_bias = m_max_bias;
  else if(m_bias < -m_max_bias)
    m_bias = -m_max_bias;

}

//-------------------------------------------------------------
// Procedure: setBias()

void TurnGenWilliamson::setBias(double bias)
{
  m_bias = bias;
    
  if(m_bias > m_max_bias)
    m_bias = m_max_bias;

  if(m_bias < -m_max_bias)
    m_bias = -m_max_bias;
}

//-------------------------------------------------------------
// Procedure: setBiasPct()

void TurnGenWilliamson::setBiasPct(double bias_pct)
{
  if(bias_pct > 100)
    bias_pct = 100;
  else if(bias_pct < -100)
    bias_pct = -100;
  
  double bias = (bias_pct/100) * m_max_bias;
  
  m_bias = bias;
    
  if(m_bias > m_max_bias)
    m_bias = m_max_bias;

  if(m_bias < -m_max_bias)
    m_bias = -m_max_bias;
}

//-------------------------------------------------------------
// Procedure: setDesiredExtent()

void TurnGenWilliamson::setDesiredExtent(double extent)
{
  m_desired_extent = extent;
  if(m_desired_extent < 0)
    m_desired_extent = 0;
}

//-------------------------------------------------------------
// Procedure: generate()

void TurnGenWilliamson::generate()
{
  // Part 1: Determine if this is a port or starboard turn
  double osang = relAng(m_start_osx, m_start_osy, m_endx, m_endy);
  double angle_diff = angle180(osang - m_start_osh);

  // Part 2: Determine the lane gap
  double px = 0;
  double py = 0;
  projectPoint(m_start_osh, 20, m_start_osx, m_start_osy, px, py);
  m_lane_gap = distPointToLine(m_endx, m_endy, m_start_osx,
  			       m_start_osy, px, py);

  if(m_auto_turn_dir > 0) {
    if(m_lane_gap > m_auto_turn_dir) {
      if(angle_diff < 0)
	m_port_turn = true;
      else
	m_port_turn = false;
    }
  }
    
  // Part 3: Determine if destination point is fore of ownship, and
  // calculate distance if so, and add to the extent.
  bool fore = false;
  if((angle_diff < 90) && (angle_diff > -90))
    fore = true;

  if(fore) {
    double star_angle = angle360(m_start_osh + 90);
    projectPoint(star_angle, 20, m_start_osx, m_start_osy, px, py);
    m_natural_extent = distPointToLine(m_endx, m_endy, m_start_osx,
				    m_start_osy, px, py);
  }
  else {
    double star_angle = angle360(m_start_osh + 90);
    projectPoint(star_angle, 20, m_start_osx, m_start_osy, px, py);
    m_natural_extent = -1 * distPointToLine(m_endx, m_endy, m_start_osx,
					    m_start_osy, px, py);
  }

  generateLaneSwitch();

#if 0
  cout << "lane_gap:" << m_lane_gap << endl;
  cout << "m_port_turn:" << m_port_turn << endl;
  if((m_lane_gap == 0) && false && m_port_turn) {
    cout << "reversing points!!!" << endl;
    vector<XYPoint> new_pts;
    unsigned int psize = m_points.size();
    for(unsigned int i=0; i<psize; i++) {
      unsigned int other_ix = psize-1-i;
      string other_color = m_points[other_ix].get_color_str("vertex");
      XYPoint new_pt = m_points[other_ix];
      new_pt.set_color("vertex", other_color);
      new_pts.push_back(new_pt);
    }
    m_points = new_pts;
  }
  
  // Hack for visually detecting start/end of points in willturn gui
  if(m_points.size() > 8)
    m_points[5] = m_points[6];
#endif

  
  return;
  if(m_lane_gap != 0)
    generateLaneSwitch();
  else
    generateNormal();
}

//-------------------------------------------------------------
// Procedure: generateNormal()

void TurnGenWilliamson::generateNormal()
{
  cout << "generateNormal()" << endl;
  if(!valid())
    return;

  m_points.clear();

  // Part 1: Determine the extent or distance between the radii of
  // the near circle and the far circle.
  double extent = (2 * m_turn_radius) / cos(3.1415926 * 30 / 180);

  // Part 2: Calculate the center points of each circle
  double cx1 = 0;
  double cy1 = 0;
  double cx2 = 0;
  double cy2 = 0;
  double port_angle = angle360(m_start_osh - 90);
  double star_angle = angle360(m_start_osh + 90);
  if(m_port_turn) 
    projectPoint(port_angle, m_turn_radius, m_start_osx, m_start_osy, cx1, cy1);
  else
    projectPoint(star_angle, m_turn_radius, m_start_osx, m_start_osy, cx1, cy1);

  projectPoint(m_start_osh, extent, cx1, cy1, cx2, cy2);
  
  // Part 3: Calculate the near circle points.
  double px = 0;
  double py = 0;
  double start_angle = port_angle;
  if(m_port_turn)
    start_angle = star_angle;

  for(double delta=0; delta <= 60; delta+=m_ptgap) {
    double angle = angle360(start_angle + delta);
    if(m_port_turn)
      angle = angle360(start_angle - delta);
    projectPoint(angle, m_turn_radius, cx1, cy1, px, py);
    XYPoint pt(px, py);
    m_points.push_back(pt);
  }
  
  // Part 4: Calculate the far circle points.
  start_angle = port_angle + 240;
  if(m_port_turn)
    start_angle = star_angle - 240;

  for(double delta=0; delta <= 240; delta+=m_ptgap) {
    double angle = angle360(start_angle - delta);
    if(m_port_turn)
      angle = angle360(start_angle + delta);
    projectPoint(angle, m_turn_radius, cx2, cy2, px, py);
    XYPoint pt(px, py);
    m_points.push_back(pt);
  }
  
}


//-------------------------------------------------------------
// Procedure: generateLaneSwitch()

void TurnGenWilliamson::generateLaneSwitch()
{
  if((m_turn_radius * 2) > m_lane_gap)
    generateLaneSwitch1();
  else
    generateLaneSwitch2();
}    

//-------------------------------------------------------------
// Procedure: generateLaneSwitch1()
//                                
//            Port Turn                     Starboard Turn           |
//             ---|---                          ---|---              |
//           o         o                      o         o            |
//         o             o                  o             o          |
//       o                 o              o                 o        |
//      o                   o            o                   o       |
//     o          c3         o          o          c3         o      |
//     o          *          o          o          *          o      |
//     o        / w \        o          o        / w \        o      |
//      o     /   |   \r    o            o     /   |   \r    o       |
//        o  /    |     \ o                o  /    |     \ o         |
//         o     b|      o                  o     b|      o          |
//       /  o     |     o  \              /  o     |     o  \        |
//          |     -     |                    |     -     |           |
//  c2      |     c0    |    c1       c2     |     c0    |    c1     |
//          |           |                    |           |           |
//          |           |                    |           |           |
//          | - - d - - |                    | - - d - - |           |
//          |           |  ^              ^  |           |           |
//          |           |  |              |  |           |           |
//                    ownship           ownship                      |

void TurnGenWilliamson::generateLaneSwitch1()
{
  // Part 1: Establish the turn headings
  double port_angle = angle360(m_start_osh - 90);
  double star_angle = angle360(m_start_osh + 90);

  // Part 2: Calculate core angle and distance
  double r  = m_turn_radius;
  double rr = r * r;
  double d  = m_lane_gap;
  double v  = (r+(d/2))/(2*r);
  double theta_rad = acos(v);
  double theta_deg = radToDegrees(theta_rad);
  m_ang_w = (90 - theta_deg)*2;
  m_dist_b = sin(theta_rad)*(2*r);

  double be = m_dist_b + m_desired_extent;
  //cout << "be: " << be << endl;
  
  double k1 = r + (d/2) - m_bias;
  double k2 = r + (d/2) + m_bias;
  //cout << "k1: " << k1 << endl;
  //cout << "k2: " << k2 << endl;
   
  // If bias is non-zero, may have to auto-increase the extent
  if(m_bias != 0) {
    double min_extent1 = sqrt(4*rr - (k1*k1));
    double min_extent2 = sqrt(4*rr - (k2*k2));
    //cout << "min_extent1: " << min_extent1 << endl;
    //cout << "min_extent2: " << min_extent2 << endl;
    if(be < min_extent1)
      be = min_extent1 + 1;
    if(be < min_extent2)
      be = min_extent2 + 1;
    //cout << "new_be: " << be << endl;
  }
  
  // Part 3 Calculate key intermediate vertices
  double cx0=0, cy0=0;
  double cx1=0, cy1=0;
  double cx2=0, cy2=0;
  double cx3=0, cy3=0;
  if(m_port_turn) {
    projectPoint(port_angle, m_lane_gap/2 - m_bias,  m_start_osx, m_start_osy, cx0, cy0);
    projectPoint(star_angle, m_turn_radius, m_start_osx, m_start_osy, cx1, cy1);
    projectPoint(port_angle, m_turn_radius+d, m_start_osx, m_start_osy, cx2, cy2);
    projectPoint(m_start_osh, be, cx0, cy0, cx3, cy3);
  }
  else {
    projectPoint(star_angle, m_lane_gap/2 - m_bias,  m_start_osx, m_start_osy, cx0, cy0);
    projectPoint(star_angle, m_turn_radius+d, m_start_osx, m_start_osy, cx1, cy1);
    projectPoint(port_angle, m_turn_radius, m_start_osx, m_start_osy, cx2, cy2);
    projectPoint(m_start_osh, be, cx0, cy0, cx3, cy3);
  }

  if(m_natural_extent > 0) {
    projectPoint(m_start_osh, m_natural_extent, cx0, cy0, cx0, cy0);
    projectPoint(m_start_osh, m_natural_extent, cx1, cy1, cx1, cy1);
    projectPoint(m_start_osh, m_natural_extent, cx2, cy2, cx2, cy2);
    projectPoint(m_start_osh, m_natural_extent, cx3, cy3, cx3, cy3);
    XYPoint pt(m_start_osx, m_start_osy);
    pt.set_vertex_color("white");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }
  
  m_c0.set_vertex(cx0, cy0);
  m_c1.set_vertex(cx1, cy1);
  m_c2.set_vertex(cx2, cy2);
  m_c3.set_vertex(cx3, cy3);
  
  m_theta1 = (180 - m_ang_w) / 2;
  m_theta2 = (180 - m_ang_w) / 2;
  
  m_theta1 = theta_deg;
  m_theta2 = (180 - m_ang_w) / 2;
  
  // Part 2B: Handle if non-negative extent
  if(be > m_dist_b) {
    double mrad1 = atan(be / k1);
    double mrad2 = atan(be / k2);
    double mdeg1 = radToDegrees(mrad1);
    double mdeg2 = radToDegrees(mrad2);

    double h1 = sqrt((be * be) + (k1 * k1)) / 2;
    double h2 = sqrt((be * be) + (k2 * k2)) / 2;
    double nrad1 = acos(r/h1);
    double nrad2 = acos(r/h2);

    double ndeg1 = radToDegrees(nrad1);
    double ndeg2 = radToDegrees(nrad2);

    m_theta1 = mdeg1 - ndeg1;
    m_theta2 = mdeg2 - ndeg2;

    m_ang_w = (180 - (m_theta1 + m_theta2));
  }
  
  // Part 4: Calculate initial turn points
  //double turnlim = 180 - (m_ang_w/2) - 90;
  double turnlim = m_theta1;
  double start_angle = star_angle;
  if(m_port_turn)
    start_angle = port_angle;

  for(double delta=0; delta <= turnlim; delta+=m_ptgap) {
    double px = 0;
    double py = 0;
    if(m_port_turn) {
      double angle = angle360(start_angle + delta);
      projectPoint(angle, m_turn_radius, cx1, cy1, px, py);
    }
    else {
      double angle = angle360(start_angle - delta);
      projectPoint(angle, m_turn_radius, cx2, cy2, px, py);
    }
    XYPoint pt(px, py);
    pt.set_vertex_color("white");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }
  
  // Part 5: Calculate main turn points
  turnlim = 360 - m_ang_w;
  start_angle = angle360(m_start_osh - 90 - m_theta1);
  if(m_port_turn)
    start_angle = angle360(m_start_osh + 90 + m_theta1);
  
  for(double delta=0; delta <= turnlim; delta+=m_ptgap) {
    double px = 0;
    double py = 0;
    if(m_port_turn) {
      double angle = angle360(start_angle - delta);
      projectPoint(angle, m_turn_radius, cx3, cy3, px, py);
    }
    else {
      double angle = angle360(start_angle + delta);
      projectPoint(angle, m_turn_radius, cx3, cy3, px, py);
    }
    XYPoint pt(px, py);
    pt.set_vertex_color("light_green");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }
  
  // Part 6: Calculate final turn points
  turnlim = m_theta2;
  start_angle = angle360(port_angle + turnlim);
  if(m_port_turn)
    start_angle = angle360(star_angle - turnlim);

  for(double delta=0; delta <= turnlim; delta+=m_ptgap) {
    double px = 0;
    double py = 0;
    if(m_port_turn) {
      double angle = angle360(start_angle + delta);
      projectPoint(angle, m_turn_radius, cx2, cy2, px, py);
    }
    else {
      double angle = angle360(start_angle - delta);
      projectPoint(angle, m_turn_radius, cx1, cy1, px, py);
    }
    XYPoint pt(px, py);
    pt.set_vertex_color("red");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }

  if(m_natural_extent < 0) {
    XYPoint pt(m_endx, m_endy);
    pt.set_vertex_color("red");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }


  // Part 7: Calculate the last turn point. If too close to the previous
  // turn point, remove the previous turn point
#if 0
  double fx = 0;
  double fy = 0;
  if(m_port_turn)
    projectPoint(port_angle, m_lane_gap, m_start_osx, m_start_osy, fx, fy);
  else
    projectPoint(star_angle, m_lane_gap, m_start_osx, m_start_osy, fx, fy);
  XYPoint pt(fx, fy);

  bool swapped = false;
  unsigned int psize = m_points.size();
  if(psize > 0) {
    double px = m_points[psize-1].get_vx();
    double py = m_points[psize-1].get_vy();
    double dist = hypot(px-fx, py-fy);
    if(dist < 5) {
      m_points[psize-1].set_vx(fx);
      m_points[psize-1].set_vy(fy);
      swapped = true;
    }
  }
  if(!swapped) {
    pt.set_vertex_color("red");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }
#endif
  
}


//-------------------------------------------------------------
// Procedure: generateLaneSwitch2()
//                                
//               -------------------------------------               |
//           o                                          o            |
//         o                                              o          |
//       o                                                  o        |
//      o                                                    o       |
//     o          c2              c3               c1         o      |
//     |          *               *                *          |      |
//  Ext|--- r ----|------ k ------|                           |      |
//     |                                                      |      |
//     o                          *                           o      |
//     |                          c0                          |      |
//     |                                                      |      |
//     |                                                      |      |
//     |        k = (lane_gap)/2 - r                          |      |
//     |                                                      |      |


void TurnGenWilliamson::generateLaneSwitch2()
{
  // Part 1: Establish the turn headings
  double port_angle = angle360(m_start_osh - 90);
  double star_angle = angle360(m_start_osh + 90);

  double k  = (m_lane_gap / 2) - m_turn_radius;

  // Part 3 Calculate key intermediate vertices
  double cx0=0, cy0=0;
  double cx1=0, cy1=0;
  double cx2=0, cy2=0;
  double cx3=0, cy3=0;
  if(m_port_turn) 
    projectPoint(port_angle, m_lane_gap/2,  m_start_osx, m_start_osy, cx0, cy0);
  else 
    projectPoint(star_angle, m_lane_gap/2,  m_start_osx, m_start_osy, cx0, cy0);
  
  projectPoint(m_start_osh, m_desired_extent, cx0, cy0, cx3, cy3);
  projectPoint(star_angle, k, cx3, cy3, cx1, cy1);
  projectPoint(port_angle, k, cx3, cy3, cx2, cy2);


  if(m_natural_extent > 0) {
    projectPoint(m_start_osh, m_natural_extent, cx0, cy0, cx0, cy0);
    projectPoint(m_start_osh, m_natural_extent, cx1, cy1, cx1, cy1);
    projectPoint(m_start_osh, m_natural_extent, cx2, cy2, cx2, cy2);
    projectPoint(m_start_osh, m_natural_extent, cx3, cy3, cx3, cy3);
    XYPoint pt(m_start_osx, m_start_osy);
    pt.set_vertex_color("white");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }
 



  m_c0.set_vertex(cx0, cy0);
  m_c1.set_vertex(cx1, cy1);
  m_c2.set_vertex(cx2, cy2);
  m_c3.set_vertex(cx3, cy3);

  // Part 5: Calculate initial turn points
  double turnlim = 90;
  double start_angle = angle360(m_start_osh - 90);
  if(m_port_turn)
    start_angle = angle360(m_start_osh + 90);
  
  for(double delta=0; delta <= turnlim; delta+=m_ptgap) {
    double px = 0;
    double py = 0;
    if(m_port_turn) {
      double angle = angle360(start_angle - delta);
      projectPoint(angle, m_turn_radius, cx1, cy1, px, py);
    }
    else {
      double angle = angle360(start_angle + delta);
      projectPoint(angle, m_turn_radius, cx2, cy2, px, py);
    }
    XYPoint pt(px, py);
    pt.set_vertex_color("light_green");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }
  
  // Part 6: Calculate final turn points
  turnlim = 90;
  start_angle = m_start_osh;
  
  for(double delta=0; delta <= turnlim; delta+=m_ptgap) {
    double px = 0;
    double py = 0;
    if(m_port_turn) {
      double angle = angle360(start_angle - delta);
      projectPoint(angle, m_turn_radius, cx2, cy2, px, py);
    }
    else {
      double angle = angle360(start_angle + delta);
      projectPoint(angle, m_turn_radius, cx1, cy1, px, py);
    }
    XYPoint pt(px, py);
    pt.set_vertex_color("dodger_blue");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }

  if(m_natural_extent < 0) {
    XYPoint pt(m_endx, m_endy);
    pt.set_vertex_color("dodger_blue");
    pt.set_vertex_size(4);
    m_points.push_back(pt);
  }




}


