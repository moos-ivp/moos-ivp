/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Cache.cpp                                        */
/*    DATE: Jan 12th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "AOF_Cache.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

AOF_Cache::AOF_Cache()
{
  aof = 0; 

  fvals = rvals = bvals = gvals = 0;
  xcnt  = ycnt  = zcnt  = 0; 
  xmin  = ymin  = zmin  = 0;
  xmax  = ymax  = zmax  = 0;

  min_val = max_val = 0;
}

//-------------------------------------------------------------
// Procedure: setAOF

void AOF_Cache::setAOF(AOF *g_aof)
{
  aof = g_aof;

  if(aof)
    m_domain = aof->getDomain();

  // Even if new AOF is NULL or invalid, clear the cache
  clearCache();
  
  // If new AOF is NULL, strange but OK, just return
  if(!aof)
    return;

  int dim = aof->getDim();

  // Cache only handles 2D or 3D AOF's
  if((dim != 2) && (dim != 3))
    return;
  
  // Build method same for 2D or 3D case
  buildCache();

  // Fill the cache now 
  if(dim == 2)
    fillCache2D();
  else
    fillCache3D();
  
}

//-------------------------------------------------------------
// Procedure: clearCache
//   Purpose: Free all memory for holding cache data
//            Reset other critical member variables

void AOF_Cache::clearCache()
{
  if(fvals) {
    for(int i=0; i<xcnt; i++) {
      for(int j=0; j<ycnt; j++)
	delete [] fvals[i][j];
      delete [] fvals[i];
    }
    delete [] fvals;
  }

  if(rvals) {
    for(int i=0; i<xcnt; i++) {
      for(int j=0; j<ycnt; j++)
	delete [] rvals[i][j];
      delete [] rvals[i];
    }
    delete [] rvals;
  }

  if(gvals) {
    for(int i=0; i<xcnt; i++) {
      for(int j=0; j<ycnt; j++)
	delete [] gvals[i][j];
      delete [] gvals[i];
    }
    delete [] gvals;
  }

  if(bvals) {
    for(int i=0; i<xcnt; i++) {
      for(int j=0; j<ycnt; j++)
	delete [] bvals[i][j];
      delete [] bvals[i];
    }
    delete [] bvals;
  }

  fvals = rvals = bvals = gvals = 0;
  xcnt  = ycnt  = zcnt  = 0; 
  xmin  = ymin  = zmin  = 0;
  xmax  = ymax  = zmax  = 0;

  min_val = max_val = 0;
}


//-------------------------------------------------------------
// Procedure: buildCache
//   Purpose: Allocate memory from heap for holding cache info
//            Set critical member vars based on the AOF

void AOF_Cache::buildCache()
{
  if(!aof)
    return;
  IvPDomain domain = aof->getDomain();
  int dim = aof->getDim();
  if((dim < 2) || (dim > 3))
    return;
  
  int i,j,k;
  xmin = 0;
  xmax = domain.getVarPoints(0) - 1;
  ymin = 0;
  ymax = domain.getVarPoints(1) - 1;
  zmin = 0;
  zmax = 0;
  if(dim==3) {
    zmin = 0;
    zmax = domain.getVarPoints(2) - 1;
  }
  xcnt = (xmax - xmin) + 1;
  ycnt = (ymax - ymin) + 1;
  zcnt = (zmax - zmin) + 1;

  // Initialize the fvals cache
  fvals = new double** [xcnt];
  for(i=0; i<xcnt; i++) {
    fvals[i] = new double* [ycnt];
    for(j=0; j<ycnt; j++) {
      fvals[i][j] = new double[zcnt];
      for(k=0; k<zcnt; k++)
	fvals[i][j][k] = 0.0;
    }
  }
  // Initialize the rvals cache
  rvals = new double** [xcnt];
  for(i=0; i<xcnt; i++) {
    rvals[i] = new double* [ycnt];
    for(j=0; j<ycnt; j++) {
      rvals[i][j] = new double[zcnt];
      for(k=0; k<zcnt; k++)
	rvals[i][j][k] = 0.0;
    }
  }
  // Initialize the gvals cache
  gvals = new double** [xcnt];
  for(i=0; i<xcnt; i++) {
    gvals[i] = new double* [ycnt];
    for(j=0; j<ycnt; j++) {
      gvals[i][j] = new double[zcnt];
      for(k=0; k<zcnt; k++)
	gvals[i][j][k] = 0.0;
    }
  }
  // Initialize the bvals cache
  bvals = new double** [xcnt];
  for(i=0; i<xcnt; i++) {
    bvals[i] = new double* [ycnt];
    for(j=0; j<ycnt; j++) {
      bvals[i][j] = new double[zcnt];
      for(k=0; k<zcnt; k++)
	bvals[i][j][k] = 0.0;
    }
  }

}
 
//-------------------------------------------------------------
// Procedure: fillCache2D
//      Note: Evaluate all points in the domain of the AOF and
//            store them in the cache memory.

void AOF_Cache::fillCache2D()
{
  if(!aof) return;

  bool first_val = true;

  IvPBox ebox(2,1);
  int yix = 0;
  int xix = 0;
  int yc = ymin;
  int xc = xmin;
  while(yc <= ymax) {
    xc  = xmin;
    xix = 0;
    ebox.setPTS(1, yc, yc);
    while(xc <= xmax) {
      ebox.setPTS(0, xc, xc);
      double new_fval = this->evalPtBox(&ebox);

      // Update the Min/Max Values
      if(first_val) {
	min_val = new_fval;
	max_val = new_fval;
	first_val = false;
      }
      if(new_fval > max_val)
	max_val = new_fval;
      if(new_fval < min_val)
	min_val = new_fval;

      fvals[xix][yix][0] = new_fval;
      
      xc++;
      xix++;
    }
    yc++;
    yix++;
  }  
}

//-------------------------------------------------------------
// Procedure: fillCache3D
//      Note: Evaluate all points in the domain of the AOF and
//            store them in the cache memory.

void AOF_Cache::fillCache3D()
{
  if(!aof) return;

  bool first_val = true;

  IvPBox ebox(3,1);
  int yix = 0;
  int xix = 0;
  int zix = 0;
  int xc = xmin;
  int yc = ymin;
  int zc = zmin;
  while(zc <= zmax) {
    xc  = xmin;
    yc  = ymin;
    xix = 0;
    yix = 0;
    ebox.setPTS(2, zc, zc);
    while(yc <= ymax) {
      xc  = xmin;
      xix = 0;
      ebox.setPTS(1, yc, yc);
      while(xc <= xmax) {
	ebox.setPTS(0, xc, xc);
	double new_fval = this->evalPtBox(&ebox);
	
	// Update the Min/Max Values
	if(first_val) {
	  min_val = new_fval;
	  max_val = new_fval;
	  first_val = false;
	}
	if(new_fval > max_val)
	  max_val = new_fval;
	if(new_fval < min_val)
	  min_val = new_fval;
	
	fvals[xix][yix][zix] = new_fval;
	xc++;
	xix++;
      }
      yc++;
      yix++;
    }  
    zc++;
    zix++;
  }
}

//-------------------------------------------------------------
// Procedure: applyFColorMap()
//      Note: 

void AOF_Cache::applyFColorMap(FColorMap map)
{
  if(!aof)
    return;
  if(min_val >= max_val) 
    return;

  for(int i=0; i<xcnt; i++) {
    for(int j=0; j<ycnt; j++) {
      for(int k=0; k<zcnt; k++) {
	double pct = (fvals[i][j][k] - min_val) / (max_val - min_val);
      
	rvals[i][j][k] = map.getIRVal(pct);
	gvals[i][j][k] = map.getIGVal(pct);
	bvals[i][j][k] = map.getIBVal(pct);
      }
    }
  }
}


//-------------------------------------------------------------
// Procedure: evalPtBox
//      Note: 

double AOF_Cache::evalPtBox(const IvPBox *gbox)
{
  if(!aof || !gbox) 
    return(0);
  
  unsigned int dim = gbox->getDim();

  vector<double> pvals;
  for(unsigned int d=0; d<dim; d++)
    pvals.push_back(m_domain.getVal(d, gbox->pt(d)));
  double val = aof->evalPoint(pvals);
  if(val == 0)
    return(aof->evalBox(gbox));
  return(val);
}


