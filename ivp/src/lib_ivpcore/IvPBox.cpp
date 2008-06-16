/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPBox.cpp                                           */
/*    DATE: Too long ago to remember (1996-1999)                 */
/*                                                               */
/* (IvP) The interval programming model is a mathematical        */
/* programming model for representing and solving multi-         */
/* objective optimization problems.                              */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#include <iostream>
#include <cmath>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "IvPBox.h"
#include "BoxSet.h"

#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))

using namespace std;

//-------------------------------------------------------------
// construct a new box with given weight and memory for the given
// number of dimensions

IvPBox::IvPBox(int g_dim, int g_degree)
{
  m_dim     = (short int) g_dim;
  m_degree  = (short int) g_degree;
  m_pts     = 0;
  m_bds     = 0;
  m_wts     = 0;

  m_markval = false;
  m_of      = 0;

  if(m_dim > 0) {
    int wtc = (m_degree * m_dim)+1;
    m_pts = new int[m_dim * 2];
    m_bds = new bool[m_dim * 2];
    m_wts = new double[wtc];
    
    int i;
    for(i=0; (i < m_dim); i++) {
      m_pts[i*2]   = 0;
      m_pts[i*2+1] = 0;
      m_bds[i*2]   = 1;    // Default - inclusive inequality
      m_bds[i*2+1] = 1;    // Default - inclusive inequality
    }
    
    for(i=0; i<wtc; i++)
      m_wts[i] = 0.0;
  }
}

//------------------------------------------------------ 
// Procedure: Constructor

IvPBox::IvPBox(const IvPBox &b)
{
  m_dim     = b.m_dim;
  m_degree  = b.m_degree;
  m_pts     = 0;
  m_bds     = 0;
  m_wts     = 0;

  m_markval = b.m_markval;
  m_of      = b.m_of;


  if(m_dim > 0) {
    int wtc = (m_degree * m_dim)+1;
    m_pts = new int[m_dim*2];
    m_bds = new bool[m_dim*2];
    m_wts = new double[wtc];

    int i;
    for(i=0; i<(m_dim*2); i++) {
      m_pts[i] = b.m_pts[i];
      m_bds[i] = b.m_bds[i];
    }
    
    for(i=0; i<wtc; i++)
      m_wts[i] = b.m_wts[i];
  }
}

//-------------------------------------------------------------
// Procedure: Destructor

IvPBox::~IvPBox()
{
  if(m_pts) delete [] m_pts;
  if(m_bds) delete [] m_bds;
  if(m_wts) delete [] m_wts;
}

//------------------------------------------------------
// Procedure: operator=
//      Note: Careful measure is taken to avoid allocating new 
//            memory if not necessary. If the dimension and
//            degree of the two boxes are the same, NO new memory
//            from the heap should be allocated.
// Jun1608 - Added needed logic to handle the case where the right
//           box is a null box. Null boxes have m_wts=0 pointer.

const IvPBox &IvPBox::operator=(const IvPBox &right)
{
  int i;
  if(&right != this) {
    m_markval = right.m_markval;
    m_of      = right.m_of;

    int wtc = (right.m_degree * right.m_dim) + 1;

    if(m_dim != right.m_dim) {
      if(m_pts) delete [] m_pts;
      if(m_bds) delete [] m_bds;
      m_pts = new int[right.m_dim*2];
      m_bds = new bool[right.m_dim*2];
    }
    
    if((m_dim != right.m_dim) || (m_degree != right.m_degree)) {
      if(m_wts) 
	delete [] m_wts;
      m_wts = 0;
      if(right.m_dim > 0)
	m_wts = new double[wtc];
    }

    m_dim     = right.m_dim;
    m_degree  = right.m_degree;

    for(i=0; i<(m_dim*2); i++) {
      m_pts[i] = right.m_pts[i];
      m_bds[i] = right.m_bds[i];
    }

    if(m_wts) {
      for(i=0; i<wtc; i++)
	m_wts[i] = right.m_wts[i];
    }  
  }

  return(*this);
}

//-------------------------------------------------------------
// Procedure: copy()
//      Note: Make a copy of this box, equal in all ways, including
//            degree and dimension.
//      Note: 

IvPBox *IvPBox::copy() const 
{
  IvPBox *retbox = new IvPBox(m_dim, m_degree);
  retbox->copy(this);
  return(retbox);
}

//-------------------------------------------------------------
// Procedure: copy(Box *&)
//      Note: Make a copy of this box, equal in all ways, including
//            degree and dimension.
//      Note: 

void IvPBox::copy(const IvPBox *gbox)
{
  assert((gbox->m_dim == m_dim) && (gbox->m_degree == m_degree));
      
  for(int d=0; (d < m_dim); d++) {
    setPTS(d, gbox->pt(d,0), gbox->pt(d,1));
    setBDS(d, gbox->bd(d,0), gbox->bd(d,1));
  }

  int wtc = getWtc();
  for(int i=0; i<wtc; i++)
    wt(i) = gbox->wt(i);

  m_of      = gbox->m_of;
  m_markval = gbox->m_markval;
}

//------------------------------------------------------ setWT
// Procedure: setWT

void  IvPBox::setWT(double gwt)
{
  int wtc = getWtc();
  for(int i=0; i<wtc-1; i++)
    m_wts[i] = 0.0;
  m_wts[wtc-1] = gwt;
}

//------------------------------------------------------ maxVal
// Procedure: maxVal
// Note: Layout for "wts" for degree = 0, 1, 2 for 2 dimensions
//  Deg:
//   0   z =                                             wts[0]
//   1   z =                         wts[0]x + wts[1]y + wts[2]
//   2   z = wts[0]x^2 + wts[1]y^2 + wts[2]x + wts[3]y + wts[4]

double IvPBox::maxVal() const
{
  int limit = m_degree * m_dim; // Index of constant component;

  double retval = m_wts[limit];

  if(m_degree == 0)
    return(retval);

  if(m_degree == 1) {
    for(int d=0; d<limit; d++)
      if(m_wts[d] < 0)
	retval += (m_wts[d] * (double)(pt(d,0)));
      else 
	retval += (m_wts[d] * (double)(pt(d,1)));
    return(retval);
  }

  if(m_degree == 2) {
    for(int d=0; (d < m_dim); d++) {
      int c_amt = 2;
      int c_pt[4];  // candidate points

      c_pt[0] = pt(d,0);
      c_pt[1] = pt(d,1);

      // y  = mx^2 + nx + b
      // y' = 2mx + n
      // 0  = 2mx + n
      // x = -n / 2m  = -wts[d+dim] / (2*wts[d]);

      double dx = -m_wts[2*d] / (2*m_wts[d]);
      if((dx > c_pt[0]) && (dx < c_pt[1])) {
	c_pt[2] = (int)(floor(dx));
	c_pt[3] = (int)(ceil(dx));
	c_amt  = 4;
      }

      double highval;
      for(int i=0; i<c_amt; i++) {
	double pval = (m_wts[d]*c_pt[i]*c_pt[i]) + (m_wts[d+m_dim]*c_pt[i]);
	if((i==0) || (pval > highval)) 
	  highval = pval;
      }
      retval += highval;
    }
    return(retval);
  }

  return(0);  // Really just a hint at an error condition
}

//------------------------------------------------------ minVal
// Procedure: minVal

double IvPBox::minVal() const
{
  int    limit  = m_degree * m_dim;
  double retval = m_wts[limit];

  if(m_degree == 0)
    return(retval);

  if(m_degree == 1) {
    for(int d=0; d<limit; d++)
      if(m_wts[d] < 0)
	retval += (m_wts[d] * (double)(pt(d,1)));
      else 
	retval += (m_wts[d] * (double)(pt(d,0)));
    return(retval);
  }

  if(m_degree == 2) {
    for(int d=0; (d < m_dim); d++) {
      int c_amt = 2;
      int c_pt[4];  // candidate points

      c_pt[0] = pt(d,0);
      c_pt[1] = pt(d,1);

      // y  = mx^2 + nx + b
      // y' = 2mx + n
      // 0  = 2mx + n
      // x = -n / 2m  = -wts[d+dim] / (2*wts[d]);

      double dx = -m_wts[2*d] / (2*m_wts[d]);
      if((dx > c_pt[0]) && (dx < c_pt[1])) {
	c_pt[2] = (int)(floor(dx));
	c_pt[3] = (int)(ceil(dx));
	c_amt  = 4;
      }

      double lowval;
      for(int i=0; i<c_amt; i++) {
	double pval = (m_wts[d]*c_pt[i]*c_pt[i]) + (m_wts[d + m_dim]*c_pt[i]);
	if((i==0) || (pval < lowval)) 
	  lowval = pval;
      }
      retval += lowval;
    }
    return(retval);
  }

  return(0);  // Really an error condition
}

//------------------------------------------------------ ptVal
// Procedure: ptVal
//   Purpose: Return value inside THIS box at the given PTBox.
//      Note: Only correctness checks are made with assertions.

double IvPBox::ptVal(const IvPBox *gbox) const
{
  assert(gbox);
  assert(gbox->getDim() == m_dim);
  assert(gbox->isPtBox());   
  assert(gbox->intersect(this));

  if(m_degree==0)
    return(m_wts[0]);
  else if(m_degree==1) {
    double retval = m_wts[m_dim];
    for(int d=0; (d < m_dim); d++)
      retval += (m_wts[d] * gbox->pt(d,0));
    return(retval);
  }
  else if(m_degree==2) {
    double retval = m_wts[m_dim * 2];
    for(int d=0; (d < m_dim); d++) {
      int p = gbox->pt(d,0);
      retval += (m_wts[d]*p*p) + (m_wts[d+m_dim]*p);
    }
    return(retval);
  }
  else
    return(0);
}


//------------------------------------------------------ maxPt
// Procedure: maxPt
//     Notes: Determine maximal point in the box.
//            Differs from maxPt(IvPBox&) in that it creates
//            the returned maxpt box, as a matter of convenience
//            to the caller. This version is a bit slower.

IvPBox IvPBox::maxPt() const
{
  IvPBox ret_box(m_dim, m_degree);
  this->maxPt(ret_box);
  return(ret_box);
}



//------------------------------------------------------ maxPt
// Procedure: maxPt
//     Notes: Determine maximal point in the box.

void IvPBox::maxPt(IvPBox& gbox) const
{
  //assert(dim == gbox.getDim());
  if(m_dim != gbox.getDim())
    return;

  if(m_degree==0)
    for(int d=0; (d < m_dim); d++) {
      gbox.pt(d, 0) = ((pt(d,1)-pt(d,0))/2) + pt(d,0);
      gbox.pt(d, 1) = gbox.pt(d, 0);
      gbox.setBDS(d, 1, 1);   // Always inclusive
    }

  if(m_degree==1)
    for(int d=0; (d < m_dim); d++) {
      if(m_wts[d] < 0)
	gbox.setPTS(d, this->pt(d,0), this->pt(d,0));
      else 
	gbox.setPTS(d, this->pt(d,1), this->pt(d,1));
      gbox.setBDS(d, 1, 1);   // Always inclusive
    }

  if(m_degree==2) {
    for(int d=0; (d < m_dim); d++) {
      int c_amt = 2;
      int c_pt[4];  // candidate points
      
      c_pt[0] = pt(d,0);
      c_pt[1] = pt(d,1);
      
      // y  = mx^2 + nx + b
      // y' = 2mx + n
      // 0  = 2mx + n
      // x = -n / 2m  = -wts[d+dim] / (2*wts[d]);
      
      double dx = -m_wts[2*d] / (2*m_wts[d]);
      if((dx > c_pt[0]) && (dx < c_pt[1])) {
	c_pt[2] = (int)(floor(dx));
	c_pt[3] = (int)(ceil(dx));
	c_amt  = 4;
      }
      
      int    highpt;
      double highval;
      for(int i=0; i<c_amt; i++) {
	int    p    = c_pt[i];
	double pval = (m_wts[d]*p*p) + (m_wts[d+m_dim]*p);
	if((i==0) || (pval > highval)) {
	  highval = pval;
	  highpt  = p;
	}
      }
      gbox.setPTS(d, highpt, highpt);
      gbox.setBDS(d, 1, 1);
    }
  }
}

//------------------------------------------------------ scaleWT
// Procedure: scaleWT

void  IvPBox::scaleWT(double amount)
{
  int wtc = getWtc();
  for(int i=0; i<wtc; i++)
    m_wts[i] = m_wts[i] * amount;
}

//------------------------------------------------------ intersect
// Procedure: intersect
//   Purpose: o Determine if THIS box intersects the given box.
//            o Returns either TRUE(1) or FALSE(0).
//            o Use IvPBox::intBox() to get actual area of intersection.

bool IvPBox::intersect(const IvPBox *gbox) const
{
  int d;
  for(d=0; (d < m_dim); d++) {        // make sure they intersect
    if(pt(d,0) > gbox->pt(d,1))
      return(false);

    if(pt(d,1) < gbox->pt(d,0))
      return(false);
  }

  for(d=0; (d < m_dim); d++) {        // make sure they intersect
    if(pt(d,0) == gbox->pt(d,1))
      if((bd(d,0)==0) || (gbox->bd(d,1)==0))
	return(false);

    if(pt(d,1) == gbox->pt(d,0))
      if((bd(d,1)==0) || (gbox->bd(d,0)==0))
	return(false);
  }
  return(true);
}

//------------------------------------------------------ intBox
// Procedure: intersect
//   Purpose: o set the return box (rbox) to represent the area
//              common to THIS box and given box.
//            o Returns 0 if they do not intersect.

bool IvPBox::intersect(IvPBox *gbox, IvPBox *&rbox) const
{
  int d;
  if(!this->intersect(gbox)) 
    return(false);

  if(!rbox)
    rbox = new IvPBox(m_dim, m_degree);

  for(d=0; (d < m_dim); d++) {            // For each dimension
    if(pt(d,0) > gbox->pt(d, 0)) {        // First handle lower
      rbox->pt(d, 0) = pt(d,0);           // bound. Not only set
      rbox->bd(d, 0) = bd(d,0);           // the lower bound, but
    }                                     // also set the bound
    if(pt(d,0) < gbox->pt(d, 0)) {        // type (in/exclusive).
      rbox->pt(d, 0) = gbox->pt(d, 0);
      rbox->bd(d, 0) = gbox->bd(d,0);
    }
    if(pt(d,0) == gbox->pt(d, 0)) {
      rbox->pt(d, 0) = gbox->pt(d, 0);
      if((bd(d,0)==0)||(gbox->bd(d,0)==0))
	rbox->bd(d, 0) = 0;
      else
	rbox->bd(d, 0) = 1;
    }

    if(pt(d,1) < gbox->pt(d, 1)) {        // Now handle the upper
      rbox->pt(d, 1) = pt(d,1);           // bound for the current
      rbox->bd(d, 1) = bd(d,1);           // dimension.
    }
    if(pt(d,1) > gbox->pt(d, 1)) {
      rbox->pt(d, 1) = gbox->pt(d, 1);
      rbox->bd(d, 1) = gbox->bd(d,1);
    }
    if(pt(d,1) == gbox->pt(d, 1)) {
      rbox->pt(d, 1) = gbox->pt(d, 1);
      if((bd(d,1)==0)||(gbox->bd(d,1)==0))
	rbox->bd(d, 1) = 0;
      else
	rbox->bd(d, 1) = 1;
    }
  }

  int wtc = getWtc();
  for(int i=0; i<wtc; i++)
    rbox->wt(i) = this->wt(i) + gbox->wt(i);

  return(true);
}

//-------------------------------------------------------------
// Procedure: isPtBox
//   Purpose: Determine if THIS box is a "point box".
//  Examples: Valid   PtBox: [X, X]
//                           (X-1, X)
//            Invalid PtBox: [X, Y], where X!=Y
//                           (X, X], [X, X), (X, X)
//                           [X-1, X]

bool IvPBox::isPtBox() const
{
  if(m_dim <= 0)
    return(false);

  for(int d=0; (d < m_dim); d++) {
    if(pt(d,0) != pt(d,1)) {
      if((pt(d,1)-pt(d,0)) != 1)
	return(false);
      else
	if((bd(d,0)!=0) || (bd(d,1)!=0))
	  return(false);
    }
    else
      if((bd(d,0)==0) || (bd(d,1)==0))  // (X,X), [X,X), (X,X]
	return(false);                  // All Invalid
  }
  return(true);
}

//-------------------------------------------------------------
void IvPBox::print(bool full) const
{
  if(m_dim == 0) {
    cout << "null_box" << endl;
    return;
  }
    
  for(int d=0; (d<m_dim); d++) {
    cout << "[d" << d << ":" << bd(d,0) << bd(d,1);
    cout << "](" << pt(d,0) << "-" << pt(d,1) << ") ";
  }
  int wtc = getWtc();
  
  if(!full) {
    cout << endl;
    return;
  }

  cout << " deg: " << m_degree; 
  cout << " wtc: " << wtc; 
  cout << " wt: ";
  for(int i=0; i<wtc; i++) {
    if(m_wts[i] == floor(m_wts[i]))
      printf("%d ", (int)m_wts[i]);
    else
      printf("%.5f ", m_wts[i]);
  }
  cout << "  maxval: " << this->maxVal();
  cout << "  minval: " << this->minVal();
  cout << "  of: " << m_of << endl;


}

//-------------------------------------------------------------
// Procedure: transDomain
//   Purpose:
//
//            newEdges=1, egdeMap={1,0,3}
//
//            Begin       Stage1   Stage2      Stage3
//            [0] data0   [0] -    [0] data1   [0] data1
//            [1] data1   [1] -    [1] data0   [1] data0
//            [2] data2   [2] -    [2] -       [2] newdata
//                        [3] -    [3] data2   [3] data2

void IvPBox::transDomain(int newEdges, const int *edgeMap)
{
  assert(newEdges>=0);

  // First handle the setting of the new piece boundardy
  int i, newDim = m_dim + newEdges;

  int *newPts = new int[newDim*2]; 
  for(i=0; i<newDim; i++) {        
    newPts[i*2]   = 0;
    newPts[i*2+1] = 0;
  }
  for(i=0; (i<m_dim); i++) {
    newPts[edgeMap[i]*2]   = m_pts[i*2];
    newPts[edgeMap[i]*2+1] = m_pts[i*2+1];
  }

  bool *newBds = new bool[newDim*2];
  for(i=0; i<newDim; i++) {
    newBds[i*2]   = 1;
    newBds[i*2+1] = 1;
  }
  for(i=0; i<m_dim; i++) {
    newBds[edgeMap[i]*2]   = m_bds[i*2];
    newBds[edgeMap[i]*2+1] = m_bds[i*2+1];
  }

  delete [] m_pts;  m_pts=newPts;
  delete [] m_bds;  m_bds=newBds;

  // Now handle the setting of the new interior function
  if(m_degree != 0) {
    int     newWtc = m_dim + newEdges + 1;
    double *newWts = new double[newWtc];
    for(i=0; i<newWtc; i++)
      newWts[i] = 0.0;
    for(i=0; i<m_dim; i++)
      newWts[edgeMap[i]] = m_wts[i];
    newWts[newWtc-1] = m_wts[m_dim];
    delete [] m_wts;
    m_wts = newWts;
  }

  m_dim = newDim;
}










