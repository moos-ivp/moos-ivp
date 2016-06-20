/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: QuadSet.cpp                                          */
/*    DATE: July 4th 2006                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "QuadSet.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

QuadSet::QuadSet()
{
  m_snap_val  = 0;
  m_minpt_val = 0;
  m_maxpt_val = 0;
  m_max_crs   = 0;
  m_max_spd   = 0;
  
  m_quadset_dim     = 0;
  m_ipf_priority_wt = 0;
}

//-------------------------------------------------------------
// Procedure: setIvPDomain()

void QuadSet::setIvPDomain(IvPDomain ivp_domain)
{
  m_ivp_domain = ivp_domain;
}


//-------------------------------------------------------------
// Procedure: applyIPf

bool QuadSet::applyIPF(IvPFunction *ipf, string source)
{
  if(!ipf)
    return(false);

  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();

  bool crs_spd_ipf = false;
  if(ivp_domain.size() == 2) {
    if((ivp_domain.getVarName(0) == "speed") &&
       (ivp_domain.getVarName(1) == "course"))
       crs_spd_ipf = true;
    if((ivp_domain.getVarName(0) == "course") &&
       (ivp_domain.getVarName(1) == "speed"))
       crs_spd_ipf = true;
  }
  
  if(ipf->getDim() == 1)
    return(applyIPF1D(ipf, source));
  else if((ipf->getDim() == 2) && crs_spd_ipf)
    return(applyIPF2DHS(ipf));
  else if(ipf->getDim() == 2)
    return(applyIPF2D(ipf));
  
  return(false);
}

//-------------------------------------------------------------
// Procedure: apply1DIPF

bool QuadSet::applyIPF1D(IvPFunction *ipf, string source)
{
  if(!ipf)
    return(false);

  PDMap *pdmap = ipf->getPDMap();

  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();
  if(ivp_domain.size() == 0)
    return(false);

  m_ivp_domain = ivp_domain;
  m_ipf_priority_wt = ipf->getPWT();

  m_domain_pts.clear();
  m_domain_ptsx.clear();
  m_range_vals.clear();
  m_range_val_max.clear();
  m_domain_ix_max.clear();
  m_sources.clear();

  vector<double>  domain_pts;
  vector<double>  range_vals;
  vector<bool>    domain_ptsx;
  double          range_val_max=0;
  unsigned int    domain_ix_max=0;

  double priority_wt = ipf->getPWT();

  unsigned int i, total_pts = m_ivp_domain.getVarPoints(0);

  IvPBox ptbox(1);
  for(i=0; i<total_pts; i++) {
    ptbox.setPTS(0, i, i);
    double domain_val = m_ivp_domain.getVal(0, i);
    double range_val = pdmap->evalPoint(&ptbox) * priority_wt;
    domain_pts.push_back(domain_val);
    domain_ptsx.push_back(false);
    range_vals.push_back(range_val);

    if((i==0) || (range_val > range_val_max)) {
      range_val_max = range_val;
      domain_ix_max = i;
    }
  }
  
  // Go through and determin which domain_ix vals are edges, or
  // vertices in the 1D IvPFunction.
  int j, vsize = pdmap->size();
  for(j=0; j<vsize; j++) {
    int dlow    = pdmap->bx(j)->pt(0,0);
    int dhgh    = pdmap->bx(j)->pt(0,1);
    if((dlow > 0) && (dlow <= (int)(total_pts)))
      domain_ptsx[dlow] = true;
    if((dhgh > 0) && (dhgh <= (int)(total_pts)))
      domain_ptsx[dhgh] = true;
  }

  m_domain_pts.push_back(domain_pts);
  m_range_vals.push_back(range_vals);
  m_domain_ptsx.push_back(domain_ptsx);
  m_range_val_max.push_back(range_val_max);
  m_domain_ix_max.push_back(domain_ix_max);
  m_sources.push_back(source);

  m_quadset_dim = 1;
  return(true);
}


//-------------------------------------------------------------
// Procedure: applyIPF2DHS
//      Note: For functions defined only over course or speed, we rely
//            on the caller to "expand" the function before calling.

bool QuadSet::applyIPF2DHS(IvPFunction *ipf)
{
  if(!ipf)
    return(false);

  bool wrap = true;

  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();

  unsigned int crs_pts = ivp_domain.getVarPoints("course");
  unsigned int spd_pts = ivp_domain.getVarPoints("speed");
  if((crs_pts < 2) || (spd_pts < 2))
    return(false);

  m_ivp_domain = ivp_domain;
  m_ipf_priority_wt = ipf->getPWT();

  m_quads.clear();

  // Create memory to hold the sample results
  unsigned int i,j;
  double **vals = new double*[crs_pts];
  for(i=0; i<crs_pts; i++) {
    vals[i] = new double[spd_pts];
    for(j=0; j<spd_pts; j++)
      vals[i][j] = 0;
  }

  // Create the "sample" box 
  IvPBox sbox(m_ivp_domain.size());
  
  int crs_ix = m_ivp_domain.getIndex("course");
  int spd_ix = m_ivp_domain.getIndex("speed");

  double priority_wt = ipf->getPWT();
  for(i=0; i<crs_pts; i++) {
    sbox.setPTS(crs_ix, i, i);
    for(j=0; j<spd_pts; j++) {
      sbox.setPTS(spd_ix, j, j);
      double pval = priority_wt * ipf->getPDMap()->evalPoint(&sbox);
      pval = snapToStep(pval, m_snap_val);
      vals[i][j] = pval;
    }
  }

  // Build the primary quads
  for(i=0; i<(crs_pts-1); i++) {
    for(j=0; j<(spd_pts-1); j++) {
      Quad3D new_quad;
      new_quad.setXL(i);
      new_quad.setXH(i+1);
      new_quad.setYL(j);
      new_quad.setYH(j+1);
      new_quad.setLLZ(vals[i][j]);
      new_quad.setLHZ(vals[i][j+1]);
      new_quad.setHLZ(vals[i+1][j]);
      new_quad.setHHZ(vals[i+1][j+1]);

      //new_quad.lines = false;
      new_quad.xpts = crs_pts;
      new_quad.ypts = spd_pts;

      m_quads.push_back(new_quad);
    }
  }

  if(ivp_domain.hasDomain("course") && wrap) {
  
    // Add  "bridge" quads to wrap around 359-0
    int top_crs_ix = crs_pts-1;
    for(j=0; j<(spd_pts-1); j++) {
      Quad3D new_quad;
      new_quad.setXL(top_crs_ix-1);  // usually 359
      new_quad.setXH(0);
      new_quad.setYL(j);
      new_quad.setYH(j+1);
      new_quad.setLLZ(vals[top_crs_ix][j]);
      new_quad.setLHZ(vals[top_crs_ix][j+1]);
      new_quad.setHLZ(vals[0][j]);
      new_quad.setHHZ(vals[0][j+1]);
      
      //new_quad.lines = false;
      new_quad.xpts = crs_pts;
      new_quad.ypts = spd_pts;
      
      m_quads.push_back(new_quad);
    }
  }

  // Added apr 15th 2015 delete tmp memory
  for(i=0; i<crs_pts; i++) 
    delete [] vals[i];
  delete [] vals;

  resetMinMaxVals();
  m_quadset_dim = 2;
  return(true);
}


//-------------------------------------------------------------
// Procedure: apply2DIPF
//      Note: For functions defined only over course or speed, we rely
//            on the caller to "expand" the function before calling.

bool QuadSet::applyIPF2D(IvPFunction *ipf)
{
  if(!ipf)
    return(false);

  PDMap *pdmap = ipf->getPDMap();
  if(!pdmap)
    return(false);

  IvPDomain ivp_domain = pdmap->getDomain();
  unsigned int xpts = ivp_domain.getVarPoints(0);
  unsigned int ypts = ivp_domain.getVarPoints(1);
  if((xpts < 2) || (ypts < 2))
    return(false);

  m_ivp_domain = ivp_domain;
  m_ipf_priority_wt = ipf->getPWT();

  m_quads.clear();

  for(unsigned int i=0; i<pdmap->size(); i++) {
    const IvPBox* box = pdmap->bx(i);

    Quad3D q;

    q.setXL(box->pt(0,0));
    q.setXH(box->pt(0,1));
    q.setYL(box->pt(1,0));
    q.setYH(box->pt(1,1));
    q.xpts  = xpts;
    q.ypts  = ypts;

    int degree = box->getDegree();
    
    if(degree != 1)
      return(false);

    IvPBox ebox(2,1);
    
    ebox.setPTS(0, (int)(q.getXL()), (int)(q.getXL()));
    ebox.setPTS(1, (int)(q.getYL()), (int)(q.getYL()));
    q.setLLZ(box->ptVal(&ebox));

    ebox.setPTS(0, (int)(q.getXH()), (int)(q.getXH()));
    q.setHLZ(box->ptVal(&ebox));
    
    ebox.setPTS(1, (int)(q.getYH()), (int)(q.getYH()));
    q.setHHZ(box->ptVal(&ebox));
    
    ebox.setPTS(0, (int)(q.getXL()), (int)(q.getXL()));
    q.setLHZ(box->ptVal(&ebox));

    m_quads.push_back(q);
  }

  resetMinMaxVals();
  m_quadset_dim = 2;

  return(true);
}


//-------------------------------------------------------------
// Procedure: applyColorMap

void QuadSet::applyColorMap(const FColorMap& cmap)
{
  applyColorMap(cmap, m_minpt_val, m_maxpt_val);
}


//-------------------------------------------------------------
// Procedure: applyColorMap

void QuadSet::applyColorMap(const FColorMap& cmap,
			    double given_low, double given_high)
{
  unsigned int vsize = m_quads.size();
  if(vsize == 0)
    return;
  
  double a_low_val   = m_minpt_val;
  double a_high_val  = m_maxpt_val;

  if(given_low != given_high) {
    a_low_val  = given_low;
    a_high_val = given_high;
  }

  double range = (a_high_val - a_low_val);
  // Check for the case where the range is zero
  if(range <= 0) 
    range = 1.0;
  for(unsigned int i=0; i<vsize; i++) {
    double llpct = (m_quads[i].getLLZ() - a_low_val) / range;
    double lhpct = (m_quads[i].getLHZ() - a_low_val) / range;
    double hlpct = (m_quads[i].getHLZ() - a_low_val) / range;
    double hhpct = (m_quads[i].getHHZ() - a_low_val) / range;

    m_quads[i].setLLR(cmap.getIRVal(llpct));
    m_quads[i].setLLG(cmap.getIGVal(llpct));
    m_quads[i].setLLB(cmap.getIBVal(llpct));
    
    m_quads[i].setLHR(cmap.getIRVal(lhpct));
    m_quads[i].setLHG(cmap.getIGVal(lhpct));
    m_quads[i].setLHB(cmap.getIBVal(lhpct));
    
    m_quads[i].setHLR(cmap.getIRVal(hlpct));
    m_quads[i].setHLG(cmap.getIGVal(hlpct));
    m_quads[i].setHLB(cmap.getIBVal(hlpct));
    
    m_quads[i].setHHR(cmap.getIRVal(hhpct));
    m_quads[i].setHHG(cmap.getIGVal(hhpct));
    m_quads[i].setHHB(cmap.getIBVal(hhpct));

    // For testing purposes - we can grab a range of values and
    // render them as grey (0.5)
#if 0
    double tl = 50;
    double th = 60;
    double dv = 0.5;
    if((m_quads[i].getLLZ() >= tl) && (m_quads[i].getLLZ() <= th) && 
       (m_quads[i].getLHZ() >= tl) && (m_quads[i].getLHZ() <= th) &&
       (m_quads[i].getHLZ() >= tl) && (m_quads[i].getHLZ() <= th) && 
       (m_quads[i].getHHZ() >= tl) && (m_quads[i].getHHZ() <= th)) {
      
      m_quads[i].setLLR(dv);
      m_quads[i].setLLG(dv);
      m_quads[i].setLLB(dv);
      
      m_quads[i].setLHR(dv);
      m_quads[i].setLHG(dv);
      m_quads[i].setLHB(dv);
      
      m_quads[i].setHLR(dv);
      m_quads[i].setHLG(dv);
      m_quads[i].setHLB(dv);

      m_quads[i].setHHR(dv);
      m_quads[i].setHHG(dv);
      m_quads[i].setHHB(dv);
    }
#endif
  }
}

//-------------------------------------------------------------
// Procedure: addQuadSet

void QuadSet::addQuadSet(const QuadSet& g_quads)
{
  // If given quadset is empty, it may be due to the given quadset
  // having not been derived from a heading/speed ipf.
  if(g_quads.isEmptyND())
    return;

  unsigned g_quadset_dim = g_quads.getQuadSetDim();

  // After this quadset becomes non-empty, only accept additions of 
  // other quadsets if their dimensions match.
  if(!isEmptyND() && (m_quadset_dim != g_quadset_dim))
    return;

  // Handle 2D Addition Case
  if(g_quadset_dim == 2) {
    unsigned int i;
    unsigned int msize  = size2D();
    unsigned int gsize  = g_quads.size2D();
    
    // If this is an empty quadset, just set to the given quadset
    if(msize == 0) {
      for(i=0; i<gsize; i++)
	m_quads.push_back(g_quads.getQuad(i));
      resetMinMaxVals();
      m_quadset_dim     = g_quads.getQuadSetDim();
      m_ivp_domain      = g_quads.getDomain();
      m_ipf_priority_wt = g_quads.getPriorityWt();
     return;
    }

    // If the two quadsets are of different size, it may be due to the
    // given quadset being defined not over heading/speed.
    if(msize != gsize)
      return;

    for(i=0; i<msize; i++) {
      m_quads[i].addLLZ(g_quads.getQuad(i).getLLZ());
      m_quads[i].addLHZ(g_quads.getQuad(i).getLHZ());
      m_quads[i].addHLZ(g_quads.getQuad(i).getHLZ());
      m_quads[i].addHHZ(g_quads.getQuad(i).getHHZ());
    }
    resetMinMaxVals();
    return;
  }

  // Handle 1D Addition Case
  if(g_quadset_dim == 1) {
    unsigned int i;
    unsigned int msize  = size1D();
    unsigned int gsize  = g_quads.size1D();
    
    if((msize != 0) && (msize != gsize))
      return;

    vector<double>  domain_pts    = g_quads.getDomainPts();
    vector<double>  range_vals    = g_quads.getRangeVals();
    vector<bool>    domain_ptsx   = g_quads.getDomainPtsX();
    double          range_val_max = g_quads.getRangeValMax();
    unsigned int    domain_ix_max = g_quads.getDomainIXMax();
    string          source        = g_quads.getSource();
    
    // If this is an empty quadset, just set to the given quadset
    if(msize == 0) {
      m_domain_pts.push_back(domain_pts);
      m_range_vals.push_back(range_vals);
      m_domain_ptsx.push_back(domain_ptsx);
      m_range_val_max.push_back(range_val_max);
      m_domain_ix_max.push_back(domain_ix_max);
      m_sources.push_back("collective");

      m_domain_pts.push_back(domain_pts);
      m_range_vals.push_back(range_vals);
      m_domain_ptsx.push_back(domain_ptsx);
      m_range_val_max.push_back(range_val_max);
      m_domain_ix_max.push_back(domain_ix_max);
      m_sources.push_back(source);

      m_quadset_dim = g_quads.getQuadSetDim();
      m_ivp_domain = g_quads.getDomain();
      return;
    }
    else {
      for(i=0; i<msize; i++) {
	m_range_vals[0][i] += range_vals[i];
	m_domain_ptsx[0][i] = (m_domain_ptsx[0][i] || domain_ptsx[i]);
	if((i==0) || (m_range_vals[0][i] > m_range_val_max[0])) {
	  m_range_val_max[0] = m_range_vals[0][i];
	  m_domain_ix_max[0] = i;
	}
      }
      m_domain_pts.push_back(domain_pts);
      m_range_vals.push_back(range_vals);
      m_domain_ptsx.push_back(domain_ptsx);
      m_range_val_max.push_back(range_val_max);
      m_domain_ix_max.push_back(domain_ix_max);
      m_sources.push_back(source);
    }
  }
}

//-------------------------------------------------------------
// Procedure: normalize()

void QuadSet::normalize(double target_base, double target_range)
{
  double existing_range = m_maxpt_val - m_minpt_val;
  if((existing_range <= 0) || (target_range <= 0))
    return;

  unsigned int i, msize = m_quads.size();
  if(msize == 0)
    return;

  double pct;
  for(i=0; i<msize; i++) {
    pct = ((m_quads[i].getLLZ() - m_minpt_val) / existing_range);
    m_quads[i].setLLZ(target_base + (pct * target_range));

    pct = ((m_quads[i].getHLZ() - m_minpt_val) / existing_range);
    m_quads[i].setHLZ(target_base + (pct * target_range));

    pct = ((m_quads[i].getLHZ() - m_minpt_val) / existing_range);
    m_quads[i].setLHZ(target_base + (pct * target_range));

    pct = ((m_quads[i].getHHZ() - m_minpt_val) / existing_range);
    m_quads[i].setHHZ(target_base + (pct * target_range));
  }

  m_minpt_val = target_base;
  m_maxpt_val = target_base + target_range;
}

//-------------------------------------------------------------
// Procedure: isEmptyND()

bool QuadSet::isEmptyND() const
{
  return((m_quads.size()==0) && (m_domain_pts.size()==0));
}


//-------------------------------------------------------------
// Procedure: size1D

unsigned int QuadSet::size1D() const
{
  if(m_domain_pts.size() == 0)
    return(0);
  
  return(m_domain_pts[0].size());   
}

//-------------------------------------------------------------
// Procedure: getDomainPts

vector<double> QuadSet::getDomainPts(unsigned int index) const
{
  vector<double> rvector;
  if(index >= m_domain_pts.size())
    return(rvector);
  
  return(m_domain_pts[index]);   
}

//-------------------------------------------------------------
// Procedure: getRangeVals

vector<double> QuadSet::getRangeVals(unsigned int index) const
{
  vector<double> rvector;
  if(index >= m_range_vals.size())
    return(rvector);

  return(m_range_vals[index]);   
}

//-------------------------------------------------------------
// Procedure: getDomainPtsx

vector<bool> QuadSet::getDomainPtsX(unsigned int index) const
{
  vector<bool> rvector;
  if(index >= m_domain_ptsx.size())
    return(rvector);

  return(m_domain_ptsx[index]);   
}

//-------------------------------------------------------------
// Procedure: getRangeValMax

double QuadSet::getRangeValMax(unsigned int index) const
{
  if(index >= m_range_val_max.size())
    return(0);

  return(m_range_val_max[index]);   
}

//-------------------------------------------------------------
// Procedure: getDomainIXMax

unsigned int QuadSet::getDomainIXMax(unsigned int index) const
{
  if(index >= m_domain_ix_max.size())
    return(0);

  return(m_domain_ix_max[index]);   
}

//-------------------------------------------------------------
// Procedure: getSource

string QuadSet::getSource(unsigned int index) const
{
  if(index >= m_sources.size())
    return("");

  return(m_sources[index]);   
}



//-------------------------------------------------------------
// Procedure: getMaxPoint

double QuadSet::getMaxPoint(string varname) const
{
  if(varname == "course")
    return(m_max_crs);
  else if(varname == "speed")
    return(m_max_spd);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: getMaxPointQIX

unsigned int QuadSet::getMaxPointQIX(string varname) const
{
  if(varname == "course")
    return(m_max_crs_qix);
  else if(varname == "speed")
    return(m_max_spd_qix);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: print()

void QuadSet::print() const
{
  cout << "QuadSet::print() " << endl;
  cout << "  Dimension: " << m_quadset_dim << endl;
  cout << "  2Dsize: " << m_quads.size() << endl;
  cout << "    minpt_val: " << m_minpt_val << endl;
  cout << "    maxpt_val: " << m_maxpt_val << endl;
  cout << "    maxpt_hdg: " << getMaxPoint("course") << endl;
  cout << "    maxpt_spd: " << getMaxPoint("speed")  << endl;

  unsigned int slots1d = m_domain_ptsx.size();
  cout << "  Total 1D slots:" << slots1d << endl;
  if(slots1d == 0)
    return;
  cout << "  1Dsize: " << endl;
  cout << "    DomainPts: " << m_domain_pts[0].size() << endl;
  cout << "    RangeVals: " << m_range_vals[0].size() << endl;
  cout << "    DomainPtsX: " << m_domain_ptsx[0].size() << endl;
  cout << "    RangeValMax: " << m_range_val_max[0]  << endl;
}


//-------------------------------------------------------------
// Procedure: resetMinMaxVals()

void QuadSet::resetMinMaxVals()
{
  unsigned int i, msize  = m_quads.size();
  if(msize == 0)
    return;
  
  m_max_crs_qix = 0;
  m_max_spd_qix = 0;

  // Recalculate the new global low and high values.
  m_minpt_val = m_quads[0].getLLZ();
  m_maxpt_val = m_quads[0].getLLZ();

  for(i=0; i<msize; i++) {    
    if(m_quads[i].getLLZ() < m_minpt_val)  //------- (L,L)
      m_minpt_val  = m_quads[i].getLLZ();
    if(m_quads[i].getLLZ() > m_maxpt_val) {
      m_maxpt_val = m_quads[i].getLLZ();
      m_max_crs_qix = m_quads[i].getXL();
      m_max_spd_qix = m_quads[i].getYL();
    }

    if(m_quads[i].getLHZ() < m_minpt_val)  //------- (L,H)  
      m_minpt_val  = m_quads[i].getLHZ();
    if(m_quads[i].getLHZ() > m_maxpt_val) {  
      m_maxpt_val = m_quads[i].getLHZ();
      m_max_crs_qix = m_quads[i].getXL();
      m_max_spd_qix = m_quads[i].getYH();
    }

    if(m_quads[i].getHLZ() < m_minpt_val)  //------- (H,L)  
      m_minpt_val  = m_quads[i].getHLZ();
    if(m_quads[i].getHLZ() > m_maxpt_val) {
      m_maxpt_val = m_quads[i].getHLZ();
      m_max_crs_qix = m_quads[i].getXH();
      m_max_spd_qix = m_quads[i].getYL();
    }

    if(m_quads[i].getHHZ() < m_minpt_val) //------- (H,H)    
      m_minpt_val  = m_quads[i].getHHZ();
    if(m_quads[i].getHHZ() > m_maxpt_val) {
      m_maxpt_val = m_quads[i].getHHZ();
      m_max_crs_qix = m_quads[i].getXH();
      m_max_spd_qix = m_quads[i].getYH();
    }
  }

  int crs_ix = m_ivp_domain.getIndex("course");
  int spd_ix = m_ivp_domain.getIndex("speed");
  m_max_crs = m_ivp_domain.getVal(crs_ix, m_max_crs_qix);
  m_max_spd = m_ivp_domain.getVal(spd_ix, m_max_spd_qix);
}




