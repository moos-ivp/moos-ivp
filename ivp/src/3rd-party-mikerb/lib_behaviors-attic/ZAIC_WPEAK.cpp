/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_WPEAK.cpp                                       */
/*    DATE: June 19th 2006                                       */
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
#include "ZAIC_WPEAK.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_WPEAK::ZAIC_WPEAK(IvPDomain g_domain, const string& g_varname) 
{
  varname      = g_varname;
  ivp_domain   = subDomain(g_domain, varname);

  summit       = 0;
  base_width   = 0;
  peak_width   = 0;
  summit_delta = 0;

  domain_ix    = ivp_domain.getIndex(varname);
  domain_high  = ivp_domain.getVarHigh(domain_ix);
  domain_low   = ivp_domain.getVarLow(domain_ix);
  domain_pts   = ivp_domain.getVarPoints(domain_ix);
  domain_delta = ivp_domain.getVarDelta(domain_ix);

  for(int i=0; i<domain_pts; i++)
    ptvals.push_back(0.0);  
}

//-------------------------------------------------------------
// Procedure: setSummit

bool ZAIC_WPEAK::setSummit(double g_val)
{
  bool ok = ((g_val >= domain_low) && (g_val <= domain_high));
  if(ok)
    summit = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setBaseWidth

bool ZAIC_WPEAK::setBaseWidth(double g_val)
{
  bool ok = (g_val >= 0);
  if(ok)
    base_width = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setPeakWidth

bool ZAIC_WPEAK::setPeakWidth(double g_val)
{
  bool ok = (g_val >= 0);
  if(ok)
    peak_width = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setSummitDelta

bool ZAIC_WPEAK::setSummitDelta(double g_val)
{
  bool ok = ((g_val >= 0) && (g_val <= 100));
  if(ok)
    summit_delta = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setParam

bool ZAIC_WPEAK::setParam(const string& param, const string& g_val)
{
  string val = tolower(stripBlankEnds(g_val));
  if(param == "summit")
    return(setSummit(atof(g_val.c_str())));
  
  else if(param == "peak_width")
    return(setPeakWidth(atof(g_val.c_str())));

  else if(param == "base_width")
    return(setBaseWidth(atof(g_val.c_str())));

  else if(param == "summit_delta")
    return(setSummitDelta(atof(g_val.c_str())));

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

IvPFunction *ZAIC_WPEAK::extractOF()
{
  if(domain_ix == -1)
    return(0);

  int i;
  for(i=0; i<domain_pts; i++)
    ptvals[i] = evalPoint(i);

  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *of = new IvPFunction(pdmap);

  return(of);
}


//-------------------------------------------------------------
// Procedure: evalPoint(int)
//

double ZAIC_WPEAK::evalPoint(int ix)
{
  if((ix < 0) || (ix >= domain_pts))
    return(0);

  double loc_summit_delta = summit_delta;
  if(peak_width <= 0)
    loc_summit_delta = 0;

  double dval = (((double)(ix)) * domain_delta) + domain_low;
  
  double summit_to_top_dist = domain_high - summit;
  double summit_to_bot_dist = summit - domain_low;

  double dist_left, dist_right;
  if(summit > dval) {
    dist_right = summit - dval;
    dist_left  = (dval - domain_low) + summit_to_top_dist;
  }
  else {
    dist_left  = dval - summit;
    dist_right = (domain_high - dval) + summit_to_bot_dist;
  }

  double dist_from_summit;
  if(dist_left < dist_right)
    dist_from_summit = dist_left;
  else
    dist_from_summit = dist_right;

  // Case 0: Eval pt is outside peak and base
  if(dist_from_summit >= (peak_width + base_width))
    return(0);
  
  // Case 1: Eval pt is within peakwidth
  if(dist_from_summit <= peak_width) {
    if(loc_summit_delta <= 0)
      return(100.0);
    else {
      double slope = (loc_summit_delta / peak_width);
      double rise  = slope * dist_from_summit;
      return(100.0 - rise);
    }
  }
  
  // Case 2: Eval pt is within basewidth, outside peakwidth
  double edge_of_peak_height = 100.0 - loc_summit_delta;
  if((dist_from_summit > peak_width) &&
     (dist_from_summit <= (peak_width + base_width))) {
    double slope = edge_of_peak_height / base_width;
    double rise  = slope * (dist_from_summit - peak_width);
    return(edge_of_peak_height - rise);
  }
  
  // Should never get here - always Case1,2,or3
  //assert(0);

}

//-------------------------------------------------------------
// Procedure: setPDMap()
//

PDMap *ZAIC_WPEAK::setPDMap(double tolerance)
{
  int    i;
  int    first_pt  = 0;
  double first_val = ptvals[0];


  bool   trend = false; // No trend to start
  double s_m;
  double s_b;

  vector<IvPBox*> pieces;
  for(i=1; i<domain_pts; i++) {

    if(!trend) {
      trend  = true;
      s_m = (ptvals[i] - first_val) / (i - first_pt);
      s_b = (ptvals[i] - (s_m * i));
    }

    // Project from the running linear line what the value 
    // should be at this point [i]. Then compare to the actual
    // point and see if it lies within the tolerance.
    double ext_val = (s_m * i) + s_b;
    bool lbreak = false;
    if((fabs(ext_val - ptvals[i])) > tolerance)
      lbreak = true;

    // In addition to applying the tolerance criteria, also
    // declare a break if there is a change in sign of slope
    // to nail exactly a peak in the function.
    double loc_m = (ptvals[i] - ptvals[i-1]) / (i - (i-1));
    if((loc_m < 0) && (s_m > 0))
      lbreak = true;
    if((loc_m > 0) && (s_m < 0))
      lbreak = true;
    
    bool last_point = (i == domain_pts-1);
    
    if(last_point) {
      IvPBox *piece = new IvPBox(1,1);
      if(lbreak) {
	piece->setPTS(0, i, i);
	piece->wt(0) = 0;
	piece->wt(1) = ptvals[i];
	pieces.push_back(piece);
      }
      else {
	piece->setPTS(0, first_pt, i);
	double rise   = ptvals[i] - first_val;
	double run    = i - first_pt;
	double slope  = rise / run;
	double intcpt = first_val - (slope * first_pt);
	piece->wt(0) = slope;
	piece->wt(1) = intcpt;
	pieces.push_back(piece);
      }
    }
	
    if(lbreak) {
      IvPBox *piece = new IvPBox(1,1);
      piece->setPTS(0, first_pt, i-1);
      double rise   = ptvals[i-1] - first_val;
      double run    = (i-1) - first_pt;
      double slope  = rise / run;
      double intcpt = first_val - (slope * first_pt);
      piece->wt(0) = slope;
      piece->wt(1) = intcpt;
      pieces.push_back(piece);
      trend = false;
      first_pt  = i;
      first_val = ptvals[i]; 
    }
    
  }
  
  int piece_count = pieces.size();

  PDMap *pdmap = new PDMap(piece_count, ivp_domain, 1);

  for(i=0; i<piece_count; i++)
    pdmap->bx(i) = pieces[i];

  return(pdmap);
}







