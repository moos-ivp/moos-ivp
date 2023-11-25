/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
  m_minpt_val = 0;
  m_maxpt_val = 0;
  m_max_x_qix = 0;
  m_max_y_qix = 0;
  m_dense = false;
}

//-------------------------------------------------------------
// Procedure: getQuad()

Quad3D QuadSet::getQuad(unsigned int ix) const
{
  if(ix < m_quads.size())
    return(m_quads[ix]);
  
  Quad3D null_quad;
  return(null_quad);
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
  if(g_quads.size() == 0)
    return;

  unsigned int msize  = size();
  unsigned int gsize  = g_quads.size();
    
  // If this is an empty quadset, just set to the given quadset
  if(msize == 0) {
    for(unsigned int i=0; i<gsize; i++)
      m_quads.push_back(g_quads.getQuad(i));
    resetMinMaxVals();
    m_ivp_domain      = g_quads.getDomain();
    return;
  }

  // If the two quadsets are of different size, it may be due to the
  // given quadset being defined not over heading/speed.
  if(msize != gsize)
    return;

  for(unsigned int i=0; i<msize; i++) {
    m_quads[i].addLLZ(g_quads.getQuad(i).getLLZ());
    m_quads[i].addLHZ(g_quads.getQuad(i).getLHZ());
    m_quads[i].addHLZ(g_quads.getQuad(i).getHLZ());
    m_quads[i].addHHZ(g_quads.getQuad(i).getHHZ());
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
// Procedure: interpolate()

void QuadSet::interpolate(double xdelta)
{
  for(unsigned int i=0; i<m_quads.size(); i++) 
    m_quads[i].interpolate(xdelta);
}

//-------------------------------------------------------------
// Procedure: applyColorIntensity()

void QuadSet::applyColorIntensity(double intensity)
{
  for(unsigned int i=0; i<m_quads.size(); i++) 
    m_quads[i].applyColorIntensity(intensity);
}

//-------------------------------------------------------------
// Procedure: applyScale()

void QuadSet::applyScale(double scale)
{
  for(unsigned int i=0; i<m_quads.size(); i++) 
    m_quads[i].applyScale(scale);
}

//-------------------------------------------------------------
// Procedure: applyBase()

void QuadSet::applyBase(double base_delta)
{
  for(unsigned int i=0; i<m_quads.size(); i++) 
    m_quads[i].applyBase(base_delta);
}

//-------------------------------------------------------------
// Procedure: setBase()

void QuadSet::setBase(double base)
{
  double base_delta = base - m_minpt_val;
  for(unsigned int i=0; i<m_quads.size(); i++) 
    m_quads[i].applyBase(base_delta);

  m_maxpt_val += base_delta;
  m_minpt_val += base_delta;
}

//-------------------------------------------------------------
// Procedure: applyTranslation()

void QuadSet::applyTranslation(double xdist, double ydist)
{
  for(unsigned int i=0; i<m_quads.size(); i++) 
    m_quads[i].applyTranslation(xdist, ydist);
}

//-------------------------------------------------------------
// Procedure: applyTranslation()
//   Purpose: Automatically determine the translation distances
//            based on the size of the IvPDomain

void QuadSet::applyTranslation()
{
  if(m_ivp_domain.size() != 2)
    return;

  double xpts = (double)(m_ivp_domain.getVarPoints(0));
  double ypts = (double)(m_ivp_domain.getVarPoints(1));

  double xdist = -(xpts/2);
  double ydist = -(ypts/2);
  
  for(unsigned int i=0; i<m_quads.size(); i++) 
    m_quads[i].applyTranslation(xdist, ydist);
}

//-------------------------------------------------------------
// Procedure: applyPolar()

void QuadSet::applyPolar(double rad_extra, int polar_dim)
{
  if((polar_dim != 1) && (polar_dim != 2))
    return;
  if(m_ivp_domain.size() != 2)
    return;

  unsigned int pts = m_ivp_domain.getVarPoints(0);
  if(polar_dim == 2)
    pts = m_ivp_domain.getVarPoints(1);
  
  for(unsigned int i=0; i<m_quads.size(); i++) 
    m_quads[i].applyPolar(rad_extra, polar_dim, pts);
}


//-------------------------------------------------------------
// Procedure: getMaxPoint
//   Purpose: Calculate and return the value for the given decision
//            variable, at the peak of the objective function.
//            We cache the value of the decision variable in terms
//            of its index in the IvP domain, but it just needs
//            to be converted here from the index to the native
//            units of the decision variable.
//            If the decision variable is not part of the IvPDomain
//            or if the index is beyond the number of points in the
//            the IvPDomain for that variable, then zero is returned.

double QuadSet::getMaxPoint(string varname) const
{
  if(!m_ivp_domain.hasDomain(varname))
    return(0);

  unsigned int dom_ix = m_ivp_domain.getIndex(varname);
  if(dom_ix == 0) {
    if(m_max_x_qix < m_ivp_domain.getVarPoints(dom_ix))
      return(m_ivp_domain.getVal(dom_ix, m_max_x_qix));
    return(0);
  }
  else if(dom_ix == 1) {
    if(m_max_y_qix < m_ivp_domain.getVarPoints(dom_ix))
      return(m_ivp_domain.getVal(dom_ix, m_max_y_qix));
    return(0);
  }
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: getMaxPointQIX

unsigned int QuadSet::getMaxPointQIX(string varname) const
{
  if(varname == "course")
    return(m_max_x_qix);
  else if(varname == "speed")
    return(m_max_y_qix);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: print()

void QuadSet::print() const
{
  cout << "QuadSet::print() " << endl;
  cout << "  2Dsize: " << m_quads.size() << endl;
  cout << "    minpt_val: " << m_minpt_val << endl;
  cout << "    maxpt_val: " << m_maxpt_val << endl;
  cout << "    maxpt_hdg: " << getMaxPoint("course") << endl;
  cout << "    maxpt_spd: " << getMaxPoint("speed")  << endl;
}


//-------------------------------------------------------------
// Procedure: resetMinMaxVals()

void QuadSet::resetMinMaxVals()
{
  unsigned int i, msize  = m_quads.size();
  if(msize == 0)
    return;
  
  m_max_x_qix = 0;
  m_max_y_qix = 0;

  // Recalculate the new global low and high values.
  m_minpt_val = m_quads[0].getLLZ();
  m_maxpt_val = m_quads[0].getLLZ();

  for(i=0; i<msize; i++) {    
    if(m_quads[i].getLLZ() < m_minpt_val)  //------- (L,L)
      m_minpt_val  = m_quads[i].getLLZ();
    if(m_quads[i].getLLZ() > m_maxpt_val) {
      m_maxpt_val = m_quads[i].getLLZ();
      m_max_x_qix = m_quads[i].getLLX();
      m_max_y_qix = m_quads[i].getLLY();
    }

    if(m_quads[i].getLHZ() < m_minpt_val)  //------- (L,H)  
      m_minpt_val  = m_quads[i].getLHZ();
    if(m_quads[i].getLHZ() > m_maxpt_val) {  
      m_maxpt_val = m_quads[i].getLHZ();
      m_max_x_qix = m_quads[i].getLHX();
      m_max_y_qix = m_quads[i].getLHY();
    }

    if(m_quads[i].getHLZ() < m_minpt_val)  //------- (H,L)  
      m_minpt_val  = m_quads[i].getHLZ();
    if(m_quads[i].getHLZ() > m_maxpt_val) {
      m_maxpt_val = m_quads[i].getHLZ();
      m_max_x_qix = m_quads[i].getHLX();
      m_max_y_qix = m_quads[i].getHLY();
    }

    if(m_quads[i].getHHZ() < m_minpt_val) //------- (H,H)    
      m_minpt_val  = m_quads[i].getHHZ();
    if(m_quads[i].getHHZ() > m_maxpt_val) {
      m_maxpt_val = m_quads[i].getHHZ();
      m_max_x_qix = m_quads[i].getHHX();
      m_max_y_qix = m_quads[i].getHHY();
    }
  }
}






