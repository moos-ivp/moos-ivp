/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Cache.h                                          */
/*    DATE: Jan 12th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef AOF_CACHE_HEADER
#define AOF_CACHE_HEADER

#include <vector>
#include "AOF.h"
#include "FColorMap.h"

class AOF_Cache {
public:
  AOF_Cache();
  ~AOF_Cache() {clearCache();};

  void setAOF(AOF*);
  void applyFColorMap(FColorMap);

  AOF* getAOF() {return(aof);};

  double getFVal(int i, int j, int k=0) {return(fvals[i][j][k]);};
  double getRVal(int i, int j, int k=0) {return(rvals[i][j][k]);};
  double getGVal(int i, int j, int k=0) {return(gvals[i][j][k]);};
  double getBVal(int i, int j, int k=0) {return(bvals[i][j][k]);};

  int getXMin() {return(xmin);};
  int getXMax() {return(xmax);};
  int getYMin() {return(ymin);};
  int getYMax() {return(ymax);};
  int getZMin() {return(zmin);};
  int getZMax() {return(zmax);};

protected:
  void clearCache();
  void buildCache();
  void fillCache2D();
  void fillCache3D();
  void applyFCMap();

  double evalPtBox(const IvPBox*);

protected:
  
  AOF*      aof;
  IvPDomain m_domain;

  double ***fvals;
  double ***rvals;
  double ***gvals;
  double ***bvals;
  int    xcnt;
  int    ycnt;
  int    zcnt;

  int    xmin;
  int    xmax;
  int    ymin; 
  int    ymax;
  int    zmin;
  int    zmax;

  double min_val;
  double max_val;
};
#endif







