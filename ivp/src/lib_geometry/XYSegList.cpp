/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYSegList.cpp                                        */
/*    DATE: Apr 20th, 2005                                       */
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
#include <cstdlib>
#include <cstring>
#include "XYSegList.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: add_vertex

void XYSegList::add_vertex(double x, double y, double z, string vprop)
{
  m_vx.push_back(x);
  m_vy.push_back(y);
  m_vz.push_back(z);
  m_vprop.push_back(vprop);
}

//---------------------------------------------------------------
// Procedure: add_vertex

void XYSegList::add_vertex(const XYPoint &pt, string vprop)
{
  m_vx.push_back(pt.x());
  m_vy.push_back(pt.y());
  m_vz.push_back(pt.z());
  m_vprop.push_back(vprop);
}

//---------------------------------------------------------------
// Procedure: alter_vertex
//   Purpose: Given a new vertex, find the existing vertex that is
//            closest, and replace it with the new one.

void XYSegList::alter_vertex(double x, double y, double z, string vprop)
{
  unsigned int vsize = m_vx.size();
  if(vsize == 0)
    return;

  unsigned int ix = closest_vertex(x, y); 
  m_vx[ix]    = x;
  m_vy[ix]    = y;
  m_vz[ix]    = z;
  m_vprop[ix] = vprop;
}

//---------------------------------------------------------------
// Procedure: delete_vertex
//   Purpose: Given a new vertex, find the existing vertex that is
//            closest, and delete it.

void XYSegList::delete_vertex(double x, double y)
{
  unsigned int vsize = m_vx.size();
  if(vsize == 0)
    return;

  unsigned int ix = closest_vertex(x, y); 

  delete_vertex(ix);
}

//---------------------------------------------------------------
// Procedure: delete_vertex
//   Purpose: Given a valid vertex index, delete that vertex from
//            the SegList.

void XYSegList::delete_vertex(unsigned int ix)
{
  unsigned int vsize = m_vx.size();
  if(ix >= vsize)
    return;

  vector<double> new_x;
  vector<double> new_y;
  vector<double> new_z;
  
  for(unsigned int i=0; i<ix; i++) {
    new_x.push_back(m_vx[i]);
    new_y.push_back(m_vy[i]);
    new_z.push_back(m_vz[i]);
  }
  for(unsigned int i=ix+1; i<vsize; i++) {
    new_x.push_back(m_vx[i]);
    new_y.push_back(m_vy[i]);
    new_z.push_back(m_vz[i]);
  }
  
  m_vx = new_x;
  m_vy = new_y;
  m_vz = new_z;
}

//---------------------------------------------------------------
// Procedure: insert_vertex
//   Purpose: Given a new vertex, find the existing segment that is
//            closest, and add the vertex between points

void XYSegList::insert_vertex(double x, double y, double z, string vprop)
{
  unsigned int vsize = m_vx.size();
  if(vsize <= 1)
    return(add_vertex(x,y));

  unsigned int i, ix = closest_segment(x, y); 

  vector<double> new_x;
  vector<double> new_y;
  vector<double> new_z;
  vector<string> new_p;
  
  for(i=0; i<=ix; i++) {
    new_x.push_back(m_vx[i]);
    new_y.push_back(m_vy[i]);
    new_z.push_back(m_vz[i]);
    new_p.push_back(m_vprop[i]);
  }
  
  new_x.push_back(x);
  new_y.push_back(y);
  new_z.push_back(z);
  new_p.push_back(vprop);

  for(i=ix+1; i<vsize; i++) {
    new_x.push_back(m_vx[i]);
    new_y.push_back(m_vy[i]);
    new_z.push_back(m_vz[i]);
    new_p.push_back(m_vprop[i]);
  }
  
  m_vx = new_x;
  m_vy = new_y;
  m_vz = new_z;
  m_vprop = new_p;
}

//---------------------------------------------------------------
// Procedure: clear

void XYSegList::clear()
{
  XYObject::clear();
  m_vx.clear();
  m_vy.clear();
  m_vz.clear();
  m_vprop.clear();
}


//---------------------------------------------------------------
// Procedure: shift_horz

void XYSegList::shift_horz(double shift_val)
{
  unsigned int i, vsize = m_vx.size();
  for(i=0; i<vsize; i++)
    m_vx[i] += shift_val;
}

//---------------------------------------------------------------
// Procedure: shift_vert

void XYSegList::shift_vert(double shift_val)
{
  unsigned int i, vsize = m_vy.size();
  for(i=0; i<vsize; i++)
    m_vy[i] += shift_val;
}

//---------------------------------------------------------------
// Procedure: grow_by_pct

void XYSegList::grow_by_pct(double pct)
{
  double cx = get_centroid_x();
  double cy = get_centroid_y();

  unsigned int i, vsize = m_vy.size();
  for(i=0; i<vsize; i++)
    grow_pt_by_pct(pct, cx, cy, m_vx[i], m_vy[i]);
}

//---------------------------------------------------------------
// Procedure: grow_by_amt

void XYSegList::grow_by_amt(double amt)
{
  double cx = get_centroid_x();
  double cy = get_centroid_y();

  unsigned int i, vsize = m_vy.size();
  for(i=0; i<vsize; i++)
    grow_pt_by_amt(amt, cx, cy, m_vx[i], m_vy[i]);
}

//---------------------------------------------------------------
// Procedure: rotate

void XYSegList::rotate(double degval, double cx, double cy)
{
  unsigned int i, vsize = m_vy.size();
  for(i=0; i<vsize; i++)
    rotate_pt(degval, cx, cy, m_vx[i], m_vy[i]);
}

//---------------------------------------------------------------
// Procedure: rotate

void XYSegList::rotate(double degval)
{
  double cx = get_centroid_x();
  double cy = get_centroid_y();

  rotate(degval, cx, cy);
}

//---------------------------------------------------------------
// Procedure: apply_snap

void XYSegList::apply_snap(double snapval)
{
  unsigned int i, vsize = m_vy.size();
  for(i=0; i<vsize; i++) {
    m_vx[i] = snapToStep(m_vx[i], snapval);
    m_vy[i] = snapToStep(m_vy[i], snapval);
  }
}

//---------------------------------------------------------------
// Procedure: reverse

void XYSegList::reverse()
{
  vector<double> new_x;
  vector<double> new_y;
  vector<double> new_z;
  vector<string> new_p;

  unsigned int i, vsize = m_vy.size();
  for(i=0; i<vsize; i++) {
    new_x.push_back(m_vx[(vsize-1)-i]);
    new_y.push_back(m_vy[(vsize-1)-i]);
    new_z.push_back(m_vz[(vsize-1)-i]);
    new_p.push_back(m_vprop[(vsize-1)-i]);
  }
  m_vx = new_x;
  m_vy = new_y;
  m_vz = new_z;
  m_vprop = new_p;
}

//---------------------------------------------------------------
// Procedure: new_center

void XYSegList::new_center(double new_cx, double new_cy)
{
  double diff_x = new_cx - get_center_x();
  double diff_y = new_cy - get_center_y();
  
  shift_horz(diff_x);
  shift_vert(diff_y);
}

//---------------------------------------------------------------
// Procedure: new_centroid

void XYSegList::new_centroid(double new_cx, double new_cy)
{
  double diff_x = new_cx - get_centroid_x();
  double diff_y = new_cy - get_centroid_y();
  
  shift_horz(diff_x);
  shift_vert(diff_y);
}

//---------------------------------------------------------------
// Procedure: valid

bool XYSegList::valid() const
{
  if((m_vx.size() == 0) && active())
    return(false);
  return(true);
}

//---------------------------------------------------------------
// Procedure: get_vx

double XYSegList::get_vx(unsigned int i) const
{
  if(i<m_vx.size())
     return(m_vx[i]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: get_vy

double XYSegList::get_vy(unsigned int i) const
{
  if(i<m_vy.size())
    return(m_vy[i]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: get_vz

double XYSegList::get_vz(unsigned int i) const
{
  if(i<m_vz.size())
    return(m_vz[i]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: get_vprop

string XYSegList::get_vprop(unsigned int i) const
{
  if(i<m_vprop.size())
    return(m_vprop[i]);
  else
    return("");
}

//---------------------------------------------------------------
// Procedure: get_center_x
//   Purpose: Return the mid point between the extreme x low, high

double XYSegList::get_center_x() const
{
  unsigned int i, vsize = m_vx.size();
  if(vsize == 0) 
    return(0.0);

  double x_high = m_vx[0];
  double x_low  = m_vx[0];
  for(i=1; i<vsize; i++) {
    if(m_vx[i] > x_high)
      x_high = m_vx[i];
    if(m_vx[i] < x_low)
      x_low  = m_vx[i];
  }
  return((x_high + x_low) / 2.0);
}

//---------------------------------------------------------------
// Procedure: get_center_y
//   Purpose: Return the mid point between the extreme y low, high

double XYSegList::get_center_y() const
{
  unsigned int i, vsize = m_vy.size();
  
  if(vsize == 0) 
    return(0.0);

  double y_high = m_vy[0];
  double y_low  = m_vy[0];
  for(i=1; i<vsize; i++) {
    if(m_vy[i] > y_high)
      y_high = m_vy[i];
    if(m_vy[i] < y_low)
      y_low  = m_vy[i];
  }
  return((y_high + y_low) / 2.0);
}

//---------------------------------------------------------------
// Procedure: get_centroid_x
//   Purpose: Return the x center of mass of all points

double XYSegList::get_centroid_x() const
{
  unsigned int i, vsize = m_vx.size();
  if(vsize == 0) 
    return(0);

  double total = 0;
  for(i=0; i<vsize; i++) 
    total += m_vx[i];
  
  return(total / ((double)(vsize)));
}

//---------------------------------------------------------------
// Procedure: get_centroid_y
//   Purpose: Return the y center of mass of all points

double XYSegList::get_centroid_y() const
{
  unsigned int i, vsize = m_vy.size();
  if(vsize == 0) 
    return(0);

  double total = 0;
  for(i=0; i<vsize; i++) 
    total += m_vy[i];
  
  return(total / ((double)(vsize)));
}

//---------------------------------------------------------------
// Procedure: get_min_x
//   Purpose: Return the min of the x values

double XYSegList::get_min_x() const
{
  unsigned int i, vsize = m_vx.size();
  if(vsize == 0) 
    return(0);

  double x_min = m_vx[0];
  for(i=1; i<vsize; i++)
    if(m_vx[i] < x_min)
      x_min = m_vx[i];
  return(x_min);
}

//---------------------------------------------------------------
// Procedure: get_max_x
//   Purpose: Return the max of the x values

double XYSegList::get_max_x() const
{
  unsigned int i, vsize = m_vx.size();
  if(vsize == 0) 
    return(0);

  double x_max = m_vx[0];
  for(i=1; i<vsize; i++)
    if(m_vx[i] > x_max)
      x_max = m_vx[i];
  return(x_max);
}

//---------------------------------------------------------------
// Procedure: get_min_y
//   Purpose: Return the min of the y values

double XYSegList::get_min_y() const
{
  unsigned int i, vsize = m_vy.size();
  if(vsize == 0) 
    return(0.0);

  double y_min = m_vy[0];
  for(i=1; i<vsize; i++)
    if(m_vy[i] < y_min)
      y_min = m_vy[i];
  return(y_min);
}

//---------------------------------------------------------------
// Procedure: get_max_y
//   Purpose: Return the max of the x values

double XYSegList::get_max_y() const
{
  unsigned int i, vsize = m_vy.size();
  if(vsize == 0) 
    return(0.0);

  double y_max = m_vy[0];
  for(i=1; i<vsize; i++)
    if(m_vy[i] > y_max)
      y_max = m_vy[i];
  return(y_max);
}

//---------------------------------------------------------------
// Procedure: get_avg_x
//   Purpose: Return the avg of the x values

double XYSegList::get_avg_x() const
{
  unsigned int i, vsize = m_vx.size();
  if(vsize == 0) 
    return(0.0);

  double x_total = 0.0;
  for(i=0; i<vsize; i++)
    x_total += m_vx[i];

  return(x_total / (double)(vsize));
}

//---------------------------------------------------------------
// Procedure: get_avg_y
//   Purpose: Return the avg of the y values

double XYSegList::get_avg_y() const
{
  unsigned int i, vsize = m_vy.size();
  if(vsize == 0) 
    return(0.0);

  double y_total = 0.0;
  for(i=0; i<vsize; i++)
    y_total += m_vy[i];

  return(y_total / (double)(vsize));
}

//---------------------------------------------------------------
// Procedure: dist_to_ctr
//   Purpose: 

double XYSegList::dist_to_ctr(double x, double y) const
{
  double ctr_x = get_center_x();
  double ctr_y = get_center_y();
  double dist  = hypot((ctr_x-x), (ctr_y-y));
  return(dist);
}


//---------------------------------------------------------------
// Procedure: max_dist_to_ctr
//   Purpose: Return the maximum distance between the center and
//            any one of the vertices in the SegList.

double XYSegList::max_dist_to_ctr() const
{
  double ctr_x = get_center_x();
  double ctr_y = get_center_y();
  
  double max_dist = 0;

  unsigned int i, vsize = m_vx.size();
  for(i=0; i<vsize; i++) {
    double dist  = hypot((ctr_x - m_vx[i]), 
			 (ctr_y - m_vy[i]));
    if(dist > max_dist)
      max_dist = dist;
  }  

  return(max_dist);
}



//---------------------------------------------------------------
// Procedure: segs_cross
//   Purpose: Determine if any two segments intersect one another
//            We exclude from consideration any two segments that
//            share a vertex. If the result is false, then this set
//            of line segments should form a polygon, although not
//            necessarily a convex polygon.
//                                                                       
//         (implied line segment)                                      
//   0 o ~~~~~~~~~~~~~~~~~~~~~~~~~ o 7    vsize = 8                      
//     |                           |                                      
//     |       3 o--------o 4      |     last pair of edges considered:  
//     |         |        |        |       5-6 with 7-0 (loop==true)
//     |         |        |        |       4-5 with 6-7 (loop==false) 
//     |         |        |        |                         
//   1 o---------o 2    5 o--------o 6               

bool XYSegList::segs_cross(bool loop) const
{
  unsigned int i, j, vsize = m_vx.size();
  if(vsize <= 3)
    return(false);

  unsigned int limit = vsize-1;
  if(loop)
    limit = vsize;

  for(i=0; i<limit; i++) {
    double x1 = m_vx[i];
    double y1 = m_vy[i];
    double x2 = m_vx[i+1];
    double y2 = m_vy[i+1];

    for(j=i+2; j<limit; j++) {
      double x3 = m_vx[j];
      double y3 = m_vy[j];
      
      unsigned int k = j+1;
      // Check if we're at the end and considering the last implied
      // segment made by connecting the last vertex to the first 
      // vertex. This will not be reached if loop==false
      if(k >= vsize)
	k = 0;
      if(!((k==0)&&(i==0))) {

	double x4 = m_vx[k];
	double y4 = m_vy[k];
	
	if(segmentsCross(x1, y1, x2, y2, x3, y3, x4, y4))
	  return(true);
      }
    }
  }

  return(false);
}

//---------------------------------------------------------------
// Procedure: length
//   Purpose: Determine the overall length between the first and
//            the last point - distance in the X-Y Plane only

double XYSegList::length()
{
  unsigned int i, vsize = m_vx.size();
  if(vsize == 0)
    return(0);

  double prev_x = m_vx[0];
  double prev_y = m_vy[0];

  double total_length = 0;
  for(i=1; i<vsize; i++) {
    double x = m_vx[i];
    double y = m_vy[i];
    total_length += hypot(x-prev_x, y-prev_y);
    prev_x = x;
    prev_y = y;
  }
  return(total_length);
}

//---------------------------------------------------------------
// Procedure: get_spec

string XYSegList::get_spec(unsigned int precision) const
{
  return(get_spec(precision, ""));
}

//---------------------------------------------------------------
// Procedure: get_spec

string XYSegList::get_spec(string param) const
{
  return(get_spec(1, param));
}

//---------------------------------------------------------------
// Procedure: get_spec
//   Purpose: Get a string specification of the seglist. We set 
//            the vertex precision to be at the integer by default.

string XYSegList::get_spec(unsigned int precision, string param) const
{
  string spec;

  // Clip the precision to be at most 6
  if(precision > 6)
    precision = 6;

  unsigned int i, vsize = m_vx.size();
  if(vsize > 0)
    spec += "pts={";
  for(i=0; i<vsize; i++) {
    spec += doubleToStringX(m_vx[i],precision);
    spec += ",";
    spec += doubleToStringX(m_vy[i],precision);
    if((m_vz[i] != 0) || (m_vprop[i] != ""))
      spec += "," + doubleToStringX(m_vz[i], precision);
    if(m_vprop[i] != "")
      spec += "," + m_vprop[i];
    if(i != vsize-1)
      spec += ":";
    else
      spec += "}";
  }
  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "") {
    if(spec != "")
      spec += ",";
    spec += obj_spec;
  }

  return(spec);
}


//---------------------------------------------------------------
// Procedure: get_spec_pts
//   Purpose: Get a string specification of the just the points. We set
//            the vertex precision to be at the integer by default.

string XYSegList::get_spec_pts(unsigned int precision) const
{
  string spec;

  // Clip the precision to be at most 6
  if(precision > 6)
    precision = 6;

  unsigned int i, vsize = m_vx.size();
  if(vsize > 0)
    spec += "pts={";
  for(i=0; i<vsize; i++) {
    spec += doubleToStringX(m_vx[i],precision);
    spec += ",";
    spec += doubleToStringX(m_vy[i],precision);
    if((m_vz[i] != 0) || (m_vprop[i] != ""))
      spec += "," + doubleToStringX(m_vz[i], precision);
    if(m_vprop[i] != "")
      spec += "," + m_vprop[i];
    if(i != vsize-1)
      spec += ":";
    else
      spec += "}";
  }
  return(spec);
}

//---------------------------------------------------------------
// Procedure: closest_vertex
//   Purpose: Find the existing vertex that is closest to the 
//            given point.

unsigned int XYSegList::closest_vertex(double x, double y) const
{
  unsigned int vsize = m_vx.size();
  if(vsize == 0)
    return(0);

  double dist = distPointToPoint(m_vx[0], m_vy[0], x, y);

  unsigned int i, ix = 0;
  for(i=1; i<vsize; i++) {
    double idist = distPointToPoint(m_vx[i], m_vy[i], x, y);
    if(idist < dist) {
      dist = idist; 
      ix = i;
    }
  }
  return(ix);
}


//---------------------------------------------------------------
// Procedure: dist_to_point
//   Purpose: Find the closest distance from the given point to
//            any point on any segment.

double XYSegList::dist_to_point(double x, double y) const
{
  if(m_vx.size() == 0)
    return(0);
  if(m_vx.size() == 1)
    return(hypot(m_vx[0]-x, m_vy[0]-y));
  
  // Use the distance to the first segment as initial "best-so-far"
  double min_dist = distPointToSeg(m_vx[0], m_vy[0], 
				   m_vx[1], m_vy[1], x, y);

  for(unsigned int i=1; i<m_vx.size()-1; i++) {
    double dist = distPointToSeg(m_vx[i], m_vy[i], 
				 m_vx[i+1], m_vy[i+1], x, y);
    if(dist < min_dist) 
      min_dist = dist; 
  }
 
  return(min_dist);
}


//---------------------------------------------------------------
// Procedure: closest_segment
//   Purpose: Find the existing segment that is closest to the 
//            given point.
//      Note: Returns the "leading" index of the segment. 

unsigned int XYSegList::closest_segment(double x, double y,
					bool check_implied_seg) const
{
  unsigned int vsize = m_vx.size();
  if(vsize <= 1)
    return(0);

  // Use the distance to the first segment as initial "best-so-far"
  double dist = distPointToSeg(m_vx[0], m_vy[0], 
			       m_vx[1], m_vy[1], x, y);

  unsigned int i, ix = 0;
  for(i=1; i<vsize-1; i++) {
    double idist = distPointToSeg(m_vx[i], m_vy[i], 
				  m_vx[i+1], m_vy[i+1], x, y);
    if(idist < dist) {
      dist = idist; 
      ix = i;
    }
  }
 
  // Check the "implied" segment from vertex n-1 to vertex zero.
  if(check_implied_seg) {
    double edist = distPointToSeg(m_vx[vsize-1], m_vy[vsize-1], 
				  m_vx[0], m_vy[0], x, y);
    if(edist < dist)
      ix = vsize-1;
  }
 
  return(ix);
}


//---------------------------------------------------------------
// Procedure: grow_pt_by_pct                                    |
//                                  o (px, py)                  |
//                                   \                          |
//                                    \                         |
//                o (px, py)           \                        |
//                 \                    \                       |
//                  \                    \                      |
//                   \                    \                     |
//                    \                    \                    |
//                     o (cx,cy)            o (cx, cy)          |
//

void XYSegList::grow_pt_by_pct(double pct, double cx, double cy, 
			       double &px, double &py)
{
  px += ((px - cx) * pct);
  py += ((py - cy) * pct);
}


//---------------------------------------------------------------
// Procedure: grow_pt_by_amt                                    |
//                                  o (px, py)                  |
//                                   \                          | 
//                                    \                         |
//                o (px, py)           \                        |
//                 \                    \                       |
//                  \                    \                      |
//                   \                    \                     |
//                    \                    \                    |
//                     o (cx,cy)            o (cx, cy)          |
//

void XYSegList::grow_pt_by_amt(double amt, double cx, double cy, 
			       double &px, double &py)
{
  double angle = relAng(cx, cy, px, py);
  projectPoint(angle, amt, px, py, px, py);
}


//---------------------------------------------------------------
// Procedure: rotate_pt

void XYSegList::rotate_pt(double deg, double cx, double cy, 
			  double &px, double &py)
{
  //cout << "Rotate_pt: " << endl;
  //cout << "cx: " << cx << "  cy: " << cy << endl;
  //cout << "px: " << px << "  py: " << py << endl;

  double curr_dist  = hypot((cx-px), (cy-py));
  double curr_angle = relAng(cx, cy, px, py);
  double new_angle  = curr_angle + deg;

  double nx, ny;

  projectPoint(new_angle, curr_dist, cx, cy, nx, ny);

  //cout << "dist: " << curr_dist << endl;
  //cout << "deg:  " << deg << endl;
  //cout << "cang: " << curr_angle << endl;
  //cout << "nang: " << new_angle << endl;
  //cout << "nx: " << nx << "  ny: " << ny << endl;
  
  px = nx; 
  py = ny;
}


















