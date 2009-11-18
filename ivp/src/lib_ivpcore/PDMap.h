/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PDMap.h                                              */
/*    DATE: June 1st 2004 (pulled from OF class)                 */
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

#ifndef PDMAP_HEADER
#define PDMAP_HEADER

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
  
  int       getDim() const        {return(m_domain.size());};
  IvPGrid*  getGrid()             {return(m_grid);};
  IvPBox    getGelBox() const     {return(m_gelbox);};
  IvPDomain getDomain() const     {return(m_domain);};
  BoxSet*   getBS(const IvPBox*); 
  IvPBox    getUniverse() const;

  int       size() const          {return(m_boxCount);};
  int       getDegree() const     {return(m_degree);};
  double    getMinWT() const;
  double    getMaxWT() const;

  void      updateGrid(bool BX=1, bool UB=1);
  void      setGelBox(const IvPBox& box);
  void      setGelBox();

  double    evalPoint(const IvPBox*, bool* covered=0) const;

  void      print(bool full=true) const;
  void      growBoxArray(int);
  void      growBoxCount(int i=1) {m_boxCount += i;};
  bool      freeOfNan() const;

  const IvPBox *getBox(int i) const {return(m_boxes[i]);};

  IvPBox*&  bx(int i) {return(m_boxes[i]);};

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










