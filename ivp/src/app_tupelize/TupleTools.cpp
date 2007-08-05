/***********************************************************/
/*    NAME: Michael Benjamin                               */
/*    FILE: TupleTools.cpp                                 */
/*    DATE: Sep 15 2005                                    */
/***********************************************************/

#include "TupleTools.h"

using namespace std;


//----------------------------------------------------------
// Procedure: tupelize

vector<Tuple3D> tupelize(const IvPBox *box) 
{
  vector<Tuple3D> rvector;

  double wt0 = box->wt(0);
  double wt1 = box->wt(1);
  double wt2 = box->wt(2);
  
  double xl  = (double)(box->pt(0,0));
  double xh  = (double)(box->pt(0,1));
  double yl  = (double)(box->pt(1,0));
  double yh  = (double)(box->pt(1,1));
  
  double z0 = (xl * wt0) + (yl * wt1) + wt2;
  double z1 = (xl * wt0) + (yh * wt1) + wt2;
  double z2 = (xh * wt0) + (yl * wt1) + wt2;
  double z3 = (xh * wt0) + (yh * wt1) + wt2;
 
  Tuple3D tuple0(xl,yl,z0);
  Tuple3D tuple1(xl,yh,z1);
  Tuple3D tuple2(xh,yl,z2);
  Tuple3D tuple3(xh,yh,z3);
  
  rvector.push_back(tuple0);
  rvector.push_back(tuple1);
  rvector.push_back(tuple2);
  rvector.push_back(tuple3);

  return(rvector);
}

//----------------------------------------------------------
// Procedure: tupelize
//      Note: Create tuples where the x,y values are in the 
//            scope if the IvPDomain rather than the raw 
//            indices of the Box.

vector<Tuple3D> tupelize(const IvPBox *box, IvPDomain domain)
{
  vector<Tuple3D> rvector;

  double wt0 = box->wt(0);
  double wt1 = box->wt(1);
  double wt2 = box->wt(2);
  
  double xl  = (double)(box->pt(0,0));
  double xh  = (double)(box->pt(0,1));
  double yl  = (double)(box->pt(1,0));
  double yh  = (double)(box->pt(1,1));
  
  double z0 = (xl * wt0) + (yl * wt1) + wt2;
  double z1 = (xl * wt0) + (yh * wt1) + wt2;
  double z2 = (xh * wt0) + (yl * wt1) + wt2;
  double z3 = (xh * wt0) + (yh * wt1) + wt2;
 
  double dxl, dxh, dyl, dyh;
  domain.getVal(0, box->pt(0,0), dxl);
  domain.getVal(0, box->pt(0,1), dxh);
  domain.getVal(1, box->pt(1,0), dyl);
  domain.getVal(1, box->pt(1,1), dyh);

  Tuple3D tuple0(dxl,dyl,z0);
  Tuple3D tuple1(dxl,dyh,z1);
  Tuple3D tuple2(dxh,dyl,z2);
  Tuple3D tuple3(dxh,dyh,z3);
  
  rvector.push_back(tuple0);
  rvector.push_back(tuple1);
  rvector.push_back(tuple2);
  rvector.push_back(tuple3);

  return(rvector);
}


//----------------------------------------------------------
// Procedure: tupelize

vector<Tuple3D> tupelize(PDMap *pdmap) 
{
  vector<Tuple3D> rvector;
  vector<Tuple3D> ivector;

  int psize = pdmap->size();
  for(int i=0; i<psize; i++) {
    const IvPBox *box = pdmap->bx(i);
    ivector = tupelize(box);
    for(int j=0; j<ivector.size(); j++)
      rvector.push_back(ivector[j]);
  }
  return(rvector);
}

//----------------------------------------------------------
// Procedure: tupelize

vector<Tuple3D> tupelize(PDMap *pdmap, IvPDomain domain) 
{
  vector<Tuple3D> rvector;
  vector<Tuple3D> ivector;

  int psize = pdmap->size();
  for(int i=0; i<psize; i++) {
    const IvPBox *box = pdmap->bx(i);
    ivector = tupelize(box, domain);
    for(int j=0; j<ivector.size(); j++)
      rvector.push_back(ivector[j]);
  }
  return(rvector);
}
