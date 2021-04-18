/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: QuadSet1D.h                                          */
/*    DATE: June 19th 2016                                       */
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

#ifndef QUAD_SET_1D_HEADER
#define QUAD_SET_1D_HEADER

#include <vector>
#include <string>
#include "Quad3D.h"
#include "IvPFunction.h"
#include "IvPDomain.h"
#include "FColorMap.h"

class QuadSet1D 
{
public:
  QuadSet1D();
  ~QuadSet1D() {}

  // Set/Apply Information
  bool   applyIPF(IvPFunction *ipf, std::string src="");

  void   addQuadSet1D(const QuadSet1D&);
  void   setIvPDomain(IvPDomain);
  
  // Get Information
  bool         isEmpty1D() const             {return(size1D()==0);}
  bool         isEmptyND() const;
  double       getPriorityWt() const         {return(m_ipf_priority_wt);}
  IvPDomain    getDomain() const             {return(m_ivp_domain);}
  unsigned int size1D() const;
  unsigned int size1DFs() const              {return(m_domain_pts.size());}

  void         print() const;

  std::vector<double>  getDomainPts(unsigned int=0)  const;
  std::vector<double>  getRangeVals(unsigned int=0)  const;
  std::vector<bool>    getDomainPtsX(unsigned int=0) const;
  double               getRangeValMax(unsigned int=0) const;
  unsigned int         getDomainIXMax(unsigned int=0) const;
  std::string          getSource(unsigned int=0) const;
  
protected:
  IvPDomain    m_ivp_domain;
  double       m_ipf_priority_wt;

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








