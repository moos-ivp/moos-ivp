/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Regressor.cpp                                        */
/*    DATE: Dec 5th, 2004 (Sat at Brueggers)                     */
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
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "Regressor.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor
//     Notes: g_aof is the underlying function to be approximated.
//      Note: degree = 0  SCALAR
//            degree = 1  LINEAR    (default)
//            degree = 2  QUADRATIC

Regressor::Regressor(const AOF *g_aof, int g_degree) 
{
  assert(g_aof != 0);

  aof    = g_aof;
  degree = g_degree;

  // We don't know the dim until runtime, but when we do, it will
  // be the same for each and every box that has its weight set 
  // by this regressor. So we will allocate some useful memory
  // as member variables here to be used repeatedly in each call
  // to setWeight().
  dim = aof->getDim();

  // The coner_points represent the points of a box being fitted.
  // We create a box for each point of the appropriate dimension
  // and use them repeatedly in each call to setWeight().
  corners      = (int)(pow(2.0, (double)dim));
  corner_point = new IvPBox* [corners];
  corner_val   = new double [corners];
  for(int i=0; i<corners; i++)
    corner_point[i] = new IvPBox(dim);

  // The vals array holds all the linear and/or coefficients 
  // determined during the setWeight function. Enough memory is
  // allocated to hande quadratic, linear or scalar.
  // Allocating this memory here rather than once per each call 
  // to setWeight is done to limit calls to the heap, saving time.
  vals = new double[(dim*2)+1];

  // The center_point is a placeholder for point sampled at the 
  // center of a given box being fitted - if it has a center.
  center_point = new IvPBox(dim);

  // The mask array is useful on each setWeight call
  // mask[0]=1, mask[1]=2, mask[2]=4, and so on.
  mask = new int[dim];
  mask[0] = 1;
  for(int d=1; d<dim; d++)
    mask[d] = 2 * mask[d-1];

  // If true, then after setWeight is finished for a given box, 
  // all points, when evaluated by the interior function, must
  // be within the range of values for all the sampled pts when
  // constructing the interior function. This is sometimes a
  // desirable property, but there is typically a small measure
  // of overall fit that is sacrificed.
  strict_range = true;
}

//-------------------------------------------------------------
// Procedure: Destructor

Regressor::~Regressor()
{
  delete(center_point);

  for(int i=0; i<corners; i++)
    delete(corner_point[i]);
  delete [] corner_point;
  delete [] corner_val;
  delete [] mask;
  delete [] vals;
}

//-------------------------------------------------------------
// Procedure: setWeight
//      Note: degree = 0  SCALAR
//            degree = 1  LINEAR    (default)
//            degree = 2  QUADRATIC

double Regressor::setWeight(IvPBox *gbox, bool feedback)
{
  if(degree==0)  // Piecewise Scalar
    return(setWeight0(gbox, feedback));
  if(degree==1)  // Piecewise Linear
    return(setWeight1(gbox, feedback));
  if(degree==2)  // Piecewise Quadratic
    return(setWeight2(gbox, feedback));
}

//-------------------------------------------------------------
// Procedure: setWeight0
//   Purpose: Set the interior function of the box to a SCALAR
//            value that is just the average of points sampled
//            at the corners and at the center point (if there
//            is indeed a center point).

double Regressor::setWeight0(IvPBox *gbox, bool feedback)
{
  int i, d;
  setCorners(gbox);
  
  bool center_flag = centerBox(gbox, center_point);
  if(center_flag)
    center_val = aof->evalBox(center_point);

  double val = 0.0;
  for(i=0; i<corners; i++)
    val += corner_val[i];
  double denominator = (double)(corners);
  if(center_flag) {
    val += center_val;
    denominator += 1.0;
  }
  val = val / denominator;
  gbox->setWT(val);

  if(!feedback)
    return(0);  // returning error est. yet to be implemented
  else {
    double error = 0.0;
    for(i=0; i<corners; i++) {
      double delta  = corner_val[i] - val;
      error += (delta * delta);
    }
    return(sqrt(error));
  }
    

}

//-------------------------------------------------------------
// Procedure: setWeight1
//   Purpose: Set the interior function of the box to a LINEAR
//            function in d-dimensions. This is the primary type
//            of interior function for IvP functions. 

double Regressor::setWeight1(IvPBox *gbox, bool feedback)
{
  int i, d;
  setCorners(gbox);
  
  bool center_flag = centerBox(gbox, center_point);
  if(center_flag)
    center_val = aof->evalBox(center_point);

  for(d=0; d<=dim; d++)
    vals[d] = 0.0;
  
  // Determine the highest and lowest values of all the evaluated
  // points at each corner and at the center if have center point.
  double high_val = corner_val[0];
  double low_val  = corner_val[0];
  for(i=1; i<corners; i++) {
    if(corner_val[i] > high_val) 
      high_val = corner_val[i];
    if(corner_val[i] < low_val)
      low_val = corner_val[i];
  }
  if(center_flag) {
    if(center_val > high_val) 
      high_val = center_val;
    if(center_val < low_val)
      low_val = center_val;
  }    
  
  // Now calculate the various "slopes" for each dimension. There
  // should be 2^(dim-1) calculations for each dimension.
  for(d=0; d<dim; d++) {
    int    diff = mask[d];
    int    jump_cnt = diff;
    double run  = (double)(gbox->pt(d,1) - gbox->pt(d,0));
    
    if(run == 0)
      vals[d] = 0.0;
    else {
      double rise = 0.0;
      for(i=diff; i<corners; i++) {
	rise += corner_val[i] - corner_val[i-diff];
	jump_cnt--;
	if(!jump_cnt) {
	  i += diff;
	  jump_cnt = diff;
	}
      }
      rise = rise / ((double)(corners)/2.0);
      vals[d] = rise / (double)run;
    }
  }
  
  // Calculated and used only if strict_range is true
  double max_intercept, min_intercept; 
  
  // For each of the corners, calculate what would be the correct
  // intercept for that point. If strict_range is true, also 
  // calculate what would be the correct intercept at that point
  // for the overall high_val and low_val.
  
  double total_intercept = 0.0;
  for(i=0; i<corners; i++) {
    double planeVal = 0.0;
    for(d=0; d<dim; d++)
      planeVal += vals[d] * (double)(corner_point[i]->pt(d, 0));
    total_intercept += (corner_val[i] - planeVal);
    
    if(strict_range) {
      double imax_intercept = (high_val - planeVal);
      double imin_intercept = (low_val - planeVal);
      if((i==0) || (imax_intercept < max_intercept))
	max_intercept = imax_intercept;
      if((i==0) || (imin_intercept > min_intercept))
	min_intercept = imin_intercept;
    }
  }
  
  // If there is a center_point being fitted, do the same
  // as for each of the corner points above.
  if(center_flag) {
    double planeVal = 0.0;
    for(d=0; d<dim; d++)
      planeVal += vals[d] * (double)(center_point->pt(d, 0));
    total_intercept += (center_val - planeVal);
    
    if(strict_range) {
      double cmax_intercept = (high_val - planeVal);
      double cmin_intercept = (low_val - planeVal);
      if(cmax_intercept < max_intercept)
	max_intercept = cmax_intercept;
      if(cmin_intercept > min_intercept)
	min_intercept = cmin_intercept;
    }
  }
  
  // Now calculate the average intercept given all the corners
  // and possibly the center_point as well.
  double avg_intercept;
  if(center_flag)
    avg_intercept = total_intercept / (double)(corners+1);
  else
    avg_intercept = total_intercept / (double)(corners);
  
  // If strict_range is true, we will have calculated an 
  // allowable range for the intercept. If the avg_intercept
  // is outside the allowable range, adjust it.
  if(strict_range) {
    if(avg_intercept > max_intercept)
      avg_intercept = max_intercept;
    if(avg_intercept < min_intercept)
      avg_intercept = min_intercept;
  }
  
  // Put this intercept value where it belongs with the other
  // linear coefficients.
  vals[dim] = avg_intercept;
  
  // Finally, set the linear coefficients on the given box.
  for(d=0; d<dim+1; d++)
    gbox->wt(d) = vals[d];

  if(!feedback)
    return(0);
  else {
    double error = 0;
    for(i=0; i<corners; i++) {
      double linear_value = vals[dim];
      for(int d=0; d<dim; d++)
	linear_value += (vals[d] * corner_point[i]->pt(d,0));
      double delta = (linear_value - corner_val[i]);
      error += (delta * delta);
    }
    if(center_flag) {
      double linear_value = vals[dim];
      for(int d=0; d<dim; d++)
	linear_value += (vals[d] * center_point->pt(d,0));
      double delta = (linear_value - center_val);
      error += (delta * delta);
    }
    
    error = sqrt(error);

    if(center_flag)
      error = error / (double)(corners+1);
    else
      error = error / (double)(corners);

    return(error);
  }	
}


//-------------------------------------------------------------
// Procedure: setWeight2
//   Purpose: Set the interior function of the box to a QUADRATIC
//            function in d-dimensions. 
//
//            BUGGY - UNDER DEVELOPMENT

double Regressor::setWeight2(IvPBox *gbox, bool feedback)
{
  int i, d;
  setCorners(gbox);
  
  bool center_flag = centerBox(gbox, center_point);
  if(center_flag)
    center_val = aof->evalBox(center_point);

  for(d=0; d<=(dim*2); d++)
    vals[d] = 0.0;
  
  // Now calculate the various quadratic coefficients for each 
  // dimension. 
  for(d=0; d<dim; d++) {
    double run  = (double)(gbox->pt(d,1) - gbox->pt(d,0));
    int    diff = mask[d];
    int    jump_cnt = diff;
    
    if(run == 0) // do nothing - vals[d], vals[d+dim] already 0.
      {} 

    else if(run == 1) {   // will be no quadratic component
      double rise = 0.0;
      for(i=diff; i<corners; i++) {
	rise += corner_val[i] - corner_val[i-diff];
	jump_cnt--;
	if(!jump_cnt) {
	  i += diff;
	  jump_cnt = diff;
	}
      }
      rise = rise / ((double)(corners)/2.0);
      vals[d] = 0;
      vals[d+dim] = rise / (double)run;
    }
    else {
      double x1, y1, x2, y2, x3, y3, qm, qn, qb;
      double tm = 0;
      double tn = 0;
      double tb = 0;
      int count = 0;
      for(i=diff; i<corners; i++) {
	count++;
	x3 = corner_point[i]->pt(d,0);
	x2 = center_point->pt(d,0);
	x1 = corner_point[i-diff]->pt(d,0);
	y3 = corner_val[i];
	y2 = center_val;
	y1 = corner_val[i-diff];
	setQuadCoeffs(x1, y1, x2, y2, x3, y3, qm, qn, qb);
	cout << "x1:" << x1 << " y1:" << y1 << "   x2:" << x2 << " y2:" 
	     << y2 << "   x3:" << x3 << " y3:" << y3 << endl;
	cout << "qm: " << qm << "  qn: " << qn << "  qb: " << qb << endl;
	tm += qm;
	tn += qn;
	tb += qb;
	cout << "tm: " << tm << "  tn: " << tn << "  tb: " << tb << endl;
	jump_cnt--;
	if(!jump_cnt) {
	  i += diff;
	  jump_cnt = diff;
	}
      }
      vals[d]      = (tm / (double)(count));
      vals[d+dim]  = (tn / (double)(count));
      vals[dim*2] += (tb / (double)(count));
    }
    cout << "vals[" << d << "]: " << vals[d];
    cout << "  vals[" << d+dim << "]: " << vals[d+dim];
    cout << "  vals[" << dim*2 << "]: " << vals[dim*2] << endl;

    cout << "---------------" << endl;
  }

  double total_intercept = 0.0;
  for(i=0; i<corners; i++) {
    double quadVal = 0.0;
    for(d=0; d<dim; d++) {
      double p = (double)(corner_point[i]->pt(d, 0));
      quadVal += vals[d]*p*p + vals[d+dim]*p;
    } 
    total_intercept += (corner_val[i] - quadVal);
    
  }
  
  // If there is a center_point being fitted, do the same
  // as for each of the corner points above.
  if(center_flag) {
    double quadVal = 0.0;
    for(d=0; d<dim; d++) {
      double p = (double)(center_point->pt(d, 0));
      quadVal += vals[d]*p*p + vals[d+dim]*p;
    }
    total_intercept += (center_val - quadVal);
    
  }
  
  // Now calculate the average intercept given all the corners
  // and possibly the center_point as well.
  double avg_intercept;
  if(center_flag)
    avg_intercept = total_intercept / (double)(corners+1);
  else
    avg_intercept = total_intercept / (double)(corners);
  
  // Put this intercept value where it belongs with the other
  // quadratic coefficients.
  vals[dim*2] = avg_intercept;
  
  // Finally, set the coefficients on the given box.
  for(d=0; d<=(dim*2); d++) {
    gbox->wt(d) = vals[d];
  }
  return(0);  // returning error est. yet to be implemented
}

//-------------------------------------------------------------
// Procedure: setCoeffs
//      Note: Use Gaussian elimination to solve a group of three
//            equations with three unknowns.
//
//            qm(x1^2) + qn(x1) + qb = y1
//            qm(x2^2) + qn(x2) + qb = y2
//            qm(x3^2) + qn(x3) + qb = y3
//
//                                -1
//            |m11 m12 m13|   |qm|     |m14|
//            |m21 m22 m23| * |qn|   = |m24|
//            |m31 m32 m33|   |qb|     |m34|

void Regressor::setQuadCoeffs(double  x1, double  y1, double  x2, 
			      double  y2, double  x3, double  y3, 
			      double &qm, double &qn, double &qb)
{
  if((x1==x2) || (x1==x3) || (x2==x3))
    return;

  double m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34;
  m11 = x1*x1;    m12 = x1;    m13 = 1;    m14 = y1;  
  m21 = x2*x2;    m22 = x2;    m23 = 1;    m24 = y2;
  m31 = x3*x3;    m32 = x3;    m33 = 1;    m34 = y3;

  // Gaussian elimination step 1
  m31 = m21 - m31;
  m32 = m22 - m32;
  m33 = 0;
  m34 = m24 - m34;

  // Gaussian elimination step 2
  m21 = m11 - m21;
  m22 = m12 - m22;
  m23 = 0;
  m24 = m14 - m24;

  // Gaussian elimination pre-step 3 - make m22 and m32 equal
  double temp_m22 = m22;
  m21 *= m32;   
  m22 *= m32;   
  m24 *= m32;   
  m31 *= temp_m22; 
  m32 *= temp_m22;
  m34 *= temp_m22;

  // Gaussian elimination step 3
  m31 = m21 - m31;
  m32 = 0;
  m33 = 0;
  m34 = m24 - m34;

  // solve for qm
  qm = m34 / m31;

  // solve for qn
  qn = (m24 -(m21 * qm)) / m22; 

  // solve for qb
  qb = (m14 - (m11 * qm) - (m12 * qn)) / m13; 
}


//-------------------------------------------------------------
// Procedure: setCorners
//   Purpose: To set the corners of the given box, and then to 
//            eval the AOF at each corner. The trick is to NOT
//            eval the AOF more than once if a box has an edge 
//            length equal to 1 in one or more dimensions. It is
//            thought that evaluating the AOF is typically the 
//            most expensive part of regression so we try to 
//            avoid this if the opportunity is there.
//        
//   Example: In the example, the box has edge length=1 in the
//            dimension=1. So we should do only 4 evals on the
//            AOF rather than 8. 
//              given 3D box:  
//                 box->setPTS(0, 0, 10)
//                 box->setPTS(1, 5, 5)    So emask = 010 = 2
//                 box->setPTS(2, 2, 15)
//           
//            Borrow = (emask & i)
//            Lender = ((emask & i) ^ i)    ^ is XOR operator
//                         
//                        Borrow   Lender
//            i=0  0 0 0     no      n/a
//              1  0 0 1     no      n/a
//              2  0 1 0     yes     000
//              3  0 1 1     yes     001
//              4  1 0 0     no      n/a
//              5  1 0 1     no      n/a
//              6  1 1 0     yes     100
//              7  1 1 1     yes     101
//           
//           
//                        (110)
//                       6------------7 (111)
//                      /|           /|
//               dim=2 / |          / |
//                    /            /  |
//                   /   |        /   |
//                  2------------3    |
//                  |            |    |
//                  |    |       |    | 
//                  |    4- - - -| - -5 (101)
//            dim=1 |   / (100)  |   /
//                  |            |  /   
//                  | /          | /
//                  |/           |/
//                  0------------1 (001)
//                       dim=0
//           

void Regressor::setCorners(IvPBox *gbox)
{
  int i, d;
  
  for(i=0; i<corners; i++) {
    for(d=0; d<dim; d++) {
      bool edge = (i & mask[d]);
      int  val  = gbox->pt(d, edge);
      corner_point[i]->setPTS(d, val, val);
    }
  }
  
  emask = 0;
  for(d=0; d<dim; d++)
    if(gbox->pt(d,1) == gbox->pt(d,0))
      emask += mask[d];

  // Evaluate the AOF at each of the corners. If one or more of the 
  // edge lengths of the gbox is 1 (high==low) then avoid evaluating
  // the AOF at that point by "borrowing" its value from another pt.
  corner_val[0] = aof->evalBox(corner_point[0]);
  int count = 1;
  for(i=1; i<corners; i++) {
    bool borrow = (emask & i);
    if(borrow) {
      int lender = ((emask & i) ^ i);
      corner_val[i] = corner_val[lender];
    }
    else
      corner_val[i] = aof->evalBox(corner_point[i]);

  }
}

//-------------------------------------------------------------
// Procedure: evalPtBox()
//   Purpose: Evaluate a point box based on the set of linear coefficients.

double Regressor::evalPtBox(const IvPBox *gbox, const double *vals)
{
  double return_value = vals[dim];
  for(int d=0; d<dim; d++)
    return_value += (vals[d] * gbox->pt(d,0));
  
  return(return_value);
}



//---------------------------------------------------------------
// Procedure: centerBox
//   Purpose: Return a point box "inside" the container_box. If the
//            container box has no interior boxes, it returns false.
//            An interior point box is one that is contained by the 
//            container_box, but is *not* one of the corners.
//      Note: If rbox is non-null and ther 


bool Regressor::centerBox(const IvPBox *container_box, IvPBox *rbox)
{
  // If error conditions, halt with assert, or return false if
  // compiled with optimization flags
  if(!container_box || !rbox) {
    assert(0);
    return(false);
  }

  int dim = container_box->getDim();

  if(dim != rbox->getDim()) {
    assert(0);
    return(false);
  }

  bool valid = false;
  for(int d=0; d<dim; d++) {
    int diff = container_box->pt(d,1) - container_box->pt(d,0);
    if(diff > 1) 
      valid = true;
    int pt = container_box->pt(d,0) + (diff / 2);
    rbox->setPTS(d, pt, pt);
  }
  
  return(valid);
}








