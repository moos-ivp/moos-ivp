/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Regressor.h                                          */
/*    DATE: Dec 5th, 2004 (Sat at Brueggers)                     */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef REGRESSOR_HEADER
#define REGRESSOR_HEADER

#include "AOF.h"

class Regressor {
public:
  Regressor(const AOF*, int deg=1);
  virtual ~Regressor();

public:  
  int     getDegree() const   {return(m_degree);};

  double  setWeight(IvPBox*, bool feedback=false);
  void    setStrictRange(bool val) {m_strict_range = val;};

  const AOF* getAOF() {return(m_aof);};

protected:
  void    setCorners(IvPBox*);
  double  setWeight0(IvPBox*, bool);
  double  setWeight1(IvPBox*, bool);
  double  setWeight2(IvPBox*, bool);
  void    setQuadCoeffs(double, double,  double,  double, double, 
			double, double&, double&, double&);
  double  evalPtBox(const IvPBox*);
  bool    centerBox(const IvPBox*, IvPBox*);
  
protected:
  // AOF represents the underlying function.
  const AOF* m_aof;         
  IvPDomain  m_domain;

  int        m_dim;
  bool       m_strict_range;

  // The below data structures are used repeatedly on 
  // successive calls to "setWeight". So they are allocated 
  // once for efficiency sake.
  IvPBox*   m_center_point;
  double    m_center_val;
  IvPBox**  m_corner_point; 
  double*   m_corner_val;   
  int       m_corners;      
  int*      m_mask;
  double*   m_vals;

  int       m_degree;
};

#endif










