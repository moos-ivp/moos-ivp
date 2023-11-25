/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: QuadSet1D.cpp                                        */
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
#include "QuadSet1D.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

QuadSet1D::QuadSet1D()
{
  m_ipf_priority_wt = 0;
}

//-------------------------------------------------------------
// Procedure: setIvPDomain()

void QuadSet1D::setIvPDomain(IvPDomain ivp_domain)
{
  m_ivp_domain = ivp_domain;
}


//-------------------------------------------------------------
// Procedure: applyIPf

bool QuadSet1D::applyIPF(IvPFunction *ipf, string source)
{
  if(!ipf || (ipf->getDim() != 1))
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

  unsigned int total_pts = m_ivp_domain.getVarPoints(0);

  IvPBox ptbox(1);
  for(unsigned int i=0; i<total_pts; i++) {
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

  return(true);
}


//-------------------------------------------------------------
// Procedure: addQuadSet1D

void QuadSet1D::addQuadSet1D(const QuadSet1D& g_quads)
{
  if(g_quads.isEmptyND())
    return;

  // After this quadset1D becomes non-empty, only accept additions of 
  // other quadset1Ds if their dimensions match.
  if(!isEmptyND())
    return;

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
  
  // If this is an empty quadset1D, just set to the given quadset1D
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
    
    m_ivp_domain = g_quads.getDomain();
    return;
  }
  else {
    for(unsigned int i=0; i<msize; i++) {
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


//-------------------------------------------------------------
// Procedure: isEmptyND()

bool QuadSet1D::isEmptyND() const
{
  return(m_domain_pts.size() == 0);
}


//-------------------------------------------------------------
// Procedure: size1D

unsigned int QuadSet1D::size1D() const
{
  if(m_domain_pts.size() == 0)
    return(0);
  
  return(m_domain_pts[0].size());   
}

//-------------------------------------------------------------
// Procedure: getDomainPts

vector<double> QuadSet1D::getDomainPts(unsigned int index) const
{
  vector<double> rvector;
  if(index >= m_domain_pts.size())
    return(rvector);
  
  return(m_domain_pts[index]);   
}

//-------------------------------------------------------------
// Procedure: getRangeVals

vector<double> QuadSet1D::getRangeVals(unsigned int index) const
{
  vector<double> rvector;
  if(index >= m_range_vals.size())
    return(rvector);

  return(m_range_vals[index]);   
}

//-------------------------------------------------------------
// Procedure: getDomainPtsX

vector<bool> QuadSet1D::getDomainPtsX(unsigned int index) const
{
  vector<bool> rvector;
  if(index >= m_domain_ptsx.size())
    return(rvector);

  return(m_domain_ptsx[index]);   
}

//-------------------------------------------------------------
// Procedure: getRangeValMax

double QuadSet1D::getRangeValMax(unsigned int index) const
{
  if(index >= m_range_val_max.size())
    return(0);

  return(m_range_val_max[index]);   
}

//-------------------------------------------------------------
// Procedure: getDomainIXMax

unsigned int QuadSet1D::getDomainIXMax(unsigned int index) const
{
  if(index >= m_domain_ix_max.size())
    return(0);

  return(m_domain_ix_max[index]);   
}

//-------------------------------------------------------------
// Procedure: getSource

string QuadSet1D::getSource(unsigned int index) const
{
  if(index >= m_sources.size())
    return("");

  return(m_sources[index]);   
}

//-------------------------------------------------------------
// Procedure: print()

void QuadSet1D::print() const
{
  cout << "QuadSet1D::print() " << endl;
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







