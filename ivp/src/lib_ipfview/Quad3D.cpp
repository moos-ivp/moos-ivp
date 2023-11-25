/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

  //xpts=0;
  //ypts=0;
}

//-------------------------------------------------------------
// Procedure: expand()

void Quad3D::expand(double val)
{
  if(val > 1)
    val = 1;
  if(val < -1)
    val = -1;

  ll_xval += val;
  hl_xval += val;
  hh_xval += val;
  lh_xval += val;

  ll_yval += val;
  hl_yval += val;
  hh_yval += val;
  lh_yval += val;
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
  if((rad_extra <= 0) || (polar_dim < 1) || (polar_dim > 2) || (pts <= 0))
    return;
  
  if(polar_dim == 1) {
    double delta = 360.0 / pts;
    projectPoint(ll_xval*delta, ll_yval*rad_extra, 0, 0, ll_xval, ll_yval);
    projectPoint(hl_xval*delta, hl_yval*rad_extra, 0, 0, hl_xval, hl_yval);
    projectPoint(hh_xval*delta, hh_yval*rad_extra, 0, 0, hh_xval, hh_yval);
    projectPoint(lh_xval*delta, lh_yval*rad_extra, 0, 0, lh_xval, lh_yval);

    for(unsigned int i=0; i<m_xin_low.size(); i++) {
      projectPoint(m_xin_low[i]*delta, m_yin_low[i]*rad_extra, 0, 0,
		   m_xin_low[i], m_yin_low[i]);
      projectPoint(m_xin_hgh[i]*delta, m_yin_hgh[i]*rad_extra, 0, 0,
		   m_xin_hgh[i], m_yin_hgh[i]);
    }

  }
  else if(polar_dim == 2) {
    double delta = 360.0 / pts;
    projectPoint(ll_yval*delta, ll_xval*rad_extra, 0, 0, ll_yval, ll_xval);
    projectPoint(hl_yval*delta, hl_xval*rad_extra, 0, 0, hl_yval, hl_xval);
    projectPoint(hh_yval*delta, hh_xval*rad_extra, 0, 0, hh_yval, hh_xval);
    projectPoint(lh_yval*delta, lh_xval*rad_extra, 0, 0, lh_yval, lh_xval);
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

  for(unsigned int i=0; i<m_rin_low.size(); i++)
    m_rin_low[i] *= intensity;
  for(unsigned int i=0; i<m_rin_hgh.size(); i++)
    m_rin_hgh[i] *= intensity;

  for(unsigned int i=0; i<m_gin_low.size(); i++)
    m_gin_low[i] *= intensity;
  for(unsigned int i=0; i<m_gin_hgh.size(); i++)
    m_gin_hgh[i] *= intensity;

  for(unsigned int i=0; i<m_bin_low.size(); i++)
    m_bin_low[i] *= intensity;
  for(unsigned int i=0; i<m_bin_hgh.size(); i++)
    m_bin_hgh[i] *= intensity;
}

//-------------------------------------------------------------
// Procedure: applyScale()

void Quad3D::applyScale(double given_scale)
{
  ll_hgt *= given_scale;
  hl_hgt *= given_scale;
  hh_hgt *= given_scale;
  lh_hgt *= given_scale;

  for(unsigned int i=0; i<m_zin_low.size(); i++)
    m_zin_low[i] *= given_scale;
  for(unsigned int i=0; i<m_zin_hgh.size(); i++)
    m_zin_hgh[i] *= given_scale;  
}

//-------------------------------------------------------------
// Procedure: applyBase()

void Quad3D::applyBase(double given_base)
{
  ll_hgt += given_base;
  hl_hgt += given_base;
  hh_hgt += given_base;
  lh_hgt += given_base;
  for(unsigned int i=0; i<m_zin_low.size(); i++)
    m_zin_low[i] += given_base;
  for(unsigned int i=0; i<m_zin_hgh.size(); i++)
    m_zin_hgh[i] += given_base;
}

//-------------------------------------------------------------
// Procedure: applyTranslation

void Quad3D::applyTranslation(double xval, double yval)
{
  ll_xval += xval;
  hl_xval += xval;
  hh_xval += xval;
  lh_xval += xval;

  ll_yval += xval;
  hl_yval += xval;
  hh_yval += xval;
  lh_yval += xval;

  for(unsigned int i=0; i<m_xin_low.size(); i++)
    m_xin_low[i] += xval;
  for(unsigned int i=0; i<m_xin_hgh.size(); i++)
    m_xin_hgh[i] += xval;

  for(unsigned int i=0; i<m_yin_low.size(); i++)
    m_yin_low[i] += yval;
  for(unsigned int i=0; i<m_yin_hgh.size(); i++)
    m_yin_hgh[i] += yval;
}


//-------------------------------------------------------------
// Procedure: getXinLOW()

double Quad3D::getXinLOW(unsigned int ix) const
{
  if(ix >= m_xin_low.size())
    return(0);
  return(m_xin_low[ix]);
}

//-------------------------------------------------------------
// Procedure: getYinLOW()

double Quad3D::getYinLOW(unsigned int ix) const
{
  if(ix >= m_yin_low.size())
    return(0);
  return(m_yin_low[ix]);
}

//-------------------------------------------------------------
// Procedure: getZinLOW()

double Quad3D::getZinLOW(unsigned int ix) const
{
  if(ix >= m_zin_low.size())
    return(0);
  return(m_zin_low[ix]);
}

//-------------------------------------------------------------
// Procedure: getRinLOW()

double Quad3D::getRinLOW(unsigned int ix) const
{
  if(ix >= m_rin_low.size())
    return(0);
  return(m_rin_low[ix]);
}

//-------------------------------------------------------------
// Procedure: getGinLOW()

double Quad3D::getGinLOW(unsigned int ix) const
{
  if(ix >= m_gin_low.size())
    return(0);
  return(m_gin_low[ix]);
}

//-------------------------------------------------------------
// Procedure: getBinLOW()

double Quad3D::getBinLOW(unsigned int ix) const
{
  if(ix >= m_bin_low.size())
    return(0);
  return(m_bin_low[ix]);
}



//-------------------------------------------------------------
// Procedure: getXinHGH()

double Quad3D::getXinHGH(unsigned int ix) const
{
  if(ix >= m_xin_hgh.size())
    return(0);
  return(m_xin_hgh[ix]);
}

//-------------------------------------------------------------
// Procedure: getYinHGH()

double Quad3D::getYinHGH(unsigned int ix) const
{
  if(ix >= m_yin_hgh.size())
    return(0);
  return(m_yin_hgh[ix]);
}

//-------------------------------------------------------------
// Procedure: getZinHGH()

double Quad3D::getZinHGH(unsigned int ix) const
{
  if(ix >= m_zin_hgh.size())
    return(0);
  return(m_zin_hgh[ix]);
}

//-------------------------------------------------------------
// Procedure: getRinHGH()

double Quad3D::getRinHGH(unsigned int ix) const
{
  if(ix >= m_rin_hgh.size())
    return(0);
  return(m_rin_hgh[ix]);
}

//-------------------------------------------------------------
// Procedure: getGinHGH()

double Quad3D::getGinHGH(unsigned int ix) const
{
  if(ix >= m_gin_hgh.size())
    return(0);
  return(m_gin_hgh[ix]);
}

//-------------------------------------------------------------
// Procedure: getBinHGH()

double Quad3D::getBinHGH(unsigned int ix) const
{
  if(ix >= m_bin_hgh.size())
    return(0);
  return(m_bin_hgh[ix]);
}

//-------------------------------------------------------------
// Procedure: interpolate
//      Note: Assumed to be invoked before applying polar and colormap

void Quad3D::interpolate(double xdelta)
{
  m_xin_low.clear();
  m_yin_low.clear();
  m_zin_low.clear();
  m_rin_low.clear();
  m_gin_low.clear();
  m_bin_low.clear();

  m_xin_hgh.clear();
  m_yin_hgh.clear();
  m_zin_hgh.clear();
  m_rin_hgh.clear();
  m_gin_hgh.clear();
  m_bin_hgh.clear();

  for(double v=ll_xval+xdelta; v<hl_xval; v+=xdelta) {
    double pct = (v-ll_xval) / (hl_xval-ll_xval);

    // Part 1 of 12 - calculating m_xin_low
    double xin_low = ll_xval + (pct * (hl_xval-ll_xval));
    m_xin_low.push_back(xin_low);
    // Part 2 of 12 - calculating m_yin_low
    m_yin_low.push_back(ll_yval);
    // Part 3 of 12 - calculating m_zin_low
    double zin_low = ll_hgt + (pct * (hl_hgt-ll_hgt));
    m_zin_low.push_back(zin_low);

    // Part 4 of 12 - calculating m_rin_low
    double rin_low = ll_red + (pct * (hl_red-ll_red));
    m_rin_low.push_back(rin_low);
    // Part 5 of 12 - calculating m_gin_low
    double gin_low = ll_grn + (pct * (hl_grn-ll_grn));
    m_gin_low.push_back(gin_low);
    // Part 6 of 12 - calculating m_bin_low
    double bin_low = ll_blu + (pct * (hl_blu-ll_blu));
    m_bin_low.push_back(bin_low);

#if 1
    // Part 7 of 12 - calculating m_xin_hgh
    double xin_hgh = lh_xval + (pct * (hh_xval-lh_xval));
    m_xin_hgh.push_back(xin_hgh);
    // Part 8 of 12 - calculating m_yin_hgh
    m_yin_hgh.push_back(hh_yval);
    // Part 9 of 12 - calculating m_zin_hgh
    double zin_hgh = lh_hgt + (pct * (hh_hgt-lh_hgt));
    m_zin_hgh.push_back(zin_hgh);
    // Part 10 of 12 - calculating m_rin_hgh
    double rin_hgh = lh_red + (pct * (hh_red-lh_red));
    m_rin_hgh.push_back(rin_hgh);
    // Part 11 of 12 - calculating m_gin_hgh
    double gin_hgh = lh_grn + (pct * (hh_grn-lh_grn));
    m_gin_hgh.push_back(gin_hgh);
    // Part 12 of 12 - calculating m_bin_hgh
    double bin_hgh = lh_blu + (pct * (hh_blu-lh_blu));
    m_bin_hgh.push_back(bin_hgh);
#endif
#if 0
    // Part 7 of 12 - calculating m_xin_hgh
    double xin_hgh = hh_xval + (pct * (lh_xval-hh_xval));
    m_xin_hgh.push_back(xin_hgh);
    // Part 8 of 12 - calculating m_yin_hgh
    m_yin_hgh.push_back(hh_yval);
    // Part 9 of 12 - calculating m_zin_hgh
    double zin_hgh = hh_hgt + (pct * (lh_hgt-hh_hgt));
    m_zin_hgh.push_back(zin_hgh);
    // Part 10 of 12 - calculating m_rin_hgh
    double rin_hgh = hh_red + (pct * (lh_red-hh_red));
    m_rin_hgh.push_back(rin_hgh);
    // Part 11 of 12 - calculating m_gin_hgh
    double gin_hgh = hh_grn + (pct * (lh_grn-hh_grn));
    m_gin_hgh.push_back(gin_hgh);
    // Part 12 of 12 - calculating m_bin_hgh
    double bin_hgh = hh_blu + (pct * (lh_blu-hh_blu));
    m_bin_hgh.push_back(bin_hgh);
#endif
  }
}







