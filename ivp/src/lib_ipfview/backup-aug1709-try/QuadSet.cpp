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
  m_weight          = 0;
  m_low_val         = 0;
  m_high_val        = 0;
  m_low_val_adjust  = 0;
  m_high_val_adjust = 0;
  m_snap_val        = 2.0;
}

//-------------------------------------------------------------
// Procedure: clear()

void QuadSet::clear()
{
  m_quads.clear();
  m_weight          = 0;
  m_low_val         = 0;
  m_high_val        = 0;
  m_low_val_adjust  = 0;
  m_high_val_adjust = 0;
}

//-------------------------------------------------------------
// Procedure: applyIPF

void QuadSet::applyIPF(IvPFunction *ipf, string var_a, string var_b)
{
  m_quads.clear();
  if(!ipf) 
    return;
  
  m_weight = ipf->getPWT();

  IvPDomain domain = ipf->getPDMap()->getDomain();
  int       dsize  = domain.size();
  int       i,j;

  // Vector will be built focussing on TWO domain variables
  // Check which ones were requested and that they exists in 
  // the IvPDomain associated with the function.

  if(!domain.hasDomain(var_a) || !domain.hasDomain(var_b))
    return;

  int dom_a_ix  = domain.getIndex(var_a);
  int dom_b_ix  = domain.getIndex(var_b);
  int dom_a_pts = domain.getVarPoints(var_a);
  int dom_b_pts = domain.getVarPoints(var_b);
  
  if((dom_a_pts < 2) || (dom_b_pts < 2))
    return;

  // Create the "sample" box
  IvPBox sbox = domainToBox(domain);
  for(i=0; i<dsize; i++) {
    int ipts = domain.getVarPoints(i);
    sbox.setPTS(i, ipts/2, ipts/2);
  }
  
  double **vals = new double*[dom_a_pts];
  for(i=0; i<dom_a_pts; i++) {
    vals[i] = new double[dom_b_pts];
    for(j=0; j<dom_b_pts; j++)
      vals[i][j] = 0;
  }

  for(i=0; i<dom_a_pts; i++) {
    sbox.setPTS(dom_a_ix, i, i);
    for(j=0; j<dom_b_pts; j++) {
      sbox.setPTS(dom_b_ix, j, j);
      double pval = ipf->getPDMap()->evalPoint(&sbox);
      pval = snapToStep(pval, m_snap_val);
      vals[i][j] = pval;
    }
  }

  m_low_val  = vals[0][0];
  m_high_val = vals[0][0];
  for(i=0; i<dom_a_pts; i++) {
    for(j=0; j<dom_b_pts; j++) {
      if(vals[i][j] > m_high_val)
	m_high_val = vals[i][j];
      if(vals[i][j] < m_low_val)
	m_low_val = vals[i][j];
    }
  }
  
  //cout << "lowest_pval: "  << m_low_val << endl;
  //cout << "highest_pval: " << m_high_val << endl;

  for(i=0; i<(dom_a_pts-1); i++) {
    for(j=0; j<(dom_b_pts-1); j++) {
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
      new_quad.xpts = dom_a_pts;
      new_quad.ypts = dom_b_pts;

      m_quads.push_back(new_quad);
    }
  }
  
  // Add  "bridge" quads to wrap around 359-0
  int top_crs_ix = dom_a_pts-1;
  for(j=0; j<(dom_b_pts-1); j++) {
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
    new_quad.xpts = dom_a_pts;
    new_quad.ypts = dom_b_pts;
    
    m_quads.push_back(new_quad);
  }

  for(i=0; i<dom_a_pts; i++)
    delete [] vals[i];
  delete [] vals;
}


//-------------------------------------------------------------
// Procedure: applyColorMap

void QuadSet::applyColorMap(const FColorMap& cmap,
			    double given_low, double given_high)
{
  int vsize = m_quads.size();
  if(vsize == 0)
    return;
  
  double a_low_val   = m_low_val  + m_low_val_adjust;
  double a_high_val  = m_high_val - m_high_val_adjust;

  if(given_low != given_high) {
    a_low_val  = given_low;
    a_high_val = given_high;
  }

  double range = (a_high_val - a_low_val);
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

bool QuadSet::addQuadSet(const QuadSet* g_quads)
{
  if(!g_quads)
    return(false);

  int msize       = m_quads.size();
  int gsize       = g_quads->size();
  double g_weight = g_quads->getWeight();

#if 0
  // If this is an empty quadset, just set to the given quadset
  if(msize == 0) {
    for(int i=0; i<gsize; i++)
      m_quads.push_back(g_quads->getQuad(i));
    m_low_val = g_quads->m_low_val;
    m_high_val = g_quads->m_high_val;
    return(true);
  }
#endif

  // If this is an empty quadset, just set to the given quadset
  if(msize == 0) {
    for(int i=0; i<gsize; i++) {
      Quad3D new_quad = g_quads->getQuad(i);
      new_quad.llval *= g_weight;
      new_quad.hlval *= g_weight;
      new_quad.lhval *= g_weight;
      new_quad.hhval *= g_weight;
      m_quads.push_back(new_quad);
    }
    m_low_val  = g_quads->m_low_val * g_weight;
    m_high_val = g_quads->m_high_val * g_weight;
    return(true);
  }

  if(msize != gsize)
    return(false);

  for(int i=0; i<msize; i++) {
    m_quads[i].llval += (g_quads->getQuad(i).llval * g_weight);
    m_quads[i].lhval += (g_quads->getQuad(i).lhval * g_weight);
    m_quads[i].hlval += (g_quads->getQuad(i).hlval * g_weight);
    m_quads[i].hhval += (g_quads->getQuad(i).hhval * g_weight);
  }

  // Recalculate the new global low and high values.
  m_low_val  = m_quads[0].llval;
  m_high_val = m_quads[0].llval;
  for(int i=0; i<msize; i++) {
    if(m_quads[i].llval < m_low_val)   m_low_val  = m_quads[i].llval;
    if(m_quads[i].llval > m_high_val)  m_high_val = m_quads[i].llval;

    if(m_quads[i].lhval < m_low_val)   m_low_val  = m_quads[i].lhval;
    if(m_quads[i].lhval > m_high_val)  m_high_val = m_quads[i].lhval;

    if(m_quads[i].hlval < m_low_val)   m_low_val  = m_quads[i].hlval;
    if(m_quads[i].hlval > m_high_val)  m_high_val = m_quads[i].hlval;

    if(m_quads[i].hhval < m_low_val)   m_low_val  = m_quads[i].hhval;
    if(m_quads[i].hhval > m_high_val)  m_high_val = m_quads[i].hhval;
  }

  return(true);
}


//-------------------------------------------------------------
// Procedure: normalize()

void QuadSet::normalize(double target_base, double target_range)
{
  double existing_range = m_high_val - m_low_val;
  if((existing_range <= 0) || (target_range <= 0))
    return;

  int msize = m_quads.size();
  if(msize == 0)
    return;

  double pct;
  for(int i=0; i<msize; i++) {
    pct = ((m_quads[i].llval - m_low_val) / existing_range);
    m_quads[i].llval = target_base + (pct * target_range);
    pct = ((m_quads[i].hlval - m_low_val) / existing_range);
    m_quads[i].hlval = target_base + (pct * target_range);
    pct = ((m_quads[i].lhval - m_low_val) / existing_range);
    m_quads[i].lhval = target_base + (pct * target_range);
    pct = ((m_quads[i].hhval - m_low_val) / existing_range);
    m_quads[i].hhval = target_base + (pct * target_range);
  }

  m_low_val = target_base;
  m_high_val = target_base + target_range;
}

//-------------------------------------------------------------
// Procedure: setAdjust

void QuadSet::setAdjust(double low_adjust, double high_adjust)
{
  if(low_adjust < 0)
    low_adjust = 0;
  if(high_adjust < 0)
    high_adjust = 0;

  if((m_low_val + low_adjust) >= (m_high_val - high_adjust))
    return;

  m_low_val_adjust = low_adjust;
  m_high_val_adjust = high_adjust;
}



//-------------------------------------------------------------
// Procedure: applyWeight()

void QuadSet::applyWeight()
{
  int msize = m_quads.size();
  for(int i=0; i<msize; i++) {
    m_quads[i].llval *= m_weight;
    m_quads[i].hlval *= m_weight;
    m_quads[i].lhval *= m_weight;
    m_quads[i].hhval *= m_weight;
  }

  m_low_val *= m_weight;
  m_high_val *= m_weight;
}


//-------------------------------------------------------------
// Procedure: print()

void QuadSet::print() const
{
  cout << "QuadSet::print() " << endl;
  cout << "  size: " << m_quads.size() << endl;
  cout << "  lowval: " << m_low_val << endl;
  cout << "  highval: " << m_high_val << endl;
}
