/*****************************************************************/
/*    NAME: Michael Benjamin and Henrik Schmidt                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_MANUAL.cpp                                      */
/*    DATE: Aug 5th 2008                                         */
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
#include "ZAIC_MANUAL.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_MANUAL::ZAIC_MANUAL(IvPDomain g_domain, const string& g_varname) 
{
  m_state_ok = true;

  m_ivp_domain = subDomain(g_domain, g_varname);

  // If the domain variable is not found in the given domain, flip
  // the m_state_ok bit to false. 
  if(m_ivp_domain.size() == 0)
    m_state_ok = false;

  m_value_wrap    = false;

  m_domain_ix    = m_ivp_domain.getIndex(g_varname);
  m_domain_high  = m_ivp_domain.getVarHigh(m_domain_ix);
  m_domain_low   = m_ivp_domain.getVarLow(m_domain_ix);
  m_domain_pts   = m_ivp_domain.getVarPoints(m_domain_ix);
  m_domain_delta = m_ivp_domain.getVarDelta(m_domain_ix);

  unsigned int i;
  for(i=0; i<m_domain_pts; i++)
    m_ptvals.push_back(0.0);  
}

//-------------------------------------------------------------
// Procedure: addComponent
//    Return: index of the new summit just added

int ZAIC_MANUAL::addComponent(double low,  bool lowinc,  double lowutil,
			      double high, bool highinc, double highutil)
{
  if(low > high) {
    m_warning += "bad new-component spec, (low>high):";
    m_state_ok = false;
  }
  
  if((low==high) && ((!lowinc || !highinc))) {
    m_warning += "bad new-component spec, (low==high, not incl)";
    m_state_ok = false;
  }    
  
  if((low==high) && (lowutil != highutil)) {
    m_warning += "bad new-component spec, (low==high, diff utils):";
    m_state_ok = false;
  }
  
  if(m_state_ok == false)
    return(m_low_val.size()-1);

  m_low_val.push_back(low);
  m_low_val_inc.push_back(lowinc);
  m_low_val_util.push_back(lowutil);

  m_high_val.push_back(high);
  m_high_val_inc.push_back(highinc);
  m_high_val_util.push_back(highutil);

  return(m_low_val.size()-1);
}


//-------------------------------------------------------------
// Procedure: addComponent

bool ZAIC_MANUAL::addComponent(string bnds, double lowpt, double lowutil,
			       double highpt, double highutil)
{
  if(bnds.size() != 2) {
    m_warning += "bad new-component spec, (bad boundary):";
    m_state_ok = false;
    return(false);
  }

  bool lowinc;
  if((bnds.at(0) == 'm') || (bnds.at(0) == 'M')) {
    lowpt = m_domain_low;
    lowinc = true;
  }
  else if(bnds.at(0) == '[') 
    lowinc = true;
  else if(bnds.at(0) == '(')
    lowinc = false;
  else {
    m_warning += "bad new-component spec, (bad boundary):";
    m_state_ok = false;
    return(false);
  }

  bool highinc;
  if((bnds.at(1) == 'm') || (bnds.at(1) == 'M')) {
    highpt = m_domain_high;
    highinc = true;
  }
  else if(bnds.at(1) == ']') 
    highinc = true;
  else if(bnds.at(1) == ')')
    highinc = false;
  else {
    m_warning += "bad new-component spec, (bad boundary):";
    m_state_ok = false;
    return(false);
  }


  if(lowpt > highpt) {
    m_warning += "bad new-component spec, (low>high):";
    m_state_ok = false;
  }
  
  if((lowpt == highpt) && ((!lowinc || !highinc))) {
    m_warning += "bad new-component spec, (low==high, not incl)";
    m_state_ok = false;
  }    
  
  if((lowpt==highpt) && (lowutil != highutil)) {
    m_warning += "bad new-component spec, (low==high, diff utils):";
    m_state_ok = false;
  }
  
  if(m_state_ok == false)
    return(m_low_val.size()-1);

  m_low_val.push_back(lowpt);
  m_low_val_inc.push_back(lowinc);
  m_low_val_util.push_back(lowutil);

  m_high_val.push_back(highpt);
  m_high_val_inc.push_back(highinc);
  m_high_val_util.push_back(highutil);

  return(true);
}


//-------------------------------------------------------------
// Procedure: getParam

double ZAIC_MANUAL::getParam(string param)
{
  if(param == "summit")
    return(0);
  else if(param == "basewidth")
    return(0);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: getParam

string ZAIC_MANUAL::getParam(string param, int index)
{
  if((index < 0) || (index >= m_low_val.size()))
    return("err");

  double return_val = 0;
  if(param == "lowval") 
    return_val = m_low_val[index];
  else if(param == "lowinc") 
    return_val = m_low_val_inc[index];
  else if(param == "lowutil") 
    return_val = m_low_val_util[index];
  else if(param == "highval") 
    return_val = m_high_val[index];
  else if(param == "highinc") 
    return_val = m_high_val_inc[index];
  else if(param == "highutil") 
    return_val = m_high_val_util[index];
  else
    return("err");

  return(dstringCompact(doubleToString(return_val, 2)));
}


//-------------------------------------------------------------
// Procedure: evalPoint(int)
//

double ZAIC_MANUAL::evalPoint(unsigned int ix)
{
  double dval = (((double)(ix)) * m_domain_delta) + m_domain_low;

  int catch_count = 0;
  
  double total = 0;

  int vsize = m_low_val.size();
  for(int i=0; i<vsize; i++) {
    bool in_low = false;
    if(dval > m_low_val[i])
      in_low = true;
    else if(m_low_val_inc[i] && (dval >= m_low_val[i]))
      in_low = true;

    bool in_high = false;
    if(dval < m_high_val[i])
      in_high = true;
    else if(m_high_val_inc[i] && (dval <= m_high_val[i]))
      in_high = true;

    if(in_low && in_high) {
      catch_count++;
      double range = (m_high_val[i] - m_low_val[i]);
      if(range == 0)
	total += m_low_val_util[i];
      else {
	double pct = (dval - m_low_val[i]) / range;
	double util_range = m_high_val_util[i] - m_low_val_util[i];
	double eval = (pct * util_range) + m_low_val_util[i];
	total += eval;
      }
    }
  }

  if(catch_count > 1) {
    m_state_ok = false;
    m_warning += "overlapping-components: ";
  }

  return(total);
}

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *ZAIC_MANUAL::extractOF()
{
  if((m_domain_ix == -1) || (m_state_ok == false))
    return(0);
  
  unsigned int i;
  for(i=0; i<m_domain_pts; i++)
    m_ptvals[i] = evalPoint(i);

  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *ipf = new IvPFunction(pdmap);

  return(ipf);
}

//-------------------------------------------------------------
// Procedure: setPDMap()
//

PDMap *ZAIC_MANUAL::setPDMap(double tolerance)
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


