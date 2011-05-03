/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: QuadSet.cpp                                         */
/*    DATE: July 4th 2006                                       */
/****************************************************************/

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
}

//-------------------------------------------------------------
// Procedure: applyIPF
//      Note: For functions defined only over course or speed, we rely
//            on the caller to "expand" the function before calling.

bool QuadSet::applyIPF(IvPFunction *ipf, bool wrap)
{
  if(!ipf)
    return(false);

  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();

  unsigned int crs_pts = ivp_domain.getVarPoints("course");
  unsigned int spd_pts = ivp_domain.getVarPoints("speed");
  if((crs_pts < 2) || (spd_pts < 2))
    return(false);

  m_ivp_domain = ivp_domain;
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
      new_quad.xl = i;
      new_quad.xh = i+1;
      new_quad.yl = j;
      new_quad.yh = j+1;
      new_quad.llval = vals[i][j];
      new_quad.lhval = vals[i][j+1];
      new_quad.hlval = vals[i+1][j];
      new_quad.hhval = vals[i+1][j+1];

      new_quad.lines = false;
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
      new_quad.xl = top_crs_ix-1;  // usually 359
      new_quad.xh = 0;
      new_quad.yl = j;
      new_quad.yh = j+1;
      new_quad.llval = vals[top_crs_ix][j];
      new_quad.lhval = vals[top_crs_ix][j+1];
      new_quad.hlval = vals[0][j];
      new_quad.hhval = vals[0][j+1];
      
      new_quad.lines = false;
      new_quad.xpts = crs_pts;
      new_quad.ypts = spd_pts;
      
      m_quads.push_back(new_quad);
    }
  }

  resetMinMaxVals();
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
  int vsize = m_quads.size();
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
  for(int i=0; i<vsize; i++) {
    double llpct = (m_quads[i].llval - a_low_val) / range;
    double lhpct = (m_quads[i].lhval - a_low_val) / range;
    double hlpct = (m_quads[i].hlval - a_low_val) / range;
    double hhpct = (m_quads[i].hhval - a_low_val) / range;

    m_quads[i].llval_r = cmap.getIRVal(llpct);
    m_quads[i].llval_g = cmap.getIGVal(llpct);
    m_quads[i].llval_b = cmap.getIBVal(llpct);
    
    m_quads[i].lhval_r = cmap.getIRVal(lhpct);
    m_quads[i].lhval_g = cmap.getIGVal(lhpct);
    m_quads[i].lhval_b = cmap.getIBVal(lhpct);
    
    m_quads[i].hlval_r = cmap.getIRVal(hlpct);
    m_quads[i].hlval_g = cmap.getIGVal(hlpct);
    m_quads[i].hlval_b = cmap.getIBVal(hlpct);
    
    m_quads[i].hhval_r = cmap.getIRVal(hhpct);
    m_quads[i].hhval_g = cmap.getIGVal(hhpct);
    m_quads[i].hhval_b = cmap.getIBVal(hhpct);


    // For testing purposes - we can grab a range of values and
    // render them as grey (0.5)
#if 0
    double tl = 50;
    double th = 60;
    double dv = 0.5;
    if((m_quads[i].llval >= tl) && (m_quads[i].llval <= th) && 
       (m_quads[i].lhval >= tl) && (m_quads[i].lhval <= th) &&
       (m_quads[i].hlval >= tl) && (m_quads[i].hlval <= th) && 
       (m_quads[i].hhval >= tl) && (m_quads[i].hhval <= th)) {
      
      m_quads[i].llval_r = dv;
      m_quads[i].llval_g = dv;
      m_quads[i].llval_b = dv;
      
      m_quads[i].lhval_r = dv;
      m_quads[i].lhval_g = dv;
      m_quads[i].lhval_b = dv;
      
      m_quads[i].hlval_r = dv;
      m_quads[i].hlval_g = dv;
      m_quads[i].hlval_b = dv;

      m_quads[i].hhval_r = dv;
      m_quads[i].hhval_g = dv;
      m_quads[i].hhval_b = dv;
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
  if(g_quads.isEmpty())
    return;

  unsigned int i;
  unsigned int msize  = m_quads.size();
  unsigned int gsize  = g_quads.size();

  // If this is an empty quadset, just set to the given quadset
  if(msize == 0) {
    for(i=0; i<gsize; i++)
      m_quads.push_back(g_quads.getQuad(i));
    resetMinMaxVals();
    return;
  }

  // If the two quadsets are of different size, it may be due to the
  // given quadset being defined not over heading/speed.
  if(msize != gsize)
    return;

  for(i=0; i<msize; i++) {
    m_quads[i].llval += (g_quads.getQuad(i).llval);
    m_quads[i].lhval += (g_quads.getQuad(i).lhval);
    m_quads[i].hlval += (g_quads.getQuad(i).hlval);
    m_quads[i].hhval += (g_quads.getQuad(i).hhval);
  }

  resetMinMaxVals();
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
    pct = ((m_quads[i].llval - m_minpt_val) / existing_range);
    m_quads[i].llval = target_base + (pct * target_range);
    pct = ((m_quads[i].hlval - m_minpt_val) / existing_range);
    m_quads[i].hlval = target_base + (pct * target_range);
    pct = ((m_quads[i].lhval - m_minpt_val) / existing_range);
    m_quads[i].lhval = target_base + (pct * target_range);
    pct = ((m_quads[i].hhval - m_minpt_val) / existing_range);
    m_quads[i].hhval = target_base + (pct * target_range);
  }

  m_minpt_val = target_base;
  m_maxpt_val = target_base + target_range;
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
  cout << "  size: " << m_quads.size() << endl;
  cout << "  minpt_val: " << m_minpt_val << endl;
  cout << "  maxpt_val: " << m_maxpt_val << endl;
  cout << "  maxpt_hdg: " << getMaxPoint("course") << endl;
  cout << "  maxpt_spd: " << getMaxPoint("speed")  << endl;
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
  m_minpt_val = m_quads[0].llval;
  m_maxpt_val = m_quads[0].llval;
  for(i=0; i<msize; i++) {    
    if(m_quads[i].llval < m_minpt_val)  //------- (L,L)
      m_minpt_val  = m_quads[i].llval;
    if(m_quads[i].llval > m_maxpt_val) {
      m_maxpt_val = m_quads[i].llval;
      m_max_crs_qix = m_quads[i].xl;
      m_max_spd_qix = m_quads[i].yl;
    }

    if(m_quads[i].lhval < m_minpt_val)  //------- (L,H)  
      m_minpt_val  = m_quads[i].lhval;
    if(m_quads[i].lhval > m_maxpt_val) {  
      m_maxpt_val = m_quads[i].lhval;
      m_max_crs_qix = m_quads[i].xl;
      m_max_spd_qix = m_quads[i].yh;
    }

    if(m_quads[i].hlval < m_minpt_val)  //------- (H,L)  
      m_minpt_val  = m_quads[i].hlval;
    if(m_quads[i].hlval > m_maxpt_val) {
      m_maxpt_val = m_quads[i].hlval;
      m_max_crs_qix = m_quads[i].xh;
      m_max_spd_qix = m_quads[i].yl;
    }

    if(m_quads[i].hhval < m_minpt_val) //------- (H,H)    
      m_minpt_val  = m_quads[i].hhval;
    if(m_quads[i].hhval > m_maxpt_val) {
      m_maxpt_val = m_quads[i].hhval;
      m_max_crs_qix = m_quads[i].xh;
      m_max_spd_qix = m_quads[i].yh;
    }
  }

  int crs_ix = m_ivp_domain.getIndex("course");
  int spd_ix = m_ivp_domain.getIndex("speed");
  m_max_crs = m_ivp_domain.getVal(crs_ix, m_max_crs_qix);
  m_max_spd = m_ivp_domain.getVal(spd_ix, m_max_spd_qix);
}
