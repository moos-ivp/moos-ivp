/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SurveyGenerator.cpp                                  */
/*    DATE: August 15th 2020                                     */
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

#include <cmath>
#include <iostream>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "SurveyGenerator.h"
#include "RectHullGenerator.h"
#include "XYPatternBlock.h"
#include "EdgeTagSet.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

SurveyGenerator::SurveyGenerator()
{
  m_start_x = 0;
  m_start_y = 0;

  m_start_x_set = false;
  m_start_y_set = false;

  m_lane_angle  = 0;
  m_lane_width  = 20;
  m_lane_widthx = 20;

  m_pack_lanes = true;
  m_drop_lane  = true;
  m_auto_shift = false;
  m_auto_dense = false;
  m_core_width = false;
}


//-------------------------------------------------------------
// Procedure: generate()

void SurveyGenerator::generate()
{
  generateBlock();

  if(m_pack_lanes)
    packLanes();

  if(m_drop_lane) {
    adjustLastLane();
    adjustFirstLane();
  }
}

//-------------------------------------------------------------
// Procedure: generateBlock()

void SurveyGenerator::generateBlock()
{
  if(m_auto_shift)
    cout << "Do AutoShift Stuff here!!!";
  if(m_auto_dense)
    cout << "Do AutoDense Stuff here!!!";

  //=================================================
  // Part 1: Build the Full Block
  //=================================================
  RectHullGenerator rgen;
  rgen.setRotationAngle(m_lane_angle);
  for(unsigned int i=0; i<m_region.size(); i++) {
    double px = m_region.get_vx(i);
    double py = m_region.get_vy(i);
    rgen.addPoint(px,py);
  }
  
  m_rehull = rgen.generateRectHull();
  
  double cx = m_rehull.get_center_x();
  double cy = m_rehull.get_center_y();
  
  double wid = rgen.getPolyWidth();
  double hgt = rgen.getPolyHeight();
			
  m_lane_widthx = m_lane_width;
  if(m_auto_dense) {
    double lanes = (wid / m_lane_width);
    cout << "wid:" << wid << ", lanes1:" << lanes << endl;
    if(trunc(lanes) != lanes) {
      cout << "wid:" << wid << ", lanes2:" << lanes << endl;
      m_lane_widthx = wid / (double)(trunc(lanes)+1);
      cout << "widx:" << m_lane_widthx << endl;
    }
  }

  cout << "lane_width:" << m_lane_width << ", widx:" << m_lane_widthx << endl;
  

  XYPoint pt(cx,cy);  
  XYPatternBlock pblock;
  pblock.addIDPoint(pt);
  pblock.setParam("block_width", wid);
  pblock.setParam("block_length", hgt);
  pblock.setParam("angle", m_lane_angle);
  pblock.setParam("swath_width", m_lane_widthx);
  pblock.setCoreWidth(m_core_width);
 
  pblock.buildCompositeSegList(m_start_x, m_start_y);

  XYSegList new_segl = pblock.getCompositeSegList();
  m_survey = new_segl;
}


//-------------------------------------------------------------
// Procedure: packLanes()

void SurveyGenerator::packLanes()
{
  // Sanity check
  unsigned int vsize = m_survey.size();
  if(vsize <= 1)
    return;

  double half_lane = m_lane_width / 2;
  EdgeTagSet edge_tags = m_survey.get_edge_tags();
  for(unsigned int i=0; i<vsize-1; i++) {
    bool is_lane = edge_tags.matches(i, i+1, "lane");

    //   x3,y3   ang3       x1,y1    ang4       x4,y4
    //     o------<-----------o-------->----------o
    //                        |                                 
    //                        |                                 
    //                        |                                 
    //     |                  v ang1              |                   
    //     - dist3            |                   - dist4              
    //                        |  
    //                        |                                 
    //                        |---  lanewid/2  ---|                             
    //                        |  
    //                        |                                 
    //     - dist5            |                   - dist6              
    //     |                  ^ ang2              |                   
    //                        |                                 
    //                        |                                 
    //     o-------<----------o-------->----------o
    //   x5,y5   ang5       x2,y2    ang6       x6,y6

    if(is_lane) {     
      //cout << "===============================================" << endl;
      //cout << "Lane: " << i << endl;
      //cout << "===============================================" << endl;


      double x1 = m_survey.get_vx(i);
      double y1 = m_survey.get_vy(i);
      double x2 = m_survey.get_vx(i+1);
      double y2 = m_survey.get_vy(i+1);
      //cout << "  x1:" << x1 << endl;
      //cout << "  y1:" << y1 << endl;
      //cout << "  x2:" << x2 << endl;
      //cout << "  y2:" << y2 << endl;
      
      // Part 1: Get the extend points and angles      
      double ang1 = relAng(x1,y1, x2,y2);
      double ang2 = relAng(x2,y2, x1,y1);
      double ang3 = angle360(ang1+90);
      double ang4 = angle360(ang1-90);
      double ang5 = angle360(ang1-90);
      double ang6 = angle360(ang1+90);

      //cout << "  ang1:" << ang1 << endl;
      //cout << "  ang2:" << ang2 << endl;
      //cout << "  ang3:" << ang3 << endl;
      //cout << "  ang4:" << ang4 << endl;
      //cout << "  ang5:" << ang5 << endl;
      //cout << "  ang6:" << ang6 << endl;
      
      // Part 1: Get the extend points and angles
      
      double x3,y3, x4,y4, x5,y5, x6,y6;
      projectPoint(ang3, half_lane, x1,y1, x3,y3);
      projectPoint(ang4, half_lane, x1,y1, x4,y4);
      projectPoint(ang5, half_lane, x2,y2, x5,y5);
      projectPoint(ang6, half_lane, x2,y2, x6,y6);
      
      // Part 2: Get distance in from side 1 to the poly
      double dist3 = m_region.dist_to_poly(x3, y3, ang1);
      double dist4 = m_region.dist_to_poly(x4, y4, ang1);
      // find the adjustment distance if any
      double adist1 = 0;
      if((dist3 < 0) && (dist4 < 0))
	adist1 = 0;
      else if((dist3 < 0) && (dist4 >= 0))
	adist1 = dist4;
      else if((dist3 >= 0) && (dist4 < 0))
	adist1 = dist3;
      else if(dist3 < dist4)
	adist1 = dist3;
      else 
	adist1 = dist4;
      
      // Part 3: Get distance in from side 2 to the poly
      double dist5 = m_region.dist_to_poly(x5, y5, ang2);
      double dist6 = m_region.dist_to_poly(x6, y6, ang2);
      // find the adjustment distance if any
      double adist2 = 0;
      if((dist5 < 0) && (dist6 < 0))
	adist2 = 0;
      else if((dist5 < 0) && (dist6 >= 0))
	adist2 = dist6;
      else if((dist5 >= 0) && (dist6 < 0))
	adist2 = dist5;
      else if(dist5 < dist6)
	adist2 = dist5;
      else 
	adist2 = dist6;

      //cout << "  adist1(a): " << adist1 << endl;
      //cout << "  adist2(a): " << adist2 << endl;
      
      // Part 4: Consider polygon vertices instead of edges
      unsigned int psize = m_region.size();
      for(unsigned int j=0; j<psize; j++) {
	double px = m_region.get_vx(j);
	double py = m_region.get_vy(j);
	// determine if this vertex is within half a lanewidth of this lane
	double dist = distPointToLine(px,py, x1,y1, x2,y2);

	//cout << "    px=" << px << ", py=" << py;
	//cout << ", x1=" << x1 << ", y1=" << y1;
	//cout << ", x2=" << x2 << ", y2=" << y2   << endl;
	//cout << "    pt_dist: " << dist << endl;
	if(dist < half_lane) {
	  double mdist1 = distPointToSeg(x3,y3, x4,y4, px,py);
	  //cout << "      mdist1: " << mdist1 << endl;  
	  if((mdist1 < adist1) || (adist1 < 0)) {
	    adist1 = mdist1;
	  }
	  double mdist2 = distPointToSeg(x5,y5, x6,y6, px,py);
	  //cout << "      mdist2: " << mdist2 << endl;  
	  if((mdist2 < adist2) || (adist2 < 0)) {
	    adist2 = mdist2;
	  }
	  
	}
      } 
      
      //cout << "  adist1(b): " << adist1 << endl;
      //cout << "  adist2(b): " << adist2 << endl;
      
      if(adist1 > 0) {
	projectPoint(ang1, adist1, x1, y1, x1, y1);
	m_survey.mod_vertex(i, x1, y1);
      }
      if(adist2 > 0) {
	projectPoint(ang2, adist2, x2, y2, x2, y2);
	m_survey.mod_vertex(i+1, x2, y2);
      }

    }
  }
}

//-------------------------------------------------------------
// Procedure: adjustLastLane()
//   Purpose: Examine the final lane, if it is more than half a
//            lane width away from the closest point in the poly
//            then remove it, and the edge leg.

void SurveyGenerator::adjustLastLane()
{
  unsigned int vsize = m_survey.size();
  if(vsize < 2)
    return;

  EdgeTagSet edge_tags = m_survey.get_edge_tags();

  unsigned int i = vsize - 2;
  unsigned int j = vsize - 1;
  if(!edge_tags.matches(i, i+1, "lane"))
    return;

  double x1 = m_survey.get_vx(i);
  double y1 = m_survey.get_vy(i);
  double x2 = m_survey.get_vx(j);
  double y2 = m_survey.get_vy(j);

  if(m_region.seg_intercepts(x1,y1, x2,y2))
    return;

  double dist_to_poly = m_region.dist_to_poly(x1,y1, x2,y2);
  if(dist_to_poly <= (m_lane_width/2))
    return;

  m_survey.pop_last_vertex();
  m_survey.pop_last_vertex();
}


//-------------------------------------------------------------
// Procedure: adjustFirstLane()
//   Purpose: Examine the first lane, if it is more than half a
//            lane width away from the closest point in the poly
//            then remove it, and the edge leg.

void SurveyGenerator::adjustFirstLane()
{
  unsigned int vsize = m_survey.size();
  if(vsize < 2)
    return;

  EdgeTagSet edge_tags = m_survey.get_edge_tags();

  if(!edge_tags.matches(0, 1, "lane"))
    return;

  double x1 = m_survey.get_vx(0);
  double y1 = m_survey.get_vy(0);
  double x2 = m_survey.get_vx(1);
  double y2 = m_survey.get_vy(1);

  if(m_region.seg_intercepts(x1,y1, x2,y2))
    return;

  double dist_to_poly = m_region.dist_to_poly(x1,y1, x2,y2);
  if(dist_to_poly <= (m_lane_width/2))
    return;

  m_survey.delete_vertex(0);
  m_survey.delete_vertex(0);
}



//-------------------------------------------------------------
// Procedure: setRegion()

void SurveyGenerator::setRegion(XYPolygon region)
{
  if(!region.is_convex())
    return;

  m_region = region;
}

//-------------------------------------------------------------
// Procedure: setEntryPoint()

void SurveyGenerator::setEntryPoint(double osx, double osy)
{
  m_start_x = osx;
  m_start_y = osy;

  m_start_x_set = true;
  m_start_y_set = true;
}

//-------------------------------------------------------------
// Procedure: setLaneWidth()

void SurveyGenerator::setLaneWidth(double lane_width)
{
  if(lane_width < 1)
    lane_width = 1;
  m_lane_width = lane_width;
}

//-------------------------------------------------------------
// Procedure: setLaneAngle()

void SurveyGenerator::setLaneAngle(double angle)
{
  m_lane_angle = angle360(angle);
}

//---------------------------------------------------------
// Procedure: getLaneCount()
//      Note: o===o---o===o---o===o---o===o---o===o
//            0   1   2   3   4   5   6   7   8   9  size=10

unsigned int SurveyGenerator::getLaneCount() const
{
  unsigned lane_count = (unsigned int)(m_survey.size() / 2);
  return(lane_count);
}
