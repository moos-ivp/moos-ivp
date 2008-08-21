/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYSegList.cpp                                        */
/*    DATE: Apr 20th, 2005                                       */
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
#include <stdlib.h>
#include <string.h>
#include "XYSegList.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: add_vertex

inline void XYSegList::add_vertex(double x, double y, double z)
{
  vertex_x.push_back(x);
  vertex_y.push_back(y);
  vertex_z.push_back(z);
}

//---------------------------------------------------------------
// Procedure: alter_vertex
//   Purpose: Given a new vertex, find the existing vertex that is
//            closest, and replace it with the new one.

void XYSegList::alter_vertex(double x, double y, double z)
{
  int vsize = vertex_x.size();

  if(vsize == 0)
    return;

  int ix   = closest_vertex(x, y); 
  vertex_x[ix] = x;
  vertex_y[ix] = y;
  vertex_z[ix] = z;
}

//---------------------------------------------------------------
// Procedure: delete_vertex
//   Purpose: Given a new vertex, find the existing vertex that is
//            closest, and delete it.

void XYSegList::delete_vertex(double x, double y)
{
  int vsize = vertex_x.size();

  if(vsize == 0)
    return;

  int i, ix = closest_vertex(x, y); 

  vector<double> new_x;
  vector<double> new_y;
  vector<double> new_z;
  
  for(i=0; i<ix; i++) {
    new_x.push_back(vertex_x[i]);
    new_y.push_back(vertex_y[i]);
    new_z.push_back(vertex_z[i]);
  }
  for(i=ix+1; i<vsize; i++) {
    new_x.push_back(vertex_x[i]);
    new_y.push_back(vertex_y[i]);
    new_z.push_back(vertex_z[i]);
  }
  
  vertex_x = new_x;
  vertex_y = new_y;
  vertex_z = new_z;
}

//---------------------------------------------------------------
// Procedure: insert_vertex
//   Purpose: Given a new vertex, find the existing segment that is
//            closest, and add the vertex between points

void XYSegList::insert_vertex(double x, double y, double z)
{
  int vsize = vertex_x.size();

  if(vsize <= 1)
    return(add_vertex(x,y));

  int i, ix = closest_segment(x, y); 

  vector<double> new_x;
  vector<double> new_y;
  vector<double> new_z;
  
  for(i=0; i<=ix; i++) {
    new_x.push_back(vertex_x[i]);
    new_y.push_back(vertex_y[i]);
    new_z.push_back(vertex_z[i]);
  }
  
  new_x.push_back(x);
  new_y.push_back(y);
  new_z.push_back(z);

  for(i=ix+1; i<vsize; i++) {
    new_x.push_back(vertex_x[i]);
    new_y.push_back(vertex_y[i]);
    new_z.push_back(vertex_z[i]);
  }
  
  vertex_x = new_x;
  vertex_y = new_y;
  vertex_z = new_z;
}

//---------------------------------------------------------------
// Procedure: clear

void XYSegList::clear()
{
  vertex_x.clear();
  vertex_y.clear();
  vertex_z.clear();
}


//---------------------------------------------------------------
// Procedure: shift_horz

void XYSegList::shift_horz(double shift_val)
{
  int vsize = vertex_x.size();
  for(int i=0; i<vsize; i++)
    vertex_x[i] += shift_val;
}

//---------------------------------------------------------------
// Procedure: shift_vert

void XYSegList::shift_vert(double shift_val)
{
  int vsize = vertex_y.size();
  for(int i=0; i<vsize; i++)
    vertex_y[i] += shift_val;
}

//---------------------------------------------------------------
// Procedure: grow_by_pct

void XYSegList::grow_by_pct(double pct)
{
  double cx = get_center_x();
  double cy = get_center_y();

  int vsize = vertex_y.size();
  for(int i=0; i<vsize; i++)
    grow_pt_by_pct(pct, cx, cy, vertex_x[i], vertex_y[i]);
}

//---------------------------------------------------------------
// Procedure: grow_by_amt

void XYSegList::grow_by_amt(double amt)
{
  double cx = get_center_x();
  double cy = get_center_y();

  int vsize = vertex_y.size();
  for(int i=0; i<vsize; i++)
    grow_pt_by_amt(amt, cx, cy, vertex_x[i], vertex_y[i]);
}

//---------------------------------------------------------------
// Procedure: rotate

void XYSegList::rotate(double degval)
{
  double cx = get_center_x();
  double cy = get_center_y();

  int vsize = vertex_y.size();
  for(int i=0; i<vsize; i++)
    rotate_pt(degval, cx, cy, vertex_x[i], vertex_y[i]);
}

//---------------------------------------------------------------
// Procedure: apply_snap

void XYSegList::apply_snap(double snapval)
{
  int vsize = vertex_y.size();
  for(int i=0; i<vsize; i++) {
    vertex_x[i] = snapToStep(vertex_x[i], snapval);
    vertex_y[i] = snapToStep(vertex_y[i], snapval);
  }
}

//---------------------------------------------------------------
// Procedure: reverse

void XYSegList::reverse()
{
  vector<double> new_x;
  vector<double> new_y;
  vector<double> new_z;

  int vsize = vertex_y.size();
  for(int i=0; i<vsize; i++) {
    new_x.push_back(vertex_x[(vsize-1)-i]);
    new_y.push_back(vertex_y[(vsize-1)-i]);
    new_z.push_back(vertex_z[(vsize-1)-i]);
  }
  vertex_x = new_x;
  vertex_y = new_y;
  vertex_z = new_z;
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
// Procedure: print

void XYSegList::print() const
{
  cout << "label:" << m_label << endl;
  int vsize = vertex_x.size();
  for(int i=0; i<vsize; i++)
    cout << "  x=" << vertex_x[i] 
	 << "  y=" << vertex_y[i]
	 << "  z=" << vertex_z[i] << endl;
}

//---------------------------------------------------------------
// Procedure: get_vx

double XYSegList::get_vx(unsigned int i) const
{
  if((i>=0) && (i<vertex_x.size()))
     return(vertex_x[i]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: get_vy

double XYSegList::get_vy(unsigned int i) const
{
  if((i>=0) && (i<vertex_y.size()))
    return(vertex_y[i]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: get_vz

double XYSegList::get_vz(unsigned int i) const
{
  if((i>=0) && (i<vertex_z.size()))
    return(vertex_z[i]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: get_center_x
//   Purpose: Return the mid point between the extreme x low, high

double XYSegList::get_center_x() const
{
  int vsize = vertex_x.size();
  
  if(vsize == 0) return(0.0);

  double x_high = vertex_x[0];
  double x_low  = vertex_x[0];
  for(int i=1; i<vsize; i++) {
    if(vertex_x[i] > x_high)
      x_high = vertex_x[i];
    if(vertex_x[i] < x_low)
      x_low  = vertex_x[i];
  }
  return((x_high + x_low) / 2.0);
}

//---------------------------------------------------------------
// Procedure: get_center_y
//   Purpose: Return the mid point between the extreme y low, high

double XYSegList::get_center_y() const
{
  int vsize = vertex_y.size();
  
  if(vsize == 0) return(0.0);

  double y_high = vertex_y[0];
  double y_low  = vertex_y[0];
  for(int i=1; i<vsize; i++) {
    if(vertex_y[i] > y_high)
      y_high = vertex_y[i];
    if(vertex_y[i] < y_low)
      y_low  = vertex_y[i];
  }
  return((y_high + y_low) / 2.0);
}

//---------------------------------------------------------------
// Procedure: get_avg_x
//   Purpose: Return the avg of the x values

double XYSegList::get_avg_x() const
{
  int vsize = vertex_x.size();
  
  if(vsize == 0) return(0.0);

  double x_total = 0.0;
  for(int i=0; i<vsize; i++)
    x_total += vertex_x[i];

  return(x_total / vsize);
}

//---------------------------------------------------------------
// Procedure: get_avg_y
//   Purpose: Return the avg of the y values

double XYSegList::get_avg_y() const
{
  int vsize = vertex_y.size();
  
  if(vsize == 0) return(0.0);

  double y_total = 0.0;
  for(int i=0; i<vsize; i++)
    y_total += vertex_y[i];

  return(y_total / vsize);
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
  int i, j, k, vsize = vertex_x.size();
  if(vsize <= 3)
    return(false);

  int limit = vsize-1;
  if(loop)
    limit = vsize;

  for(i=0; i<limit; i++) {
    double x1 = vertex_x[i];
    double y1 = vertex_y[i];
    double x2 = vertex_x[i+1];
    double y2 = vertex_y[i+1];

    for(int j=i+2; j<limit; j++) {
      double x3 = vertex_x[j];
      double y3 = vertex_y[j];
      
      int k = j+1;
      // Check if we're at the end and considering the last implied
      // segment made by connecting the last vertex to the first 
      // vertex. This will not be reached if loop==false
      if(k >= vsize)
	k = 0;
      if(!((k==0)&&(i==0))) {

	double x4 = vertex_x[k];
	double y4 = vertex_y[k];
	
	if(segmentsCross(x1,y1, x2,y2, x3,y3, x4,y4))
	  return(true);
      }
    }
  }

  return(false);
}

//---------------------------------------------------------------
// Procedure: get_spec
//   Purpose: Get a string specification of the seglist. We set 
//            the vertex precision to be at the integer by default.

string XYSegList::get_spec(int precision) const
{
  string spec;

  // Clip the precision to be between 0 and 6
  if(precision < 0)
    precision = 0;
  else if(precision > 6)
    precision = 6;
  
  if(m_label != "")
    spec += "label," + m_label + " : "; 

  int vsize = vertex_x.size();
  for(int i=0; i<vsize; i++) {
    spec += dstringCompact(doubleToString(vertex_x[i],precision));
    spec += ",";
    spec += dstringCompact(doubleToString(vertex_y[i],precision));
    //  spec += ",";
    //  spec += dstringCompact(doubleToString(vertex_z[i]));
    if(i != vsize-1)
      spec += ":";
  }
  return(spec);
}

//---------------------------------------------------------------
// Procedure: closest_vertex
//   Purpose: Find the existing vertex that is closest to the 
//            given point.

int XYSegList::closest_vertex(double x, double y) const
{
  int vsize = vertex_x.size();

  if(vsize == 0)
    return(0);

  int    ix   = 0; 
  double dist = distPointToPoint(vertex_x[0], vertex_y[0], x, y);

  for(int i=1; i<vsize; i++) {
    double idist = distPointToPoint(vertex_x[i], vertex_y[i], x, y);
    if(idist < dist) {
      dist = idist; 
      ix = i;
    }
  }
  return(ix);
}



//---------------------------------------------------------------
// Procedure: closest_segment
//   Purpose: Find the existing segment that is closest to the 
//            given point.
//      Note: Returns the "leading" index of the segment. 

int XYSegList::closest_segment(double x, double y) const
{
  int vsize = vertex_x.size();

  if(vsize <= 1)
    return(0);

  // Use the distance to the first segment as initial "best-so-far"
  int    ix   = 0; 
  double dist = distPointToSeg(vertex_x[0], vertex_y[0], 
			       vertex_x[1], vertex_y[1], x, y);


  for(int i=1; i<vsize-1; i++) {
    double idist = distPointToSeg(vertex_x[i], vertex_y[i], 
				  vertex_x[i+1], vertex_y[i+1], x, y);
    if(idist < dist) {
      dist = idist; 
      ix = i;
    }
  }
 
  // Check the "implied" segment from vertex n-1 to vertex zero.
  double edist = distPointToSeg(vertex_x[vsize-1], vertex_y[vsize-1], 
				vertex_x[0], vertex_y[0], x, y);
  if(edist < dist)
    ix = vsize-1;
 
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
  //if(pct <= 0)
  //  return;

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









