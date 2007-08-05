/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_Epsilon.cpp                                       */
/*    DATE: Aug 26th 2005 (from OFR_AOF written long ago)        */
/*    NOTE: "RT_" stands for "Reflector Tool"                    */
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
#include <assert.h>
#include "RT_Epsilon.h"
#include "BuildUtils.h"
#include "PDMap.h"
#include "Regressor.h"

#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

RT_Epsilon::RT_Epsilon(const AOF *g_aof, Regressor *g_reg) 
{
  m_aof        = g_aof;
  m_regressor  = g_reg;
  m_pdmap      = 0;
}

//-------------------------------------------------------------
// Procedure: create
//   Purpose:

PDMap* RT_Epsilon::create(PDMap *g_pdmap, double epsilon)
{
  m_pdmap  = g_pdmap;
  
  IvPDomain domain = m_pdmap->getDomain();

  BoxSet  p_boxes;   // Perfect boxes
  BoxSet  ip_boxes;  // Imperfect boxes

  int boxCount = m_pdmap->size();
  // Initialize the set of 'imperfect' boxes. If boxes already
  // exist in the boxes array, use these, and reset the boxes
  // and boxCount variables. If no boxes exists, start with one
  // imperfect box equal to the universe. These two starting
  // states are supported so that this function can be called
  // after uniform boxes have been made, or just from scratch.
  if(boxCount > 0) {
    for(int i=0; i<boxCount; i++)
      ip_boxes.addBox(m_pdmap->bx(i)->copy());
  }
  else {
    IvPBox* begin_box = m_pdmap->getUniverse().copy();
    ip_boxes.addBox(begin_box);
  }

  // Until there are no more 'imperfect' boxes, repeatedly pick
  // one, carve off a 'perfect' box, add the perfect box to the
  // set of perfect boxes, and put the remainders in the set of
  // imperfect boxes.
  while(ip_boxes.getSize() != 0) {
    BoxSetNode *bsn = ip_boxes.remBSN(FIRST);
    IvPBox *ip_box = bsn->getBox();
    IvPBox *p_box  = carve_two(ip_box, epsilon);
    delete(bsn);

    assert(p_box);
    p_boxes.addBox(p_box);

#if 0
    vector<IvPBox> new_ip_boxes = subtractBox(*ip_box, *p_box);
    for(int i=0; i<new_ip_boxes.size(); i++)
      ip_boxes.addBox(new_ip_boxes[i].copy(), LAST);
#endif

#if 1
    BoxSet *new_ip_boxes = subtractBox(*ip_box, *p_box);
    delete(ip_box);
    ip_boxes.merge(*new_ip_boxes);
    delete(new_ip_boxes);
#endif

  }
  
  // Populate the boxes array with the set of 'perfect' boxes
  int newBoxCount = p_boxes.getSize();
  int degree      = m_pdmap->getDegree();
  PDMap *new_pdmap = new PDMap(newBoxCount, domain, degree);

  for(int i=0; i<newBoxCount; i++) {
    BoxSetNode* bsn = p_boxes.remBSN(FIRST);
    new_pdmap->bx(i) = bsn->getBox();
    delete(bsn);
  }
  delete(m_pdmap);
  m_pdmap = new_pdmap;

  return(m_pdmap);
}

//-------------------------------------------------------------
// Procedure: carve_one
//   Purpose:

IvPBox* RT_Epsilon::carve_one(const IvPBox *g_box, double epsilon)
{
  int d;
  int dim = m_pdmap->getDim();

  int **bnds = new int*[dim];
  for(d=0; d<dim; d++) {
    bnds[d] = new int[2];
    bnds[d][0] = g_box->pt(d,0);
    bnds[d][1] = g_box->pt(d,1);
  }

  int *curr_pos = new int[dim];
  for(d=0; d<dim; d++)
    curr_pos[d] = bnds[d][0];

  IvPBox pt_box(dim, 1);
  for(d=0; d<dim; d++)
    pt_box.setPTS(d, curr_pos[d], curr_pos[d]);
  double first_val = m_aof->evalBox(&pt_box);

  IvPBox *ret_box = g_box->copy();
  ret_box->setWT(first_val);

  bool done = false;
  while(!done) {

    for(d=0; d<dim; d++)
      pt_box.setPTS(d, curr_pos[d], curr_pos[d]);

    double val = m_aof->evalBox(&pt_box);

    if(val != first_val) {
      done = true;

      bool cut_back = false;
      for(d=dim-1; d>0; d--) {
	if(curr_pos[d] > bnds[d][0]) {
	  cut_back = true;
	  ret_box->setPTS(d, bnds[d][0], curr_pos[d]-1);
	}
	else
	  ret_box->setPTS(d, bnds[d][0], bnds[d][0]);
      }
      if(cut_back)
	ret_box->setPTS(d, bnds[d][0], bnds[d][1]);
      else
	ret_box->setPTS(d, bnds[d][0], curr_pos[d]-1);
    }


    // Advance the curr_pos indicating which point is to be
    // examined next. Set done=true if we detect the end.
    int ix = dim;
    for(d=dim-1; d>=0; d--)
      if(curr_pos[d] < bnds[d][1])
	ix = d;
    if(ix==dim) {
      done = true;
    }
    else {
      for(d=0; d<ix; d++)
	curr_pos[d] = bnds[d][0];
      curr_pos[ix]++;
    }
  }

  assert(ret_box);
  return(ret_box);
}


//-------------------------------------------------------------
// Procedure: carve_two
//   Purpose:

IvPBox* RT_Epsilon::carve_two(const IvPBox *g_box, double epsilon)
{
  int d, dim = m_pdmap->getDim();

  // Find out the value at the initial point. It will be
  // the value of the return box. Remaining question only
  // is what the intervals of that return box will be.
  IvPBox *pt_box = g_box->copy();
  for(d=0; d<dim; d++)
    pt_box->pt(d, 1) = g_box->pt(d,0);
  double first_val = m_aof->evalBox(pt_box);
  delete(pt_box);

  IvPBox *ret_box = g_box->copy();
  ret_box->setWT(first_val);

  if(g_box->isPtBox())
    return(ret_box);

  int **bnds      = new int*[dim];
  bool *maxed_out = new bool[dim];
  int  *curr_pos  = new int[dim];
  for(d=0; d<dim; d++) {
    bnds[d] = new int[2];
    bnds[d][0]   = g_box->pt(d,0);
    bnds[d][1]   = g_box->pt(d,1);
    curr_pos[d]  = bnds[d][0];
    maxed_out[d] = false;
    if(bnds[d][0] == bnds[d][1])
      maxed_out[d] = true;
  }

  bool done = false;
  while(!done) {
    bool first_spread = true;
    int  min_spread;
    int  dim_ix = -1;
    for(d=0; d<dim; d++) {
      if(!maxed_out[d]) {
	int spread = curr_pos[d] - bnds[d][0];
	if(first_spread || (spread < min_spread)) {
	  min_spread = spread;
	  first_spread = false;
	  dim_ix = d;
	}
      }
    }
    curr_pos[dim_ix]++;
    IvPBox *plane_box = g_box->copy();
    for(d=0; d<dim; d++)
      plane_box->setPTS(d, bnds[d][0], curr_pos[d]);
    plane_box->setPTS(dim_ix, curr_pos[dim_ix], curr_pos[dim_ix]);

    bool result = uniform_check(plane_box, first_val);
    delete(plane_box);

    if(curr_pos[dim_ix] == bnds[dim_ix][1])
      maxed_out[dim_ix] = true;
    if(result == false) {
      curr_pos[dim_ix]--;
      maxed_out[dim_ix] = true;
    }

    // Done if all indices are maxed out
    done = true;
    for(d=0; d<dim; d++)
      done = done && maxed_out[d];
  }

  for(d=0; d<dim; d++)
    ret_box->setPTS(d, bnds[d][0], curr_pos[d]);

  for(d=0; d<dim; d++)
    delete [] bnds[d];
  delete [] bnds;
  delete [] maxed_out;
  delete [] curr_pos;

  return(ret_box);
}


//-------------------------------------------------------------
// Procedure: uniform_check
//   Purpose:

bool RT_Epsilon::uniform_check(const IvPBox *g_box, double g_val)
{
  //  return(false);
  //cout << "Uniform_check box:" << endl;
  //g_box->print();

  int d, dim = m_pdmap->getDim();

  int** bnds     = new int*[dim];
  int*  curr_pos = new int[dim];
  for(d=0; d<dim; d++) {
    bnds[d] = new int[2];
    bnds[d][0]  = g_box->pt(d,0);
    bnds[d][1]  = g_box->pt(d,1);
    curr_pos[d] = bnds[d][0];
  }

  IvPBox pt_box(dim);

  bool done = false;
  while(!done) {
    for(d=0; d<dim; d++)
      pt_box.setPTS(d, curr_pos[d], curr_pos[d]);

    //for(d=0; d<dim; d++)
    //  cout << "[" << d << ":" << curr_pos[d] << "] ";
    //cout << endl;

    double val = m_aof->evalBox(&pt_box);
    //cout << "val:" << val << endl;
    if(val != g_val) {
      for (int i=0; i<dim; i++) {
        delete[] bnds[i];
      }
      delete[] bnds;
      delete[] curr_pos;
      return(false);
    }
    // Advance the curr_pos indicating which point is to be
    // examined next. Set done=true if we detect the end.
    int ix = dim;
    for(d=dim-1; d>=0; d--)
      if(curr_pos[d] < bnds[d][1])
	ix = d;
    if(ix==dim) {
      done = true;
    }
    else {
      for(d=0; d<ix; d++)
	curr_pos[d] = bnds[d][0];
      curr_pos[ix]++;
    }
  }

  for (int i = 0; i < dim; ++i) {
    delete[] bnds[i];
  }
  delete[] bnds;
  delete[] curr_pos;  

  return(true);
}










