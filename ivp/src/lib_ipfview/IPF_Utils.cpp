/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

QuadSet buildQuadSetFromIPF(IvPFunction *ipf, bool dense)
{
  // Part 1: Sanity checks
  QuadSet null_quadset;
  if(!ipf)
    return(null_quadset);

  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();
  int dim = ivp_domain.size();
  if((dim != 2))
    return(null_quadset);

  if(dense)
    return(buildQuadSetDense2DFromIPF(ipf));

  return(buildQuadSet2DFromIPF(ipf));
}

//-------------------------------------------------------------
// Procedure: buildQuadSet1DFromIPF

QuadSet1D buildQuadSet1DFromIPF(IvPFunction *ipf, string source)
{
  QuadSet1D null_quadset;
  if(!ipf)
    return(null_quadset);

  QuadSet1D quadset;
  quadset.applyIPF(ipf, source);
  
  return(quadset);
}


//-------------------------------------------------------------
// Procedure: buildQuadSetDense2DFromIPF
//      Note: If the function is defined over course and speed, we
//            ensure that the first dimension (0) is course.

QuadSet buildQuadSetDense2DFromIPF(IvPFunction *ipf)
{
  QuadSet null_quadset;

  //===========================================================
  // Part 1: Sanity checks
  if(!ipf || (ipf->getPDMap() == 0))
    return(null_quadset);
  
  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();
  if(ivp_domain.size() != 2)
    return(null_quadset);

  //===========================================================
  // Part 2: Build the cache of values
  int x_ix = 0;
  int y_ix = 1;
  unsigned int x_pts = ivp_domain.getVarPoints(0);
  unsigned int y_pts = ivp_domain.getVarPoints(1);

  // If this function is over course and speed, then make sure dimension 
  // zero, aka the x axis, is course and the other dimension is speed.
  if(ivp_domain.hasDomain("course") && ivp_domain.hasDomain("speed")) {
    x_ix = ivp_domain.getIndex("course");
    y_ix = ivp_domain.getIndex("speed");
    x_pts = ivp_domain.getVarPoints("course");
    y_pts = ivp_domain.getVarPoints("speed");
  }

  // Create cache to hold the sample results
  vector<vector<double> > vals;

  IvPBox sbox(2);
  for(unsigned int i=0; i<x_pts; i++) {
    vector<double> ivector;
    sbox.setPTS(x_ix, i, i);
    for(unsigned int j=0; j<y_pts; j++) {
      sbox.setPTS(y_ix, j, j);
      // double pval = ipf->getPDMap()->evalPoint(&sbox); mikerb
      double pval = ipf->getPDMap()->evalPoint(&sbox) * ipf->getPWT();
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
  //quadset.markDense(true);
  return(quadset);
}

//-------------------------------------------------------------
// Procedure: buildQuadSetDense2DFromAOF
//      Note: If the function is defined over course and speed, we
//            ensure that the first dimension (0) is course.

QuadSet buildQuadSetDense2DFromAOF(AOF *aof, unsigned int patch)
{
  QuadSet null_quadset;

  //===========================================================
  // Part 1: Sanity checks
  if(!aof || (aof->getDomain().size() == 0))
    return(null_quadset);
  
  IvPDomain ivp_domain = aof->getDomain();
  if(ivp_domain.size() != 2)
    return(null_quadset);

  //===========================================================
  // Part 2: Build the cache of values
  int x_ix = 0;
  int y_ix = 1;
  unsigned int x_pts = ivp_domain.getVarPoints(0);
  unsigned int y_pts = ivp_domain.getVarPoints(1);

  // If this function is over course and speed, then make sure dimension 
  // zero, aka the x axis, is course and the other dimension is speed.
  if(ivp_domain.hasDomain("course") && ivp_domain.hasDomain("speed")) {
    x_ix = ivp_domain.getIndex("course");
    y_ix = ivp_domain.getIndex("speed");
    x_pts = ivp_domain.getVarPoints("course");
    y_pts = ivp_domain.getVarPoints("speed");
  }

  // Create cache to hold the sample results
  vector<vector<double> > vals;

  IvPBox sbox(2);
  for(unsigned int i=0; i<x_pts; i++) {
    vector<double> ivector;
    sbox.setPTS(x_ix, i, i);
    for(unsigned int j=0; j<y_pts; j++) {
      sbox.setPTS(y_ix, j, j);
      double pval = aof->evalBox(&sbox);
      ivector.push_back(pval);
    }
    vals.push_back(ivector);
  }

  //===========================================================
  // Part 3: Build the Quads from the Cache

  vector<Quad3D> quads = buildQuadsFromCache(vals, patch);

  //===========================================================
  // Part 4: Assemble the QuadSet

  QuadSet quadset;
  quadset.setIvPDomain(ivp_domain);

  for(unsigned int i=0; i<quads.size(); i++) 
    quadset.addQuad3D(quads[i]);
  
  quadset.resetMinMaxVals();
  //quadset.markDense(true);
  return(quadset);
}

//-------------------------------------------------------------
// Procedure: buildQuadsFromCache()

vector<Quad3D> buildQuadsFromCache(const vector<vector<double> >& vals,
				   unsigned int patch_size)
{
  vector<Quad3D> rvector;
  // Sanity checks
  if(vals.size() == 0)
    return(rvector);
  
  unsigned int crs_pts = vals.size();
  unsigned int spd_pts = vals[0].size();

  unsigned int p = patch_size;
  // Build the primary quads 
  for(unsigned int i=0; i<(crs_pts-1); i+=p  ) {
    for(unsigned int j=0; j<(spd_pts-1); j+=p) {
      Quad3D new_quad;
      //new_quad.setXL(i);
      //new_quad.setXH(i+1);
      //new_quad.setYL(j);
      //new_quad.setYH(j+1);

      unsigned int xl = i;
      unsigned int xh = i+p;
      unsigned int yl = j;
      unsigned int yh = j+p;

      if(xh > (crs_pts-2))
	xh = crs_pts-2;
      if(yh > (spd_pts-2))
	yh = spd_pts-2;
      
      new_quad.setLLX(xl);
      new_quad.setHLX(xh);
      new_quad.setHHX(xh);
      new_quad.setLHX(xl);
      
      new_quad.setLLY(yl);
      new_quad.setHLY(yl);
      new_quad.setHHY(yh);
      new_quad.setLHY(yh);
      
      new_quad.setLLZ(vals[xl][yl]);
      new_quad.setLHZ(vals[xl][yh]);
      new_quad.setHLZ(vals[xh][yl]);
      new_quad.setHHZ(vals[xh][yh]);

      rvector.push_back(new_quad);
    }
  }

  bool wrap = true;
  if(wrap) {
    // Add  "bridge" quads to wrap around 359-0
    int top_crs_ix = crs_pts-1;
    for(unsigned int j=0; j<(spd_pts-1); j++) {
      Quad3D new_quad;
      //new_quad.setXL(top_crs_ix-1);  // usually 359
      //new_quad.setXH(0);
      //new_quad.setYL(j);
      //new_quad.setYH(j+1);

      new_quad.setLLX(top_crs_ix-1);
      new_quad.setHLX(0);
      new_quad.setHHX(0);
      new_quad.setLHX(top_crs_ix-1);
      
      new_quad.setLLY(j);
      new_quad.setHLY(j);
      new_quad.setHHY(j+1);
      new_quad.setLHY(j+1);
      
      new_quad.setLLZ(vals[top_crs_ix][j]);
      new_quad.setLHZ(vals[top_crs_ix][j+1]);
      new_quad.setHLZ(vals[0][j]);
      new_quad.setHHZ(vals[0][j+1]);
      
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

  QuadSet quadset;
  quadset.setIvPDomain(ivp_domain);

  for(int i=0; i<pdmap->size(); i++) {
    const IvPBox* box = pdmap->bx(i);

    Quad3D quad;

    int xl = box->pt(0,0);
    int xh = box->pt(0,1);
    int yl = box->pt(1,0);
    int yh = box->pt(1,1);
    
    quad.setLLX(xl);
    quad.setHLX(xh);
    quad.setHHX(xh);
    quad.setLHX(xl);

    quad.setLLY(yl);
    quad.setHLY(yl);
    quad.setHHY(yh);
    quad.setLHY(yh);

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
  //quadset.markDense(false);

  return(quadset);
}







