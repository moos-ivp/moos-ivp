/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_LEQ.cpp                                         */
/*    DATE: June 15th 2006                                       */
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
#include "ZAIC_LEQ.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *ZAIC_LEQ::extractOF()
{
  // Check for error conditions first
  if(!m_state_ok)
    return(0);

  setPointLocations();

  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *ipf = new IvPFunction(pdmap);

  return(ipf);
}

//-------------------------------------------------------------
// Procedure: setPointLocations() 
//                                                             
//     |------o------o                                        |
//     |               \                                      |
//     |                 \                                    |
//     |                   \                                  |
//     |                     \                                |
//     |                       \                              |
//     |                         \                            |
//     o--------------------------o---------------------------o
//     0             1            2                           3
//                                                             

void ZAIC_LEQ::setPointLocations()
{
  int    domain_pts  = m_ivp_domain.getVarPoints(0);
  double delta       = m_ivp_domain.getVarDelta(0);

  double dpt_one = m_summit;
  double dpt_two = m_summit + m_basewidth;

  m_ipt_low  = 0;
  m_ipt_one  = (int)((dpt_one/delta)+0.5);
  m_ipt_two  = (int)((dpt_two/delta)+0.5);
  m_ipt_high = domain_pts - 1;

  // Ensure that both middle points are within bounds and that
  // point one is never greater than point two
  if(m_ipt_two > m_ipt_high)
    m_ipt_two = m_ipt_high;
  if(m_ipt_one > m_ipt_high)
    m_ipt_one = m_ipt_high;

  if(m_ipt_one < 0)
    m_ipt_one = 0;
  if(m_ipt_two < 0)
    m_ipt_two = 0;

  if(m_ipt_one > m_ipt_two)
    m_ipt_one = m_ipt_two;
  
  i_basewidth = (int)((m_basewidth + (delta/2)) / delta);
}
     
//-------------------------------------------------------------
// Procedure: setPDMap()
//            

PDMap *ZAIC_LEQ::setPDMap()
{
  int i;
  int piece_count = 0;

  IvPBox *piece[4];
  for(i=0; i<4; i++)
    piece[i] = 0;

  // Handle piece0 if it exists
  if(m_ipt_low <= m_ipt_one) {  // changed to leq, mikerb, nov1918
    piece[0] = new IvPBox(1,1);
    piece[0]->setPTS(0, 0, m_ipt_one);
    piece[0]->wt(0) = m_break_ties;
    piece[0]->wt(1) = m_maxutil;
    piece_count++;
  }

  // Handle piece1 if it exists. Building this piece involves
  // reclaiming one point from the first piece - the point at 
  // the summit.
  if((m_summit_delta > 0) && (m_ipt_one > 1)) {
    piece[1] = new IvPBox(1,1);
    piece[1]->setPTS(0, m_ipt_one, m_ipt_one);
    piece[1]->wt(0) = 0.0;
    piece[1]->wt(1) = m_maxutil + m_summit_delta;
    piece[0]->setPTS(0, 0, m_ipt_one-1);
    piece_count++;
  }

  // Handle piece2 if it exists
  if(m_ipt_one < m_ipt_two) {
    piece[2] = new IvPBox(1,1);
    piece[2]->setPTS(0, m_ipt_one+1, m_ipt_two);

    double run    = (double)(i_basewidth);
    double slope  = -(m_maxutil - m_minutil) / run;
    double intcpt;
    if(m_ipt_one > 0)
      intcpt = m_maxutil + (-slope * m_ipt_one);
    else
      intcpt = m_minutil + (-slope * m_ipt_two);

    piece[2]->wt(0) = slope;
    piece[2]->wt(1) = intcpt;
    piece_count++;
  }

  // Handle piece3 if it exists
  if(m_ipt_two < m_ipt_high) {
    piece[3] = new IvPBox(1,1);
    piece[3]->setPTS(0, m_ipt_two+1, m_ipt_high);
    piece[3]->wt(0) = -m_break_ties;
    piece[3]->wt(1) = m_minutil;
    piece_count++;
  }

  
  PDMap *pdmap;
  pdmap = new PDMap(piece_count, m_ivp_domain, 1);

  int ix = 0;
  for(i=0; i<4; i++) {
    if(piece[i]) {
      pdmap->bx(ix) = piece[i];
      ix++;
    }
  }
  return(pdmap);
}










