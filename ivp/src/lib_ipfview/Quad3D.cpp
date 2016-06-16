/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Quad3D.cpp                                           */
/*    DATE: June 13th 2016                                       */
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
#include "GeomUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

Quad3D::Quad3D()
{
  xl=0;
  xh=0;
  yl=0;
  yh=0;

  // x position for each of four vertices
  ll_xval=0;
  hl_xval=0;
  hh_xval=0;
  lh_xval=0;
  
  // y position for each of four vertices
  ll_yval=0;
  hl_yval=0;
  hh_yval=0;
  lh_yval=0;
  
  // z position (height) for each of four vertices
  ll_hgt=0;
  hl_hgt=0;
  hh_hgt=0;
  lh_hgt=0;

  // red component color (of RGB) for each of four vertices
  ll_red=0;
  hl_red=0;
  hh_red=0;
  lh_red=0;

  // green component color (of RGB) for each of four vertices
  ll_grn=0;
  hl_grn=0;
  hh_grn=0;
  lh_grn=0;

  // blue component color (of RGB) for each of four vertices
  ll_blu=0;
  hl_blu=0;
  hh_blu=0;
  lh_blu=0;

  xpts=0;
  ypts=0;
}

//-------------------------------------------------------------
// Procedure: applyPolar
//   Purpose: Apply the polar translation from raw xl,..yh to cached x,y
//            It's assumed that this cannot be changed back once it's
//            done. If a user wants to toggle, it will involve the
//            rebuilding of the quadset from the IvP function. Otherwise
//            an additional four member variables would need to be
//            added to hold and preserve the "original" xl,...yh vals.

void Quad3D::applyPolar(double rad_extra, int polar_dim, int pts)
{
  // Sanity checks
  if((rad_extra <= 0) || (polar_dim < 0) || (polar_dim > 2))
    return;
    
  if(polar_dim == 0) {
    ll_xval = xl;
    hl_xval = xh;
    hh_xval = xh;
    lh_xval = xl;

    ll_yval = yl;
    hl_yval = yl;
    hh_yval = yh;
    lh_yval = yh;
  }
  if(polar_dim == 1) {
    double delta = 360.0 / pts;
    projectPoint(xl*delta, yl*rad_extra, 0, 0, ll_xval, ll_yval);
    projectPoint(xh*delta, yl*rad_extra, 0, 0, hl_xval, hl_yval);
    projectPoint(xh*delta, yh*rad_extra, 0, 0, hh_xval, hh_yval);
    projectPoint(xl*delta, yh*rad_extra, 0, 0, lh_xval, lh_yval);
  }
  else {
    double delta = 360.0 / pts;
    projectPoint(yl*delta, xl*rad_extra, 0, 0, ll_yval, ll_xval);
    projectPoint(yh*delta, xl*rad_extra, 0, 0, hl_yval, hl_xval);
    projectPoint(yh*delta, xh*rad_extra, 0, 0, hh_yval, hh_xval);
    projectPoint(yl*delta, xh*rad_extra, 0, 0, lh_yval, lh_xval);
  }      
}

//-------------------------------------------------------------
// Procedure: applyColorIntensity()

void Quad3D::applyColorIntensity(double intensity)
{
  ll_red *= intensity;
  hl_red *= intensity;
  hh_red *= intensity;
  lh_red *= intensity;

  ll_grn *= intensity;
  hl_grn *= intensity;
  hh_grn *= intensity;
  lh_grn *= intensity;

  ll_blu *= intensity;
  hl_blu *= intensity;
  hh_blu *= intensity;
  lh_blu *= intensity;
}

//-------------------------------------------------------------
// Procedure: applyScale()

void Quad3D::applyScale(double given_scale)
{
  ll_hgt *= given_scale;
  hl_hgt *= given_scale;
  hh_hgt *= given_scale;
  lh_hgt *= given_scale;
}

//-------------------------------------------------------------
// Procedure: applyBase()

void Quad3D::applyBase(double given_base)
{
  ll_hgt += given_base;
  hl_hgt += given_base;
  hh_hgt += given_base;
  lh_hgt += given_base;
}

//-------------------------------------------------------------
// Procedure: applyTranslation

void Quad3D::applyTranslation(double xval, double yval)
{
  xl += xval;
  xh += xval;
  yl += yval;
  yh += yval;

  ll_xval += xval;
  hl_xval += xval;
  hh_xval += xval;
  lh_xval += xval;

  ll_yval += xval;
  hl_yval += xval;
  hh_yval += xval;
  lh_yval += xval;
}


