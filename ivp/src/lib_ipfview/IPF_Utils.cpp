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

  // Sanity checks
  if(!ipf)
    return(null_quadset);
  PDMap *pdmap = ipf->getPDMap();
  if(!pdmap)
    return(null_quadset);
  
  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();
  
  bool wrap = true;

  unsigned int crs_pts = ivp_domain.getVarPoints("course");
  unsigned int spd_pts = ivp_domain.getVarPoints("speed");
  if((crs_pts < 2) || (spd_pts < 2))
    return(null_quadset);

  QuadSet quadset;
  quadset.setIvPDomain(ivp_domain);
  
  // Create memory to hold the sample results
  unsigned int i,j;
  double **vals = new double*[crs_pts];
  for(i=0; i<crs_pts; i++) {
    vals[i] = new double[spd_pts];
    for(j=0; j<spd_pts; j++)
      vals[i][j] = 0;
  }

  // Create the "sample" box 
  IvPBox sbox(ivp_domain.size());
  
  int crs_ix = ivp_domain.getIndex("course");
  int spd_ix = ivp_domain.getIndex("speed");

  for(i=0; i<crs_pts; i++) {
    sbox.setPTS(crs_ix, i, i);
    for(j=0; j<spd_pts; j++) {
      sbox.setPTS(spd_ix, j, j);
      double pval = ipf->getPDMap()->evalPoint(&sbox);
      //pval = snapToStep(pval, m_snap_val);
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

      new_quad.xpts = crs_pts;
      new_quad.ypts = spd_pts;

      quadset.addQuad3D(new_quad);
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
      
      new_quad.xpts = crs_pts;
      new_quad.ypts = spd_pts;
      
      quadset.addQuad3D(new_quad);
    }
  }

  // Added apr 15th 2015 delete tmp memory
  for(i=0; i<crs_pts; i++) 
    delete [] vals[i];
  delete [] vals;

  quadset.resetMinMaxVals();
  return(quadset);
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


