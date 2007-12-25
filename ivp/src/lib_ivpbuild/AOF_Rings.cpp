/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Rings.cpp                                        */
/*    DATE: Very old - modernized Jan 21 2006                    */
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
#include <stdio.h> 
#include "MBUtils.h"
#include "AOF_Rings.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//     Notes: The universe will be NULL unless provide to 
//            the contructor upon creation.

AOF_Rings::AOF_Rings(IvPDomain g_domain) : AOF(g_domain)
{
  m_snapval = 0;
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_Rings::evalBox(const IvPBox *gbox) const
{
  double totval=0;
  int rsize = m_rings.size();
  for(int i=0; i<rsize; i++)
    totval += m_rings[i].evalBox(gbox);
  
  double weight = (totval / rsize);    
  
  if(m_snapval > 0)
    return(snapToStep(weight, (double)m_snapval));
  else
    return(weight);
}

//----------------------------------------------------------------
// Procedure: setParam(string, double)

bool AOF_Rings::setParam(const string& param, double val)
{
  if(param == "snapval")
    m_snapval = val;
  else {
    int rsize = m_rings.size();
    if(rsize != 0) {
      bool result = m_rings[rsize-1].setParam(param, val);
      return(result);
    }
    else
      return(false);
  }

  return(true);
}

//----------------------------------------------------------------
// Procedure: setParam(string, string)

bool AOF_Rings::setParam(const string& param, const string& val)
{
  if(param == "location") {
    AOF_Ring new_ring(m_domain);
    bool result = new_ring.setParam(param, val);
    if(!result)
      return(false);
    m_rings.push_back(new_ring);
  }
  else {
    int rsize = m_rings.size();
    if(rsize != 0) {
      bool result = m_rings[rsize-1].setParam(param, val);
      return(result);
    }
    else
      return(false);
  }
  return(true);
}


//----------------------------------------------------------------
// Procedure: print

void AOF_Rings::print() const
{
  int rsize = m_rings.size();
  for(int j=0; j<rsize; j++) {
    cout << "Ring#" << j << ":  ";
    m_rings[j].print();
  }
}

//----------------------------------------------------------------
// Procedure: latexSTR

string AOF_Rings::latexSTR(int full) const
{
  string retstr;

  if(full) retstr += "\\fbox{ \\LARGE \\begin{tabular}{ll} $f(x, y) = $ ";

  int rsize = m_rings.size();
  for(int i=0; i<rsize; i++) 
    retstr += m_rings[i].latexSTR(0);
  
  if(full) retstr += " \\end{tabular}} \\normalsize";
  return(retstr);
}









