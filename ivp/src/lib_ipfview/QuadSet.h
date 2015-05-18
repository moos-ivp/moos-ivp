/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
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

  // Set/Apply Information
  bool   applyIPF(IvPFunction *ipf, std::string src="");

  bool   applyIPF1D(IvPFunction *ipf, std::string src);
  bool   applyIPF2D(IvPFunction *ipf);

  void   applyColorMap(const FColorMap&);
  void   applyColorMap(const FColorMap&, double low, double hgh);
  void   addQuadSet(const QuadSet&);
  void   normalize(double, double);

  // Get Information
  bool         isEmpty1D() const             {return(size1D()==0);}
  bool         isEmpty2D() const             {return(size2D()==0);}
  bool         isEmptyND() const;
  unsigned int getQuadSetDim() const         {return(m_quadset_dim);}
  Quad3D       getQuad(unsigned int i) const {return(m_quads[i]);}
  double       getMaxVal() const             {return(m_maxpt_val);}
  double       getMinVal() const             {return(m_minpt_val);}
  double       getPriorityWt() const         {return(m_ipf_priority_wt);}
  IvPDomain    getDomain() const             {return(m_ivp_domain);}
  unsigned int size2D() const                {return(m_quads.size());}
  unsigned int size1D() const;
  unsigned int size1DFs() const              {return(m_domain_pts.size());}

  double       getMaxPoint(std::string) const;
  unsigned int getMaxPointQIX(std::string) const;
  void         print() const;

  void         resetMinMaxVals();
  
  std::vector<double>  getDomainPts(unsigned int=0)  const;
  std::vector<double>  getRangeVals(unsigned int=0)  const;
  std::vector<bool>    getDomainPtsX(unsigned int=0) const;
  double               getRangeValMax(unsigned int=0) const;
  unsigned int         getDomainIXMax(unsigned int=0) const;
  std::string          getSource(unsigned int=0) const;
  
protected:
  std::vector<Quad3D> m_quads;

  IvPDomain    m_ivp_domain;
  double       m_ipf_priority_wt;
  unsigned int m_quadset_dim;

  // Cache Min/Max Utility values (high/low of the function)
  double      m_maxpt_val;
  double      m_minpt_val;

  // Cache the location of the high/low of the function
  double      m_max_crs;
  double      m_max_spd;

  // Cache the location of the high/low of the function in units of
  // the IvP domain, or quadset index.
  double      m_max_crs_qix;
  double      m_max_spd_qix;

  // Values of the objecive function can be made to snap to 
  // intervals. Can be used to alter the rendering.
  double      m_snap_val;


  // Values for representing 1D IPFs
  // Each outer index below is for one source, typically:
  //   [0] Collective [1] Source#1 [2] Source#2 ...
  std::vector<std::vector<double> >  m_domain_pts;
  std::vector<std::vector<bool> >    m_domain_ptsx; // true if pt piece edge
  std::vector<std::vector<double> >  m_range_vals;
  std::vector<double>                m_range_val_max;
  std::vector<unsigned int>          m_domain_ix_max;
  std::vector<std::string>           m_sources;

};
#endif




