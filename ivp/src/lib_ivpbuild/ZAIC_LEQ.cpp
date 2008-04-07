/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_LEQ.cpp                                         */
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
#include "ZAIC_LEQ.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_LEQ::ZAIC_LEQ(IvPDomain g_domain, const string& varname) 
{
  m_state_ok     = true;

  m_ivp_domain   = subDomain(g_domain, varname);
  if(m_ivp_domain.size() != 1)
    m_state_ok = false;

  m_summit       = 0;
  m_basewidth    = 0;
  m_minutil      = 0;
  m_maxutil      = 100.0;

  m_ipt_low      = 0;
  m_ipt_one      = 0;
  m_ipt_two      = 0;
  m_ipt_high     = 0;
}

//-------------------------------------------------------------
// Procedure: setSummit

bool ZAIC_LEQ::setSummit(double val)
{
  double dpt_low  = m_ivp_domain.getVarLow(0);
  double dpt_high = m_ivp_domain.getVarHigh(0);

  if(val < dpt_low)
    val = dpt_low;
  else if(val > dpt_high)
    val = dpt_high;
  
  m_summit = val;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setBaseWidth

bool ZAIC_LEQ::setBaseWidth(double val)
{
  bool ok = (val >= 0);
  if(!ok) {
    m_state_ok = false;
    m_warning += "setBaseWidth:val<0 : ";
  }
  else
    m_basewidth = val;

  return(ok);
}

//------------------------------------------------
bool ZAIC_LEQ::setMinMaxUtil(double minval, double maxval)
{
  if(minval >= maxval) {
    m_state_ok = false;
    m_warning += "setMinMaxUtil:min>=max : ";
    return(false);
  }

  m_minutil = minval;
  m_maxutil = maxval;
  
  return(true);
}

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

  bool ok = setPointLocations();
  if(!ok) 
    return(0);

  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *ipf = new IvPFunction(pdmap);

  return(ipf);
}

//-------------------------------------------------------------
// Procedure: getParam
//   Purpose: 

double ZAIC_LEQ::getParam(string param)
{
  param = tolower(param);
  if(param == "summit")
    return(m_summit);
  else if(param == "basewidth")
    return(m_basewidth);
  else if(param == "minutil")
    return(m_minutil);
  else if(param == "maxutil")
    return(m_maxutil);
  else
    return(0);
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

bool ZAIC_LEQ::setPointLocations()
{
  double dpt_low   = m_ivp_domain.getVarLow(0);
  double dpt_high  = m_ivp_domain.getVarHigh(0);
  
  if((m_summit < dpt_low) || (m_summit > dpt_high)) {
    m_state_ok = false;
    return(false);
  }
  
  int    domain_pts   = m_ivp_domain.getVarPoints(0);
  double domain_delta = m_ivp_domain.getVarDelta(0);

  double dpt_one = m_summit;
  double dpt_two = m_summit + m_basewidth;

  if(dpt_two > dpt_high)
    dpt_two = dpt_high;

  double d = domain_delta;

  m_ipt_low  = 0;
  m_ipt_one  = (int)((dpt_one/d)+0.5);
  m_ipt_two  = (int)((dpt_two/d)+0.5);
  m_ipt_high = domain_pts - 1;

  i_basewidth = (int)((m_basewidth + (d/2)) / d);
    
#if 0
  cout << "dpt_low:      " << dpt_low    << endl;
  cout << "dpt_one:      " << dpt_one    << endl;
  cout << "dpt_two:      " << dpt_two    << endl;
  cout << "dpt_high:     " << dpt_high   << endl;
  cout << endl;
  cout << "ipt_low:      " << m_ipt_low    << endl;
  cout << "ipt_one:      " << m_ipt_one    << endl;
  cout << "ipt_two:      " << m_ipt_two    << endl;
  cout << "ipt_high:     " << m_ipt_high   << endl;
#endif

  return(true);
}
     
//-------------------------------------------------------------
// Procedure: setPDMap()
//            

PDMap *ZAIC_LEQ::setPDMap()
{
  int i;
  int piece_count = 0;

  IvPBox *piece[3];
  for(i=0; i<3; i++)
    piece[i] = 0;

  // Handle piece0 if it exists
  if(m_ipt_low < m_ipt_one) {
    piece[0] = new IvPBox(1,1);
    piece[0]->setPTS(0, 0, m_ipt_one);
    piece[0]->wt(0) = 0.0;
    piece[0]->wt(1) = m_maxutil;
    piece_count++;
  }

  // Handle piece1 if it exists
  if(m_ipt_one < m_ipt_two) {
    piece[1] = new IvPBox(1,1);
    piece[1]->setPTS(0, m_ipt_one+1, m_ipt_two);

    double run    = (double)(i_basewidth);
    double slope  = -(m_maxutil - m_minutil) / run;
    double intcpt = -1.0 * slope * (m_ipt_one + i_basewidth);

    piece[1]->wt(0) = slope;
    piece[1]->wt(1) = intcpt + m_minutil;
    piece_count++;
  }

  // Handle piece2 if it exists
  if(m_ipt_two < m_ipt_high) {
    piece[2] = new IvPBox(1,1);
    piece[2]->setPTS(0, m_ipt_two+1, m_ipt_high);
    piece[2]->wt(0) = 0.0;
    piece[2]->wt(1) = m_minutil;
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








