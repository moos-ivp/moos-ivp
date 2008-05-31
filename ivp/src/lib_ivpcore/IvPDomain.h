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
  bool   addDomain(const std::string&, double, double, unsigned int);
  bool   addDomain(const IvPDomain&, const std::string&);
  bool   hasDomain(const std::string&) const;
  void   clear();
  void   print() const;

  int    getIndex(const std::string&) const;

  unsigned int  size() const                 {return(m_dname.size());};
  unsigned int  getVarPoints(unsigned int i) const    
    {return(m_dpoints[i]);};

  double getVarLow(unsigned int i) const    {return(m_dlow[i]);};
  double getVarHigh(unsigned int i) const   {return(m_dhigh[i]);};
  double getVarDelta(unsigned int i) const  {return(m_ddelta[i]);};

  // Return number of points in the domain for a given variable name.
  // If the variable name is unknown, just return zero.
  int getVarPoints(const std::string& str) const
    {
      unsigned int ix = getIndex(str);
      return(((ix>=0)&&(ix<m_dpoints.size())) ? m_dpoints[ix] : 0);
    }

  // For the ith domain index, and j steps into the domain, return
  // the corresponding floating point value.
  bool getVal(unsigned int i, unsigned int j, double &val) const
    {
      unsigned int dsize = m_dlow.size();
      if((i>=0) && (i<dsize) && (j>=0) && (j<m_dpoints[i])) {
	val = m_dlow[i] + (m_ddelta[i] * j);
	return(true);
      }
      return(false);
    };

  // A simplified version of getVal where no error is indicate
  // if the domain or index is out of range.
  double getVal(unsigned int d, unsigned int j) const
    {
      unsigned int dsize = m_dlow.size();
      if((d>=0) && (d<dsize) && (j>=0) && (j<m_dpoints[d]))
	return(m_dlow[d] + (m_ddelta[d] * j));
      return(0);
    };

  // For domain given by the varible name, and j steps into the 
  // domain, return the corresponding floating point value.
  bool getVal(const std::string str, unsigned int j, double &val) const
    {
      return(getVal(getIndex(str), j, val));
    };
      
  // Return the variable name of ith element of the domain
  std::string getVarName(unsigned int i) const
    {
      if((i<0) || (i>m_dname.size()))
	return("");
      return(m_dname[i]);
    }

  // Return the discrete index into the domain given by a double
  // input value. Round up, down or closest, depending on snaptype.
  unsigned int getDiscreteVal(unsigned int index, 
			      double val, int snaptype) const;

private:
  std::vector<std::string>  m_dname;
  std::vector<double>       m_dlow;
  std::vector<double>       m_dhigh;
  std::vector<double>       m_ddelta;
  std::vector<unsigned int> m_dpoints;
};

#endif










