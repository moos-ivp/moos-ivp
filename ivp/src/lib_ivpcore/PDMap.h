/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PDMap.h                                              */
/*    DATE: June 1st 2004 (pulled from OF class)                 */
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

#ifndef PDMAP_HEADER
#define PDMAP_HEADER

#include <string> 
#include "IvPBox.h"
#include "BoxSet.h"
#include "IvPGrid.h"
#include "IvPDomain.h"

class PDMap {
public:
  PDMap(int pcs, const IvPDomain& gdomain, int deg=1);
  PDMap(const PDMap*);
  virtual ~PDMap();

  int       getIX(const IvPBox *);
  void      applyWeight(double);
  void      applyScalar(double);
  void      normalize(double base, double range);
  
  int       getDim() const        {return(m_domain.size());}
  IvPGrid*  getGrid()             {return(m_grid);}
  IvPBox    getGelBox() const     {return(m_gelbox);}
  IvPDomain getDomain() const     {return(m_domain);}
  BoxSet*   getBS(const IvPBox*); 
  IvPBox    getUniverse() const;

  int       size() const          {return(m_boxCount);}
  int       getDegree() const     {return(m_degree);}
  double    getMinWT() const;
  double    getMaxWT() const;

  void      updateGrid(bool BX=1, bool UB=1);
  bool      setGelBox(const IvPBox& box);
  void      setGelBox();
  std::string getGridConfig() const;
  
  double    evalPoint(const IvPBox*, bool* covered=0) const;

  void      print(bool full=true) const;
  void      growBoxArray(int);
  void      growBoxCount(int i=1) {m_boxCount += i;}
  bool      freeOfNan() const;

  bool      valid(bool verbose=false) const;
  
  const IvPBox *getBox(int i) const {return(m_boxes[i]);}

  IvPBox*&  bx(int i) {return(m_boxes[i]);}

public: // Conversion Functions
  bool      transDomain(const IvPDomain&, const int*);
  void      removeNULLs();

protected:
  IvPDomain m_domain;
  IvPBox**  m_boxes;
  int       m_boxCount;
  int       m_degree;   // Zero:Scalar, Nonzero: Linear
  IvPBox    m_gelbox;
  IvPGrid*  m_grid;
}; 
#endif








