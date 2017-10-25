/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Regressor.h                                          */
/*    DATE: Dec 5th, 2004 (Sat at Brueggers)                     */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef REGRESSOR_HEADER
#define REGRESSOR_HEADER

#include <vector>
#include <string>
#include "AOF.h"

class Regressor {
public:
  Regressor(const AOF*, int deg=1);
  virtual ~Regressor();

public:  
  int     getDegree() const   {return(m_degree);}

  double  setWeight(IvPBox*, bool feedback=false);
  void    setStrictRange(bool val) {m_strict_range = val;}

  unsigned int getMessageCnt() const {return(m_messages.size());}
  std::string  getMessage(unsigned int);

  const AOF* getAOF() {return(m_aof);}

  unsigned int getTotalSetWts() const {return(m_total_setwts);}
  unsigned int getTotalEvals() const {return(m_total_evals);}
  
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

  std::vector<std::string> m_messages;

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

  double    m_pteval_min;
  double    m_pteval_max;
  bool      m_pteval_set;

  unsigned int m_total_setwts;
  unsigned int m_total_evals;
  
  
};

#endif


















