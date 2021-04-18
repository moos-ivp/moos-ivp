/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_Vector.cpp                                      */
/*    DATE: Apr 30th, 2010                                       */
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

#include <list>
#include <iostream>
#include <cmath>
#include "ZAIC_Vector.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_Vector::ZAIC_Vector(IvPDomain g_domain, const string& varname) 
{
  m_domain_ok = true;

  m_ivp_domain = subDomain(g_domain, varname);
  if(m_ivp_domain.size() != 1)
    m_domain_ok = false;

  if(m_ivp_domain.getVarDelta(0) <= 0)
    m_domain_ok = false;

  m_domain_ix    = m_ivp_domain.getIndex(varname);
  m_domain_high  = m_ivp_domain.getVarHigh(m_domain_ix);
  m_domain_low   = m_ivp_domain.getVarLow(m_domain_ix);
  m_domain_pts   = m_ivp_domain.getVarPoints(m_domain_ix);
  m_domain_delta = m_ivp_domain.getVarDelta(m_domain_ix);

  m_minutil = 0.0;
  m_maxutil = 0.0;
  m_tolerance = 0.1;
  
  m_sort_needed = false;
}

//-------------------------------------------------------------
// Procedure: getParam
//   Purpose: Standard hook for grabbing values of member vars.

double ZAIC_Vector::getParam(string param)
{
  param = tolower(param);
  if(param == "minutil")
    return(m_minutil);
  else if(param == "maxutil")
    return(m_maxutil);
  else if(param == "tolerance")
    return(m_tolerance);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: setDomainVals()
//   Purpose: Set the vector of domain values.
//    Checks: (1) All domain vals fall within the IvP Domain
//            (2) All domain vals are in order

void ZAIC_Vector::setDomainVals(vector<double> vals)
{
  unsigned int i, vsize = vals.size();

  if(vsize == 0)
    return;

  double pval = vals[0];
  if((pval < m_domain_low) || (pval > m_domain_high))
    return;

  bool some_entries_out_of_bounds = false;
  for(i=1; i<vsize; i++) {
    double curr_val = vals[i];
    // Check that all domain values are within the IvPDomain
    if(curr_val < m_domain_low) {
      some_entries_out_of_bounds = true;
      curr_val = m_domain_low;
    }
    if(curr_val > m_domain_high) {
      some_entries_out_of_bounds = true;
      curr_val = m_domain_high;
    }
    // Check that all domain values are increasing in value.
    if(curr_val < pval)
      m_sort_needed = true;
    pval = curr_val;
  }
  
  if(some_entries_out_of_bounds)
    addConfigWarning("Some domain entries outside IvPDomain");
  
  // All is fine so make the assignment
  m_domain_vals = vals;
}

//-------------------------------------------------------------
// Procedure: setRangeVals
//   Purpose: Set the values in the mapping from the IvP
//            domain to the utility values

void ZAIC_Vector::setRangeVals(vector<double> vals)
{
  m_range_vals = vals;

  m_minutil = 0;
  m_maxutil = 0;

  if(m_range_vals.size() == 0)
    return;

  m_minutil = m_range_vals[0];
  m_maxutil = m_range_vals[0];
  unsigned int i, isize = m_range_vals.size();
  for(i=1; i<isize; i++) {
    if(m_range_vals[i] < m_minutil)
      m_minutil = m_range_vals[i];
    if(m_range_vals[i] > m_maxutil)
      m_maxutil = m_range_vals[i];
  }
}

//-------------------------------------------------------------
// Procedure: setTolerance
//   Purpose: Set the error tolerance for building a piecewise mapping

void ZAIC_Vector::setTolerance(double val)
{
  if(val < 0)
    val = 0;
  m_tolerance = val;
}


//-------------------------------------------------------------
// Procedure: stateOK()
//   Purpose: Determine if the ZAIC is configured properly.

bool ZAIC_Vector::stateOK()
{
  if(!m_domain_ok)
    return(false);

  if(m_minutil > m_maxutil)
    return(false);

  if(m_domain_vals.size() != m_range_vals.size())
    return(false);

  return(true);
}

//-------------------------------------------------------------
// Procedure: convertValues()
//   Purpose: (1) Use the mapping from domain to range values to
//            create a mapping from domain discrete points to vals.
//            (2) If no mapping exists from the low and high 
//            discrete domain values, create them based on their 
//            nearest neighbor.

void ZAIC_Vector::convertValues()
{
  if(!stateOK()) {
    addError("Unable to convert values from raw to indexed");
    return;
  }

  // Phase I: Convert raw mapping to index mapping
  unsigned int prev_ix = 0;
  list<unsigned int> tmp_idomain_vals;
  list<double>       tmp_irange_vals;
  // Remember if multiple raw domain vals map to single index domain val.
  bool overlap_vals = false;
  unsigned int i, vsize = m_domain_vals.size();
  for(i=0; i<vsize; i++) {
    double val = m_domain_vals[i];
    unsigned int ix = m_ivp_domain.getDiscreteVal(m_domain_ix, val, 2);
    if((i!=0) && (prev_ix == ix))
      overlap_vals = true;
    tmp_idomain_vals.push_back(ix);
    tmp_irange_vals.push_back(m_range_vals[i]);
    prev_ix = ix;
  }
  if(overlap_vals)
    addBuildWarning("Overlapping domain components");

  
  // Phase II: Add endpoints if necessary.
  if(tmp_idomain_vals.size() > 0) {
    unsigned int front_dom = tmp_idomain_vals.front();
    double       front_rng = tmp_irange_vals.front();
    if(front_dom != 0) {
      addBuildWarning("Domain-Range mapping doesnt extend to lower bound");
      tmp_idomain_vals.push_front(0);
      tmp_irange_vals.push_front(front_rng);
    }
    unsigned int back_dom = tmp_idomain_vals.back();
    double       back_rng = tmp_irange_vals.back();
    if(back_dom != m_domain_pts-1) {
      addBuildWarning("Domain-Range mapping doesnt extend to upper bound");
      tmp_idomain_vals.push_back(m_domain_pts-1);
      tmp_irange_vals.push_back(back_rng);
    }
  }

  // Phase III: Collapse mappings that map to the same idomain val
  m_idomain_vals.clear();
  m_irange_vals.clear();
  while(tmp_idomain_vals.size() != 0) {
    unsigned int count = 1;
    unsigned int idomain = tmp_idomain_vals.front();
    double       total   = tmp_irange_vals.front();
    tmp_idomain_vals.pop_front();
    tmp_irange_vals.pop_front();
    bool done = false;
    if(tmp_idomain_vals.size() == 0)
      done = true;
    while(!done) {
      unsigned int idom = tmp_idomain_vals.front();
      //double irng = tmp_irange_vals.front();
      if(idom != idomain)
	done = true;
      else {
	total += tmp_irange_vals.front();
	count++;
	tmp_idomain_vals.pop_front();
	tmp_irange_vals.pop_front();
      }
      if(tmp_idomain_vals.size() == 0)
	done = true;
    }

    double avg = total / ((double)(count));

    m_idomain_vals.push_back(idomain);
    m_irange_vals.push_back(avg);
    
    if(tmp_idomain_vals.size() == 0)
      done = true;
  }
}

//-------------------------------------------------------------
// Procedure: addConfigWarning
// Procedure: addBuildWarning
// Procedure: addError

void ZAIC_Vector::addConfigWarning(string warning_str)
{
  m_config_warnings.push_back(warning_str);
}

void ZAIC_Vector::addBuildWarning(string warning_str)
{
  m_build_warnings.push_back(warning_str);
}

void ZAIC_Vector::addError(string error_str)
{
  m_errors.push_back(error_str);
}

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *ZAIC_Vector::extractOF()
{
  m_build_warnings.clear();

  if(!stateOK())
    return(0);
  
  if(m_sort_needed)
    sortMapping();
  
  convertValues();

  PDMap *pdmap = setPDMap2();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *ipf = new IvPFunction(pdmap);
  return(ipf);
}


//-------------------------------------------------------------
// Procedure: setPDMap

PDMap *ZAIC_Vector::setPDMap()
{
  if(!stateOK()) {
    addError("Unable to set the PDMap - ZAIC not in ok state");
    return(0);
  }

  // vector of values should be at least size two.
  if(m_idomain_vals.size() < 2) {
    addError("Unable to set the PDMap - less than 2 pairs given");
    return(0);
  }

  //unsigned int prev_idom = 0;

  bool all_pieces_built_ok = true; 
  unsigned int i, vsize = m_idomain_vals.size();
  for(i=0; (i<vsize-1) && all_pieces_built_ok; i++) {
    unsigned int ix_low = m_idomain_vals[i];
    unsigned int ix_hgh = m_idomain_vals[i+1];
    double val_low = m_irange_vals[i];
    double val_hgh = m_irange_vals[i+1];

    IvPBox *piece = buildBox(ix_low, val_low, ix_hgh, val_hgh);
    if(piece) {
      if(i!=0) {
	int low_ix = piece->pt(0,0);
	piece->pt(0,0) = low_ix + 1;
      }
      m_bld_pieces.push_back(piece);
    }
    else {
      all_pieces_built_ok = false;
      addError("Failure in building a piece in the IvPFunction");
    }
  }

  // Check if all pieces were built without a problem. If a problem
  // occurred, clean up the mess and return a null PDMap.
  if(!all_pieces_built_ok) {
    clearBldPieces();
    return(0);
  }

  unsigned int piece_count = m_bld_pieces.size();
  if(piece_count == 0) {
    addError("No pieces with which to build a PDMap/IvPFunction");
    return(0);
  }

  PDMap *pdmap = new PDMap(piece_count, m_ivp_domain, 1);
  if(!pdmap) {
    addError("Null PDMap generated");
    clearBldPieces();
    return(0);
  }

  for(i=0; i<piece_count; i++)
    pdmap->bx(i) = m_bld_pieces[i];

  // The returning PDMap now "owns" the pieces so clear the vector
  // of pointers.
  m_bld_pieces.clear();

  // We always want to exit this function with an empty m_bld_pieces.
  return(pdmap);
}


//-------------------------------------------------------------
// Procedure: clearBldPieces
//   Purpose: Make sure we don't have any inadvertant memory leaks 
//            due to aborted attempts to create an IvP function.

void ZAIC_Vector::clearBldPieces()
{
  unsigned int i, isize = m_bld_pieces.size();
  for(i=0; i<isize; i++) {
    if(m_bld_pieces[i])
      delete(m_bld_pieces[i]);
  }
  m_bld_pieces.clear();
}

//-------------------------------------------------------------
// Procedure: buildBox

IvPBox *ZAIC_Vector::buildBox(unsigned int ix_low, double val_low,
			      unsigned int ix_hgh, double val_hgh)
{
  if(ix_low > ix_hgh) {
    addError("Building box with low>high end of interval");
    return(0);
  }

  IvPBox *piece = new IvPBox(1,1);
  if(!piece)
    return(0);

  double rise = (val_hgh - val_low);
  double run  = (double)(ix_hgh-ix_low);

  double slope = 0;
  if(run > 0)
    slope = rise / run;

  // b = y-mx
  double intercept = val_low - (slope * (double)(ix_low));

  piece->setPTS(0, ix_low, ix_hgh);
  piece->wt(0) = slope;
  piece->wt(1) = intercept;

  return(piece);
}


//-------------------------------------------------------------
// Procedure: sortMapping
//   Purpose: Sort the original domain-range mapping to have the
//            pairs increasing in order.

void ZAIC_Vector::sortMapping()
{
  unsigned int dom_size = m_domain_vals.size();
  unsigned int rng_size = m_range_vals.size();
  if(dom_size != rng_size)
    return;

  vector<bool> hits(dom_size, false);

  vector<double> new_domain_vals;
  vector<double> new_range_vals;

  bool done = false;
  while(!done) {
    bool   hit = false;
    double smallest_val = 0;
    unsigned int i, smallest_ix = 0;
    for(i=0; i<dom_size; i++) {
      if(!hits[i] && (!hit || (m_domain_vals[i] < smallest_val))) {
	hit = true;
	smallest_val = m_domain_vals[i];
	smallest_ix = i;
      }
    }
    if(!hit)
      done = true;
    else {
      hits[smallest_ix] = true;
      new_domain_vals.push_back(m_domain_vals[smallest_ix]);
      new_range_vals.push_back(m_range_vals[smallest_ix]);
    }
  }
  m_domain_vals = new_domain_vals;
  m_range_vals = new_range_vals;

  m_sort_needed = false;
}


//-------------------------------------------------------------
// Procedure: print

void ZAIC_Vector::print()
{
  cout << "ZAIC_Vector::print():" << endl;

  if(!hasWarnings())
    cout << "Warnings: NONE" << endl;
  else {
    cout << "Warnings: " << endl;
    cout << "  " << getWarnings() << endl;
  }

  cout << "m_domain_pts:"   << m_domain_pts << endl;
  cout << "m_domain_low:"   << m_domain_low << endl;
  cout << "m_domain_high:"  << m_domain_high << endl;
  cout << "m_domain_delta:" << m_domain_delta << endl;
  cout << "m_domain_ok:" << boolToString(m_domain_ok) << endl;

  cout << "Errors: ";
  if(m_errors.size() == 0)
    cout << "NONE" << endl;
  else {
    cout << endl;
    unsigned int i, isize = m_errors.size();
    for(i=0; i<isize; i++) 
      cout << "  [" << i << "]" << m_errors[i] << endl;
  }

  cout << "domain values:" << endl;
  unsigned int j, jsize = m_domain_vals.size();
  for(j=0; j<jsize; j++) {
    cout << "  [" << j << "]:" << m_domain_vals[j] << ", " << m_range_vals[j] << endl;
  }

  cout << "idomain values:" << endl;
  unsigned int k, ksize = m_idomain_vals.size();
  for(k=0; k<ksize; k++) {
    cout << "  [" << k << "]:" << m_idomain_vals[k] << ", " << m_irange_vals[k] << endl;
  }
}


//-------------------------------------------------------------
// Procedure: getErrors()
// Procedure: hasErrors()
// Procedure: clearErrors()

string ZAIC_Vector::getErrors()
{
  unsigned int count = m_errors.size();
  if(count == 0)
    return("");

  string return_str = "(" + uintToString(count) + "):";

  unsigned int j, jsize = m_errors.size();
  for(j=0; j<jsize; j++) {
    if(return_str != "")
      return_str += ":";
    return_str += m_errors[j];
  }
  return(return_str);
}

bool ZAIC_Vector::hasErrors()
{
  return(m_errors.size() > 0);
}

void ZAIC_Vector::clearErrors()
{
  m_errors.clear();
}

//-------------------------------------------------------------
// Procedure: getWarnings()
// Procedure: hasWarnings()
// Procedure: clearWarnings()

string ZAIC_Vector::getWarnings()
{
  unsigned int count = m_config_warnings.size() + m_build_warnings.size();
  if(count == 0)
    return("");

  string return_str = "(" + uintToString(count) + ")";

  unsigned int i, isize = m_config_warnings.size();
  for(i=0; i<isize; i++)
    return_str += ":" + m_config_warnings[i];

  unsigned int j, jsize = m_build_warnings.size();
  for(j=0; j<jsize; j++) 
    return_str += ":" + m_build_warnings[j];
  return(return_str);
}


//-------------------------------------------------------------
// Procedure: hasWarnings()

bool ZAIC_Vector::hasWarnings()
{
  return((m_config_warnings.size() + m_build_warnings.size()) > 0);
}

//-------------------------------------------------------------
// Procedure: clearWarnings()

void ZAIC_Vector::clearWarnings()
{
  m_build_warnings.clear();
}


//-------------------------------------------------------------
// Procedure: setPDMap2()

PDMap *ZAIC_Vector::setPDMap2()
{
  int    first_pt  = 0;
  double first_val = m_irange_vals[0];

  bool   trend = false; // No trend to start
  double s_m = 0;
  double s_b = 0;

  //unsigned int domain_pts = m_irange_vals.size();
  
  vector<IvPBox*> pieces;
  for(unsigned int i=1; i<m_domain_pts; i++) {

    if(!trend) {
      trend  = true;
      s_m = (m_irange_vals[i] - first_val) / (i - first_pt);
      s_b = (m_irange_vals[i] - (s_m * i));
    }

    // Project from the running linear line what the value 
    // should be at this point [i]. Then compare to the actual
    // point and see if it lies within the tolerance.
    double ext_val = (s_m * i) + s_b;
    bool lbreak = false;
    if((fabs(ext_val - m_irange_vals[i])) > m_tolerance)
      lbreak = true;

    // In addition to applying the tolerance criteria, also
    // declare a break if there is a change in sign or slope
    // to nail exactly a peak in the function.
    double loc_m = (m_irange_vals[i] - m_irange_vals[i-1]) / (i - (i-1));
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
	piece->wt(1) = m_irange_vals[i];
	pieces.push_back(piece);
      }
      else {
	piece->setPTS(0, first_pt, i);
	double rise   = m_irange_vals[i] - first_val;
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
      double rise   = m_irange_vals[i-1] - first_val;
      double run    = (i-1) - first_pt;
      double slope  = rise / run;
      double intcpt = first_val - (slope * first_pt);
      piece->wt(0) = slope;
      piece->wt(1) = intcpt;
      pieces.push_back(piece);
      trend = false;
      first_pt  = i;
      first_val = m_irange_vals[i]; 
    }
    
  }
  
  unsigned int piece_count = pieces.size();

  PDMap *pdmap = new PDMap(piece_count, m_ivp_domain, 1);

  for(unsigned int i=0; i<piece_count; i++)
    pdmap->bx(i) = pieces[i];

  return(pdmap);
}




