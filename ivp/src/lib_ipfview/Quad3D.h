/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: Quad3D.h                                            */
/*    DATE: Jan 12th 2006                                       */
/****************************************************************/

#ifndef QUAD3D_HEADER
#define QUAD3D_HEADER

class Quad3D {
public:
  Quad3D() {
    xl=0; xh=0; yl=0; yh=0; llval=0; hlval=0; hhval=0; lhval=0;
    llval_r=0; llval_g=0; llval_b=0;
    hlval_r=0; hlval_g=0; hlval_b=0;
    hhval_r=0; hhval_g=0; hhval_b=0;
    lhval_r=0; lhval_g=0; lhval_b=0;
    base=0; scale=1.0; lines=true; xpts=0; ypts=0;
  }
  ~Quad3D() {};

public:  
  double xl;
  double xh;
  double yl;
  double yh;
  double llval;
  double hlval;
  double hhval;
  double lhval;
  double llval_r;
  double llval_g;
  double llval_b;
  double hlval_r;
  double hlval_g;
  double hlval_b;
  double hhval_r;
  double hhval_g;
  double hhval_b;
  double lhval_r;
  double lhval_g;
  double lhval_b;
  double base;
  double scale;
  bool   lines;
  int    xpts;
  int    ypts;
};
#endif
