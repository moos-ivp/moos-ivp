/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPDomain.h                                          */
/*    DATE: May 29 2004 At Indigo cafe in Montreal               */
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

#ifndef IVPDOMAIN_HEADER
#define IVPDOMAIN_HEADER

#include <string>
#include <vector>
#include "IvPBox.h"

class IvPDomain {
public:
  IvPDomain() {};
  ~IvPDomain() {};

 public: bool operator==(const IvPDomain&) const;

public:
  bool   addDomain(const std::string&, double, double, int);
  bool   addDomain(const IvPDomain&, const std::string&);
  bool   hasDomain(const std::string&) const;
  void   clear();
  void   print();

  int    getIndex(const std::string&) const;
  int    size() const                 {return(m_dname.size());};
  double getVarLow(int i) const       {return(m_dlow[i]);};
  double getVarHigh(int i) const      {return(m_dhigh[i]);};
  double getVarDelta(int i) const     {return(m_ddelta[i]);};
  int    getVarPoints(int i) const    {return(m_dpoints[i]);};

  int    getVarPoints(const std::string& str) const
    {
      unsigned int ix = getIndex(str);
      return(((ix>=0)&&(ix<m_dpoints.size())) ? m_dpoints[ix] : 0);
    }

  bool getVal(int i, int j, double &val) const
    {
      int dsize = (int)(m_dlow.size());
      if((i>=0) && (i<dsize) && (j>=0) && (j<m_dpoints[i])) {
	val = m_dlow[i] + (m_ddelta[i] * j);
	return(true);
      }
      return(false);
    };


  bool getVal(const std::string str, int j, double &val) const
    {
      return(getVal(getIndex(str), j, val));
    };
      
  std::string getVarName(int i) const
    {
      if((i<0) || (i>(int)m_dname.size()))
	return("");
      return(m_dname[i]);
    }

private:
  std::vector<std::string> m_dname;
  std::vector<double>      m_dlow;
  std::vector<double>      m_dhigh;
  std::vector<double>      m_ddelta;
  std::vector<int>         m_dpoints;
};

#endif









