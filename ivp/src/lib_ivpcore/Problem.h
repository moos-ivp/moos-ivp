/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Problem.h                                            */
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

#ifndef PROBLEM_HEADER
#define PROBLEM_HEADER

#include "IvPFunction.h"
#include "IvPDomain.h"
#include "IvPBox.h"

class Problem {
public:
  Problem();
  virtual ~Problem();

public:    // Virtual Function
  virtual bool solve(const IvPBox *b=0) = 0;

  void   setDomain(IvPDomain);
  void   addOF(IvPFunction*);
  bool   alignOFs();
  int    getDim();
  int    getOFNUM() const  {return(m_ofnum);};
  double getMaxWT()        {return(m_maxwt);};
  void   setSilent(bool x) {m_silent=x;};
  void   initialSolution1();
  void   initialSolution2();
  void   sortOFs(bool high_to_low=true);
  void   processInitSol(const IvPBox*);
  void   setEpsilon(double v) {if(v>=0) m_epsilon=v;};
  double getEpsilon()         {return(m_epsilon);};
  double getResult(const std::string&, bool *v=0);

  IvPFunction* getOF(int);

  const  IvPBox* getMaxBox()  {return(m_maxbox);};

protected:
  bool     universesInSync();
  void     newSolution(double, const IvPBox*);

protected:
  IvPBox*       m_maxbox;   // Box of best working solution
  double        m_maxwt;    // Value of best working solution

  IvPFunction** m_ofs;      // array of objective functions
  int           m_ofnum;    // # of objective functions
  bool          m_silent;   // true if no output during solve
  double        m_epsilon;  // delta threshold for new max weight

  IvPDomain     m_domain;
};

#endif










