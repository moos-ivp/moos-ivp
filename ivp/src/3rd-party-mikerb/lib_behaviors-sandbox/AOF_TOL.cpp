/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_TOL.cpp                                          */
/*    DATE: March 27, 2005                                       */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include "AOF_TOL.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_TOL::AOF_TOL(IvPDomain g_domain) : AOF(g_domain)
{
  tol_ix = g_domain.getIndex("tol");
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_TOL::initialize()
{
  if(tol_ix==-1)
    return(false);

  tol_max = m_domain.getVarPoints(tol_ix);

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).

double AOF_TOL::evalBox(const IvPBox *b) const
{
  double eval_tol;

  m_domain.getVal(tol_ix, b->pt(tol_ix,0), eval_tol);

  double val = (eval_tol / tol_max) * 100.0;

  return(val);
}








