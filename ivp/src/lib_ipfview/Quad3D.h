/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Quad3D.h                                             */
/*    DATE: Jan 12th 2006                                        */
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
  ~Quad3D() {}

  bool contains(double x, double y) {
    return((x>=xl)&&(x<=xh)&&(y>=yl)&&(y<=yh));
  }

  double getAvgVal() {return((llval+hlval+hhval+lhval)/4);}

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




