/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_HEQ.cpp                                         */
/*    DATE: June 15th 2006                                       */
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
#include "ZAIC_HEQ.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_HEQ::ZAIC_HEQ(IvPDomain g_domain, const string& g_varname) 
{
  m_varname      = g_varname;
  m_ivp_domain   = subDomain(g_domain, m_varname);

  m_summit       = 0;
  m_basewidth    = 0;

  m_domain_ix    = m_ivp_domain.getIndex(m_varname);
  m_domain_pts   = m_ivp_domain.getVarPoints(m_domain_ix);
  m_domain_delta = m_ivp_domain.getVarDelta(m_domain_ix);

  m_dpt_low      = m_ivp_domain.getVarLow(m_domain_ix);
  m_dpt_high     = m_ivp_domain.getVarHigh(m_domain_ix);
  m_dpt_one      = 0;
  m_dpt_two      = 0;

  m_ipt_low      = 0;
  m_ipt_one      = 0;
  m_ipt_two      = 0;
  m_ipt_high     = 0;
}

//-------------------------------------------------------------
// Procedure: setSummit

bool ZAIC_HEQ::setSummit(double g_val)
{
  bool ok = ((g_val >= m_dpt_low) && (g_val <= m_dpt_high));
  if(ok)
    m_summit = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setBaseWidth

bool ZAIC_HEQ::setBaseWidth(double g_val)
{
  bool ok = (g_val >= 0);
  if(ok)
    m_basewidth = g_val;
  return(ok);
}


//-------------------------------------------------------------
// Procedure: setParam

bool ZAIC_HEQ::setParam(const string& param, const string& g_val)
{
  string val = tolower(stripBlankEnds(g_val));
  if(param == "summit")
    return(setSummit(atof(g_val.c_str())));
  
  else if(param == "base_width")
    return(setBaseWidth(atof(g_val.c_str())));

  else
    return(false);
}
  

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *ZAIC_HEQ::extractOF()
{
  cout << "In ExtractOF !" << endl;

  // Check for error conditions first
  if(m_domain_ix == -1)
    return(0);

  bool ok = setPointLocations();
  if(!ok) 
    return(0);

  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *of = new IvPFunction(pdmap);

  return(of);
}

//-------------------------------------------------------------
// Procedure: setPointLocations()
//                                                             
//     |                                    o-----------------|
//     |                                  /                   |
//     |                                /                     |
//     |                              /                       |
//     |                            /                         |
//     |                          /                           |
//     |                        /                             |
//     o-----------------------o------------o-----------------o
//     0                       1            2                 3
//                                                             

bool ZAIC_HEQ::setPointLocations()
{
  if((m_summit < m_dpt_low) || (m_summit > m_dpt_high))
    return(false);
  
  m_dpt_one = m_summit - m_basewidth;
  m_dpt_two = m_summit;
  
  double d = m_domain_delta;

  m_ipt_low  = 0;
  m_ipt_one  = (int)((m_dpt_one/d)+0.5);
  m_ipt_two  = (int)((m_dpt_two/d)+0.5);
  m_ipt_high = m_domain_pts - 1;

  if(m_ipt_one < m_ipt_low)
    m_ipt_one = m_ipt_low;

  i_basewidth = (int)((m_basewidth + (d/2)) / d);

  cout << "dpt_low:      " << m_dpt_low    << endl;
  cout << "dpt_one:      " << m_dpt_one    << endl;
  cout << "dpt_two:      " << m_dpt_two    << endl;
  cout << "dpt_high:     " << m_dpt_high   << endl;
  cout << endl;
  cout << "ipt_low:      " << m_ipt_low    << endl;
  cout << "ipt_one:      " << m_ipt_one    << endl;
  cout << "ipt_two:      " << m_ipt_two    << endl;
  cout << "ipt_high:     " << m_ipt_high   << endl;

  return(true);
}
     
//-------------------------------------------------------------
// Procedure: setPDMap()
//            

PDMap *ZAIC_HEQ::setPDMap()
{
  int i;
  int piece_count = 0;

  IvPBox *piece[3];
  for(i=0; i<3; i++)
    piece[i] = 0;

  // Handle piece0 if it exists
  if(m_ipt_low < m_ipt_one) {
    piece[0] = new IvPBox(1,1);
    piece[0]->setPTS(0, 0, m_ipt_one-1);
    piece[0]->wt(0) = 0.0;
    piece[0]->wt(1) = 0.0;
    piece_count++;
  }

  // Handle piece1 if it exists
  if(m_ipt_one < m_ipt_two) {
    piece[1] = new IvPBox(1,1);
    piece[1]->setPTS(0, m_ipt_one, m_ipt_two-1);

    double run    = (double)(i_basewidth);
    double slope  = 100 / run;
    double intcpt = -1.0 * slope * (m_ipt_two - i_basewidth);

    piece[1]->wt(0) = slope;
    piece[1]->wt(1) = intcpt;
    piece_count++;
  }

  // Handle piece2 if it exists
  if(m_ipt_two < m_ipt_high) {
    piece[2] = new IvPBox(1,1);
    piece[2]->setPTS(0, m_ipt_two, m_ipt_high);
    piece[2]->wt(0) = 0.0;
    piece[2]->wt(1) = 100.0;
    piece_count++;
  }

  PDMap *pdmap;
  pdmap = new PDMap(piece_count, m_ivp_domain, 1);

  int ix = 0;
  for(i=0; i<3; i++) {
    if(piece[i]) {
      pdmap->bx(ix) = piece[i];
      ix++;
    }
  }

#if 0
  cout << "piece_count: " << piece_count << endl;
  for(i=0; i<3; i++) {
    cout << "[" << i << "]: ";
    if(piece[i])
      piece[i]->print();
    else
      cout << "NULL" << endl;
  }
#endif

  return(pdmap);
}








