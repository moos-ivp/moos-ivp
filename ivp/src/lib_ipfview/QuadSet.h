/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: QuadSet.h                                            */
/*    DATE: July 4th 2006                                        */
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

#ifndef QUAD_SET_HEADER
#define QUAD_SET_HEADER

#include <vector>
#include <string>
#include "Quad3D.h"
#include "IvPFunction.h"
#include "IvPDomain.h"
#include "FColorMap.h"

class QuadSet 
{
public:
  QuadSet();
  ~QuadSet() {}

  // Building the QuadSet
  void   addQuad3D(Quad3D quad)         {m_quads.push_back(quad);}
  void   setIvPDomain(IvPDomain domain) {m_ivp_domain = domain;}
  void   resetMinMaxVals();
  //void   markDense(bool v)              {m_dense=v;}
  
  // Modifying the QuadSet
  void   applyColorMap(const FColorMap&);
  void   applyColorMap(const FColorMap&, double low, double hgh);
  void   addQuadSet(const QuadSet&);
  void   normalize(double, double);
  void   interpolate(double xdelta);
  void   applyColorIntensity(double);
  void   applyScale(double);
  void   applyBase(double);
  void   setBase(double);
  void   applyTranslation(double x, double y);
  void   applyTranslation();
  void   applyPolar(double, int);
  
  // Geting QuadSet Information
  unsigned int size() const                  {return(m_quads.size());}
  Quad3D       getQuad(unsigned int) const;
  IvPDomain    getDomain() const             {return(m_ivp_domain);}
  double       getMaxVal() const             {return(m_maxpt_val);}
  double       getMinVal() const             {return(m_minpt_val);}
  
  double       getMaxPoint(std::string) const;
  unsigned int getMaxPointQIX(std::string) const;
  void         print() const;
  
protected:
  std::vector<Quad3D> m_quads;

  IvPDomain    m_ivp_domain;
  bool         m_dense;
  
  // Cache Min/Max values. These are evaluated once all the Quads
  // have been calculated and added.
  double       m_maxpt_val;   // Max utilty all quad vertices
  double       m_minpt_val;
  unsigned int m_max_x_qix;   // Index in ivp_domain of max x value
  unsigned int m_max_y_qix;
};
#endif

