/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPBox.h                                             */
/*    DATE: Too long ago to remember (1996-1999)                 */
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

/****************************************************************/
/* Boundary-type info was added March 16th, 2001                */
/****************************************************************/

#ifndef IvPBOX_HEADER
#define IvPBOX_HEADER

class IvPBox {

  typedef unsigned short int uint16;

public:
  IvPBox(int gdim=0, int gdegree=1);
  IvPBox(const IvPBox&);
  virtual ~IvPBox();

  const IvPBox &operator=(const IvPBox&);

  void    copy(const IvPBox*);
  IvPBox* copy() const;

  void    moveIntercept(double v) {m_wts[getWtc()-1] += v;}
  void    scaleWT(double);  
  void    setWT(double w);  

  double  ptVal(const IvPBox*) const; 
  double  maxVal() const;  
  double  minVal() const;  
  void    maxPt(IvPBox&)  const;
  IvPBox  maxPt()         const;

  void    setPTS(int d, int l, int h)   {m_pts[d*2]=l; m_pts[d*2+1]=h;}
  void    setBDS(int d, bool l, bool h) {m_bds[d*2]=l; m_bds[d*2+1]=h;}

  int&    pt(int d, int e=0) {return(m_pts[d*2+e]);}
  bool&   bd(int d, int e=0) {return(m_bds[d*2+e]);}
  double& wt(int d)          {return(m_wts[d]);}
  int&    ofindex()          {return(m_of);}
  bool&   mark()             {return(m_markval);}

  const int&    pt(int d, int e=0) const {return(m_pts[d*2+e]);}
  const bool&   bd(int d, int e=0) const {return(m_bds[d*2+e]);}
  const double& wt(int d)          const {return(m_wts[d]);}
  
  int     getDim() const               {return((int)m_dim);}
  int     getDegree() const            {return((int)m_degree);}
  int     getWtc() const               {return((m_degree*m_dim)+1);}
  bool    null() const                 {return(m_dim==0);}

  bool    intersect(const IvPBox*) const;
  bool    intersect(IvPBox*, IvPBox*&) const;
  bool    isPtBox() const;

  void    print(bool full=true) const;
  void    transDomain(int, const int*);

protected:
  uint16    m_dim;
  uint16    m_degree;
  int*      m_pts;
  bool*     m_bds;
  double*   m_wts;
  int       m_of;
  bool      m_markval;
};
#endif



















