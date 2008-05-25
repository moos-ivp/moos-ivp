/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_PEAK.cpp                                        */
/*    DATE: Aug 17th 2006                                        */
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

#include <math.h>
#include "ZAIC_PEAK.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_PEAK::ZAIC_PEAK(IvPDomain g_domain, const string& g_varname) 
{
  m_state_ok = true;

  m_ivp_domain = subDomain(g_domain, g_varname);

  // If the domain variable is not found in the given domain, flip
  // the m_state_ok bit to false. 
  if(m_ivp_domain.size() == 0)
    m_state_ok = false;

  m_summit_insist = true;
  m_value_wrap    = false;

  m_domain_ix    = m_ivp_domain.getIndex(g_varname);
  m_domain_high  = m_ivp_domain.getVarHigh(m_domain_ix);
  m_domain_low   = m_ivp_domain.getVarLow(m_domain_ix);
  m_domain_pts   = m_ivp_domain.getVarPoints(m_domain_ix);
  m_domain_delta = m_ivp_domain.getVarDelta(m_domain_ix);
  
  unsigned int i;
  for(i=0; i<m_domain_pts; i++)
    m_ptvals.push_back(0.0);  

  v_summit.push_back(0);
  v_basewidth.push_back(0);
  v_peakwidth.push_back(0);
  v_summitdelta.push_back(0);
  v_minutil.push_back(0);
  v_maxutil.push_back(100);

}

//-------------------------------------------------------------
// Procedure: addComonent
//    Return: index of the new summit just added

int ZAIC_PEAK::addComponent()
{
  v_summit.push_back(0);
  v_basewidth.push_back(0);
  v_peakwidth.push_back(0);
  v_summitdelta.push_back(0);
  v_minutil.push_back(0);
  v_maxutil.push_back(100);

  return(v_summit.size()-1);
}

//-------------------------------------------------------------
// Procedure: setParams
//    Return: true if all ok

bool ZAIC_PEAK::setParams(double summit, double pwidth, 
			  double bwidth, double delta,  
			  double minutil, double maxutil, 
			  unsigned int index)
{
  setSummit(summit, index);
  setBaseWidth(bwidth, index);
  setPeakWidth(pwidth, index);

  // Important to set the min/max util before the summit-delta
  // since this sets the upper bound on allowable delta value.
  setMinMaxUtil(minutil, maxutil, index);
  setSummitDelta(delta, index);
  return(m_state_ok);
}

//-------------------------------------------------------------
// Procedure: setSummit
//            setBaseWidth
//            setPeakWidth
//            setSummitDelta
//            setMinMaxUtil

bool ZAIC_PEAK::setSummit(double val, unsigned int index)
{
  if((index < 0) || (index >= v_summit.size())) {
    m_state_ok = false;
    m_warning += "setSummit:index out of range:";
    return(false);
  }
  v_summit[index] = val;
  return(true);
}

//------------------------------------------------
bool ZAIC_PEAK::setBaseWidth(double val, unsigned int index)
{
  if((index < 0) || (index >= v_basewidth.size())) {
    m_state_ok = false;
    m_warning += "setBaseWidth:index out of range:";
    return(false);
  }
  if(val < 0) {
    m_state_ok = false;
    m_warning += "setBaseWidth:value less than zero";
    return(false);
  }

  v_basewidth[index] = val;
  return(true);
}

//------------------------------------------------
bool ZAIC_PEAK::setPeakWidth(double val, unsigned int index)
{
  if((index < 0) || (index >= v_peakwidth.size())) {
    m_state_ok = false;
    m_warning += "setPeakWidth:index out of range:";
    return(false);
  }
  if(val < 0) {
    m_state_ok = false;
    m_warning += "setPeakWidth:value less than zero";
    return(false);
  }

  v_peakwidth[index] = val;
  return(true);
}

//------------------------------------------------
bool ZAIC_PEAK::setSummitDelta(double val, unsigned int index)
{
  if((index < 0) || (index >= v_summitdelta.size())) {
    m_warning += "setSummitDelta:index out of range:";
    m_state_ok = false;
    return(false);
  }
  if(val < 0) {
    m_state_ok = false;
    m_warning += "setSummitDelta:value less than zero";
    return(false);
  }
  double util_range = (v_maxutil[index] - v_minutil[index]);
  if(val > util_range) {
    m_state_ok = false;
    m_warning += "setSummitDelta:delta greater than util range";
    return(false);
  }

  v_summitdelta[index] = val;
  return(true);
}

//------------------------------------------------
bool ZAIC_PEAK::setMinMaxUtil(double minval, double maxval, 
			      unsigned int index)
{
  if((index < 0) || (index >= v_minutil.size())) {
    m_state_ok = false;
    m_warning += "setMinMaxUtil:index out of range:";
    return(false);
  }
  if(minval >= maxval) {
    m_state_ok = false;
    m_warning += "setMinMaxUtil:min>=max:";
    return(false);
  }

  v_minutil[index] = minval;
  v_maxutil[index] = maxval;
  
  double util_range = (maxval - minval);
  if(v_summitdelta[index] > util_range)
    v_summitdelta[index] = util_range;
  
  return(true);
}



//-------------------------------------------------------------
// Procedure: getParam

double ZAIC_PEAK::getParam(string param, unsigned int index)
{
  if((index < 0) || (index >= v_summit.size()))
    return(0);

  if(param == "summit")
    return(v_summit[index]);
  else if(param == "basewidth")
    return(v_basewidth[index]);
  else if(param == "peakwidth")
    return(v_peakwidth[index]);
  else if(param == "summitdelta")
    return(v_summitdelta[index]);
  else if(param == "minutil")
    return(v_minutil[index]);
  else if(param == "maxutil")
    return(v_maxutil[index]);
  else
    return(0);
}


//-------------------------------------------------------------
// Procedure: evalPoint(int)
//

double ZAIC_PEAK::evalPoint(unsigned int ix, bool maxval)
{
  double maximum_value = 0;
  double total_value   = 0;
  int vsize = v_summit.size();
  for(int sx=0; sx<vsize; sx++) {
    double dval = evalPoint(sx, ix);
    if((sx==0) || (dval > maximum_value))
      maximum_value = dval;
    total_value += dval;
  }
  
  if(maxval)
    return(maximum_value);
  else
    return(total_value);
  
}

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *ZAIC_PEAK::extractOF(bool maxval)
{
  if((m_domain_ix == -1) || (m_state_ok == false))
    return(0);

  unsigned int i;
  for(i=0; i<m_domain_pts; i++)
    m_ptvals[i] = evalPoint(i, maxval);

  if(m_summit_insist) {
    int vsize = v_summit.size();
    for(int sx=0; sx<vsize; sx++)
      insistSummit(sx);
  }

  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *ipf = new IvPFunction(pdmap);

  return(ipf);
}



//-------------------------------------------------------------
// Procedure: evalPoint(int, int)
//

double ZAIC_PEAK::evalPoint(unsigned int sx, unsigned int ix)
{
  double summit       = v_summit[sx];
  double peak_width   = v_peakwidth[sx];
  double base_width   = v_basewidth[sx];
  double summit_delta = v_summitdelta[sx];
  double min_util     = v_minutil[sx];
  double max_util     = v_maxutil[sx];

  if((ix < 0) || (ix >= m_domain_pts))
    return(min_util);

  double loc_summit_delta = summit_delta;
  if(peak_width <= 0)
    loc_summit_delta = 0;

  double dval = (((double)(ix)) * m_domain_delta) + m_domain_low;
  double dist_from_summit;

  // The value_wrap setting determines the distance between the
  // eval point and the summit. Otherwise all logic the same 
  // whether wrapping or not.
  if(m_value_wrap == false) {
    if(summit >= dval)
      dist_from_summit = summit - dval;
    else
      dist_from_summit = dval - summit;
  }
  else {
    double summit_to_top_dist = m_domain_high - summit;
    double summit_to_bot_dist = summit - m_domain_low;

    double dist_left, dist_right;
    if(summit > dval) {
      dist_right = summit - dval;
      dist_left  = (dval - m_domain_low) + summit_to_top_dist + m_domain_delta;
    }
    else {
      dist_left  = dval - summit;
      dist_right = (m_domain_high - dval) + summit_to_bot_dist + m_domain_delta;
    }
    if(dist_left < dist_right)
      dist_from_summit = dist_left;
    else
      dist_from_summit = dist_right;
  }

  double return_val = 0;

  // Case 0: Eval pt is within peakwidth
  if(dist_from_summit <= peak_width) {
    if(loc_summit_delta <= 0)
      return_val = max_util;
    else {
      double slope = (loc_summit_delta / peak_width);
      double rise  = slope * dist_from_summit;
      return_val  = max_util - rise;
    }
  }
  
  // Case 1: Eval pt is within basewidth, outside peakwidth
  else if((dist_from_summit > peak_width) &&
     (dist_from_summit <= (peak_width + base_width))) {
    double edge_of_peak_height = max_util - loc_summit_delta;
    double slope = (edge_of_peak_height - min_util) / base_width;
    double rise  = slope * (dist_from_summit - peak_width);
    return_val   = edge_of_peak_height - rise;
  }
  
  // Case 2: Eval pt is outside peak and base
  else
    return_val = min_util;
  
  //if(return_val < min_util)
  //  return_val = min_util;
  if(return_val > max_util)
    return_val = max_util;
  return(return_val);

}

//-------------------------------------------------------------
// Procedure: insistSummit
//

void ZAIC_PEAK::insistSummit(unsigned int sx)
{
  double summit       = v_summit[sx];
  double min_util     = v_minutil[sx];
  double max_util     = v_maxutil[sx];

  // First check to see if there is a need to insist a summit.
  // This is the case only if all ptvals have the same value
  // If so, all vals=min_util=max_util
  unsigned int i;
  for(i=0; i<m_domain_pts; i++)
    if(m_ptvals[i] != min_util)
      return;
  
  // If all ptvals are same, one of three things are the case:
  // (1) The summit is less than the lowest domain point
  // and the base+peak width are insufficient to reach lowest
  // domain point. We set the lowest domain point to max_util.
  // (2) The summit is greater than the highest domain point
  // and the base+peak width are insufficient to reach highest
  // domain point. We set the highest domain point to max_util.
  // (3) The summit is between domain points and the base+peak
  // widths are insufficient to reach a domain point in either
  // direction. We find the domain point nearest the summit and
  // set that to max_util.

  // Handle Case (1)
  if(summit < m_domain_low) {
    m_ptvals[0] = max_util;
    return;
  }
  // Handle Case (2)
  if(summit > m_domain_high) {
    m_ptvals[m_domain_pts-1] = max_util;
    return;
  }
  // Handle Case (3)
  double min_delta;
  int    min_delta_ix;
  for(i=0; i<m_domain_pts; i++) {
    double dval = (((double)(i)) * m_domain_delta) + m_domain_low;
    double delta = dval - summit;
    if(delta < 0)
      delta *= -1;
    if((i==0) || (delta<min_delta)) {
      min_delta = delta;
      min_delta_ix = i;
    }
  }
  m_ptvals[min_delta_ix] = max_util;
}

//-------------------------------------------------------------
// Procedure: setPDMap()
//

PDMap *ZAIC_PEAK::setPDMap(double tolerance)
{
  unsigned int i;
  
  int    first_pt  = 0;
  double first_val = m_ptvals[0];

  bool   trend = false; // No trend to start
  double s_m;
  double s_b;

  vector<IvPBox*> pieces;
  for(i=1; i<m_domain_pts; i++) {

    if(!trend) {
      trend  = true;
      s_m = (m_ptvals[i] - first_val) / (i - first_pt);
      s_b = (m_ptvals[i] - (s_m * i));
    }

    // Project from the running linear line what the value 
    // should be at this point [i]. Then compare to the actual
    // point and see if it lies within the tolerance.
    double ext_val = (s_m * i) + s_b;
    bool lbreak = false;
    if((fabs(ext_val - m_ptvals[i])) > tolerance)
      lbreak = true;

    // In addition to applying the tolerance criteria, also
    // declare a break if there is a change in sign of slope
    // to nail exactly a peak in the function.
    double loc_m = (m_ptvals[i] - m_ptvals[i-1]) / (i - (i-1));
    if((loc_m < 0) && (s_m > 0))
      lbreak = true;
    if((loc_m > 0) && (s_m < 0))
      lbreak = true;
    
    bool last_point = (i == m_domain_pts-1);
    
    if(last_point) {
      IvPBox *piece = new IvPBox(1,1);
      if(lbreak) {
	piece->setPTS(0, i, i);
	piece->wt(0) = 0;
	piece->wt(1) = m_ptvals[i];
	pieces.push_back(piece);
      }
      else {
	piece->setPTS(0, first_pt, i);
	double rise   = m_ptvals[i] - first_val;
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
      double rise   = m_ptvals[i-1] - first_val;
      double run    = (i-1) - first_pt;
      double slope  = rise / run;
      double intcpt = first_val - (slope * first_pt);
      piece->wt(0) = slope;
      piece->wt(1) = intcpt;
      pieces.push_back(piece);
      trend = false;
      first_pt  = i;
      first_val = m_ptvals[i]; 
    }
    
  }
  
  unsigned int piece_count = pieces.size();

  PDMap *pdmap = new PDMap(piece_count, m_ivp_domain, 1);

  for(i=0; i<piece_count; i++)
    pdmap->bx(i) = pieces[i];

  return(pdmap);
}


