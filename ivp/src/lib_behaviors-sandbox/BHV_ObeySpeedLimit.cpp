/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_ObeySpeedLimit.cpp                               */
/*    DATE: Sep 13 2005                                          */
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

#include <iostream>
#include <assert.h>
#include <math.h> 
#include <stdlib.h>
#include "BHV_ObeySpeedLimit.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ObeySpeedLimit::BHV_ObeySpeedLimit(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_obey_spdlimit");

  m_domain = subDomain(m_domain, "speed");

  spd_limit = 0;
  spd_cheat = 0;
  spd_death = 0;
  cheat_val   = 90;
  max_utility = 100;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ObeySpeedLimit::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "spd_limit") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    spd_limit = dval;
    if(spd_cheat < spd_limit) spd_cheat = spd_limit;
    if(spd_death < spd_limit) spd_death = spd_limit;
    return(true);
  }
  if(param == "spd_cheat") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    spd_cheat = dval;
    if(spd_limit > spd_cheat) spd_limit = spd_cheat;
    if(spd_death < spd_cheat) spd_death = spd_cheat;
    return(true);
  }
  if(param == "spd_death") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    spd_death = dval;
    if(spd_limit > spd_death) spd_limit = spd_death;
    if(spd_cheat < spd_death) spd_cheat = spd_death;
    return(true);
  }
  if(param == "cheat_val") {
    cheat_val = atof(val.c_str());
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState
//
//          ^               limit    
//      100-|-----------------o
//          | (piece-0)         \       
//          |                      \       
//          |             (piece-1)   \  cheat  
// Utility  |                            o  -  -  -  cheat_val
//          |                             \
//          |                              \
//          |                   (piece-2)   \
//          |___ ____________________________\death___________
//                    Speed

IvPFunction *BHV_ObeySpeedLimit::onRunState() 
{
  int    spdIndex  = m_domain.getIndex("speed");
  double spdBase   = m_domain.getVarLow(spdIndex);
  double spdDelta  = m_domain.getVarDelta(spdIndex);
  int    spdPoints = m_domain.getVarPoints(spdIndex);

  // build piece 1
  double dbl_limit_ix = (spd_limit - spdBase) / spdDelta;
  int    int_limit_ix = (int)(floor(dbl_limit_ix + 0.5));
  double dbl_cheat_ix = (spd_cheat - spdBase) / spdDelta;
  int    int_cheat_ix = (int)(floor(dbl_cheat_ix + 0.5));
  double dbl_death_ix = (spd_death - spdBase) / spdDelta;
  int    int_death_ix = (int)(floor(dbl_death_ix + 0.5));

  IvPBox *piece0 = 0;
  if(int_limit_ix > 0) {
    piece0 = new IvPBox(1,1);
    piece0->setPTS(0, 0, int_limit_ix);
    double slope = 0;
    double intercept = max_utility;
    piece0->wt(0) = slope;
    piece0->wt(1) = intercept;
#if 0
    cout << "slope0: " << slope << endl;
    cout << "b0: " << intercept << endl;
    cout << "x: " << int_limit_ix << endl;
    cout << "y: " << (slope * (double)int_limit_ix) + intercept << endl;
#endif
  }
  
  IvPBox *piece1 = 0;
  if(int_cheat_ix > int_limit_ix) {
    piece1 = new IvPBox(1,1);
    piece1->setPTS(0, int_limit_ix+1, int_cheat_ix);
    double slope = (cheat_val - max_utility) / 
      ((double)(int_cheat_ix) - (double)(int_limit_ix));
    double intercept = (cheat_val) - 
      (slope * (double)(int_cheat_ix));
    piece1->wt(0) = slope;
    piece1->wt(1) = intercept;
#if 0
    cout << "slope1: " << slope << endl;
    cout << "b1: " << intercept << endl;
    cout << "x: " << int_cheat_ix << endl;
    cout << "y: " << (slope * (double)int_cheat_ix) + intercept << endl;
#endif
  }

  IvPBox *piece2 = 0;
  if(int_death_ix > int_cheat_ix) {
    piece2 = new IvPBox(1,1);
    piece2->setPTS(0, int_cheat_ix+1, int_death_ix);
    double slope = (0 - cheat_val) / 
      ((double)(int_death_ix) - (double)(int_cheat_ix));
    double intercept = 0 - (slope * (double)(int_death_ix));
    piece2->wt(0) = slope;
    piece2->wt(1) = intercept;
  }

  int piece_count = 0;
  if(piece0) piece_count++;
  if(piece1) piece_count++;
  if(piece2) piece_count++;

  PDMap *pdmap = new PDMap(piece_count, m_domain, 1);

  piece_count = 0;
  if(piece0) {
    pdmap->bx(piece_count) = piece0;
    piece_count++;
  }
  if(piece1) {
    pdmap->bx(piece_count) = piece1;
    piece_count++;
  }
  if(piece2) {
    pdmap->bx(piece_count) = piece2;
    piece_count++;
  }

  pdmap->updateGrid();

  IvPFunction *of = new IvPFunction(pdmap);
  of->setPWT(m_priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: errorCheck
//

bool BHV_ObeySpeedLimit::errorCheck()
{
  bool ok = true;
  if(spd_limit <= 0) 
    ok = false;
  if(spd_cheat < spd_limit)
    ok = false;
  if(spd_death < spd_cheat)
    ok = false;
  if(cheat_val > max_utility)
    ok = false;
  if(cheat_val < 0)
    ok = false;

  if(!ok)
    postEMessage("Invalid behavior parameters");
  
  return(ok);
}








