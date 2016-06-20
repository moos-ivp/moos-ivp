/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPF_Utils.cpp                                        */
/*    DATE: June 17th 2006                                       */
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
#include "MBUtils.h"
#include "IPF_Utils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: buildQuadSetFromIPF

QuadSet buildQuadSetFromIPF(IvPFunction *ipf)
{
  QuadSet null_quadset;
  if(!ipf)
    return(null_quadset);

  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();
  int dim = ivp_domain.size();
  
  bool crs_spd_ipf = false;
  if(ivp_domain.hasDomain("course") && ivp_domain.hasDomain("speed"))
    crs_spd_ipf = true;
  
  if(dim == 1)
    return(buildQuadSet1DFromIPF(ipf, "hello_source"));
  else if((ipf->getDim() == 2) && crs_spd_ipf)
    return(buildQuadSet2DHSFromIPF(ipf));
  else if(ipf->getDim() == 2)
    return(buildQuadSet2DFromIPF(ipf));

  return(null_quadset);
}

//-------------------------------------------------------------
// Procedure: buildQuadSet1DFromIPF

QuadSet buildQuadSet1DFromIPF(IvPFunction *ipf, string source)
{
  QuadSet null_quadset;
  return(null_quadset);
}


//-------------------------------------------------------------
// Procedure: buildQuadSet2DHSFromIPF
//      Note: For functions defined only over course or speed, we rely
//            on the caller to "expand" the function before calling.

QuadSet buildQuadSet2DHSFromIPF(IvPFunction *ipf)
{
  QuadSet null_quadset;

  //===========================================================
  // Part 1: Sanity checks
  if(!ipf || (ipf->getPDMap() == 0))
    return(null_quadset);
  
  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();
  
  if(!ivp_domain.hasDomain("course") || !ivp_domain.hasDomain("speed"))
    return(null_quadset);
    
  //===========================================================
  // Part 2: Build the cache of values
  int crs_ix = ivp_domain.getIndex("course");
  int spd_ix = ivp_domain.getIndex("speed");
  unsigned int crs_pts = ivp_domain.getVarPoints("course");
  unsigned int spd_pts = ivp_domain.getVarPoints("speed");

  // Create cache to hold the sample results
  vector<vector<double> > vals;

  IvPBox sbox(ivp_domain.size());
  for(unsigned int i=0; i<crs_pts; i++) {
    vector<double> ivector;
    sbox.setPTS(crs_ix, i, i);
    for(unsigned int j=0; j<spd_pts; j++) {
      sbox.setPTS(spd_ix, j, j);
      double pval = ipf->getPDMap()->evalPoint(&sbox);
      ivector.push_back(pval);
    }
    vals.push_back(ivector);
  }

  //===========================================================
  // Part 3: Build the Quads from the Cache

  vector<Quad3D> quads = buildQuadsFromCache(vals);

  //===========================================================
  // Part 4: Assemble the QuadSet

  QuadSet quadset;
  quadset.setIvPDomain(ivp_domain);

  for(unsigned int i=0; i<quads.size(); i++) 
    quadset.addQuad3D(quads[i]);
  
  quadset.resetMinMaxVals();
  return(quadset);
}


//-------------------------------------------------------------
// Procedure: buildQuadsFromCache()

vector<Quad3D> buildQuadsFromCache(const vector<vector<double> >& vals)
{
  vector<Quad3D> rvector;
  // Sanity checks
  if(vals.size() == 0)
    return(rvector);
  
  unsigned int crs_pts = vals.size();
  unsigned int spd_pts = vals[0].size();

  // Build the primary quads 
  for(unsigned int i=0; i<(crs_pts-1); i++) {
    for(unsigned int j=0; j<(spd_pts-1); j++) {
      Quad3D new_quad;
      new_quad.setXL(i);
      new_quad.setXH(i+1);
      new_quad.setYL(j);
      new_quad.setYH(j+1);
      new_quad.setLLZ(vals[i][j]);
      new_quad.setLHZ(vals[i][j+1]);
      new_quad.setHLZ(vals[i+1][j]);
      new_quad.setHHZ(vals[i+1][j+1]);

      new_quad.xpts = crs_pts;
      new_quad.ypts = spd_pts;

      rvector.push_back(new_quad);
    }
  }

  bool wrap = true;
  if(wrap) {
    // Add  "bridge" quads to wrap around 359-0
    int top_crs_ix = crs_pts-1;
    for(unsigned int j=0; j<(spd_pts-1); j++) {
      Quad3D new_quad;
      new_quad.setXL(top_crs_ix-1);  // usually 359
      new_quad.setXH(0);
      new_quad.setYL(j);
      new_quad.setYH(j+1);
      new_quad.setLLZ(vals[top_crs_ix][j]);
      new_quad.setLHZ(vals[top_crs_ix][j+1]);
      new_quad.setHLZ(vals[0][j]);
      new_quad.setHHZ(vals[0][j+1]);
      
      new_quad.xpts = crs_pts;
      new_quad.ypts = spd_pts;
      
      rvector.push_back(new_quad);
    }
  }

  return(rvector);
}


//-------------------------------------------------------------
// Procedure: buildQuadSet2DFromIPF
//      Note: For functions defined only over course or speed, we rely
//            on the caller to "expand" the function before calling.

QuadSet buildQuadSet2DFromIPF(IvPFunction *ipf)
{
  QuadSet null_quadset;

  // Sanity checks
  if(!ipf)
    return(null_quadset);
  PDMap *pdmap = ipf->getPDMap();
  if(!pdmap)
    return(null_quadset);

  IvPDomain ivp_domain = pdmap->getDomain();

  unsigned int xpts = ivp_domain.getVarPoints(0);  // rid?
  unsigned int ypts = ivp_domain.getVarPoints(1);  // rid?
  if((xpts < 2) || (ypts < 2))                     // rid?
    return(null_quadset);                          // rid?

  QuadSet quadset;
  quadset.setIvPDomain(ivp_domain);

  for(unsigned int i=0; i<pdmap->size(); i++) {
    const IvPBox* box = pdmap->bx(i);

    Quad3D quad;

    int xl = box->pt(0,0);
    int xh = box->pt(0,1);
    int yl = box->pt(1,0);
    int yh = box->pt(1,1);
    
    quad.setXL(xl);
    quad.setXH(xh);
    quad.setYL(yl);
    quad.setYH(yh);
    quad.xpts  = xpts;  // rid?
    quad.ypts  = ypts;  // rid?

    int degree = box->getDegree();
    
    if(degree != 1)
      return(null_quadset);

    IvPBox ebox(2,1);
    
    ebox.setPTS(0, xl, xl);
    ebox.setPTS(1, yl, yl);
    quad.setLLZ(box->ptVal(&ebox));

    ebox.setPTS(0, xh, xh);
    quad.setHLZ(box->ptVal(&ebox));
    
    ebox.setPTS(1, yh, yh);
    quad.setHHZ(box->ptVal(&ebox));
    
    ebox.setPTS(0, xl, xl);
    quad.setLHZ(box->ptVal(&ebox));

    quadset.addQuad3D(quad);
  }

  quadset.resetMinMaxVals();

  return(quadset);
}


