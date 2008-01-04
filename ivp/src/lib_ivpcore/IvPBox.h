/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPBox.h                                             */
/*    DATE: Too long ago to remember (1996-1999)                 */
/*                                                               */
/* (IvP) The interval programming model is a mathematical        */
/* programming model for representing and solving multi-         */
/* objective optimization problems.                              */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
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

  void    moveIntercept(double v) {m_wts[getWtc()-1] += v;};
  void    scaleWT(double);  
  void    setWT(double w);  

  double  ptVal(const IvPBox*) const; 
  double  maxVal() const;  
  double  minVal() const;  
  void    maxPt(IvPBox&)  const;
  IvPBox  maxPt()         const;

  void    setPTS(int d, int l, int h)   {m_pts[d*2]=l; m_pts[d*2+1]=h;};
  void    setBDS(int d, bool l, bool h) {m_bds[d*2]=l; m_bds[d*2+1]=h;};

  int&    pt(int d, int e=0) {return(m_pts[d*2+e]);};
  bool&   bd(int d, int e=0) {return(m_bds[d*2+e]);};
  double& wt(int d)          {return(m_wts[d]);};
  int&    ofindex()          {return(m_of);};
  bool&   mark()             {return(m_markval);};

  const int&    pt(int d, int e=0) const {return(m_pts[d*2+e]);};
  const bool&   bd(int d, int e=0) const {return(m_bds[d*2+e]);};
  const double& wt(int d)          const {return(m_wts[d]);};
  
  int     getDim() const               {return((int)m_dim);};
  int     getDegree() const            {return((int)m_degree);};
  int     getWtc() const               {return((m_degree*m_dim)+1);};
  bool    null() const                 {return(m_dim==0);};

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










